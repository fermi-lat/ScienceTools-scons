#$Id$
Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

orbitSimLib = libEnv.StaticLibrary('orbitSim', listFiles(['src/*.cxx']))

progEnv.Tool('orbitSimLib')
gttakosim = progEnv.Program('gttakosim', listFiles(['src/orbSim/*.cxx']))

progEnv.Tool('registerObjects', package = 'orbitSim', libraries = [orbitSimLib], includes = listFiles(['orbitSim/*.h']), binaries = [gttakosim],
             pfiles = listFiles(['pfiles/*.par']))