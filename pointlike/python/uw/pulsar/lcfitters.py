"""
A module implementing binned and unbinned likelihood for weighted and
unweighted sets of photon phases.  The model is encapsulated in LCTemplate,
a mixture model.

LCPrimitives are combined to form a light curve (LCTemplate).  
LCFitter then performs a maximum likielihood fit to determine the 
light curve parameters.

LCFitter also allows fits to subsets of the phases for TOA calculation.

$Header$

author: M. Kerr <matthew.kerr@gmail.com>

"""

import numpy as np
from scipy.optimize import fmin,leastsq,fmin_slsqp
from uw.pulsar.stats import z2mw,hm,hmw

SECSPERDAY = 86400.

def prim_io(template):
    """ Read files and build LCPrimitives. """

    def read_gaussian(toks):
        primitives = []
        for i,tok in enumerate(toks):
            if tok[0].startswith('phas'):
                g = LCGaussian()
                g.p[2] = float(tok[2])
                g.errors[2] = float(tok[4])
                primitives += [g]
            elif tok[0].startswith('fwhm'):
                g = primitives[-1]
                g.p[1] = float(tok[2])/2.3548200450309493      # kluge for now
                g.errors[1] = float(tok[4])/2.3548200450309493
            elif tok[0].startswith('ampl'):
                g = primitives[-1]
                g.p[0] = float(tok[2])
                g.errors[0] = float(tok[4])
        return primitives

    toks = [line.strip().split() for line in file(template) if len(line.strip()) > 0]
    if 'gauss' in toks[0]:     return read_gaussian(toks[1:])
    elif 'kernel' in toks[0]:  return [LCKernelDensity(input_file=toks[1:])]
    elif 'fourier' in toks[0]: return [LCEmpiricalFourier(input_file=toks[1:])]
    raise ValueError,'Template format not recognized!'

def weighted_light_curve(nbins,phases,weights,normed=False):
    """ Return a set of bins, values, and errors to represent a
        weighted light curve."""
    bins = np.linspace(0,1,nbins+1)
    counts = np.histogram(phases,bins=bins,normed=False)[0]
    w1 = (np.histogram(phases,bins=bins,weights=weights,normed=False)[0]).astype(float)
    w2 = (np.histogram(phases,bins=bins,weights=weights**2,normed=False)[0]).astype(float)
    errors = np.where(counts > 1, w2**0.5, counts)
    norm = w1.sum()/nbins if normed else 1.
    return bins,w1/norm,errors/norm


def LCFitter(template,phases,weights=None,times=1,binned_bins=100):
    """ Factory class for light curve fitters.
        Arguments:
        template -- an instance of LCTemplate
        phases   -- list of photon phases

        Keyword arguments:
        weights     [None] optional photon weights
        times       [None] optional photon arrival times
        binned_bins [100]  # of bins to use in binned likelihood
    """
    kwargs = dict(times=np.asarray(times),binned_bins=binned_bins)
    if weights is None:
        kwargs['weights'] = None
        return UnweightedLCFitter(template,phases,**kwargs)
    kwargs['weights'] = np.asarray(weights)
    return WeightedLCFitter(template,phases,**kwargs)

class UnweightedLCFitter(object):

    def __init__(self,template,phases,**kwargs):
        self.template = template
        self.phases = np.asarray(phases)
        self.__dict__.update(kwargs)
        self._hist_setup()
        # default is unbinned likelihood
        self.loglikelihood = self.unbinned_loglikelihood
        self.gradient = self.unbinned_gradient

    def _hist_setup(self):
        """ Setup binning for a quick chi-squared fit."""
        h = hm(self.phases)
        nbins = 25
        if h > 100: nbins = 50
        if h > 1000: nbins = 100
        hist = np.histogram(self.phases,bins=np.linspace(0,1,nbins))
        if len(hist[0])==nbins: raise ValueError,'Histogram too old!'
        x = ((hist[1][1:] + hist[1][:-1])/2.)[hist[0]>0]
        counts = (hist[0][hist[0]>0]).astype(float)
        y    = counts / counts.sum() * nbins
        yerr = counts**0.5  / counts.sum() * nbins
        self.chistuff = x,y,yerr
        # now set up binning for binned likelihood
        nbins = self.binned_bins+1
        hist = np.histogram(self.phases,bins=np.linspace(0,1,nbins))
        self.counts_centers = ((hist[1][1:] + hist[1][:-1])/2.)[hist[0]>0]
        self.counts = hist[0][hist[0]>0]

    def unbinned_loglikelihood(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if (not t.shift_mode) and np.any(p<0):
            return 2e20
        args[0].set_parameters(p)
        rvals = -np.log(t(self.phases)).sum()
        if np.isnan(rvals): return 2e20 # NB need to do better accounting of norm
        return rvals

    def binned_loglikelihood(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if (not t.shift_mode) and np.any(p<0):
            return 2e20
        return -(self.counts*np.log(t(self.counts_centers))).sum()
      
    def unbinned_gradient(self,p,*args):
        args[0].set_parameters(p); t = self.template
        return -(t.gradient(self.phases)/t(self.phases)).sum(axis=1)

    def binned_gradient(self,p,*args):
        args[0].set_parameters(p); t = self.template
        return -(self.counts*t.gradient(self.counts_centers)/t(self.counts_centers)).sum(axis=1)

    def chi(self,p,*args):
        x,y,yerr = self.chistuff
        if not self.template.shift_mode and np.any(p < 0):
            return 2e100*np.ones_like(x)/len(x)
        args[0].set_parameters(p)
        chi = (self.template(x) - y)/yerr
        return chi

    def quick_fit(self):
        t = self.template
        p0 = t.get_parameters().copy()
        chi0 = (self.chi(t.get_parameters(),t)**2).sum()
        f = leastsq(self.chi,t.get_parameters(),args=(t))
        chi1 = (self.chi(t.get_parameters(),t)**2).sum()
        print chi0,chi1,' chi numbers'
        if (chi1 > chi0):
            print 'Failed least squares fit -- reset and proceed to likelihood.'
            t.set_parameters(p0)

    def _fix_state(self,restore_state=None):
        old_state = []
        counter = 0
        for p in self.template.primitives:
            for i in xrange(len(p.p)):
                old_state.append(p.free[i])
                if restore_state is not None:
                    p.free[i] = restore_state[counter]
                else:
                    if i<(len(p.p)-1):
                        p.free[i] = False
                counter += 1
        return old_state

    def _set_unbinned(self,unbinned=True):
        if unbinned:
            self.loglikelihood = self.unbinned_loglikelihood
            self.gradient = self.unbinned_gradient
        else:
            self.loglikelihood = self.binned_loglikelihood
            self.gradient = self.binned_gradient

         
    def fit(self,quick_fit_first=False, estimate_errors=True, unbinned=True, use_gradient=True, positions_first=False):

        self._set_unbinned(unbinned)

        if positions_first:
            print 'Running positions first'
            restore_state = self._fix_state()
            self.fit(quick_fit_first=quick_fit_first,estimate_errors=False, unbinned=unbinned, use_gradient=use_gradient, positions_first=False)
            self._fix_state(restore_state)

        # an initial chi squared fit to find better seed values
        if quick_fit_first: self.quick_fit()

        ll0 = -self.loglikelihood(self.template.get_parameters(),self.template)
        p0 = self.template.get_parameters().copy()
        if use_gradient:
            f = self.fit_tnc()
        else:
            f = self.fit_fmin()
        print ll0,self.ll
        if (ll0 > self.ll) or (self.ll==-2e20) or (np.isnan(self.ll)):
            self.bad_p = self.template.get_parameters().copy()
            print 'Failed likelihood fit -- resetting parameters.'
            self.template.set_parameters(p0)
            self.ll = ll0; self.fitvals = p0
        elif estimate_errors:
            self._errors()
            self.template.set_errors(np.diag(self.cov_matrix)**0.5)

    def fit_fmin(self,ftol=1e-5):
        fit = fmin(self.loglikelihood,self.template.get_parameters(),args=(self.template,),disp=0,ftol=ftol,full_output=True)
        self.fitval = fit[0]
        self.ll  = -fit[1]
        return fit

    def fit_cg(self):
        from scipy.optimize import fmin_cg
        fit = fmin_cg(self.loglikelihood,self.template.get_parameters(),fprime=self.gradient,args=(self.template,),full_output=1,disp=1)
        return fit

    def fit_bfgs(self):
        from scipy.optimize import fmin_bfgs
        #bounds = self.template.get_bounds()
        fit = fmin_bfgs(self.loglikelihood,self.template.get_parameters(),fprime=self.gradient,args=(self.template,),full_output=1,disp=1,gtol=1e-5,norm=2)
        self.template.set_errors(np.diag(fit[3])**0.5)
        self.fitval = fit[0]
        self.ll  = -fit[1]
        self.cov_matrix = fit[3]
        return fit

    def fit_tnc(self,ftol=1e-5):
        from scipy.optimize import fmin_tnc
        bounds = self.template.get_bounds()
        fit = fmin_tnc(self.loglikelihood,self.template.get_parameters(),fprime=self.gradient,args=(self.template,),ftol=ftol,pgtol=1e-5,bounds=bounds,maxfun=2000,messages=8)
        self.fitval = fit[0]
        self.ll = -self.loglikelihood(self.template.get_parameters(),self.template)
        return fit

    def fit_l_bfgs_b(self):
        from scipy.optimize import fmin_l_bfgs_b
        fit = fmin_l_bfgs_b(self.loglikelihood,self.template.get_parameters(),fprime=self.gradient,args=(self.template,),bounds=self.template.get_bounds(),factr=1e-5)
        return fit

    def _errors(self):
        from numpy.linalg import inv
        h1 = hessian(self.template,self.loglikelihood)
        try: 
            c1 = inv(h1)
            h2 = hessian(self.template,self.loglikelihood,delt=np.diag(c1)**0.5)
            c2 = inv(h2)
            if np.all(np.diag(c2)>0): self.cov_matrix = c2
            elif np.all(np.diag(c1)>0): self.cov_matrix = c1
            else: raise ValueError
        except:
            print 'Unable to invert hessian!'
            self.cov_matrix = np.zeros_like(h1)

    def __str__(self):
        if 'll' in self.__dict__.keys():
            return '\nLog Likelihood for fit: %.2f\n'%(self.ll) + str(self.template)
        return str(self.template)

    def write_template(self,outputfile='template.gauss'):
        s = self.template.prof_string(outputfile=outputfile)

    def plot(self,nbins=50,fignum=2, axes=None):
        import pylab as pl
        weights = self.weights
        dom = np.linspace(0,1,200)

        if axes is None:
            fig = pl.figure(fignum)
            axes = fig.add_subplot(111)

        axes.hist(self.phases,bins=np.linspace(0,1,nbins+1),histtype='step',ec='red',normed=True,lw=1,weights=weights)
        if weights is not None:
            bg_level = 1-(weights**2).sum()/weights.sum()
            axes.axhline(bg_level,color='blue')
            cod = self.template(dom)*(1-bg_level)+bg_level
            axes.plot(dom,cod,color='blue')
            x,w1,errors = weighted_light_curve(nbins,self.phases,weights,normed=True)
            x = (x[:-1]+x[1:])/2
            axes.errorbar(x,w1,yerr=errors,capsize=0,marker='',ls=' ',color='red')
        else:
            cod = self.template(dom)
            axes.plot(dom,cod,color='blue',lw=1)
            h = np.histogram(self.phases,bins=np.linspace(0,1,nbins+1))
            x = (h[1][:-1]+h[1][1:])/2
            n = float(h[0].sum())/nbins
            axes.errorbar(x,h[0]/n,yerr=h[0]**0.5/n,capsize=0,marker='',ls=' ',color='red')
        pl.axis([0,1,pl.axis()[2],max(pl.axis()[3],cod.max()*1.05)])
        axes.set_ylabel('Normalized Profile')
        axes.set_xlabel('Phase')
        axes.grid(True)

    def plot_residuals(self,nbins=50,fignum=3):
        import pylab as pl
        edges = np.linspace(0,1,nbins+1)
        lct = self.template
        cod = np.asarray([lct.integrate(e1,e2) for e1,e2 in zip(edges[:-1],edges[1:])])*len(self.phases)
        pl.figure(fignum)
        counts= np.histogram(self.phases,bins=edges)[0]
        pl.errorbar(x=(edges[1:]+edges[:-1])/2,y=counts-cod,yerr=counts**0.5,ls=' ',marker='o',color='red')
        pl.axhline(0,color='blue')
        pl.ylabel('Residuals (Data - Model)')
        pl.xlabel('Phase')
        pl.grid(True)

    def __getstate__(self):
        """ Cannot pickle self.loglikelihood and self.gradient since
            these are instancemethod objects.
            See: http://mail.python.org/pipermail/python-list/2000-October/054610.html """
        result = self.__dict__.copy()
        del result['loglikelihood']
        del result['gradient']
        return result

    def __setstate__(self,state):
        self.__dict__ = state
        self.loglikelihood = self.unbinned_loglikelihood
        self.gradient = self.unbinned_gradient

class WeightedLCFitter(UnweightedLCFitter):

    def _hist_setup(self):
        """ Setup binning for a quick chi-squared fit."""
        h = hmw(self.phases,self.weights)
        nbins = 25
        if h > 100: nbins = 50
        if h > 1000: nbins = 100
        bins,counts,errors = weighted_light_curve(nbins,self.phases,self.weights)
        mask = counts > 0
        N = counts.sum()
        self.bg_level = 1-(self.weights**2).sum()/N
        x = ((bins[1:]+bins[:-1])/2)
        y    = counts / N * nbins
        yerr = errors / N * nbins
        self.chistuff = x[mask],y[mask],yerr[mask]
        # now set up binning for binned likelihood
        nbins = self.binned_bins
        bins = np.linspace(0,1,nbins+1)
        a = np.argsort(self.phases)
        self.phases = self.phases[a]
        self.weights = self.weights[a]
        self.counts_centers = []
        self.slices = []
        indices = np.arange(len(self.weights))
        for i in xrange(nbins):
            mask = (self.phases >= bins[i]) & (self.phases < bins[i+1])
            if mask.sum() > 0:
                w = self.weights[mask]
                if w.sum()==0: continue
                p = self.phases[mask]
                self.counts_centers.append((w*p).sum()/w.sum())
                self.slices.append(slice(indices[mask].min(),indices[mask].max()+1))
        self.counts_centers = np.asarray(self.counts_centers)

    def chi(self,p,*args):
        x,y,yerr = self.chistuff
        bg = self.bg_level
        if not self.template.shift_mode and np.any(p < 0):
            return 2e100*np.ones_like(x)/len(x)
        args[0].set_parameters(p)
        chi = (bg + (1-bg)*self.template(x) - y)/yerr
        return chi

    def unbinned_loglikelihood(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if (t.norm()>1) or (not t.shift_mode and np.any(p<0)):
            return 2e20
        return -np.log(1+self.weights*(t(self.phases)-1)).sum()
        #return -np.log(1+self.weights*(self.template(self.phases,suppress_bg=True)-1)).sum()

    def binned_loglikelihood(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if (t.norm()>1) or (not t.shift_mode and np.any(p<0)):
            return 2e20
        template_terms = t(self.counts_centers)-1
        phase_template_terms = np.empty_like(self.weights)
        for tt,sl in zip(template_terms,self.slices):
            phase_template_terms[sl] = tt
        return -np.log(1+self.weights*phase_template_terms).sum()

    def unbinned_gradient(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if t.norm()>1: return np.ones_like(p)*2e20
        numer = self.weights*t.gradient(self.phases)
        denom = 1+self.weights*(t(self.phases)-1)
        return -(numer/denom).sum(axis=1)

    def binned_gradient(self,p,*args):
        args[0].set_parameters(p); t = self.template
        if t.norm()>1: return np.ones_like(p)*2e20
        nump = len(p)
        template_terms = t(self.counts_centers)-1
        gradient_terms = t.gradient(self.counts_centers)
        phase_template_terms = np.empty_like(self.weights)
        phase_gradient_terms = np.empty([nump,len(self.weights)])
        # distribute the central values to the unbinned phases/weights
        for tt,gt,sl in zip(template_terms,gradient_terms.transpose(),self.slices):
            phase_template_terms[sl] = tt
            for j in xrange(nump):
                phase_gradient_terms[j,sl] = gt[j]
        numer = self.weights*phase_gradient_terms
        denom = 1+self.weights*(phase_template_terms)
        return -(numer/denom).sum(axis=1)

def hessian(m,mf,*args,**kwargs):
    """Calculate the Hessian; mf is the minimizing function, m is the model,args additional arguments for mf."""
    p = m.get_parameters().copy()
    if 'delt' in kwargs.keys():
        delta = kwargs['delt']
    else:
        # try to estimate the correct diagonal elements
        # NB -- quick and crude -- replace with real search later...
        ll0 = mf(p,m,*args)
        delta = [0.01]*len(p)
        for i in xrange(len(p)):
            for j in xrange(20):
                if j==19:
                    print 'Warning, did not converge on diagonal element %d.'%i
                incr = delta[i]*p[i]
                p[i] += incr
                delta_ll = abs(ll0-mf(p,m,*args))
                p[i] -= incr
                if delta_ll > 3:
                    delta[i] /= 2
                elif delta_ll < 0.1:
                    delta[i] *= 2
                else:
                    break 
    hessian=np.zeros([len(p),len(p)])
    for i in xrange(len(p)):
        delt = delta[i]
        for j in xrange(i,len(p)): #Second partials by finite difference; could be done analytically in a future revision
         
            xhyh,xhyl,xlyh,xlyl=p.copy(),p.copy(),p.copy(),p.copy()
            xdelt = delt if p[i] >= 0 else -delt
            ydelt = delt if p[j] >= 0 else -delt

            xhyh[i]*=(1+xdelt)
            xhyh[j]*=(1+ydelt)

            xhyl[i]*=(1+xdelt)
            xhyl[j]*=(1-ydelt)

            xlyh[i]*=(1-xdelt)
            xlyh[j]*=(1+ydelt)

            xlyl[i]*=(1-xdelt)
            xlyl[j]*=(1-ydelt)

            hessian[i][j]=hessian[j][i]=(mf(xhyh,m,*args)-mf(xhyl,m,*args)-mf(xlyh,m,*args)+mf(xlyl,m,*args))/\
                                       (p[i]*p[j]*4*delt**2)

    mf(p,m,*args) #call likelihood with original values; this resets model and any other values that might be used later
    return hessian

def get_errors(template,total,n=100):
    """ This is, I think, for making MC estimates of TOA errors."""
    from scipy.optimize import fmin
    ph0 = template.get_location()
    def logl(phi,*args):
        phases = args[0]
        template.set_overall_phase(phi%1)
        return -np.log(template(phases)).sum()
    errors = np.empty(n)
    fitvals = np.empty(n)
    errors_r = np.empty(n)
    delta = 0.01
    mean = 0
    for i in xrange(n):
        template.set_overall_phase(ph0)
        ph = template.random(total)
        results = fmin(logl,ph0,args=(ph,),full_output=1,disp=0)
        phi0,fopt = results[0],results[1]
        fitvals[i] = phi0
        mean += logl(phi0+delta,ph)-logl(phi0,ph)
        errors[i] = (logl(phi0+delta,ph)-fopt*2+logl(phi0-delta,ph))/delta**2
        my_delta = errors[i]**-0.5
        errors_r[i] = (logl(phi0+my_delta,ph)-fopt*2+logl(phi0-my_delta,ph))/my_delta**2
    print 'Mean: %.2f'%(mean/n)
    return fitvals-ph0,errors**-0.5,errors_r**-0.5

def make_err_plot(template,totals=[10,20,50,100,500],n=1000):
    import pylab as pl
    fvals = []; errs = []
    bins = np.arange(-5,5.1,0.25)
    for tot in totals:
        f,e = get_errors(template,tot,n=n)
        fvals += [f]; errs += [e]
        pl.hist(f/e,bins=np.arange(-5,5.1,0.5),histtype='step',normed=True,label='N = %d'%tot);
    g = lambda x: (np.pi*2)**-0.5*np.exp(-x**2/2)
    dom = np.linspace(-5,5,101)
    pl.plot(dom,g(dom),color='k')
    pl.legend()
    pl.axis([-5,5,0,0.5])


