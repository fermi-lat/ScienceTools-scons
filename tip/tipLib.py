#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
	env.Tool('addLibrary', library = ['tip'])
	if env['PLATFORM']=='win32' and env.get('CONTAINERNAME','')=='GlastRelease':
	    env.Tool('findPkgPath', package = 'tip') 
	    env.Tool('findPkgPath', package = 'facilities') 
    if kw.get('incsOnly', 0) == 1:
        env.Tool('findPkgPath', package = 'facilities')         
        return

    env.Tool('addLibrary', library = env['cfitsioLibs'] + env['rootLibs'])

def exists(env):
	return 1

