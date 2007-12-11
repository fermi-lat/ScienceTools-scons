# -*- python -*-
#
# $Id$

import glob, os, platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if platform.system() == 'Windows':
    progEnv.Append(CPPFLAGS = '/wd4290')

dataSubselectorLib = libEnv.StaticLibrary('dataSubselector', 
                                          listFiles(['src/*.cxx']))

progEnv.Tool('dataSubselectorLib')

gtselectBin = progEnv.Program('gtselect', 
                              listFiles(['src/dataSubselector/*.cxx']))

gtmktimeBin = progEnv.Program('gtmktime', listFiles(['src/gtmaketime/*.cxx']))

gtvcutBin = progEnv.Program('gtvcut', listFiles(['src/viewCuts/*.cxx']))

progEnv.Tool('registerObjects', package = 'dataSubselector', 
             libraries = [dataSubselectorLib],
             binaries = [gtselectBin, gtmktimeBin, gtvcutBin], 
             includes = listFiles(['dataSubselector/*.h']),
             pfiles = listFiles(['pfiles/*.par']))
