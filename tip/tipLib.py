def generate(env, **kw):
	env.Tool('addLibrary', library = ['tip'], package = 'tip')
	env.Tool('addLibrary', library = env['cfitsioLibs'] + env['rootLibs'])

def exists(env):
	return 1

