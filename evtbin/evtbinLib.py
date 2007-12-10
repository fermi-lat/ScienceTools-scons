def generate(env, **kw):
    env.Tool('addLibrary', library = ['evtbin'], package = 'evtbin')
    env.Tool('astroLib')
    env.Tool('st_appLib')
    env.Tool('st_facilitiesLib')
    env.Tool('st_streamLib')
    env.Tool('tipLib')

def exists(env):
    return 1
