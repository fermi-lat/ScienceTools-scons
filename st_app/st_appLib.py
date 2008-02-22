def generate(env, **kw):
	env.Tool('addLibrary', library = ['st_app'], package = 'st_app')
	env.Tool('hoopsLib')
	env.Tool('st_graphLib')
	env.Tool('st_streamLib')
	env.Tool('facilitiesLib')

def exists(env):
	return 1
