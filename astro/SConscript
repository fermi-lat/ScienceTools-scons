# @file SConscript
# @brief build info
#
# $Header$

import glob,os

Import('baseEnv')
Import('listFiles')
libEnv = baseEnv.Clone()
progEnv = baseEnv.Clone()

astroLib = libEnv.DynamicLibrary('astro', listFiles(
['src/*.cxx', 
 'src/healpix/*.cxx', 
 'src/healpix/*.cc', 
 'src/wcslib/*.cxx', 
 'src/wcslib/*.c', 
 'src/jplephem/*.cxx']))

progEnv.Tool('registerObjects', package = 'astro', libraries = [astroLib], includes = listFiles(['astro/*.h']))
