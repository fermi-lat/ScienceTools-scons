import os
import SCons.Action
import SCons.Builder

pyWrapper = '''#!/bin/sh
# Autogenerated by SCons; do not edit!

${REPLACE-PY-WRAPPER}

export OUR_BINDIR=${REPLACE-SCRIPTDIR}

source ${OUR_BINDIR}/_setup.sh
${REPLACE-OUR-SETUP}

${REPLACE-PY-EXECUTE}
'''



# Helper functions to determine relative paths
# Obtained from http://code.activestate.com/recipes/208993/
# and then shamelessly stolen from generateScript
def commonpath(l1, l2, common=[]):
    if len(l1) < 1: return (common, l1, l2)
    if len(l2) < 1: return (common, l1, l2)
    if l1[0] != l2[0]: return (common, l1, l2)
    return commonpath(l1[1:], l2[1:], common+[l1[0]])

def relpath(p1, p2):
    (common,l1,l2) = commonpath(p1.split(os.path.sep), p2.split(os.path.sep))
    p = []
    if len(l1) > 0:
        p = [ '../' * len(l1) ]
    p = p + l2
    return os.path.join( *p )

## Fill contents of   scripts (wrapping python) for an SCons installation
def fillOurScript(scriptFile, env, scriptTemplate, executable):
    finalScript = scriptTemplate.get_contents()

    finalScript = finalScript.replace('${REPLACE-PY-WRAPPER}', 'export INST_DIR=`dirname $0`\nexport INST_DIR=`cd $INST_DIR/../../; pwd`\n')

    finalScript = finalScript.replace('${REPLACE-PY-EXECUTE}',
                                      'python '+os.path.join('$INST_DIR', relpath(env.Dir(env.GetOption('supersede')).abspath, executable.abspath)+' "$@"\n'))
    scriptDir = os.path.join('$INST_DIR', relpath(env.Dir(env.GetOption('supersede')).abspath, env['SCRIPTDIR'].abspath))
    finalScript = finalScript.replace('${REPLACE-SCRIPTDIR}', scriptDir)
    print "OUR_SETUP_NAME is:  ", env['OUR_SETUP_NAME']
    if env['OUR_SETUP_NAME'] != '':
        finalScript = finalScript.replace('${REPLACE-OUR-SETUP}',
                                          'source ' + env['OUR_SETUP_NAME'] + '.sh')
    else:
        finalScript = finalScript.replace('${REPLACE-OUR-SETUP}', ' ')
        
    scriptFile.write(finalScript)

def generate(env, **kw):
    # maybe complain if no keyword arg called 'ourSetupName' or if it's empty ?
    env['OUR_SETUP_NAME'] = kw.get('ourSetupName', '')
    print "kw.get yields ourSetupName = ", kw.get('ourSetupName', '')

    def createWrapper(target = None, source = None, env = None):

        scriptTemplate = source.pop(0)
        for trgt in target:
            scriptFile = open(str(trgt), 'w')
            fillOurScript(scriptFile, env, scriptTemplate, source.pop(0))
            scriptFile.close()
        return 0

    def createWrapperEmitter(target, source, env):
        target = []

        for src in source:
            target.append(env['SCRIPTDIR'].File(os.path.splitext(os.path.basename(src.abspath))[0]+ '.sh'))

        source = [ env.Value(pyWrapper) ] + source
        return (target, source)

    def createWrapperGenerator(source, target, env, for_signature):
        actions = [env.Action(createWrapper,
                              "Creating  wrapper for python files '$TARGETS'")]
        for trgt in target:
            actions.append(SCons.Defaults.Chmod(trgt, 0755))
        return actions

    print "Inside generatePythonWrapper::generate"
    env.Append(BUILDERS = {'GeneratePythonWrapper' : env.Builder(generator = createWrapperGenerator,
                                                                 emitter = createWrapperEmitter)})

def exists(env):
    return 1;

                                                
