import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

tipLib = libEnv.StaticLibrary('tip', listFiles(['src/*.cxx']))

progEnv.Tool('tipLib')
sampleProg = progEnv.Program('sample', 'src/sample/sample.cxx')
test_tipBin = progEnv.Program('test_tip', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'tip', libraries = [tipLib], binaries = [sampleProg], testApps = [test_tipBin], includes = listFiles(['tip/*.h']))
