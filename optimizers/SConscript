# -*- python -*-
#
# $Header$
# Authors: James Chiang <jchiang@slac.stanford.edu>
# Version: optimizers-04-01-02
import sys
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()
if baseEnv['PLATFORM'] == "posix":
    libEnv.Append(CPPDEFINES = 'TRAP_FPE')

libEnv.Tool('addLinkDeps', package='optimizers', toBuild='shared')

optimizersLib = libEnv.SharedLibrary('optimizers',
                                     listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('optimizersLib')
if baseEnv['PLATFORM'] == "posix":
    progEnv.Append(CPPDEFINES = 'TRAP_FPE')
if sys.platform == 'darwin':
    progEnv.Append(CPPDEFINES = 'DARWIN')
test_optimizersBin = progEnv.Program('test_optimizers', 'src/test/main.cxx')

progEnv.Tool('registerTargets', package='optimizers', 
             libraryCxts=[[optimizersLib, libEnv]],
             testAppCxts=[[test_optimizersBin, progEnv]],
             includes=listFiles(['optimizers/*.h']),
             xml=listFiles(['xml/*'], recursive = True))
