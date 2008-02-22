#$Id$

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

if baseEnv['PLATFORM'] != 'win32':
        progEnv.Append(CPPDEFINES = 'TRAP_FPE')

st_graphRootcint = libEnv.Rootcint('st_graph/st_graph_rootcint', ['st_graph/RootFrame.h', 'st_graph/LinkDef.h'], includes = ['.', 'src'])
st_graphLib = libEnv.StaticLibrary('st_graph', listFiles(['src/*.cxx']) + ['st_graph/st_graph_rootcint.cxx'])

progEnv.Tool('st_graphLib')
test_st_graphBin = progEnv.Program('test_st_graph', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerObjects', package = 'st_graph', libraries = [st_graphLib], includes = listFiles(['st_graph/*.h']), testApps = [test_st_graphBin], pfiles = listFiles(['pfiles/*.par']))
