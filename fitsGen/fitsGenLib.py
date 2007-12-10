def generate(env, **kw):
    env.Tool('addLibrary', library = ['fitsGen'], package = 'fitsGen')
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
