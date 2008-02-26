#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['embed_python'])
    env.Tool('addLibrary', library = env['pythonLibs'])

def exists(env):
    return 1
