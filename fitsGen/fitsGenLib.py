#$Id$
def generate(env, **kw):
    if not kw.get('depsOnly',0):
        env.Tool('addLibrary', library = ['fitsGen'])
	if env['PLATFORM'] == 'win32' and env.get('CONTAINERNAME','')=='GlastRelease':
	    env.Tool('findPkgPath', package = 'fitsGen') 
	    env.Tool('findPkgPath', package = 'embed_python') 
            env.Tool('findPkgPath', package = 'tip') 
            env.Tool('findPkgPath', package = 'astro') 
            env.Tool('findPkgPath', package = 'facilities') 
    env.Tool('facilitiesLib')
    env.Tool('tipLib')
    env.Tool('astroLib')
    env.Tool('embed_pythonLib')
    env.Tool('evtUtilsLib')
    env.Tool('addLibrary', library = env['rootGuiLibs'])
    if kw.get('incsOnly', 0) == 1: 
        env.Tool('findPkgPath', package = 'embed_python') 
        env.Tool('findPkgPath', package = 'tip') 
        env.Tool('findPkgPath', package = 'astro') 
        env.Tool('findPkgPath', package = 'facilities') 
        env.Tool('findPkgPath', package = 'evtUtils') 

def exists(env):
    return 1
