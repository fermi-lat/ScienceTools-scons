#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['pulsePhase'])
    env.Tool('pulsarDbLib')
    env.Tool('st_appLib')
    env.Tool('st_facilitiesLib')
    env.Tool('timeSystemLib')
    env.Tool('tipLib')

def exists(env):
    return 1
