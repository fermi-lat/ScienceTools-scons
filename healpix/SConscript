# @file SConscript
# @brief build info for package healpix
#
#$Header$

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

progEnv.Tool('registerObjects', 
      package = 'healpix', 
      includes = listFiles(['healpix/*.h']),
      libraries = [libEnv.SharedLibrary('healpix', 
                    listFiles(['src/*.cxx', 'src/base/*.cc', 'src/base/*.c']))],
      testApps = [progEnv.Program('test_healpix', listFiles(['src/test/*.cxx']))],
      )
	      