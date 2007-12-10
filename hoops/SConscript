import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

hoopsLib = libEnv.StaticLibrary('hoops', listFiles(['src/*.cxx']))

progEnv.Tool('hoopsLib')
test_sourceBin = progEnv.Program('test_source', 'src/test/hoops_lim_test.cxx')

progEnv.Tool('registerObjects', package = 'hoops', libraries = [hoopsLib], testApps = [test_sourceBin], includes = listFiles(['hoops/*.h']), pfiles = listFiles(['pfiles/*.par']))