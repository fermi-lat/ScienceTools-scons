def generate(env, **kw):
	env.Tool('addLibrary', library = ['facilities'], package = 'facilities')

def exists(env):
	return 1;
