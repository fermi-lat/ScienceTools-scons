#!/usr/bin/env python
"""
Extract sources from a flux-style xml source catalog and create a
Likelihood-style source model xml file and a ds9 region file.

@author J. Chiang <jchiang@slac.stanford.edu>
$Header$
"""

import string, sys, celgal, copy, os
import readXml
import Tkinter
import tkSimpleDialog
from FileDialog import LoadFileDialog, SaveFileDialog
from xml.dom import minidom

(inputXmlFile, ) = (os.environ['OBSERVATIONSIMROOT']
                    + '/xml/3EG_catalog_20-1e6MeV.xml', )

def ptSrc():
    (src, ) = ('  <source name=" " type="PointSource">\n'
               + '    <spectrum type="PowerLaw">\n'
               + '      <parameter free="1" max="1000.0" min="0.001" '
               + 'name="Prefactor" scale="1e-09" value="1"/>\n'
               + '      <parameter free="1" max="-1.0" min="-5." '
               + 'name="Index" scale="1.0" value="-2.1"/>\n'
               + '      <parameter free="0" max="2000.0" min="30.0" '
               + 'name="Scale" scale="1.0" value="100.0"/>\n'
               + '    </spectrum>\n'
               + '    <spatialModel type="SkyDirFunction">\n'
               + '      <parameter free="0" max="3.40282e+38" '
               + 'min="-3.40282e+38" name="RA" scale="1.0" value="83.45"/>\n'
               + '      <parameter free="0" max="3.40282e+38" '
               + 'min="-3.40282e+38" name="DEC" scale="1.0" value="21.72"/>\n'
               + '    </spatialModel>\n'
               + '  </source>\n', )
    (ptsrc,) = minidom.parseString(src).getElementsByTagName('source')
    return ptsrc

def EGDiffuse():
    (src, ) = ('   <source name="Extragalactic Diffuse" '
               + 'type="DiffuseSource">\n'
               + '    <spectrum type="PowerLaw">\n'
               + '      <parameter max="100" min="1e-05" free="1" '
               + 'name="Prefactor" scale="1e-07" value="1.60" />\n'
               + '      <parameter max="-1" min="-3.5" free="0" '
               + 'name="Index" scale="1" value="-2.1" />\n'
               + '      <parameter max="200" min="50" free="0" '
               + 'name="Scale" scale="1" value="100" />\n'
               + '    </spectrum>\n'
               + '    <spatialModel type="ConstantValue">\n'
               + '       <parameter max="10" min="0" free="0" '
               + 'name="Value" scale="1" value="1" />\n'
               + '    </spatialModel>\n'
               + '  </source>\n', )
    (egdif, ) = minidom.parseString(src).getElementsByTagName('source')
    return egdif

def GalDiffuse():
    (src, ) = ('  <source name="Galactic Diffuse" '
               + 'type="DiffuseSource">\n'
               + '    <spectrum type="PowerLaw">\n'
               + '      <parameter max="1000" min="0.001" free="1" '
               + 'name="Prefactor" scale="0.001" value="11." />\n'
               + '      <parameter max="-1" min="-3.5" free="0" '
               + 'name="Index" scale="1" value="-2.1" />\n'
               + '      <parameter max="200" min="50" free="0" '
               + 'name="Scale" scale="1" value="100" />\n'
               + '    </spectrum>\n'
               + '    <spatialModel file="$(LIKELIHOODROOT)/src/test'
               + '/Data/gas.cel" type="SpatialMap">\n'
               + '      <parameter max="1000" min="0.001" free="0" '
               + 'name="Prefactor" scale="1" value="1" />\n'
               + '    </spatialModel>\n'
               + '  </source>\n', )
    (galdif, ) = minidom.parseString(src).getElementsByTagName('source')
    return galdif

class pointSource:
    def __init__(self, fluxSrc, prefix='my'):
        self.ptsrc = copy.deepcopy(ptSrc())
        self.ptsrc.setAttribute('name', prefix + fluxSrc.getAttribute('name'))
        self.fluxSrc = fluxSrc
        self.setDir()
        self.getSpecParams()
        self.setIndex()
        self.setPrefactor()
    def setDir(self):
        (dir, ) = self.fluxSrc.getElementsByTagName('celestial_dir')
        (spatialModel, ) = self.ptsrc.getElementsByTagName('spatialModel')
        coords = spatialModel.getElementsByTagName('parameter')
        for coord in coords:
            if coord.getAttribute('name').encode('ascii') == 'RA':
                coord.setAttribute('value', dir.getAttribute('ra'))
            if coord.getAttribute('name').encode('ascii') == 'DEC':
                coord.setAttribute('value', dir.getAttribute('dec'))
    def getSpecParams(self):
        (powerlaw, ) = self.fluxSrc.getElementsByTagName('power_law')
        self.gamma = string.atof(powerlaw.getAttribute('gamma'))
        self.emin = string.atof(powerlaw.getAttribute('emin'))/100.
        self.emax = string.atof(powerlaw.getAttribute('emax'))/100.
    def setIndex(self):
        self.setSpecParam('Index', -self.gamma)
    def setPrefactor(self):
        flux = string.atof(self.fluxSrc.getAttribute('flux').encode('ascii'))
        (prefactor,) = (flux*(self.gamma-1)/100./(self.emin**(1-self.gamma)
                                                  - self.emax**(1-self.gamma))
                        *1e5, )
        self.setSpecParam('Prefactor', prefactor)
    def setSpecParam(self, paramName, value):
        (spectrum, ) = self.ptsrc.getElementsByTagName('spectrum')
        params = spectrum.getElementsByTagName('parameter')
        for param in params:
            if param.getAttribute('name').encode('ascii') == paramName:
                param.setAttribute('value', "%.3e" % value)
    def write(self):
        return self.ptsrc.toxml()

def isPtSrc(src):
    dirs = src.getElementsByTagName('celestial_dir')
    if len(dirs) != 1: return 0
    specs = src.getElementsByTagName('power_law')
    if len(specs) != 1 or not specs[0].hasAttribute('gamma'): return 0
    particles = src.getElementsByTagName('particle')
    if (len(particles) != 1
        or particles[0].getAttribute('name').encode('ascii') != 'gamma'):
        return 0
    return 1

def extractFrom3EG(ra0, dec0, radius, fluxLimit, filename, infile):
    ds9File = ds9_region_file('ds9.reg')
    ds9File.setSR(ra0, dec0, radius)
    file = open(filename, 'w')
    file.write('<?xml version="1.0" ?>\n')
    file.write('<source_library title="source library">\n')
    doc = minidom.parse(infile)
    srcList = doc.getElementsByTagName('source')
    file.write(EGDiffuse().toxml() + '\n')
    file.write(GalDiffuse().toxml() + '\n')
    for src in srcList[:-1]:
        if isPtSrc(src):
            (dir, ) = src.getElementsByTagName('celestial_dir')
            ra = string.atof(dir.getAttribute('ra').encode('ascii'))
            dec = string.atof(dir.getAttribute('dec').encode('ascii'))
            flux = string.atof(src.getAttribute('flux').encode('ascii'))
            if (celgal.dist((ra0, dec0), (ra, dec)) < radius
                and flux > fluxLimit):
                file.write(pointSource(src).write() + '\n')
                ds9File.addSrc(src)
    file.write('</source_library>\n')
    file.close()
    ds9File.write()

class ds9_region_file:
    def __init__(self, filename, fk5=0):
        self.filename = filename
        self.fk5 = fk5
        self.SR = None
        self.srcs = []
    def addSrc(self, src):
        (dir, ) = src.getElementsByTagName('celestial_dir')
        self.srcs.append( (dir.getAttribute('ra').encode('ascii'),
                           dir.getAttribute('dec').encode('ascii')) )
    def setSR(self, ra, dec, radius):
        self.SR = (ra, dec, radius)
    def write(self):
        file = open(self.filename, 'w')
        file.write("# Region file format: DS9 version 3.0\n")
        file.write('global color=green font="helvetica 10 normal" '
                   + 'select=1 edit=1 move=1 delete=1 include=1 '
                   + 'fixed=0 source\n')
        for src in self.srcs:
            if self.fk5:
                file.write('fk5;point(%s, %s) # point=circle\n' % src)
            else:
                file.write('physical;point(%s, %s) # point=circle\n' % src)
        if self.SR != None:
            if self.fk5:
                file.write('fk5;circle(%s, %s, %s) # color=red\n'
                           % self.SR)
            else:
                file.write('physical;circle(%s, %s, %s) # color=red\n'
                           % self.SR)

class NewSourceDialog(tkSimpleDialog.Dialog):
    def __init__(self, parent):
        self.parent = parent
        tkSimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        dataFrame = Tkinter.Frame(self)
        dataFrame.pack(anchor=Tkinter.N, expand=Tkinter.YES, fill=Tkinter.X)
        self.title("New Source Parameters")
        self.ra = ParamDoubleEntry(parent, 'RA', 0)
        self.dec = ParamDoubleEntry(parent, 'DEC', 1)
        self.flux = ParamDoubleEntry(parent, 'Flux above 30 MeV', 2,
                                     default=1e-8)
        self.index = ParamDoubleEntry(parent, "Photon index", 3, default=2.1)
    def ok(self, x=0):
        self.withdraw()
        self.update_idletasks()
        self.apply()
        self.cancel()
    def apply(self):
        pass
    def cancel(self):
        self.parent.focus_set()
        self.destroy()

class SourceRegionDialog(tkSimpleDialog.Dialog):
    def __init__(self, parent):
        self.parent = parent
        tkSimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        dataFrame = Tkinter.Frame(self)
        dataFrame.pack(anchor=Tkinter.N, expand=Tkinter.YES, fill=Tkinter.X)
        self.title("Source Extraction Region")
        self.ra = ParamDoubleEntry(parent, 'RA', 0)
        self.dec = ParamDoubleEntry(parent, 'DEC', 1)
        self.radius = ParamDoubleEntry(parent, 'radius', 2, default=20)
        self.fluxLimit = ParamDoubleEntry(parent, 'flux limit', 3,
                                          default=1e-2)
        flux_style_xmlFile = inputXmlFile
        self.infile = ParamFileEntry(parent, 'input file', 4,
                                     default=flux_style_xmlFile)
        self.filename = ParamFileEntry(parent, 'output file', 5,
                                       default='my_source_model.xml')
    def apply(self):
        extractFrom3EG(self.ra.value(), self.dec.value(), 
                       self.radius.value(), self.fluxLimit.value(),
                       self.filename.value(), self.infile.value())
    def ok(self, x=0):
        self.haveSources = 1
        self.withdraw()
        self.update_idletasks()
        self.apply()
        self.close()
    def cancel(self):
        self.haveSources = 0
        self.close()
    def close(self):
        self.parent.focus_set()
        self.destroy()

class ParamDoubleEntry:
    def __init__(self, parent, label, row, default=0):
        self.parent = parent
        self.variable = Tkinter.DoubleVar()
        self.variable.set(default)
        name = Tkinter.Label(parent, text = label)
        name.grid(column=0, row=row, sticky=Tkinter.E)
        entry = Tkinter.Entry(parent, textvariable=self.variable,
                              width=30, state=Tkinter.NORMAL)
        entry.grid(column=1, row=row)
    def value(self):
        return self.variable.get()

class ParamStringEntry:
    def __init__(self, parent, label, row, default=''):
        self.parent = parent
        self.variable = Tkinter.StringVar()
        self.variable.set(default)
        name = Tkinter.Label(parent, text = label)
        name.grid(column=0, row=row, sticky=Tkinter.E)
        entry = Tkinter.Entry(parent, textvariable=self.variable,
                              width=30, state=Tkinter.NORMAL)
        entry.grid(column=1, row=row)
    def value(self):
        return self.variable.get()

class ParamFileEntry:
    def __init__(self, parent, label, row, default=''):
        self.parent = parent
        self.variable = Tkinter.StringVar()
        self.variable.set(default)
        file = Tkinter.Button(parent, text=label, command=self.getFile, bd=1)
        file.grid(column=0, row=row, sticky=Tkinter.E)
        entry = Tkinter.Entry(parent, textvariable=self.variable,
                              width=30, state=Tkinter.NORMAL)
        entry.grid(column=1, row=row)
    def value(self):
        return self.variable.get()
    def getFile(self):
        file = LoadFileDialog(self.parent).go()
        if file:
            self.variable.set(file)

if __name__ == '__main__':
    root = Tkinter.Tk()
    dialog = SourceRegionDialog(root)
    root.mainloop()
