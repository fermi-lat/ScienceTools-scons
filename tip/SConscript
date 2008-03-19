#$Id$
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('tipLib', depsOnly = 1)
tipLib = libEnv.StaticLibrary('tip', listFiles(['src/*.cxx']))

progEnv.Tool('tipLib')
sampleProg = progEnv.Program('sample', 'src/sample/sample.cxx')

testEnv = progEnv.Clone()
testEnv.Tool('facilitiesLib')
test_tipBin = testEnv.Program('test_tip', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'tip', libraries = [tipLib], binaries = [sampleProg], testApps = [test_tipBin], includes = listFiles(['tip/*.h']),
             data = listFiles(['data/*'], recursive = True))
