def generate(env, **kw):
    env.Tool('addLibrary', library = ['observationSim'], package = 'observationSim')
    env.Tool('facilitiesLib')
    env.Tool('tipLib')
    env.Tool('astroLib')
    env.Tool('fluxLib')
    env.Tool('st_facilitiesLib')
    env.Tool('celestialSourcesLib')
    env.Tool('irfsLib')
    env.Tool('dataSubselectorLib')
    env.Tool('fitsGenLib')

def exists(env):
    return 1
