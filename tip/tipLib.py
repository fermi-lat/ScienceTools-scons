#$Id$
def generate(env, **kw):
	if not kw.get('depsOnly',0):
		env.Tool('addLibrary', library = ['tip'])
	env.Tool('addLibrary', library = env['cfitsioLibs'] + env['rootLibs'])

def exists(env):
	return 1

