import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

facilitiesLib = libEnv.StaticLibrary('facilities', listFiles(['src/*.cxx']))

progEnv.Tool('registerObjects', package = 'facilities', libraries = [facilitiesLib], includes = listFiles(['facilities/*.h']))

