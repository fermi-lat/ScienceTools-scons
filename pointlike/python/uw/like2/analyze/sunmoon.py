"""
Plots of the SunMoon diffuse component

$Header$

"""

import numpy as np

from . import roi_info

class SunMoon(roi_info.ROIinfo):
    """SunMoon plots
    """
    def setup(self, **kwargs):
        super(SunMoon, self).setup(**kwargs)
        self.plotfolder='sunmoon'
        self.source_name='SunMoon'
        self.title='Sun/Moon'
        t = np.any([x is not None for x in self.diffuse_models(self.source_name)])
        assert t, 'No sun-moon component in this sky model'
        self.default_plots()
        self.plots_kw=dict(ecliptic=True)
        
    def all_plots(self):
        super(SunMoon, self).all_plots()