#$Id$
def generate(env, **kw):
	if not kw.get('depsOnly',0):
	    env.Tool('addLibrary', library = ['st_facilities'])

	    if env['PLATFORM'] == 'win32'and env.get('CONTAINERNAME','')=='GlastRelease':
		env.Tool('findPkgPath', package = 'st_facilities') 


	env.Tool('astroLib')
	env.Tool('tipLib')
	env.Tool('facilitiesLib')
	env.Tool('addLibrary', library = env['cfitsioLibs'])
	env.Tool('addLibrary', library = env['f2cLibs'])

	if env['PLATFORM'] == 'win32' and env.get('CONTAINERNAME','')=='GlastRelease':

	    env.Tool('findPkgPath', package = 'facilities') 


def exists(env):
	return 1
