"""
This module searches the Science Tools and external libraries
directories for CMT packages and version numbers.  From these, it sets
the environment variables needed by the various packages following CMT
conventions.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""
import os, glob, tkFileDialog, Tkinter

def instDialog(title):
    x = InstallationDirDialog(title)
    place = x.go()
    x.destroy()
    return place

class InstallationDirDialog(Tkinter.Tk):
    def __init__(self,title):
        Tkinter.Tk.__init__(self)
        self.title=title
        self.withdraw()
    def go(self):
        return tkFileDialog.askdirectory(mustexist=1, title=self.title)

def checkEnviron():
    try:
        ST_INST = os.environ['ST_INST']
    except:
        ST_INST = instDialog("ST_INST: Where are the GLAST programs?")
        try:
            os.environ['ST_INST'] = ST_INST
        except:
            print 'Environmental variable ST_INST is not set properly.'
            assert 0
    try:
        GLAST_EXT = os.environ['GLAST_EXT']
    except:
        GLAST_EXT = instDialog("GLAST_EXT: " + 
                               "Where are the GLAST external packages?")
        try:
            os.environ['GLAST_EXT'] = GLAST_EXT
        except:
            print 'Environmental variable GLAST_EXT is not set properly.'
            assert 0
            
def ST_packages():
    packages = find_packages(os.environ['ST_INST'], 'v*')
    try:
        del packages['IExternal']
    except:
        pass
    return packages

def cmtconfig():
    if os.name == 'posix':
        return os.path.basename(os.environ['GLAST_EXT'])
    elif os.name == 'nt':
        return 'VC7debug'
    
def find_packages(basepath, pattern='*'):
    my_packages = {}
    packages = os.listdir(basepath)
    for package in packages:
        versions = glob.glob(basepath + os.sep + package + os.sep + pattern)
        versions = [os.path.basename(x) for x in versions]
        versions.sort()
        try:
            my_packages[package] = versions[-1]
        except:
            pass
    return my_packages

def setEnviron():
    #
    # Science Tools
    #
    ST_INST = os.environ['ST_INST']
    GLAST_EXT = os.environ['GLAST_EXT']
    packages = ST_packages()
    for name in packages.keys():
        os.environ[name.upper() + 'ROOT'] = os.sep.join( (ST_INST,
            name, packages[name]) )
        os.environ[name.upper() + 'CONFIG'] = cmtconfig()
    #
    # IExternal
    #
    packages = find_packages(ST_INST + os.sep + 'IExternal', 'v*')
    for name in packages.keys():
        os.environ[name.upper() + 'ROOT'] = os.sep.join((ST_INST, 'IExternal',
                                                         name, packages[name]))
        os.environ[name.upper() + 'CONFIG'] = cmtconfig()
    #
    # External libraries
    #
    packages = find_packages(GLAST_EXT)
    for name in packages.keys():
        os.environ[name.upper() + '_DIR'] = GLAST_EXT + os.sep + name
    #
    # Special cases
    #
    os.environ["INFILES"] = os.path.join(os.environ['GOODIROOT'], 'Data')
    os.environ["TPLDIR"] = os.path.join(os.environ['GOODIROOT'], 'Templates')
    os.environ['BINDIR'] = cmtconfig()
    os.environ['EXT_DIR'] = GLAST_EXT
    os.environ['EXTPACK_DIR'] = GLAST_EXT
    #os.environ['ROOTSYS'] = os.path.join(GLAST_EXT, 'ROOT/v3.04.02/root')
    if ('ROOTSYS' not in os.environ
        or os.environ['ROOTSYS'].find(GLAST_EXT) != 0):
        #
        # Find largest version number in ROOT subdirectory
        #
        rootnames = os.listdir(os.path.join(GLAST_EXT, "ROOT"))
        rootnames = [x for x in rootnames if x[0] == 'v']
        rootnames.sort()
        rootver = rootnames.pop()
        os.environ['ROOTSYS'] = os.path.join(GLAST_EXT, 'ROOT',
                                             rootver, 'root')
    os.environ["XERCES_DIR"] = os.path.join(GLAST_EXT, "Xerces-C")
    if os.name == 'posix':
        os.environ['XMLEXT_DIR'] = os.path.join(GLAST_EXT, 'xerces/1.7.0')
        os.environ['CFITSIO_DIR'] = os.path.join(GLAST_EXT, 'cfitsio/v2410')
        LD_LIBRARY_PATH = os.pathsep.join(
            (os.path.join(ST_INST, "lib"),
             os.path.join(os.environ['XMLROOT'], cmtconfig()),
             os.path.join(os.environ["XMLEXT_DIR"], "lib"),
             os.path.join(GLAST_EXT, "CLHEP/1.8.0.0/lib"),
             os.path.join(os.environ['ROOTSYS'], "bin"),
             os.path.join(os.environ['ROOTSYS'], "lib"),
             os.path.join(os.environ['CFITSIO_DIR'], "lib"),
             os.path.join(os.environ['GOODIROOT'], cmtconfig())) )
    elif os.name == 'nt':
        os.environ["XMLEXT_DIR"] = os.path.join(GLAST_EXT,
                                                "xerces-c1_7_0-win32")
        os.environ["CFITSIO_DIR"] = os.path.join(GLAST_EXT, "cfitsio_v2410")
        LD_LIBRARY_PATH = os.pathsep.join(
            (os.path.join(os.environ['XMLROOT'], cmtconfig()),
             os.path.join(GLAST_EXT, "xerces-c1_7_0-win32/bin"),
             os.path.join(GLAST_EXT, "cfitsio_v2410/lib/Debug"),
             os.path.join(os.environ['XMLEXT_DIR'], "bin"),
             os.path.join(GLAST_EXT, "CLHEP/1.8.0.0/Win32/lib"),
             os.path.join(os.environ['ROOTSYS'], "bin"),
             os.path.join(os.environ['ROOTSYS'], "lib"),
             os.path.join(os.environ['CFITSIO_DIR'], "lib/Debug"),
             os.path.join(os.environ['GOODIROOT'], cmtconfig())) )
        os.environ['PATH'] += os.pathsep + LD_LIBRARY_PATH  
        os.environ['EDITOR'] = "start wordpad"
    os.environ["LD_LIBRARY_PATH"] = LD_LIBRARY_PATH
    
def make_setup(filename):
    file = open(filename, 'w')
    for name in os.environ.keys():
        if name != "LS_COLORS":
            file.write('setenv %s "%s"\n' % (name, os.environ[name]))
    file.close()
    
checkEnviron()

setEnviron()

if __name__ == "__main__":
    if os.name == 'posix': make_setup('setup_ST.csh')
