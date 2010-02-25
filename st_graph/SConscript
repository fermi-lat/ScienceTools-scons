# -*- python -*-
# $Id$
# Authors: James Peachey <peachey@lheamail.gsfc.nasa.gov>
# Version: st_graph-01-07-07

Import('baseEnv')
Import('listFiles')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()
if baseEnv['PLATFORM'] == "posix":
    libEnv.Append(CPPDEFINES = 'TRAP_FPE')

st_graphRootcint = libEnv.Rootcint('st_graph/st_graph_rootcint',
                                   ['st_graph/RootFrame.h',
                                    'st_graph/LinkDef.h'],
                                   includes = ['.', 'src'])
st_graphLib = libEnv.StaticLibrary('st_graph', listFiles(['src/*.cxx']) + ['st_graph/st_graph_rootcint.cxx'])

progEnv.Tool('st_graphLib')
if baseEnv['PLATFORM'] == "posix":
    progEnv.Append(CPPDEFINES = 'TRAP_FPE')

test_st_graphBin = progEnv.Program('test_st_graph', listFiles(['src/test/*.cxx']))

progEnv.Tool('registerTargets', package = 'st_graph',
             staticLibraryCxts = [[st_graphLib,libEnv]],
             includes = listFiles(['st_graph/*.h']),
             testAppCxts = [[test_st_graphBin, progEnv]],
             pfiles = listFiles(['pfiles/*.par']))
