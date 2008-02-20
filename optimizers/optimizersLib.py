#$Id$
def generate(env, **kw):
    env.Tool('addLibrary', library = ['optimizers', 'g2c'], package = 'optimizers')
    env.Tool('xmlBaseLib')
    env.Tool('addLibrary', library = env['cfitsioLibs'])
    env.Tool('addLibrary', library = env['clhepLibs'])
    env.Tool('addLibrary', library = env['rootLibs'])
    env.Tool('addLibrary', library = env['rootGuiLibs'])
    env.Tool('addLibrary', library = env['minuitLibs'])

def exists(env):
    return 1
