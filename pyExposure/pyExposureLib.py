#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library='pyExposure')
    env.Tool('astroLib')
    env.Tool('irfLoaderLib')
    env.Tool('LikelihoodLib')
    env.Tool('st_facilitiesLib')
    env.Tool('xmlBaseLib')
    env.Tool('dataSubselectorLib')
    env.Tool('st_streamLib')
    env.Tool('st_appLib')
    env.Tool('tipLib')
    env.Tool('optimizersLib')

def exists(env):
    return 1
