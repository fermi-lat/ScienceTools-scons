def generate(env, **kw):
	if not kw.get('depsOnly',0):
		env.Tool('addLibrary', library=['astro'], package = 'astro')
	env.Tool('facilitiesLib')
	env.Tool('tipLib')
	env.Tool('addLibrary', library=env['clhepLibs'])
	env.Tool('addLibrary', library=env['cfitsioLibs'])

def exists(env):
	return 1
