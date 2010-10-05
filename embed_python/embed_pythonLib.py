#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['embed_python'])
	if env['PLATFORM'] == 'posix':
            env.AppendUnique(LINKFLAGS	= ['-rdynamic'])	
    env.Tool('addLibrary', library = env['pythonLibs'])

def exists(env):
    return 1
