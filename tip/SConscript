import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

tipLib = libEnv.StaticLibrary('tip', listFiles(['src/*.cxx']))

progEnv.Tool('tipLib')
sampleProg = progEnv.Program('sample', 'src/sample/sample.cxx')

progEnv.Tool('registerObjects', package = 'tip', libraries = [tipLib], binaries = [sampleProg], includes = listFiles(['tip/*.h']))