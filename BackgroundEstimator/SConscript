# -*- python -*-
# $Id$
# Authors: Vlasios Vasileiou <vlasisva@gmail.com>
# Version: BackgroundEstimator-00-04-00
import glob, os
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv  = baseEnv.Clone()

##################################################
cintSources = listFiles(['BackgroundEstimator/*.h'])
cintSources.append('src/LinkDef.h')

libEnv.Tool('BackgroundEstimatorLib', depsOnly=1)

BackgroundEstimatorCint = libEnv.Rootcint('BackgroundEstimator/BackgroundEstimator_rootcint.cxx',
                                          cintSources,
                                          includes = ['.', 'src'])
# includes = ['.', 'src', 'include'])

libEnv['rootcint_node'] = BackgroundEstimatorCint

libsources = listFiles(['src/BKGE_Tools/*.cxx']) + listFiles(['src/BackgroundEstimator/*.cxx']) + ['BackgroundEstimator/BackgroundEstimator_rootcint.cxx']

BackgroundEstimatorLib  = libEnv.RootDynamicLibrary('BackgroundEstimator', libsources)

progEnv.Tool('registerTargets', package = 'BackgroundEstimator',
             rootcintSharedCxts = [[BackgroundEstimatorLib,libEnv]],
             includes = cintSources)

