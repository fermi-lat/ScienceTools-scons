"""
@brief Compute spectrum-weighted exposure correction for counts light
curves prepared by gtbin.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
import numarray as num
from FunctionWrapper import FunctionWrapper
from readXml import SourceModel
import pyLikelihood as pyLike
from FitsNTuple import FitsNTuple
import pyExposure

class ModelFunction(object):
    _funcFactory = pyLike.SourceFactory_funcFactory()
    def __init__(self, xmlFile, srcName):
        srcModel = SourceModel(xmlFile)
        spectrum = srcModel[srcName].spectrum
        self.func = self._funcFactory.create(spectrum.type)
        pars = spectrum.parameters
        for name in pars.keys():
            self.func.setParam(name, pars[name].value)
    def __call__(self, ee):
        foo = FunctionWrapper(lambda x : self.func.value(pyLike.dArg(x)))
        return foo(ee)

class Exposure(object):
    def __init__(self, lc_file, coords=None, ft2file='DC2_FT2_v2.fits',
                 energies=None, irfs='DC2'):
        self.lc = FitsNTuple(lc_file, 'RATE')
        cuts = pyExposure.Cuts(lc_file, 'RATE', False)
        emin, emax = 20, 2e5
        for i in range(cuts.size()):
            my_cut = cuts.getCut(i)
            if my_cut.type() == 'SkyCone':
                my_cut = pyExposure.Cuts_castAsSkyConeCut(my_cut)
                self.ra = my_cut.ra()
                self.dec = my_cut.dec()
            if my_cut.type() == 'range':
                my_cut = pyExposure.Cuts_castAsRangeCut(my_cut)
                if my_cut.colname() == 'ENERGY':
                    emin = my_cut.minVal()
                    emax = my_cut.maxVal()
        energies = log_array(21, emin, emax)
        times = list(self.lc.TIME - self.lc.TIMEDEL/2.)
        times.append(self.lc.TIME[-1] + self.lc.TIMEDEL[-1]/2.)
        self.exposure = pyExposure.Exposure(ft2file, times, energies,
                                            self.ra, self.dec, irfs)
    def __getattr__(self, attrname):
        return getattr(self.exposure, attrname)
    def __call__(self, time, energy):
        return self.exposure.value(time, energy)
    def weightedAvgs(self, dnde):
        energies = self.energies()
        dnde_vals = dnde(energies)
        expvals = self.values()
        avg_exps = []
        dnde_avg = 0
        for k in range(len(energies) - 1):
            dnde_avg += ((dnde_vals[k+1]+dnde_vals[k])
                         *(energies[k+1]-energies[k])/2.)
        for exprow in expvals:
            avg_exps.append(0)
            ff = dnde_vals*num.array(exprow)
            for k in range(len(energies) - 1):
                avg_exps[-1] += (ff[k+1]+ff[k])*(energies[k+1]-energies[k])/2.
        return num.array(avg_exps)/dnde_avg

if __name__ == '__main__':
    import hippoplotter as plot
    from log_array import log_array
    
    ee = log_array(100, 20, 2e5)
    bpl = ModelFunction('solar_flare_bpl_model.xml', 'Solar Flare')
    plot.scatter(ee, bpl(ee), xlog=1, ylog=1, pointRep='Line')

    exposure = Exposure('flare_lc.fits')

    my_exp = exposure.weightedAvgs(bpl)
    times = exposure.lc.TIME
    plot.xyplot(times - times[0]-644, exposure.lc.COUNTS/(my_exp+1),
                xerr=exposure.lc.TIMEDEL/2.,
                yerr=num.sqrt(exposure.lc.COUNTS)/(my_exp+1), ylog=1,
                pointRep='Column')
