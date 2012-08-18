#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['embed_python'])
	if env['PLATFORM'] == 'posix':
            env.AppendUnique(LINKFLAGS	= ['-rdynamic'])	
        if env['PLATFORM'] == "win32" and env.get('CONTAINERNAME','') == 'GlastRelease':
            env.Tool('findPkgPath', package = 'embed_python') 
    env.Tool('addLibrary', library = env['pythonLibs'])
    # No need for incsOnly section

def exists(env):
    return 1
