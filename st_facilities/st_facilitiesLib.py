def generate(env, **kw):
	env.Tool('addLibrary', library = ['st_facilities', 'g2c'], package = 'st_facilities')
	env.Tool('astroLib')
	env.Tool('tipLib')
	env.Tool('facilitiesLib')
	env.Tool('addLibrary', library = env['cfitsioLibs'])

def exists(env):
	return 1
