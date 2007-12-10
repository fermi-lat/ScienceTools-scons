import glob,os,platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

st_streamLib = libEnv.StaticLibrary('st_stream', listFiles(['src/*.cxx']))

progEnv.Tool('registerObjects', package = 'st_stream', libraries = [st_streamLib], includes = listFiles(['st_stream/*.h']))