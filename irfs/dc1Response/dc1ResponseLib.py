def generate(env, **kw):
    env.Tool('addLibrary', library = ['dc1Response', 'g2c'], package = 'irfs/dc1Reseponse')
    env.Tool('st_facilitiesLib')
    env.Tool('irfInterfaceLib')
    env.Tool('irfUtilLib')
    env.Tool('astroLib')
    env.Tool('tipLib')
    env.Tool('addLibrary', library = env['clhepLibs'])

def exists(env):
    return 1
