def generate(env, **kw):
    env.Tool('addLibrary', library = ['embed_python'], package = 'embed_python')
    env.Tool('addLibrary', library = env['pythonLibs'])

def exists(env):
    return 1
