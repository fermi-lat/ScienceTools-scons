# -*- python -*-
#
# $Header$
# Authors: James Chiang <jchiang@slac.stanford.edu>, Pat Nolan <pln@razzle.stanford.edu>
# Version: optimizers-02-17-03
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()
libEnv.Append(CPPDEFINES = 'TRAP_FPE')

if baseEnv['PLATFORM'] == "win32":
    libEnv.Tool('optimizersLib', depsOnly = 1)

optimizersLib = libEnv.SharedLibrary('optimizers',
                                     listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('optimizersLib')
progEnv.Append(CPPDEFINES = 'TRAP_FPE')
test_optimizersBin = progEnv.Program('test_optimizers', 'src/test/main.cxx')

progEnv.Tool('registerTargets', package='optimizers', 
             libraryCxts=[[optimizersLib, libEnv]],
             testAppCxts=[[test_optimizersBin, progEnv]],
             includes=listFiles(['optimizers/*.h']),
             xml=listFiles(['xml/*'], recursive = True))
