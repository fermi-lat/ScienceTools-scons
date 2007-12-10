def generate(env, **kw):
	env.Tool('addLibrary', library = ['st_stream'], package = 'st_stream')

def exists(env):
	return 1
