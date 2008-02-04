def generate(env, **kw):
	env.Tool('addLibrary', library = ['hoops'], package = 'hoops')
	env.Tool('addLibrary', library = env['apeLibs'])

def exists(env):
	return 1
