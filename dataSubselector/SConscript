import glob,os,platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if platform.system() == 'Windows':
        progEnv.Append(CPPFLAGS = '/wd4290')

dataSubselectorLib = libEnv.StaticLibrary('dataSubselector', listFiles(['src/*.cxx']))

progEnv.Tool('dataSubselectorLib')
gtselectBin = progEnv.Program('gtselect', listFiles(['src/dataSubselector/*.cxx']))
gtmaketimeBin = progEnv.Program('gtmaketime', listFiles(['src/gtmaketime/*.cxx']))
gtviewcutsBin = progEnv.Program('gtviewcuts', listFiles(['src/viewCuts/*.cxx']))

progEnv.Tool('registerObjects', package = 'dataSubselector', libraries = [dataSubselectorLib], binaries = [gtselectBin], includes = listFiles(['dataSubselector/*.h']),
              pfiles = listFiles(['pfiles/*.par']))
