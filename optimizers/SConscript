# -*- python -*-
#
# $Header$
# Authors: James Chiang <jchiang@slac.stanford.edu>, Pat Nolan <pln@razzle.stanford.edu>
# Version: optimizers-02-17-01
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('optimizersLib', depsOnly = 1)
optimizersLib = libEnv.SharedLibrary('optimizers',
                                     listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('optimizersLib')

test_optimizersBin = progEnv.Program('test_optimizers', 'src/test/main.cxx')

progEnv.Tool('registerObjects', package='optimizers', 
             libraries=[optimizersLib],
             testApps=[test_optimizersBin],
             includes=listFiles(['optimizers/*.h']),
             xml=listFiles(['xml/*'], recursive = True))
