"""
Manage the sky model for the UW all-sky pipeline
$Header$

"""
import os, pickle, glob, types
import numpy as np
from skymaps import SkyDir, Band
from uw.utilities import keyword_options, makerec, xml_parsers
from . import sources

class SkyModel(object):
    """
    Define a model of the gamma-ray sky, including point, extended, and global sources.
    Input is currently only from a folder containing all of the ROI pickles, in the format generated by the pipeline.
    Thus pipeline is completely iterative.
    
    Implement methods to create ROI for pointlike, used by pipeline.
    """
    
    defaults= (
        ('extended_catalog_name', None,  'name of folder with extended info'),
        ('alias', dict(), 'dictionary of aliases to use for lookup'),
        ('diffuse', ('ring_24month_P74_v1.fits', 'isotrop_21month_v2.txt'), 'pair of diffuse file names: use to locked'),
        ('auxcat', None, 'name of auxilliary catalog of point sources to append or names to remove',),
        ('update_positions', None, 'set to minimum ts  update positions if localization information found in the database'),
        ('quiet',  False,  'make quiet' ),
    )
    
    @keyword_options.decorate(defaults)
    def __init__(self, folder=None,  **kwargs):
        """
        folder : string or None
            name of folder to find all files defining the sky model, including:
             a subfolder 'pickle' with files *.pickle describing each ROI, partitioned as a HEALpix set.
        """
        keyword_options.process(self, kwargs)

        if folder is None:
            folder = 'uw%02d' % int(open('version.txt').read())
        self.folder = os.path.expandvars(folder)
        if not os.path.exists(self.folder):
            raise Exception('sky model folder %s not found' % folder)
        self._setup_extended()
        self._load_sources()
        if self.diffuse is not None:
            assert len(self.diffuse)==2, 'expect 2 diffuse names'
            x = map(lambda f:os.path.expandvars(os.path.join('$FERMI','diffuse',f)), self.diffuse)
            sources.Diffuse(x[0], True)
            sources.Isotropic(x[1], True)
            
        self.load_auxcat()
        
    def __str__(self):
        return 'SkyModel %s' %self.folder\
                +'\n\t\tdiffuse: %s' %list(self.diffuse)\
                +'\n\t\textended: %s' %self.extended_catalog_name 
                
    def load_auxcat(self):
        """ modify the list of pointsources from entries in the auxcat: for now:
            * add it not there
            * move there, and new ra,dec
            * remove if ra<0
        
        """
        if self.auxcat is None or self.auxcat=='': return
        cat = self.auxcat 
        if not os.path.exists(cat):
            cat = os.expandvars(os.path.join('$FERMI','catalog', cat))
        if not os.path.exists(cat):
            raise Exception('auxilliary catalog %s not found locally or in $FERMI/catalog'%self.auxcat)
        ss = makerec.load(cat)
        names = [s.name for s in self.point_sources]
        toremove=[]
        print 'process auxcat %s' %cat
        for s in ss:
            sname = s.name.replace('_',' ')
            if sname  not in names: 
                skydir=SkyDir(float(s.ra), float(s.dec))
                index=self.hpindex(skydir)
                self.point_sources.append(sources.PointSource(name=s.name, skydir=skydir, index=index))
                print '\tadded new source %s at ROI %d' % (s.name, index)
            else: 
                print '\t source %s is in the model:' %sname, # will remove if ra<0' % sname
                ps = self.point_sources[names.index(sname)]
                if float(s.ra)<=0: 
                    toremove.append(ps)
                    print ' removed.'
                else:
                    newskydir=SkyDir(float(s.ra),float(s.dec))
                    print 'moved from %s to %s' % (ps.skydir, newskydir)
                    ps.skydir=newskydir
        for ps in toremove:
            self.point_sources.remove(ps)
            
    def _setup_extended(self):
        if not self.extended_catalog_name: return None
        extended_catalog_name = \
            os.path.expandvars(os.path.join('$FERMI','catalog',self.extended_catalog_name))
        if not os.path.exists(extended_catalog_name):
            raise Exception('extended source folder "%s" not found' % extended_catalog_name)
        self.extended_catalog= sources.ExtendedCatalog(extended_catalog_name, alias=self.alias)
        #print 'Loaded extended catalog %s' % self.extended_catalog_name
        
    #def get_extended_sources(self,skydir, radius):
    #    """ add any extended sources with center within the outer radius.
    #        set parameters free if the center is inside the HEALpix region
    #    """
    #    if self.extended_catalog is None: return []
    #    ret =self.extended_catalog.get_sources(skydir, radius)
    #    return ret 
    #          
    def _load_sources(self):
        """
        run through the pickled roi dictionaries, create lists of point and extended sources
        assume that the number of such corresponds to a HEALpix partition of the sky
        """
        self.point_sources= []
        files = glob.glob(os.path.join(self.folder, 'pickle', '*.pickle'))
        files.sort()
        self.nside = int(np.sqrt(len(files)/12))
        if len(files) != 12*self.nside**2:
            msg = 'Number of pickled ROI files, %d, found in folder %s, not consistent with HEALpix' \
                % (len(files),os.path.join(self.folder, 'pickle'))
            raise Exception(msg)
        self.global_sources = []  # allocate list to index parameters for global sources
        self.extended_sources=[]  # list of unique extended sources
        self.changed=set() # to keep track of extended models that are different from catalog
        moved=0
        for i,file in enumerate(files):
            p = pickle.load(open(file))
            index = int(os.path.splitext(file)[0][-4:])
            assert i==index, 'logic error: file name %s inconsistent with expected index %d' % (file, i)
            roi_sources = p['sources']
            for key,item in roi_sources.items():
                skydir = item['skydir']
                if self.update_positions is not None:
                    ellipse = item.get('ellipse', None)
                    ts = item['ts']
                    if ellipse is not None and not np.any(np.isnan(ellipse)) :
                        fit_ra, fit_dec, a, b, ang, qual, delta_ts = ellipse
                        if qual<5 and a < 0.2 and \
                                ts>self.update_positions and delta_ts>1:
                            skydir = SkyDir(float(fit_ra),float(fit_dec))
                            moved +=1
                ps = sources.PointSource(name=key, 
                    skydir=skydir, model= item['model'],
                    ts=item['ts'],band_ts=item['band_ts'], index=index)
                sources.validate(ps,self.nside) 
                self.point_sources.append( ps)
            # make a list of extended sources used in the model   
            t = []
            names = p.get('diffuse_names', self.diffuse )
            for name, model in zip(names, p['diffuse']):
                if len(t)<2: # always assume first two are global ????
                    if model.p[0]<-2:
                        model.p[0]=-2
                        #print 'SkyModel warning: reset norm to 1e-2 for %s' % name
                    t.append(sources.GlobalSource(name=name, model=model, skydir=None, index=index))
                else:
                    es = self.extended_catalog.lookup(name)
                    if es is None:
                        raise Exception( 'Extended source %s not found in extended catalog' %name)
                    if self.hpindex(es.skydir)!=index: continue
                    
                    if es.model.name!=model.name:
                        if name not in self.changed:
                            print 'SkyModel warning: catalog model %s changed from %s for %s'% (es.model.name, model.name, name)
                        self.changed.add(name)
                    else:
                        es.model=model #update with fit values
                    sources.validate(es,self.nside)
                    self.extended_sources.append(es)
            self.global_sources.append(t)
        # check for new extended sources not yet in model
        self._check_for_extended()
        if self.update_positions and moved>0:
            print 'updated positions of %d sources' % moved
 
    def _check_for_extended(self):
        for name in self.extended_catalog.names:
            if name.replace(' ','') not in [g.name.replace(' ','') for g in self.extended_sources]:
                print 'extended source %s added to model' % name
                self.extended_sources.append(self.extended_catalog.lookup(name))
    
    #def skydir(self, index):
    #    return Band(self.nside).dir(index)
    def hpindex(self, skydir):
        return Band(self.nside).index(skydir)
    
    def _select_and_freeze(self, sources, src_sel):
        """ 
        sources : list of Source objects
        src_sel : selection object
        -> list of selected sources selected by src_sel.include, 
            with some frozen according to src_sel.frozen
            order so the free are first
        """
        inroi = filter(src_sel.include, sources)
        for s in inroi:
            #s.freeze(src_sel.frozen(s))
            s.model.free[:] = False if src_sel.frozen(s) else s.free
        return filter(src_sel.free,inroi) + filter(src_sel.frozen, inroi)
    
    def get_point_sources(self, src_sel):
        """
        return a list of PointSource objects appropriate for the ROI
        """
        return self._select_and_freeze(self.point_sources, src_sel)
        
    def get_diffuse_sources(self, src_sel):
        """return diffuse, global and extended sources defined by src_sel
            always the global diffuse, and perhaps local extended sources.
            For the latter, make parameters free if not selected by src_sel.frozen
            TODO: feature to override free selection for globals.
        """
        globals = self.global_sources[self.hpindex(src_sel.skydir())]
        for s in globals:
            dfile = os.path.expandvars(os.path.join('$FERMI','diffuse', s.name))
            assert os.path.exists(dfile), 'file %s not found' % dfile
            ext = os.path.splitext(dfile)[-1]
            if ext=='.txt':
                s.dmodel = [sources.Isotropic(dfile).instance()]
                s.name = os.path.split(sources.Isotropic._dfile)[-1]
            elif ext=='.fits' or ext=='.fit':
                s.dmodel = [sources.Diffuse(dfile).instance()]
                s.name = os.path.split(sources.Diffuse._dfile)[-1]
            else:
                raise Exception('unrecognized diffuse file extention %s' % dfile)
            s.smodel=s.model
        extended = self._select_and_freeze(self.extended_sources, src_sel)
        for s in extended: # this seems redundant, but was necessary
            s.model.free[:] = False if src_sel.frozen(s) else s.free
            sources.validate(s,self.nside)
            s.smodel = s.model
            
        return globals, extended

    def toXML(self,filename):
        stacks= [
            xml_parsers.unparse_diffuse_sources(self.extended_sources,True,False,filename),
            xml_parsers.unparse_point_sources(self.point_sources,strict=True),
        ]
        xml_parsers.writeXML(stacks, filename)

        
class SourceSelector(object):
    """ Manage inclusion of sources in an ROI."""
    
    defaults = (
        ('max_radius',10,'Maximum radius (deg.) within which sources will be selected.'),
        ('free_radius',3,'Radius (deg.) in which sources will have free parameters'),
    )
    iteration =0
    @keyword_options.decorate(defaults)
    def __init__(self, skydir, **kwargs):
        self.mskydir = skydir
        keyword_options.process(self,kwargs)
        self.name='ROI#04d' % iteration
        self.iteration += 1
    
    def name(self):
        return 'ROI#04d' % iteration

    def near(self,source, radius):
        return source.skydir.difference(self.mskydir)< np.radians(radius)

    def include(self,source):
        """ source -- an instance of Source """
        return self.near(source, self.max_radius)

    def free(self,source):
        """ source -- an instance of Source """
        return self.near(source, self.free_radius)

    def frozen(self,source): return not self.free(source)

    def skydir(self): return self.mskydir
        
class HEALPixSourceSelector(SourceSelector):
    """ Manage inclusion of sources in an ROI based on HEALPix.
    Overrides the free method to define HEALpix-based free regions
    """

    nside=12 # default, override externally
    @keyword_options.decorate(SourceSelector.defaults)
    def __init__(self, index, **kwargs):
        """ index : int
                HEALpix index for the ROI (RING)
            nside : int
                HEALPix nside parameter
        """
        keyword_options.process(self,kwargs)
        self.myindex = index
        self.mskydir =  self.skydir(index)

    def name(self):
        return 'HP%02d_%04d' % (self.nside, self.myindex)

    def skydir(self, index=None):
        return Band(self.nside).dir(index) if index is not None else self.mskydir
        
    def index(self, skydir):
        return Band(self.nside).index(skydir)
    

    def free(self,source):
        """
        source : instance of skymodel.Source
        -> bool, if this source in in the region where fit parameters are free
        """
        return self.index(source.skydir) == self.myindex
        
