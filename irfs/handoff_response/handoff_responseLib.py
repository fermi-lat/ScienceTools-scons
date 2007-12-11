def generate(env, **kw):
    env.Tool('addLibrary', library = ['handoff_response'], package = 'irfs/handoff_response')
    env.Tool('irfInterfaceLib')
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')
    env.Tool('st_appLib')
    env.Tool('st_streamLib')
    env.Tool('irfUtilLib')
    env.Tool('embed_pythonLib')
    env.Tool('tipLib')
    env.Tool('addLibrary', library = env['rootLibs'])
    env.Tool('addLibrary', library = env['clhepLibs'])

def exists(env):
    return 1
