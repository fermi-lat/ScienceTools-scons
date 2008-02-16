#$Id$
import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if libEnv['PLATFORM'] == "win32":
	libEnv.AppendUnique(CPPFLAGS = "/wd4068")
	if libEnv['MSVS_VERSION'] == "8.0":
		libEnv.AppendUnique(CPPFLAGS = "/wd4812")

healpixSharedLib = libEnv.SharedLibrary('healpix', listFiles(['src/*.cxx', 'src/base/*.cc', 'src/base/*.c']))
healpixStaticLib = libEnv.StaticLibrary('healpix', listFiles(['src/*.cxx', 'src/base/*.cc', 'src/base/*.c']))

progEnv.Tool('healpixLib')
test_healpix = progEnv.Program('test_healpix', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'healpix', libraries = [healpixSharedLib, healpixStaticLib], includes = listFiles(['healpix/*.h']))

