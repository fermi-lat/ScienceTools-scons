#$Id$
import os,platform
## Add library dependencies or not, as appropriate for item to be built
## Keyword args are
##   package
##   toolname  (one of package, toolname must be present)
##   toBuild   (recognized values include 'static', 'shared',
##              'component', 'swig', 'rootlib', and 'program'.
##              Defaults to 'program')
##   depsOnly   (only acknowledged if toBuild == 'swig')
def generate(env, **kw):
    toolname = ''
    pkg =  kw.get('package', '')
    if pkg == '':
        if kw.get('toolname', '' ) == '':
            return
        else:
            toolname = kw.get('toolname')

    else:
        toolname = kw.get('toolname', pkg + 'Lib')

    toBuild = kw.get('toBuild','program')

    if toBuild == 'static':
        if env['PLATFORM'] == "win32" and env.get('CONTAINERNAME','') == 'GlastRelease':
            env.Tool(toolname, incsOnly = 1, depsOnly = 1)
        return
    elif toBuild == 'shared':
        if env['PLATFORM'] == "posix":
            return
        else:
            env.Tool(toolname, depsOnly = 1)

    elif toBuild == 'component':
        env.Tool(toolname, depsOnly = 1)

    elif toBuild == 'rootlib':
        env.Tool(toolname, depsOnly = 1)

    elif toBuild == 'program':
        env.Tool(toolname)

    elif toBuild == 'swig':
        depsOnlyValue = kw.get('depsOnly', 0)
        env.Tool(toolname, depsOnly = depsOnlyValue)
        env.Tool('addLibrary', library=env['pythonLibs'])
    else:
        msg = "ERROR: Unrecognized toBuild type: " + toBuild
        raise LookupError(msg)

def exists(env):
    return 1
