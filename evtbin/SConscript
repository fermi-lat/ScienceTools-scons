import glob,os,platform

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

evtbinLib = libEnv.StaticLibrary('evtbin', listFiles(['src/*.cxx']))

progEnv.Tool('evtbinLib')
gtbinBin = progEnv.Program('gtbin', listFiles(['src/gtbin/*.cxx']))
gtbindefBin = progEnv.Program('gtbindef', listFiles(['src/gtbindef/*.cxx']))

progEnv.Tool('registerObjects', package = 'evtbin', libraries = [evtbinLib], binaries = [gtbinBin, gtbindefBin], includes = listFiles(['evtbin/*.h']), pfiles = listFiles(['pfiles/*.par']))