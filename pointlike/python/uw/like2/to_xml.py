"""
Generate the XML representation of a skymodel
$Header$

"""
import os, collections, argparse, types
import numpy as np
import pandas as pd
from uw.like2 import skymodel
from uw.utilities import keyword_options
from uw.like import Models
from skymaps import SkyDir

from uw.utilities import xml_parsers
from collections import OrderedDict

class Element(object):
    """ This might be usefull for a refactoring
    """
    level=0
    stream=None
    def __init__(self, element_name,  **kw):
        """
        Example:
        
        with Element('A', Aprop=2, b=3) as t:
            t.text('A string')
            with Element('B', u=99) as s:
                s.text('test string')
                with Element('C', ctest=9):
                    pass
        """
        name = kw.pop('name', None)
        self.kw=OrderedDict( kw.items() if name is None else [('name',name)]+kw.items() )
        self.name =element_name
    def __str__(self):
        return ' '.join('%s="%s"'%item  for item in self.kw.items()) 
    def text(self, text):
        offset = '  '*Element.level
        for line in text.split('\n'):
            print >> self.stream , offset + line
    def __enter__(self):
        self.text('<%s %s>'% (self.name, self))
        Element.level += 1
        return self
    def __exit__(self, type, value, traceback):
        Element.level -= 1
        self.text('</%s>'%self.name )

class SimpleElement(Element):
    def __init__(self, element_name, **kw):
        self.kw=OrderedDict(**kw)
        self.text('<%s %s/>'% (element_name, self))

class ToXML(object):

    def __init__(self, skymodel, filename, ts_min=10, a_max=0.25, title=None, source_filter=lambda x:True, strict=False, gtlike = False):
        """ generate a file with the XML version of the sources in the model
        parameters
        ----------
        skymodel : SkyModel object to convert
        filename : string or None
            name of file to write to; if None make it up from local folder
        ts_min : None or float
            if set, only select sources with ts>ts_min
        title : string
            set title property of the source_library
        source_filter : function
            if set, function of a source that returns bool
        strict : bool
            set to True to apply strict rules
        gtlike : bool
            set to True to generate only a list of sources, no ROI information
        """
        self.skymodel = skymodel
        point_sources = self.skymodel.point_sources if ts_min is None\
            else filter(lambda s: s.ts>ts_min, self.skymodel.point_sources)
        print 'Writing XML representations of %d point sources %s and %d extended sources to %s' \
            %(len(point_sources), ('' if ts_min is None else '(with TS>%.f)'%ts_min), len(self.skymodel.extended_sources), filename)
        def pointsource_properties(s):
            return 'Pivot_Energy="%.1f" TS="%.1f"' % (s.model.e0, s.ts)
        stacks= [
            xml_parsers.unparse_diffuse_sources(self.skymodel.extended_sources, convert_extended=True, filename=filename),
            xml_parsers.unparse_point_sources(point_sources,strict=strict, properties=pointsource_properties),
        ]
        if filename is None:
            filename = '_'.join(os.path.abspath('.').split('/')[-2:])+'.xml'
            # for example, 'P202_uw10.xml'
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
                f.write('\n'.join(['\n<roi_info nside="12">',
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
            globals = self.skymodel.global_sources[i]
            for s in globals:
                prefix = s.name.split('_')[0]
                s.name, s.dmodel = prefix, self.skymodel.diffuse_dict[prefix]
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

def pmodel(source):
    """ create a pointlike model from a Series object from DataFrame row
    """
    modelname,e0, norm, pindex,index2,cutoff= [source[x] for x in 'modelname e0 flux pindex index2 cutoff'.split()]
    if modelname=='LogParabola':
        if np.abs(index2)<2e-3:
            modelname='PowerLaw'
            model = Models.PowerLaw(p=[norm, pindex ], e0=e0)
        else:
            model =Models.LogParabola(p= [norm, pindex, index2, e0])
    elif modelname=='PLSuperExpCutoff':
        model = Models.PLSuperExpCutoff(p = [norm, pindex, cutoff, index2], e0=e0)
    else:
        raise Exception('model name %s not recognized' % modelname)
    return model
        

def source_library(source_list, title='sources', stream=None, strict=False):
    Element.stream = stream
    m2x = xml_parsers.Model_to_XML(strict=True)
    with Element('source_library', title=title) as sl:
        for i,source in source_list.iterrows():
            stype = 'DiffuseSource' if np.isnan(source['locqual']) else 'PointSource'
            with Element('source', type=stype, **source) as src:
                m2x = xml_parsers.Model_to_XML(strict=strict)
                m2x.process_model(pmodel(source))
                src.text(m2x.getXML(tablevel=0))
                if stype=='PointSource':
                    src.text(xml_parsers.makePSSpatialModel(SkyDir(source['ra'],source['dec']),tablevel=0))
                else:
                    with Element('spatialModel', type='SpatialMap', 
                            file='$LATEXTDIR/Templates/%s.fits'%source['name'].replace(' ','') ) as sm:
                        SimpleElement('parameter', name='Prefactor', value=1.0, free=0, max=1e3,min=1e-3, scale=1.0)

def main( args ):
    sources = pd.read_csv(args.sources)
    print 'read %d sources from %s' %(len(sources), args.sources)
    cut_sources = sources[eval(args.cuts)]
    print 'applied cut "%s", %d remain' % (args.cuts, len(cut_sources))
    modelname = '_'.join(os.path.abspath('.').split('/')[-2:])
    filename = args.filename[0] if len(args.filename)>0 else None
    if filename is None:
        filename = modelname+'.xml'
        # for example, 'P202_uw10.xml'
    with open(filename,'w') as stream:
        source_library(cut_sources, title=modelname, stream=stream)
    
if __name__=='__main__':
    parser = argparse.ArgumentParser( description=""" Convert the skymodel in the current folder to XML""")
    parser.add_argument('filename', nargs='*', help='filename to write to (default: make it up)')
    parser.add_argument('--sources', default='sources.csv', help='input table')
    parser.add_argument('--cuts',  default='(sources.ts>10)*(sources.a<0.25)*(sources.locqual<10)', help='selection cuts')
    args = parser.parse_args()
    main(args)
