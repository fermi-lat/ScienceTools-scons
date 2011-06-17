"""
Module to calculate upper limits.

$Header:

author:  Eric Wallace <ewallace@uw.edu>
"""
import numpy as N
from scipy import integrate
from scipy.stats.distributions import chi2
from scipy.optimize import fmin

def upper_limit(roi,**kwargs):
    """Compute an upper limit on the source flux, by the "PDG Method"

    This method computes an upper limit on the flux of a specified source
    for a given time interval. The limit is computed by integrating the
    likelihood over the flux of the source, via Simpson's Rule, up to the
    desired percentile (confidence level). As such, it is essentially a
    Bayesian credible interval, using a uniform prior on the flux
    parameter.

    Note that the default integral limits are determined assuming that
    the relevant parameter is the normalization parameter of a PowerLaw
    model. For other models, especially PowerLawFlux, the limits should
    be specified appropriately.

    The limit returned is the integrated flux above 100 MeV in photons
    per square centimeter per second.

    Arguments:
        which: integer [0]
            Index of the point source for which to compute the limit.
        confidence: float [.95]
            Desired confidence level of the upper limit.
        integral_min: float [-15]
            Lower limit of the likelihood integral *in log space*.
        integral_max: float [-8]
            Upper limit of the likelihood integral *in log space*.
        simps_points: int [10]
            Number of evaluation points *per decade* for Simpson's rule.
        e_weight: float [0]
            Energy weight for the flux integral (see documentation for uw.like.Models)
        cgs: bool [False]
            If true return flux in cgs units (see documentation for uw.like.Models)
    """
    self=roi
    kw = dict(which=0,
              confidence=0.95,
              integral_min=-15,
              integral_max =-8,
              simps_points = 100,
              e_weight = 0,
              cgs = False)
    for k,v in kw.items():
        kw[k] = kwargs.pop(k,v)
    if kwargs:
        for k in kwargs.keys():
            print("Invalid keyword argument for ROIAnalysis.upper_limit: %s"%k)
    params = self.parameters().copy()
    ll_0 = self.logLikelihood(self.parameters())

    source = self.get_source(kw['which'])
    if not source.__dict__.has_key('model'):
        raise Exception("upper_limit can only calculate upper limits of point and extended sources.")
    model=source.model

    def like(norm):
        model.setp(0,norm,internal=True)
        return N.exp(ll_0-self.logLikelihood(self.parameters()))
    npoints = kw['simps_points'] * (kw['integral_max'] - kw['integral_min'])
    points = N.log10(N.logspace(kw['integral_min'],
                                  kw['integral_max'],npoints*2+1))
    y = N.array([like(x)*10**x for x in points])
    trapz1 = integrate.cumtrapz(y[::2])
    trapz2 = integrate.cumtrapz(y)[::2]
    cumsimps = (4*trapz2 - trapz1)/3.
    cumsimps /= cumsimps[-1]
    i1 = N.where(cumsimps<.95)[0][-1]
    i2 = N.where(cumsimps>.95)[0][0]
    x1, x2 = points[::2][i1], points[::2][i2]
    y1, y2 = cumsimps[i1], cumsimps[i2]
    #Linear interpolation should be good enough at this point
    limit = x1 + ((x2-x1)/(y2-y1))*(kw['confidence']-y1)
    model.setp(0,limit,internal=True)
    uflux = self.psm.models[0].i_flux(e_weight=kw['e_weight'],cgs=kw['cgs'])
    self.logLikelihood(params)
    return uflux

def upper_limit_quick(roi,which = 0,confidence = .95,e_weight = 0,cgs = False):
    """Compute an upper limit on the flux of a source assuming a gaussian likelihood.

    Arguments:
        which: integer [0]
            Index of the point source for which to compute the limit.
        confidence: float [.95]
            Desired confidence level of the upper limit.
        e_weight: float [0]
            Energy weight for the flux integral (see documentation for uw.like.Models)
        cgs: bool [False]
            If true return flux in cgs units (see documentation for uw.like.Models)

    The flux returned is an upper limit on the integral flux for the model
    above 100 MeV.

    The upper limit is found based on the change in the log likelihood
    from the maximum, using the Gaussian approximation. It is quicker
    than the Bayesian method performed by upper_limit, but less robust.
    In particular, fmin will sometimes get lost and return absurdly
    small values, and if the maximum is too far above zero, it will
    often find the lower of the two solutions to the appropriate
    equation.
    """
    self=roi

    delta_logl = chi2.ppf(2*confidence-1,1)/2.
    params = self.parameters().copy()
    #self.psm.models[which]._p[0]  = -20
    zp = self.logLikelihood(self.parameters())

    def f(norm):
        self.psm.models[which]._p[0] = N.log10(norm)
        ll = self.logLikelihood(self.parameters())
        return abs(ll - zp - delta_logl)

    limit = fmin(f,N.array([10**-6]),disp=0)[0]
    self.psm.models[which]._p[0] = N.log10(limit)
    uflux = self.psm.models[which].i_flux(e_weight = e_weight,cgs = cgs)
    self.set_parameters(params)
    return uflux

