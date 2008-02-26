#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['fitsGen'])
    env.Tool('facilitiesLib')
    env.Tool('tipLib')
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')
    env.Tool('dataSubselectorLib')
    env.Tool('st_appLib')
    env.Tool('st_streamLib')
    env.Tool('embed_pythonLib')

def exists(env):
    return 1
