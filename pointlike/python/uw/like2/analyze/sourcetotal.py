"""
Description here

$Header$

"""

from . import roi_info

class SourceTotal(roi_info.ROIinfo):
    def setup(self, **kw):
        super(SourceTotal, self).setup(**kw)
        self.plotfolder='sourcetotal'
        self.source_name='sources'
        self.title='Sources'
        self.funcs = [self.counts_map]
        self.fnames=['source_counts']

    def all_plots(self, **kwargs):
        """ Counts for all sources, per RIO"""
    
        self.runfigures([self.counts_map], ['source_counts'], **kwargs)