#!/usr/bin/env python
"""
Test code for optimizer.py module

@author J. Chiang
$Header$
"""
import optimizers, math, numarray
vector = optimizers.DoubleVector
vecvec = optimizers.DoubleVectorVector

def test_Mcmc():
    print "*** Testing Mcmc ***"
    
    rosen = optimizers.Rosen()
    startingValues = vector([2., 2.])

    rosen.setParamValues(startingValues)
    rosen.setParamBounds("x", -10., 10.)
    rosen.setParamBounds("y", -10., 10.)

    verbose = 0
    minuitObj = optimizers.Minuit(rosen)
    minuitObj.find_min(verbose, 1e-4)
    #
    # use uncertainties from Minuit to set the transition widths
    #
    sigmas = minuitObj.getUncertainty()
    scale = 1
    widths = vector( map(lambda x:x*scale, sigmas) )

    mcmcObj = optimizers.Mcmc(rosen)
    mcmcObj.setTransitionWidths(widths)

    samples = vecvec()
    nsamp = 100000
    #
    # Do a burn in...
    #
    mcmcObj.generateSamples(samples, nsamp)
    #
    # then the real thing...
    #
    nsamp = 100000
    mcmcObj.generateSamples(samples, nsamp)
    x = list(samples)
    samps = []
    for j in range(len(x[0])):
        samps.append( numarray.array(map(lambda i:x[i][j], range(len(x)))) )

    print "MCMC results for ", nsamp, " trials:"
    for j in range(len(samples[0])):
        x = samps[j]
        mc_avg = numarray.sum(x)/len(x)
        mc2_avg = numarray.sum(x*x)/len(x)
        print "Parameter %i: %e +/- %e" % (j, mc_avg, 
                                          math.sqrt(mc2_avg - mc_avg*mc_avg))

    print "*** Mcmc tests completed ***\n"
    return samps
    
def test_optimizers():
    rosen = optimizers.Rosen()
    startingValues = vector([2., 2.])

    print "*** Testing Lbfgs ***"
    rosen.setParamValues(startingValues)
    rosen.setParamBounds("x", -5., 10.)
    rosen.setParamBounds("y", -4., 10.)

    verbose = 1
    lbfgsObj = optimizers.Lbfgs(rosen)
    lbfgsObj.setMaxVarMetCorr(12)
    lbfgsObj.setPgtol(1e-7)
    lbfgsObj.find_min(verbose, 1e-6)
    print "*** Lbfgs tests completed ***\n"
    
    print "*** Testing Minuit ***"
    verbose = 3
    rosen.setParamValues(startingValues)
    rosen.setParamBounds("x", -10., 10.)
    rosen.setParamBounds("y", -10., 10.)
    
    minuitObj = optimizers.Minuit(rosen)
    minuitObj.find_min(verbose, 1e-4)
    sigmas = minuitObj.getUncertainty()
    print "*** Minuit tests completed ***\n"
    
    print "*** Testing Drmngb ***"
    rosen.setParamValues(startingValues)
    rosen.setParamBounds("x", -10., 10.)
    rosen.setParamBounds("y", -10., 10.)
    
    drmngbObj = optimizers.Drmngb(rosen)
    drmngbObj.find_min(verbose, 1e-4)
    print "Drmngb exit code: ", drmngbObj.getRetCode()
    sigmas = drmngbObj.getUncertainty()
    print "Uncertainties: "
    i = 0
    for sigma in sigmas:
        print i, "  ", sigma
        i += 1
    print "*** Drmngb tests completed ***\n"

if __name__ == "__main__":
    test_optimizers()
    samples = test_Mcmc()
#    import hippoplot
#    plotter = hippoplot.hippoplot()
#    nt = plotter.newTuple(samples, ("x", "y"))
#    plotter.Histogram(nt, "x")
#    plotter.Histogram(nt, "y")
#    plotter.XYHist(nt, "x", "y")
