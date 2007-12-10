def generate(env, **kw):
    env.Tool('addLibrary', library = ['dataSubselector'], package = 'dataSubselector')
    env.Tool('tipLib')
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')
    env.Tool('facilitiesLib')
    env.Tool('evtbinLib')

def exists(env):
    return 1
