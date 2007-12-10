def generate(env, **kw):
	env.Tool('addLibrary', library = ['hoops'], package = 'hoops')
	env.Tool('addLibrary', library = env['pilLibs'])

def exists(env):
	return 1
