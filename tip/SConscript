# -*- python -*-
# $Id$
# Authors: James Peachey <peachey@lheamail.gsfc.nasa.gov>
# Version: tip-02-16-03
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()
test2Env = baseEnv.Clone()

tipLib = libEnv.StaticLibrary('tip', listFiles(['src/*.cxx']))

progEnv.Tool('tipLib')
sampleProg = progEnv.Program('sample',[ 'src/sample/sample.cxx'])

testEnv = progEnv.Clone()
testEnv.Tool('facilitiesLib')
if baseEnv['PLATFORM'] != 'win32':
    test2Env.AppendUnique(CCFLAGS=['-O0'])
    test_tip_main_obj = test2Env.StaticObject(target = 'test_tip_main_obj.o',source = ['src/test/test_tip_main.cxx'])
    test_tipBin = testEnv.Program('test_tip', ['src/test/TestColumn.cxx','src/test/TestExtensionData.cxx',
                                               'src/test/TestFileManager.cxx',
                                               'src/test/TestFileSummary.cxx',
                                               'src/test/TestFilter.cxx',
                                               'src/test/TestHarness.cxx',
                                               'src/test/TestImage.cxx',
                                               'src/test/TestInterpolation.cxx',
                                               'src/test/TestKeyRecord.cxx',
                                               'src/test/TestTable.cxx'] +
                                  test_tip_main_obj)
else:
    test_tipBin = testEnv.Program('test_tip', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerTargets', package = 'tip',
             staticLibraryCxts = [[tipLib, libEnv]],
             binaryCxts = [[sampleProg, progEnv]],
             testAppCxts = [[test_tipBin, progEnv]],
             includes = listFiles(['tip/*.h']),
             data = listFiles(['data/*'], recursive = True))



