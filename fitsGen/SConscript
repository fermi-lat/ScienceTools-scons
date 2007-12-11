# -*- python -*-
#
# $Id$

import glob, os, platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if platform.system() == 'Linux':
    progEnv.Append(CPPDEFINES = 'TRAP_FPE')

fitsGenLib = libEnv.StaticLibrary('fitsGen', listFiles(['src/*.cxx']))

progEnv.Tool('fitsGenLib')

makeFT1Bin = progEnv.Program('makeFT1', 'src/makeFT1/makeFT1.cxx')
makeFT2Bin = progEnv.Program('makeFT2', 'src/makeFT2/makeFT2.cxx')
makeFT2aBin = progEnv.Program('makeFT2a', 'src/makeFT2a/makeFT2a.cxx')
egret2FT1Bin = progEnv.Program('egret2FT1', listFiles(['src/egret2FT1/*.cxx']))
convertFT1Bin = progEnv.Program('convertFT1', 'src/convertFT1/convertFT1.cxx')
partitionBin = progEnv.Program('partition', 'src/partition/partition.cxx')
irfTupleBin = progEnv.Program('irfTuple', listFiles(['src/irfTuple/*.cxx']))
test_classifierBin = progEnv.Program('test_classifier', 
                                     listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'fitsGen', 
             libraries = [fitsGenLib], 
             binaries = [makeFT1Bin, makeFT2Bin, makeFT2aBin, egret2FT1Bin, 
                         convertFT1Bin, partitionBin, irfTupleBin,
                         test_classifierBin],
             includes = listFiles(['fitsGen/*.h']), 
             pfiles = listFiles(['pfiles/*.par']))
