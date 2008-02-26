#$Id$
def generate(env, **kw):
	if not kw.get('depsOnly',0):
		env.Tool('addLibrary', library = ['hoops'])
	env.Tool('addLibrary', library = env['apeLibs'])

def exists(env):
	return 1
