def generate(env, **kw):
	if not kw.get('depsOnly',0):
	    env.Tool('addLibrary', library=['astro'], package = 'astro')
	    if env['PLATFORM'] == 'win32'and env.get('CONTAINERNAME','')=='GlastRelease':
		env.Tool('findPkgPath', package = 'astro') 
	env.Tool('facilitiesLib')
	env.Tool('tipLib')
	env.Tool('addLibrary', library=env['clhepLibs'])
	env.Tool('addLibrary', library=env['cfitsioLibs'])
	# EAC, add wcslib and healpix as externals
	env.Tool('addLibrary', library=env['wcslibs'])
	env.Tool('addLibrary', library=env['healpixlibs'])

def exists(env):
	return 1
