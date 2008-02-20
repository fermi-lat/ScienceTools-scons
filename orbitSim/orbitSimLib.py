#$Id$
def generate(env, **kw):
	env.Tool('addLibrary', library = ['orbitSim'], package = 'orbitSim')
	env.Tool('st_appLib')
	env.Tool('st_facilitiesLib')
	env.Tool('tipLib')

def exists(env):
	return 1;
