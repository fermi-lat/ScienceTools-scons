#!/usr/bin/env python
"""
Extract sources from a flux-style xml source catalog and create a
Likelihood-style source model xml file and a ds9 region file.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
import os, sys, string, copy
from xml.dom import minidom

import Tkinter
import tkSimpleDialog
from FileDialog import LoadFileDialog, SaveFileDialog

import celgal
from xmlSrcLib import *

_3EG_catalog = os.path.join(os.environ['OBSERVATIONSIMROOT'],
                            'xml', '3EG_catalog_20-1e6MeV.xml')

class SourceList(object):
    def __init__(self, inputFile=_3EG_catalog):
        self.inputFile = inputFile
    def _isPtSrc(self, src):
        dirs = src.getElementsByTagName('celestial_dir')
        if len(dirs) != 1: return False
        specs = src.getElementsByTagName('power_law')
        if len(specs) != 1 or not specs[0].hasAttribute('gamma'): return False
        particles = src.getElementsByTagName('particle')
        if (len(particles) != 1
            or particles[0].getAttribute('name').encode('ascii') != 'gamma'):
            return False
        return True
    def extract(self, outputFile, roiCone, fluxLimit=1e-2, useDiffuse=True):
        ra0, dec0, radius = roiCone
        ds9File = ds9_region_file('ds9.reg', fk5=1)
        ds9File.setSR(ra0, dec0, radius)
        outfile = open(outputFile, 'w')
        outfile.write('<?xml version="1.0" ?>\n')
        outfile.write('<source_library title="source library">\n')
        doc = minidom.parse(self.inputFile)
        srcList = doc.getElementsByTagName('source')
        if useDiffuse:
            outfile.write(EGDiffuse().toxml() + '\n')
            outfile.write(GalDiffuse().toxml() + '\n')
        for src in srcList[:-1]:
            if self._isPtSrc(src):
                dir = src.getElementsByTagName('celestial_dir')[0]
                ra = string.atof(dir.getAttribute('ra').encode('ascii'))
                dec = string.atof(dir.getAttribute('dec').encode('ascii'))
                flux = string.atof(src.getAttribute('flux').encode('ascii'))
                if (celgal.dist((ra0, dec0), (ra, dec)) < radius
                    and flux > fluxLimit):
                    outfile.write(PointSource(src).write() + '\n')
                    ds9File.addSrc(src)
        outfile.write('</source_library>\n')
        outfile.close()
        ds9File.write()

class PointSource:
    def __init__(self, fluxSrc, prefix='my'):
        self.ptsrc = copy.deepcopy(ptSrc())
        self.ptsrc.setAttribute('name', prefix + fluxSrc.getAttribute('name'))
        self.fluxSrc = fluxSrc
        self._setDir()
        self._getSpecParams()
        self._setIndex()
        self._setPrefactor()
    def _setDir(self):
        dir = self.fluxSrc.getElementsByTagName('celestial_dir')[0]
        spatialModel = self.ptsrc.getElementsByTagName('spatialModel')[0]
        coords = spatialModel.getElementsByTagName('parameter')
        for coord in coords:
            if coord.getAttribute('name').encode('ascii') == 'RA':
                coord.setAttribute('value', dir.getAttribute('ra'))
            if coord.getAttribute('name').encode('ascii') == 'DEC':
                coord.setAttribute('value', dir.getAttribute('dec'))
    def _getSpecParams(self):
        powerlaw = self.fluxSrc.getElementsByTagName('power_law')[0]
        self.gamma = string.atof(powerlaw.getAttribute('gamma'))
        self.emin = string.atof(powerlaw.getAttribute('emin'))/100.
        self.emax = string.atof(powerlaw.getAttribute('emax'))/100.
    def _setIndex(self):
        self._setSpecParam('Index', -self.gamma)
    def _setPrefactor(self):
        flux = string.atof(self.fluxSrc.getAttribute('flux').encode('ascii'))
        prefactor = flux*1e5*(self.gamma-1)/100./(self.emin**(1-self.gamma)
                                                  - self.emax**(1-self.gamma))
        self._setSpecParam('Prefactor', prefactor)
    def _setSpecParam(self, paramName, value):
        spectrum = self.ptsrc.getElementsByTagName('spectrum')[0]
        params = spectrum.getElementsByTagName('parameter')
        for param in params:
            if param.getAttribute('name').encode('ascii') == paramName:
                param.setAttribute('value', "%.3e" % value)
    def write(self):
        return self.ptsrc.toxml()

class ds9_region_file:
    def __init__(self, filename, fk5=0):
        self.filename = filename
        self.fk5 = fk5
        self.SR = None
        self.srcs = []
    def addSrc(self, src):
        dir = src.getElementsByTagName('celestial_dir')[0]
        self.srcs.append( (dir.getAttribute('ra').encode('ascii'),
                           dir.getAttribute('dec').encode('ascii')) )
    def addSource(self, src):
        ra, dec = `src.spatialModel.RA.value`, `src.spatialModel.DEC.value`
        self.srcs.append( (ra, dec) )
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
        if self.SR is not None:
            if self.fk5:
                file.write('fk5;circle(%s, %s, %s) # color=red\n' % self.SR)
            else:
                file.write('physical;circle(%s, %s, %s) # color=red\n'
                           % self.SR)

class DefaultValues(object):
    def __init__(self, ra, dec, radius, flux, infile, outfile):
        self.ra, self.dec, self.radius = ra, dec, radius
        self.flux, self.infile, self.outfile = flux, infile, outfile
    def set(self, dialogObject):
        self.ra = dialogObject.ra.value()
        self.dec = dialogObject.dec.value()
        self.radius = dialogObject.radius.value()
        self.flux = dialogObject.fluxLimit.value()
        self.infile = dialogObject.infile.value()
        self.outfile = dialogObject.filename.value()

_defaults = DefaultValues(266.4, -28.9, 20, 0.01,
                          _3EG_catalog, 'my_source_model')
        
class SourceRegionDialog(tkSimpleDialog.Dialog):
    def __init__(self, parent):
        self.parent = parent
        tkSimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        dataFrame = Tkinter.Frame(self)
        dataFrame.pack(anchor=Tkinter.N, expand=Tkinter.YES, fill=Tkinter.X)
        self.title("Source Extraction Region")
        self.ra = ParamDoubleEntry(parent, 'RA', 0, default=_defaults.ra)
        self.dec = ParamDoubleEntry(parent, 'DEC', 1, default=_defaults.dec)
        self.radius = ParamDoubleEntry(parent, 'radius', 2,
                                       default=_defaults.radius)
        self.fluxLimit = ParamDoubleEntry(parent, 'flux limit', 3,
                                          default=_defaults.flux)
        self.infile = ParamFileEntry(parent, 'input file', 4,
                                     default=_defaults.infile)
        self.filename = ParamFileEntry(parent, 'output file', 5,
                                       default=_defaults.outfile)
    def apply(self):
        srcList = SourceList(self.infile.value())
        roiCone = (self.ra.value(), self.dec.value(), self.radius.value())
        srcList.extract(self.filename.value(), roiCone,
                        fluxLimit=self.fluxLimit.value())
        _defaults.set(self)
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
        name = Tkinter.Label(parent, text=label)
        name.grid(column=0, row=row, sticky=Tkinter.E)
        entry = Tkinter.Entry(parent, textvariable=self.variable,
                              state=Tkinter.NORMAL, width=30)
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
    from pil import Pil
    from pfilesPath import *
    try:
        pfile = os.path.join(pfilesPath("extractSources.par"),
                             "extractSources.par")
        pars = Pil(pfile)
        if pars['input_file'] == 'DEFAULT':
            input_file = _3EG_catalog
        else:
            input_file = pars['input_file']
        srcList = SourceList(input_file)
        srcList.extract(pars['output_file'],
                        (pars['ra0'], pars['dec0'], pars['radius']),
                        pars['flux_limit'])
    except ParFileError:
        SourceRegionDialog(Tkinter.Tk())
