import glob,os,platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if platform.system() == 'Linux':
        progEnv.Append(CPPDEFINES = 'TRAP_FPE')

optimizersLib = libEnv.SharedLibrary('optimizers', listFiles(['src/*.cxx', 'src/*.c']))

progEnv.Tool('optimizersLib')
test_optimizersBin = progEnv.Program('test_optimizers', 'src/test/main.cxx')

progEnv.Tool('registerObjects', package = 'optimizers', libraries = [optimizersLib], testApps = [test_optimizersBin], includes = listFiles(['optimizers/*.h']))