#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['healpix'])
    # EAC, add dependece on externals
    env.Tool('addLibrary', library=env['healpixlibs'])  
    env.Tool('astroLib')
    
def exists(env):
    return 1;
