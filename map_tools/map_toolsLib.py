#$Id$
def generate(env, **kw):
    env.Tool('addLibrary', library = ['map_tools'], package = 'map_tools')
    env.Tool('healpixLib')
    env.Tool('astroLib')
    env.Tool('hoopsLib')
    env.Tool('tipLib')
    env.Tool('st_appLib')
    env.Tool('st_streamLib')
    env.Tool('irfLoaderLib')
    env.Tool('addLibrary', library = env['cfitsioLibs'])

def exists(env):
    return 1
