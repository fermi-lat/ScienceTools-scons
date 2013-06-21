"""
Description here

$Header$

"""

from . import seedcheck

class PGWSeedCheck(seedcheck.SeedCheck):
    require='seedcheck_PGW.zip'
    def setup(self, **kw):
        self.plotfolder = self.seedname= 'seedcheck_PGW'
        self.spectral_type = 'power law'
        self.load()