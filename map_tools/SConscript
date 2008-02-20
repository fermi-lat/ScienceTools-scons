#$Id$

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

map_toolsLib = libEnv.StaticLibrary('map_tools', listFiles(['src/*.cxx']))

progEnv.Tool('map_toolsLib')
exposure_cubeBin = progEnv.Program('exposure_cube', listFiles(['src/exposure_cube/*.cxx']))
exposure_mapBin = progEnv.Program('exposure_map', listFiles(['src/exposure_map/*.cxx']))

progEnv.Tool('registerObjects', package = 'map_tools', libraries = [map_toolsLib], binaries = [exposure_cubeBin, exposure_mapBin], includes = listFiles(['map_tools/*.h']),
             pfiles = listFiles(['pfiles/*.par']))