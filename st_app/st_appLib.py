#$Id$
def generate(env, **kw):
	if not kw.get('depsOnly',0):
		env.Tool('addLibrary', library = ['st_app'])
	env.Tool('hoopsLib')
	env.Tool('st_graphLib')
	env.Tool('st_streamLib')
	env.Tool('facilitiesLib')
	env.Tool('addLibrary', library = env['f2cLibs'])

def exists(env):
	return 1
