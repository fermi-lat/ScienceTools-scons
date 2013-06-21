"""
Description here

$Header$

"""

from . import fluxcorr

class FluxCorrIso(fluxcorr.FluxCorr):

    require = 'fluxcorriso.zip'
    def setup(self, **kw):
        super(FluxCorrIso,self).setup(source_name='fluxcorriso', **kw)
        self.title='Source-isotropic diffuse flux dependence'
        self.diffuse_name='Isotropic'
        
        self.plotfolder='fluxcorriso'