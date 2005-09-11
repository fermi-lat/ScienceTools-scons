#!/usr/bin/env python
"""
Factory methods for xml representations of Likelihood Sources and
model Functions.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

import copy
from xml.dom import minidom
from readXml import Source, Function, Parameter

#
# Spectra
#
def PowerLaw():
    func = '\n'.join( ('<spectrum type="PowerLaw">',
                       '   <parameter free="1" max="1000.0" min="0.001" '
                       + 'name="Prefactor" scale="1e-09" value="1"/>',
                       '   <parameter free="1" max="-1.0" min="-5." '
                       + 'name="Index" scale="1.0" value="-2.1"/>',
                       '   <parameter free="0" max="2000.0" min="30.0" '
                       + 'name="Scale" scale="1.0" value="100.0"/>',
                       '</spectrum>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

def PowerLaw2():
    func = """<spectrum type="PowerLaw2">
      <parameter free="1" max="1000.0" min="1e-05" name="Integral" scale="1e-06" value="1.0"/>
      <parameter free="1" max="-1.0" min="-5.0" name="Index" scale="1.0" value="-2.0"/>
      <parameter free="0" max="200000.0" min="20.0" name="LowerLimit" scale="1.0" value="30.0"/>
      <parameter free="0" max="200000.0" min="20.0" name="UpperLimit" scale="1.0" value="2e5"/>
    </spectrum>
"""
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

def BrokenPowerLaw():
    func = '\n'.join( ('<spectrum type="BrokenPowerLaw">',
                       '   <parameter free="1" max="1000.0" min="0.001" '
                       + 'name="Prefactor" scale="1e-09" value="1"/>',
                       '   <parameter free="1" max="-1.0" min="-5." '
                       + 'name="Index1" scale="1.0" value="-1.8"/>',
                       '   <parameter free="1" max="2000.0" min="30.0" '
                       + 'name="BreakValue" scale="1.0" value="1000.0"/>',
                       '   <parameter free="1" max="-1.0" min="-5." '
                       + 'name="Index2" scale="1.0" value="-2.3"/>',
                       '</spectrum>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

def LogParabola():
    func = '\n'.join( ('<spectrum type="LogParabola">',
                       '   <parameter free="1" max="1000.0" min="0.001" '
                       + 'name="norm" scale="1e-9" value="1"/>',
                       '   <parameter free="1" max="10" min="0" '
                       + 'name="alpha" scale="1.0" value="1"/>',
                       '   <parameter free="1" max="1e4" min="20" '
                       + 'name="Eb" scale="1" value="300."/>',
                       '   <parameter free="1" max="10" min="0" '
                       + 'name="beta" scale="1.0" value="2"/>',
                       '</spectrum>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

def Gaussian():
    func = '\n'.join( ('<spectrum type="Gaussian">',
                       '   <parameter free="1" max="1000.0" min="0.001" '
                       + 'name="Prefactor" scale="1e-09" value="1"/>',
                       '   <parameter free="1" max="1e5" min="1e3" '
                       + 'name="Mean" scale="1.0" value="7e4"/>',
                       '   <parameter free="1" max="30" min="1e4" '
                       + 'name="Sigma" scale="1.0" value="1e3"/>',
                       '</spectrum>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

def ConstantValueSpectrum():
    func = '\n'.join( ('<spectrum type="ConstantValue">',
                       '   <parameter max="10" min="0" free="0" '
                       + 'name="Value" scale="1" value="1" />',
                       '</spectrum>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spectrum')
    return Function(func)

#
# Spatial Models
#
def SkyDirFunction():
    func = '\n'.join( ('<spatialModel type="SkyDirFunction">',
                       '   <parameter free="0" max="360." min="-360." '
                       + 'name="RA" scale="1.0" value="83.45"/>',
                       '   <parameter free="0" max="90." min="-90." '
                       + 'name="DEC" scale="1.0" value="21.72"/>',
                       '</spatialModel>\n') )
    (func,) = minidom.parseString(func).getElementsByTagName('spatialModel')
    return Function(func)

def ConstantValue():
    func = '\n'.join( ('<spatialModel type="ConstantValue">',
                       '   <parameter max="10" min="0" free="0" '
                       + 'name="Value" scale="1" value="1" />',
                       '</spatialModel>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spatialModel')
    return Function(func)

def SpatialMap():
    func = '\n'.join( ('<spatialModel '
                       + 'file="$(LIKELIHOODROOT)/src/test/Data/gas.cel" '
                       + 'type="SpatialMap">',
                       '   <parameter max="1000" min="0.001" free="0" '
                       + 'name="Prefactor" scale="1" value="1" />',
                       '</spatialModel>\n') )
    (func, ) = minidom.parseString(func).getElementsByTagName('spatialModel')
    return Function(func)

def MapCubeFunction():
    func  = """<spatialModel file="test_image.fits" type="MapCubeFunction">
   <parameter free="0" max="1000" min="0.001" name="Normalization" scale="1" value="1" />
</spatialModel>
"""
    func = minidom.parseString(func).getElementsByTagName('spatialModel')[0]
    return Function(func)

#
# Containers
#                                   
class FuncContainer(object):
    def __init__(self):
        self.funcs = {}
    def __getitem__(self, name):
        return self.funcs[name]
    def __setitem__(self, name, value):
        if value.type == self.funcs[name].type:
            self.funcs[name] = value
    def keys(self):
        return self.funcs.keys()

class Spectra(FuncContainer):
    def __init__(self):
        FuncContainer.__init__(self)
        self.funcs['PowerLaw'] = PowerLaw()
        self.funcs['PowerLaw2'] = PowerLaw2()
        self.funcs['BrokenPowerLaw'] = BrokenPowerLaw()
        self.funcs['LogParabola'] = LogParabola()
        self.funcs['Gaussian'] = Gaussian()
        self.funcs['ConstantValue'] = ConstantValueSpectrum()

class SpatialModels(FuncContainer):
    def __init__(self):
        FuncContainer.__init__(self)
        self.funcs['SkyDirFunction'] = SkyDirFunction()
        self.funcs['ConstantValue'] = ConstantValue()
        self.funcs['SpatialMap'] = SpatialMap()
        self.funcs['MapCubeFunction'] = MapCubeFunction()

#
# Source factories
#
def PtSrc(indx=0):
    name = "point source %i" % indx
    src = '\n'.join( (('<source name= "%s" ' % name) + 'type="PointSource">',
                      '   <spectrum type="PowerLaw"/>',
                      '   <!-- point source units are cm^-2 s^-1 MeV^-1 -->',
                      '   <spatialModel type="SkyDirFunction"/>',
                      '</source>\n') )
    (src, ) = minidom.parseString(src).getElementsByTagName('source')
    src = Source(src)
    src.spectrum = PowerLaw()
    src.deleteChildElements('spectrum')
    src.node.appendChild(src.spectrum.node)
    
    src.spatialModel = SkyDirFunction()
    src.deleteChildElements('spatialModel')
    src.node.appendChild(src.spatialModel.node)

    return src

def DiffuseSrc(indx=0):
    name = "diffuse source %i" % indx
    src = '\n'.join( (('<source name="%s" ' % name)
                      + 'type="DiffuseSource">',
                      '   <spectrum type="PowerLaw"/>',
                      '   <!-- diffuse source units are ' +
                      'cm^-2 s^-1 MeV^-1 sr^-1 -->',
                      '   <spatialModel type="ConstantValue"/>', 
                      '</source>\n') )
    (src, ) = minidom.parseString(src).getElementsByTagName('source')
    src = Source(src)
    src.spectrum = PowerLaw()
    src.deleteChildElements('spectrum')
    src.node.appendChild(src.spectrum.node)

    src.spatialModel = ConstantValue()
    src.deleteChildElements('spatialModel')
    src.node.appendChild(src.spatialModel.node)

    return src
