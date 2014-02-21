# @file pointlikeLib.py
# @brief scons package dependencies
#
#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['pointlike'])
    env.Tool('addLibrary', library=env['rootLibs'])
    depends = 'astro healpix skymaps embed_python'.split()
    for pack in depends: env.Tool(pack+'Lib')
    env.Tool('addLibrary', library=env['clhepLibs'])
    
def exists(env):
    return 1
