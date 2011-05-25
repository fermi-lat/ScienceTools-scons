#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['dataSubselector'])
    env.Tool('tipLib')
    env.Tool('astroLib')
    env.Tool('st_facilitiesLib')
    env.Tool('facilitiesLib')
    env.Tool('irfLoaderLib')
    env.Tool('evtbinLib')

def exists(env):
    return 1
