import glob,os

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

orbitSimLib = libEnv.StaticLibrary('orbitSim', listFiles(['src/*.cxx']))
orbitSimBin = progEnv.Program('gttakosim', listFiles(['src/orbSim/*.cxx']))

progEnv.Tool('registerObjects', package = 'orbitSim', libraries = [orbitSimLib], includes = listFiles(['orbitSim/*.h']), binaries = [orbitSimBin],
             pfiles = listFiles(['pfiles/*.par']))