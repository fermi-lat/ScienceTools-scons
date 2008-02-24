# @file pointlikeLib.py
# @brief scons package dependencies
#
#$Id$
def generate(env, **kw):
    env.Tool('addLibrary', library = ['pointlike'])
    depends = 'astro healpix skymaps embed_python'.split()
    for pack in depends: env.Tool(pack+'Lib')
    
def exists(env):
    return 1
