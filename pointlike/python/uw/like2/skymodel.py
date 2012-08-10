"""
Manage the sky model for the UW all-sky pipeline
$Header$

"""
import os, pickle, glob, types, collections
import cPickle as pickle
from xml import sax
import numpy as np
from skymaps import SkyDir, Band
from uw.utilities import keyword_options, makerec, xml_parsers
#  this is below: only needed when want to create XML
#from uw.utilities import  xml_parsers
from ..like import Models, pointspec_helpers

from . import sources, catrec

class SkyModel(object):
    """
    Define a model of the gamma-ray sky, including point, extended, and global sources.
    Input is currently only from a folder containing all of the ROI pickles, in the format generated by the pipeline.
    Thus pipeline is completely iterative.
    
    Implement methods to create ROI for pointlike, used by pipeline.
    """
    
    defaults= (
        ('extended_catalog_name', None,  'name of folder with extended info\n'
                                         'if None, look it up in the config.txt file\n'
                                         'if "ignore", create model without extended sources'),
        ('diffuse', None,   'set of diffuse file names; if None, expect config to have'),
        ('auxcat', None, 'name of auxilliary catalog of point sources to append or names to remove',),
        ('newmodel', None, 'if not None, a string to eval\ndefault new model to apply to appended sources'),
        ('update_positions', None, 'set to minimum ts  update positions if localization information found in the database'),
        ('filter',   lambda s: True,   'selection filter: see examples at the end.'), 
        ('global_check', lambda s: None, 'check global sources: can modify parameters'),
        ('closeness_tolerance', 0., 'if>0, check each point source for being too close to another, print warning'),
        ('quiet',  False,  'make quiet' ),
        ('force_spatial_map', True, 'Force the use of a SpatialMap for extended sources'),
    )
    
    @keyword_options.decorate(defaults)
    def __init__(self, folder,  **kwargs):
        """
        folder: string
            name of folder to find all files defining the sky model, including:
             a subfolder 'pickle' with files *.pickle describing each ROI, partitioned as a HEALpix set.
             a file 'config.txt' written by the pipeline
        """
        keyword_options.process(self, kwargs)
        #if self.free_index is not None: 
        #    print 'will free photon indices for ts>%d' % self.free_index

        self.folder = os.path.expandvars(folder)
        if not os.path.exists(self.folder):
            raise Exception('sky model folder %s not found' % folder)
        self.get_config()
        self._setup_extended()
        if self.diffuse is not None:
            """ make a dictionary of (file, object) tuples with key the first part of the diffuse name"""
            assert len(self.diffuse)<4, 'expect 2 or 3 diffuse names'
        else:
            t = self.config['diffuse']
            self.diffuse = eval(t) if type(t)==types.StringType else t 
            assert self.diffuse is not None, 'SkyModel: no diffuse in config'
        self.diffuse_dict = sources.DiffuseDict(self.diffuse)
        self._load_sources()
        self.load_auxcat()

    def __str__(self):
        return 'SkyModel %s' %self.folder\
                +'\n\t\tdiffuse: %s' %list(self.diffuse)\
                +'\n\t\textended: %s' %self.extended_catalog_name 
     
    def get_config(self, fn = 'config.txt'):
        """ parse the items in the configuration file into a dictionary
        """
        self.config={}
        fn = os.path.join(self.folder,fn)
        if not os.path.exists(fn): return
        txt = open(fn).read()
        if txt[0]=='{':
            # new format: just a dumped dict
            self.config = eval(txt)        
        # old format: more readable
        for line in txt:
            item = line.split(':')
            if len(item)>1:
                self.config[item[0].strip()]=item[1].strip()
 
    def load_auxcat(self):
        """ modify the list of pointsources from entries in the auxcat: for now:
            * add it not there
            * move there, at new ra,dec
            * remove if ra<0
        
        """
        if self.auxcat is None or self.auxcat=='': 
            return
        cat = self.auxcat 
        if not os.path.exists(cat):
            cat = os.path.expandvars(os.path.join('$FERMI','catalog', cat))
        if not os.path.exists(cat):
            raise Exception('auxilliary catalog %s not found locally or in $FERMI/catalog'%self.auxcat)
        ss = makerec.load(cat)
        names = [s.name for s in self.point_sources]
        toremove=[]
        print 'process auxcat %s' %cat
        for s in ss:
            if not s.name.startswith('SEED'): # allow underscores
                sname = s.name.replace('_',' ') 
            else: sname=s.name
            if sname  not in names: 
                skydir=SkyDir(float(s.ra), float(s.dec))
                index=self.hpindex(skydir)
                self.point_sources.append(sources.PointSource(name=s.name, skydir=skydir, index=index, model=self.newmodel))
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
        """ a little confusion: 'None' means that, but None means use the config file"""
        if self.extended_catalog_name is None:
            t=self.config.get('extended')
            if t[0]=='"' or t[0]=="'": t = eval(t)
            self.extended_catalog_name=t
        if not self.extended_catalog_name or self.extended_catalog_name=='None' or self.extended_catalog_name=='ignore':
            self.extended_catalog = None
            return
        extended_catalog_name = \
            os.path.expandvars(os.path.join('$FERMI','catalog',self.extended_catalog_name))
        if not os.path.exists(extended_catalog_name):
            raise Exception('extended source folder "%s" not found' % extended_catalog_name)
        self.extended_catalog= sources.ExtendedCatalog(extended_catalog_name, force_map=self.force_spatial_map)
        #print 'Loaded extended catalog %s' % self.extended_catalog_name
        
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
        nfreed = 0
        self.tagged=set()
        source_names =[]
        for i,file in enumerate(files):
            p = pickle.load(open(file))
            index = int(os.path.splitext(file)[0][-4:])
            assert i==index, 'logic error: file name %s inconsistent with expected index %d' % (file, i)
            roi_sources = p.get('sources',  {}) # don't know why this needed
            extended_names = {} if (self.__dict__.get('extended_catalog') is None) else self.extended_catalog.names
            for key,item in roi_sources.items():
                if key in extended_names: continue
                if key in source_names:
                    print 'SkyModel warning: source with name %s in ROI %d duplicates previous entry: ignored'%(key, i)
                    continue
                source_names.append(key)
                skydir = item['skydir']
                if self.update_positions is not None:
                    ellipse = item.get('ellipse', None)
                    ts = item['ts']
                    if ellipse is not None and not np.any(np.isnan(ellipse)) :
                        fit_ra, fit_dec, a, b, ang, qual, delta_ts = ellipse
                        if qual<5 and a < 0.2 and \
                                ts>self.update_positions and delta_ts>0.1:
                            skydir = SkyDir(float(fit_ra),float(fit_dec))
                            moved +=1
                            self.tagged.add(i)
                
                ps = sources.PointSource(name=key, #name=self.rename_source(key), 
                    skydir=skydir, model= sources.convert_model(item['model']),
                    ts=item['ts'],band_ts=item['band_ts'], index=index)
                #if self.free_index is not None and not ps.free[1] and ps.ts>self.free_index:
                #    ps.free[1]=True
                #    nfreed +=1
                #    if nfreed<10: print 'Freed photon index for source %s'%ps.name
                #    elif nfreed==10: print ' [...]'
                if True: # do not need? sources.validate(ps,self.nside, self.filter):
                    self._check_position(ps) # check that it is not coincident with previous source(warning for now?)
                    self.point_sources.append( ps)
            # make a list of extended sources used in the model   
            t = []
            names = p.get('diffuse_names', self.diffuse )
            for name, oldmodel in zip(names, p['diffuse']):
                model = sources.convert_model(oldmodel) # convert from old Model version if necessary 
                key = name.split('_')[0]
                if key in self.diffuse_dict:
                    gs = sources.GlobalSource(name=name, model=model, skydir=None, index=index)
                    self.global_check(gs)
                    t.append(gs)
                elif  self.extended_catalog_name=='ignore': 
                    continue
                else:
                    es = self.extended_catalog.lookup(name) if self.extended_catalog is not None else None
                    if es is None:
                        raise Exception( 'Extended source %s not found in extended catalog' %name)
                        print 'SkyModel warning: Extended source %s not found in extended catalog, removing' %name
                        
                    if self.hpindex(es.skydir)!=index: continue
                    
                    if es.model.name!=model.name:
                        if name not in self.changed:
                            print 'SkyModel warning: catalog model %s changed from %s for %s'% (es.model.name, model.name, name)
                        self.changed.add(name)
                    else:
                        es.model=model #update with fit values
                    if sources.validate(es,self.nside, self.filter): #lambda x: True): 
                        self.extended_sources.append(es)
            self.global_sources.append(t)
        # check for new extended sources not yet in model
        self._check_for_extended()
        if self.update_positions and moved>0:
            print 'updated positions of %d sources, healpix ids in tagged' % moved
 
    def _check_for_extended(self):
        if self.__dict__.get('extended_catalog') is None: return
        for i,name in enumerate(self.extended_catalog.names):
            if name.replace(' ','') not in [g.name.replace(' ','') for g in self.extended_sources]:
                es = self.extended_catalog.sources[i]
                print 'extended source %s [%d] added to model' % (name, self.hpindex(es.skydir))
                t = self.extended_catalog.lookup(name)
                assert t is not None, 'logic error?'
                self.extended_sources.append(t)

    def _check_position(self, ps):
        if self.closeness_tolerance<0.: return
        tol = np.radians(self.closeness_tolerance)
        func = ps.skydir.difference
        for s in self.point_sources:
            delta=func(s.skydir)
            if delta<tol:
                print  'SkyModel warning: appended source %s %.2f %.2f is %.2f deg (<%.2f) from %s (%d)'\
                    %(ps.name, ps.skydir.ra(), ps.skydir.dec(), np.degrees(delta), self.closeness_tolerance, s.name, s.index)
        
    def skydir(self, index):
        return Band(self.nside).dir(index)
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
        def copy_source(s): 
            return s.copy()
        inroi = filter(src_sel.include, sources)
        for s in inroi:
            #s.freeze(src_sel.frozen(s))
            s.model.free[:] = False if src_sel.frozen(s) else s.free[:]
        return map(copy_source, filter(src_sel.free,inroi)) + filter(src_sel.frozen, inroi)
    
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
        def iterable_check(x):
            return x if hasattr(x,'__iter__') else (x,x)

        for s in globals:
            prefix = s.name.split('_')[0]
            s.name, s.dmodel = prefix, self.diffuse_dict[prefix]
            s.smodel = s.model

        extended = self._select_and_freeze(self.extended_sources, src_sel)
        for s in extended: # this seems redundant, but was necessary
            s.model.free[:] = False if src_sel.frozen(s) else s.free[:]
            sources.validate(s,self.nside, None)
            s.smodel = s.model
            
        return globals, extended

    def toXML(self,filename, ts_min=None, title=None, source_filter=lambda x:True, strict=False, gtlike = False):
        """ generate a file with the XML version of the sources in the model
        source_filter:  a function to apply
        """
        catrec = self.source_rec()
        point_sources = self.point_sources if ts_min is None else filter(lambda s: s.ts>ts_min, self.point_sources)
        print 'SkyModel: writing XML representations of %d point sources %s and %d extended sources to %s' \
            %(len(point_sources), ('' if ts_min is None else '(with TS>%.f)'%ts_min), len(self.extended_sources), filename)
        from uw.utilities import  xml_parsers # isolate this import, which brings in full pointlike
        def pointsource_properties(s):
            if hasattr(s.model,'e0'): e0 = s.model.e0
            else: e0 = 10**s.model.getp(3)
            return 'Pivot_Energy="%.1f" TS="%.1f"' % (e0, s.ts)
        stacks= [
            xml_parsers.unparse_diffuse_sources(self.extended_sources,convert_extended=True,filename=filename),
            xml_parsers.unparse_point_sources(point_sources,strict=strict, properties=pointsource_properties),
        ]
        gs_xml = self._global_sources_to_xml(filename)
        with open(filename,'wb') as f:
            if not gtlike:
                f.write('<skymodel>\n')
            f.write('<source_library title="%s">'% title)
            for stack in stacks:
                for elem in stack:
                    f.write(elem)
            f.write('\n</source_library>')
            if not gtlike:
                f.write('\n'.join(['\n<roi_info nside="{0}">'.format(self.nside),
                                   gs_xml,
                                   '</roi_info>']))
                f.write('\n</skymodel>')

    def _global_sources_to_xml(self,filename):
        stacks = []
        bad =0
        for i in xrange(1728):
            stack = xml_parsers.Stack()
            s1 = '<roi index="{0}">'.format(i)
            s2 = '</roi>'
            globals = self.global_sources[i]
            for s in globals:
                prefix = s.name.split('_')[0]
                s.name, s.dmodel = prefix, self.diffuse_dict[prefix]
                s.smodel = s.model
            try:
                diffuse_xml = xml_parsers.unparse_diffuse_sources(globals,filename=filename)
            except:
                bad +=1
                continue
            for x in diffuse_xml:
                x = '\t'+x
            diffuse_xml.appendleft(s1)
            diffuse_xml.append(s2)
            stacks+=['\n'.join(diffuse_xml)]
        if bad>0:
            print 'Failed to convert %d ROIs' % bad
        return '\n'.join(stacks)

    def write_reg_file(self, filename, ts_min=None, color='green'):
        """ generate a 'reg' file from the catalog, write to filename
        """
        catrec = self.source_rec()
        have_ellipse = 'Conf_95_SemiMajor' in catrec.dtype.names #not relevant: a TODO
        out = open(filename, 'w')
        print >>out, "# Region file format: DS9 version 4.0 global color=%s" % color
        rec = catrec if ts_min is  None else catrec[catrec.ts>ts_min]
        for s in rec:
            if have_ellipse:
                print >>out, "fk5; ellipse(%.4f, %.4f, %.4f, %.4f, %.4f) #text={%s}" % \
                                (s.ra,s,dec,
                                  s.Conf_95_SemiMinor,Conf_95_SemiMajor,Conf_95_PosAng,
                                  s.name)
            else:
                print >> out, "fk5; point(%.4f, %.4f) # point=cross text={%s}" %\
                                (s.ra, s.dec, s.name)
        out.close()

    def _load_recfiles(self, reload=False):
        """ make a cache of the recarray summary """
        recfiles = map(lambda name: os.path.join(self.folder, '%s.rec'%name) , ('rois','sources'))
        if reload or not os.path.exists(recfiles[0]):
            catrec.create_catalog(self.folder, save_local=True, minflux=1e-18, ts_min=5)
        self.rois,self.sources = map( lambda f: pickle.load(open(f)), recfiles)
        print 'loaded %d rois, %d sources' % (len(self.rois), len(self.sources))

    def roi_rec(self, reload=False):
        self._load_recfiles(reload)
        return self.rois
    def source_rec(self, reload=False):
        self._load_recfiles(reload)
        return self.sources
    def find_source(self, name):
        """ return local source reference by name, or None """
        t = filter( lambda x: x.name==name, self.point_sources+self.extended_sources)
        return t[0] if len(t)==1 else None

class XMLSkyModel(SkyModel):
    """A SkyModel initialized from a stored XML representation."""

    defaults= (
        ('auxcat', None, 'name of auxilliary catalog of point sources to append or names to remove',),
        ('newmodel', None, 'if not None, a string to eval\ndefault new model to apply to appended sources'),
        ('filter',   lambda s: True,   'selection filter: see examples at the end.'), 
        ('global_check', lambda s: None, 'check global sources: can modify parameters'),
        ('closeness_tolerance', 0., 'if>0, check each point source for being too close to another, print warning'),
        ('quiet',  False,  'make quiet' ),
    )
    @keyword_options.decorate(defaults)
    def __init__(self,xml,**kwargs):
        keyword_options.process(self,kwargs)
        self._parse_xml(xml)
        self.nside = self.handler.nside
        self._load_sources()
        self._load_globals()
        #self.nside = int(np.sqrt(len(self.global_sources)/12))
        self.load_auxcat()

    def _parse_xml(self,xml):
        self.parser = sax.make_parser()
        self.handler = SkyModelHandler()
        self.parser.setContentHandler(self.handler)
        self.parser.parse(xml)

    def _parse_global_sources(self):
        pass

    def _load_sources(self):
        self.point_sources = xml_parsers.parse_point_sources(self.handler,SkyDir(0,0),180)
        #parse diffuse sources checks the sources list, so won't grab the globals
        self.extended_sources = xml_parsers.parse_diffuse_sources(self.handler)

    def _load_globals(self):
        gds = pointspec_helpers.get_diffuse_source
        xtm = xml_parsers.XML_to_Model()
        self.global_sources = []
        self.diffuse = []
        self.diffuse_dict = {}
        for roi in self.handler.rois:
            index = int(roi['index'])
            gss = []
            for source in roi.children:
                spatial = source.getChild("spatialModel")
                spectral = source.getChild("spectrum")
                name = str(source['name'])
                if spatial['type'] == 'ConstantValue':
                    if spectral['type'] == 'FileFunction':
                        diffdir,fname = os.path.split(str(os.path.expandvars(spectral['file'])))
                        mo = xtm.get_model(spectral,name)
                        if not self.diffuse_dict.has_key(name):
                            self.diffuse_dict[name] = [gds('ConstantValue',None,mo,fname,name,diffdir=diffdir)]
                            self.diffuse += [fname]
                        gss += [sources.GlobalSource(model=mo,index=index,skydir=None,name=name)]
                    elif (spectral['type'] == 'PowerLaw' ) or (spectral['type'] == 'PowerLaw2'):
                        mo = xtm.get_model(spectral,name)
                        if not self.diffuse_dict.has_key(name):
                            self.diffuse_dict[name] = [gds('ConstantValue',None,mo,None,name)]
                        gss += [sources.GlobalSource(model=mo,index=index,skydir=None,name=name)]
                    elif spectral['type']=='CompositeSpectrum':
                        dss = []
                        fnames = []
                        for i,sp in enumerate(spectral.children):
                            diffdir,fname = os.path.split(str(os.path.expandvars(sp['file'])))
                            dss+=[gds('ConstantValue',None,mo,fname,name,diffdir=diffdir)]
                            fnames += [fname]
                            if i==0:
                                mo = xtm.get_model(sp,name)
                                gss += [sources.GlobalSource(model=mo,index=index,skydir=None,name=name)]
                        if not self.diffuse_dict.has_key(name):
                            self.diffuse_dict[name] = [dss]
                            self.diffuse += [fnames]
                    else:
                        raise Exception,'Isotropic model not implemented'
                elif spatial['type'] == 'MapCubeFunction':
                    diffdir,fname = os.path.split(str(os.path.expandvars(spatial['file'])))
                    if spectral['type'] == 'ConstantValue' or spectral['type'] == 'FrontBackConstant':
                        mo = xtm.get_model(spectral,name)
                        gss += [sources.GlobalSource(model=mo,index=index,skydir=None,name=name)]
                    elif spectral['type'] == 'PowerLaw' or spectral['type'] == 'PowerLaw2':
                        mo = xtm.get_model(spectral,name,index_offset=1)
                        gss += [sources.GlobalSource(model=mo,index=index,skydir=None,name=name)]
                    else:
                        raise Exception('Non-isotropic model "%s" not implemented' % spatial['type'])
                    if not self.diffuse_dict.has_key(name):
                        self.diffuse+=[os.path.split(fname)[1]]
                        self.diffuse_dict[name] = [gds('MapCubeFunction',fname,mo,None,name,diffdir=diffdir)]
                else:
                    raise Exception('Diffuse spatial model "%s" not recognized' % spatial['type'])
            self.global_sources += [gss]


class SkyModelHandler(sax.handler.ContentHandler,xml_parsers.Stack):
    """ContentHandler for parsing the XML representation of a SkyModel"""
    def __init__(self):
        self.outerElements = collections.deque()
        self.sources       = collections.deque()
        self.rois = collections.deque()

    def __call__(self): return self.lastOff

    def startElement(self,name,attrs):
        self.push(xml_parsers.XMLElement(name,attrs))
        if name=='roi_info':
            self.nside = int(attrs.get('nside',12))

    def endElement(self,name):
        t = self.pop()
        l = self.peek()
        if l is not None:
            l.addChild(t)
            if l.name!='roi' and t.name == 'source':
                self.sources.append(t)
            elif t.name == 'roi':
                self.rois.append(t)
        else:
            self.outerElements.append(t)

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
        self.iteration = SourceSelector.iteration
        SourceSelector.iteration += 1
    
    def name(self):
        return 'ROI#%04d' % self.iteration

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
        assert type(index)==types.IntType, 'Expect int type'
        self.myindex = index
        self.mskydir =  self.skydir(index)

    def __str__(self):
        return 'selector %s nside=%d, index=%d' %(self.__class__.__name__, self.nside, self.index)
        
    def name(self):
        return 'HP%02d_%04d' % (self.nside, self.myindex)

    def skydir(self, index=None):
        return Band(self.nside).dir(int(index)) if index is not None else self.mskydir
        
    def index(self, skydir):
        return Band(self.nside).index(skydir)
    
    def free(self,source):
        """
        source : instance of skymodel.Source
        -> bool, if this source in in the region where fit parameters are free
        """
        return self.index(source.skydir) == self.myindex
        
class Rename(object):
    """ functor class to rename sources
        pass as object:
        SkyModel( ..., rename_source=Rename(s,'tset'),...)
    """
    def __init__(self, prefix, srec):
        self.srec= srec.copy()
        self.srec.sort(order='ra')
        self.names = list(self.srec.name[-self.srec.extended])
        self.prefix=prefix
        print 'found %d names to convert' % len(self.names)
        
    def __call__(self, name):
        """ name: string, name to convert"""
        try:
            return '%s%04d' %(self.prefix,self.names.index(name))
        except:
            return name
#========================================================================================
#  These classes are filters. An object of which can be loaded by the filter parameter
# A filter must implement a __call__ method, which must return True to keep the source.
# Since it is passed a refterence to the source, it may change any characteristic, such as the model
#
# note MultiFilter that can be used to combine filters.

class RemoveByName(object):
    """ functor to remove sources, intended to be a filter for SkyModel"""
    def __init__(self, names):
        """ names : string or list of strings
            if a string, assume space-separated set of names (actually works for a single name)
        """
        tnames = names.split() if type(names)==types.StringType else names
        self.names = map( lambda x: x.replace('_', ' '), tnames)
    def __call__(self,ps):
        name = ps.name.strip().replace('_', ' ')
        return name not in self.names
    
class UpdatePulsarModel(object):
    """ special filter to replace models if necessary"""
    def __init__(self,  tol=0.25, ts_min=10, version=705, rename=True):
        import pyfits
        self.tol=tol
        self.ts_min=ts_min
        infile = os.path.expandvars(os.path.join('$FERMI','catalog','srcid', 'cat','obj-pulsar-lat_v%d.fits'%version)) 
        self.data = pyfits.open(infile)[1].data
        self.sdir = map(lambda x,y: SkyDir(float(x),float(y)), self.data.field('RAJ2000'), self.data.field('DEJ2000'))
        self.psr_names = self.data.field('Source_Name')
        self.tags = [False]*len(self.data)
        self.assoc = [['',-1, -1]]*len(self.data) #associated psr_names
        print 'Will check associations with LAT pulsar catalog %d' %version
        self.rename = rename
        
    def get_pulsar_name(self, sdir):
        """ special to check for pulsar name"""
        for i,t in enumerate(self.sdir):
            dist = np.degrees(t.difference(sdir))
            if dist<self.tol:
                self.tags[i]=True
                return self.psr_names[i]
                break
        return None
        
    def __call__(self, s):
        sdir = s.skydir
        if hasattr(s, 'spatial_model') and s.spatial_model is not None:
            return True
        for i,t in enumerate(self.sdir):
            dist = np.degrees(t.difference(sdir))
            if dist<self.tol and s.ts>self.ts_min:
                self.tags[i]=True
                self.assoc[i]=(s.name, dist, s.ts)
                if self.rename and s.name != self.psr_names[i]: 
                    print 'Skymodel: renaming %s(%d) to %s' % (s.name, s.index, self.psr_names[i])
                    s.name = self.psr_names[i]
                if s.model.name=='ExpCutoff': return True
                flux = s.model[0]
                if flux>1e-18:
                    print 'Skymodel: replacing model for: %s(%d): pulsar name: %s' % (s.name, s.index, self.psr_names[i]) 
                    s.model = Models.ExpCutoff()
                    s.free = s.model.free.copy()
                else:
                    print 'Apparent pulsar %s(%d), %s, is very weak, flux=%.2e <1e-13: leave as powerlaw' % (s.name, s.index, self.psr_names[i], flux)
                return True
        if s.model.name=='ExpCutoff':
            print 'Skymodel setup warning: %s (%d) not in LAT pulsar list, should not be expcutoff' % (s.name, s.index)
        return True
    def summary(self):
        n = len(self.tags)-sum(self.tags)
        if n==0: return
        print 'did not find %d sources ' % n
        for i in range(len(self.tags)):
            if not self.tags[i]: print '%s %9.3f %9.3f ' % (self.psr_names[i], self.sdir[i].ra(), self.sdir[i].dec())
     
class MultiFilter(list):
    """ filter that is a list of filters """
    def __init__(self, filters):
        """ filters : list
                if an element of the list is a string, evaluate it first
        """
        for filter in filters: 
            if type(filter)==types.StringType:
                filter = eval(filter)
            self.append(filter)
            
                
    def __call__(self, source):
        for filter in self:
            if not filter(source): return False
        return True

class FluxFreeOnly(object):
    """ Filter that fixes all but flux for sources"""
    def __init__(self):
        pass
    def __call__(self, source):
        if np.any(source.free):
            source.free[1:]=False
        return True
  
class FreeIndex(object):
    """ make sure all spectral indices are free"""
    def __init__(self):
        pass
    def __call__(self, ps):
        model = ps.model
        if ps.model.name=='LogParabola':
            ps.free[1]=True
        return True

