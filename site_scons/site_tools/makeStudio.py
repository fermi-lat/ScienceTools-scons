#  $Id$
import os, os.path, pprint
from SCons.Script import *
from fermidebug import fdebug

# to see debug messages, set env SCONS_FERMI_DEBUG to something.
# '-' for printing to console; other value for file

# Generate project files for each library and executable,
# and single solution file referencing them all (and also
# project files for any libraries on which they depend)

# Interface is similar to registerObjects except must supply
# per-lib and per-executable environments.  And there is no reason
# to separate testApps from binaries.  Solution file references all
# of them, so make project files for all and use same set of aliases

## Recognized keywords are
##        package - string
##        libraryCxts - list of 2-item lists: [library node, env]
##        staticLibraryCxts - list of 2-item lists: [library node, env]
##        swigLibraryCxts - list of 2-item lists: [library node, env]
##        programCxts  - list of 2-item lists: [program node, env]
##        testAppCxts  - list of 2-item lists: [program node, env]
##        includes - list of file paths
##        data     - list of file paths
##        xml      - list of file paths
##        pfiles
##        python
##        installFiles - list of pairs: [source file, dest file]

## define Action makeInstallScript which, given package name and 
## install dict (and
## sources = single array of first elt of pair for all the arrays in
## install dict), writes a file to do all the xcopy's .  The file will 
## typically be called something like installPkgname.bat and should
## be written to same directory as project files.
def makeInstallScript(target, source, env):
    instFiles = env['installFiles']
    try:
        f = open(str(target[0]), 'w')
        if len(instFiles) == 0: 
            #  write a no-op file
            f.write("REM No files to install for this package \n")
        else:
            for p in instFiles:
                f.write("xcopy /S /Y /F " + str(p[0]) + "  " + str(p[1]) + "\n")
    except IOError:
        print "Failed file operation on ", target[0]
    
    f.close()  

def _isSomething(s, something):
    cmps = s.split(".")
    if (len(cmps) == 2) and (cmps[1] == something):
        return cmps[0]
    return ''

def generate(env, **kw):
    # Will contain list of extra libraries, required by package targets,
    # which belong to other packages.  These projects have to appear
    # in solution file

    # return True if extension is not one of 'C', 'c', 'cxx', 'cpp'
    def notC(path):
        exclude = ['C', 'c', 'cxx', 'cpp']
        cmps = path.split('.')
        if cmps[len(cmps) - 1] in exclude:
            return False
        else: return True
    
    if 'ProjectInstallFiles' not in env['BUILDERS']:
        projectInstallBuilder = Builder(action = makeInstallScript, 
                                        suffix = '.bat')
        env.Append(BUILDERS = {'ProjectInstallFiles' : projectInstallBuilder})

# get list of includes
    absmisc = []
    absincs = []
    pkgname = kw.get('package', '')
    if pkgname == '': return
    elif pkgname == "*ALL*":
        allSln = env.Command(os.path.join(str(env['STUDIODIR']), 'all.sln'), 'slns', 
                             'site_scons\site_tools\writeAllSln.py ' + str(env['STUDIODIR']) + ' ' + str(env['BASESTUDIODIR']) )
        allGleamSln = env.Command(os.path.join(str(env['STUDIODIR']), 'allGleam.sln'), 'slns', 
                             'site_scons\site_tools\writeAllSln.py ' + str(env['STUDIODIR']) + ' ' + str(env['BASESTUDIODIR']) + ' Gleam' )
        env.Alias('StudioFiles', [allSln, allGleamSln])
        return

    pkgroot = os.path.dirname(str(File("SConscript").srcnode().abspath))
                   
    if kw.get('includes', '') != '':
        for header in kw.get('includes'):
            absincs.append(File(header).srcnode().abspath)
    if kw.get('xml', '') !='':
        for xmlfile in kw.get('xml'):
            if notC(str(xmlfile)):
                absmisc.append(File(xmlfile).srcnode().abspath)
    if kw.get('python', '') !='':
        for pyfile in kw.get('python'):
            absmisc.append(File(pyfile).srcnode().abspath)
    if kw.get('pfiles', '') !='':
        for pfile in kw.get('pfiles'):
            absmisc.append(File(pfile).srcnode().abspath)
    if kw.get('data', '') !='':
        for datafile in kw.get('data'):
            if notC(str(datafile)):
                absmisc.append(File(datafile).srcnode().abspath)

    installHandled = True
    if kw.get('installFiles', '') != '':
        # define the install script target
        env['installFiles'] = kw.get('installFiles')
        installSrcs = []
        for p in kw.get('installFiles') : installSrcs.append(p[0])
        installscriptPath = Dir(env['STUDIODIR']).File(pkgname + "Install.bat")
        installscript = env.ProjectInstallFiles(installscriptPath, installSrcs)
        installHandled = False

    # targetNames will contain list of projects (abs path to file)
    # input to MSVSSolution( )
    targetNames = []
    # extras is a similar list for project files built by other packages.
    # When complete it will be added to targetNames
    extras=[]
    # libSets is a dict of sets, one for each project file passed to
    # MSVSSolution.  Key is project name (so has "Lib" appended for libraries)
    libSets = {}
    
    ourLibSet = set([])
    #print "in makeStudio extlibSet from environement is: ", env['extlibSet']
    fdebug("in makeStudio extlibSet from environement is: %s" % str(env['extlibSet']) )
    cxts = kw.get('libraryCxts','')
    libdirstring = env['LIBDIR']

    for c in cxts:
        for libentry in c[0]:
            fname = (os.path.split(libentry.path))[1]
            fnameOnly = _isSomething(fname, 'lib')
            if fnameOnly != '': ourLibSet.add(fnameOnly+'Lib')

    for cxt in cxts:
        #print 'library list in cxt is: ', cxt[0]
        fdebug('library list in cxt is: %s ' % cxt[0])
        
        for libentry in cxt[0]:
            myLibName = (os.path.split(libentry.path))[1]
            # ignore non-libraries (e.g. manifest). Must end in ".dll"
            myLibNameOnly =  _isSomething(myLibName, 'dll') 
            if myLibNameOnly != '':
                #print 'Good myLibName is: ', myLibName
                targ = myLibNameOnly + 'Lib' + env['MSVSPROJECTSUFFIX']
                buildtarg = myLibName
                if installHandled == False:
                    instscript = str(installscriptPath)
                    installHandled = True
                else: instscript = ''
                cxt[1]['outdir'] = libdirstring
                projectFile=cxt[1].MSVSProject(target=targ,
                                               incs=absincs,
                                               misc=absmisc,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='dll',
                                               packageroot=pkgroot,
                                               installScript = instscript,
                                               auto_build_solution=0)
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                if instscript != '':
                    env.Depends(projectFile, installscript)
                # Compute installed abs path; needed for sln file
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))
                #print 'length of targetNames for libs: ', len(targetNames)
                
                if cxt[1].has_key('LIBS'):
                    libList = []               #3 addition
                    libList += cxt[1].get('LIBS')   #3 mod
                    libSet = set(libList) - env['extlibSet']
                    libSets[myLibNameOnly+'Lib'] = libSet
                    #print 'Libset for ', cxt[0], ' is ', libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            # if extraLib isn't one of pkg libs..
                            eLibIx = libList.index(extraLib)
                            eLibSet = set(libList[eLibIx+1:]) - env['extlibSet']
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)
                    
                env.Alias(kw.get('package'), projectInstalled)
                env.Default(projectInstalled)
                env.Alias('StudioFiles', projectInstalled)
                env.Alias('projectFiles', projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
                
    # static libraries   -----------------------------------------------
    cxts = kw.get('staticLibraryCxts','')

    for c in cxts:
        #print 'makeStudio static lib handling. len(c[0]) is ', len(c[0])
        for libentry in c[0]:
            fname = (os.path.split(libentry.path))[1]
            fnameOnly = _isSomething(fname, 'lib')
            if fnameOnly != '': ourLibSet.add(fnameOnly+'Lib')

    #print 'makeStudio called with len(staticLibraryCxts) = ', len(cxts)
    for cxt in cxts:
        #print 'library list in cxt is: ', cxt[0]
        
        for libentry in cxt[0]:
            myLibName = (os.path.split(libentry.path))[1]
            # ignore non-libraries (e.g. manifest). Must end in ".lib"
            myLibNameOnly =  _isSomething(myLibName, 'lib') 
            if myLibNameOnly != '':
                #print 'Good myLibName is: ', myLibName
                targ = myLibNameOnly + 'Lib' + env['MSVSPROJECTSUFFIX']
                buildtarg = myLibName
                if installHandled == False:
                    instscript = str(installscriptPath)
                    installHandled = True
                else: instscript = ''

                cxt[1]['outdir'] = libdirstring
                projectFile=cxt[1].MSVSProject(target=targ,
                                               #!srcs=sourcenames,
                                               incs=absincs,
                                               packageroot=pkgroot,
                                               misc=absmisc,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='lib',
                                               installScript = instscript,
                                               auto_build_solution=0)
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                if instscript != '':
                    env.Depends(projectFile, installscript)
                    
                # Compute installed abs path; needed for sln file
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))
                #print 'length of targetNames for libs: ', len(targetNames)
                
                if cxt[1].has_key('LIBS'):
                    libList = []        #3 line added
                    libList += cxt[1].get('LIBS')  #3 line modified
                    libSet = set(libList) - env['extlibSet']
                    libSets[myLibNameOnly+'Lib'] = libSet
                    #print 'Libset for ', cxt[0], ' is ', libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            # if extraLib isn't one of pkg libs..
                            eLibIx = libList.index(extraLib)
                            eLibSet = set(libList[eLibIx+1:]) - env['extlibSet']
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)
                    
                env.Alias(kw.get('package'), projectInstalled)
                env.Default(projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
                env.Alias('StudioFiles', projectInstalled)
                env.Alias('projectFiles', projectInstalled)

    # end static libraries   --------------------------

    # Now for swig libraries, if any
    cxts = kw.get('swigLibraryCxts','')
    for c in cxts:
        for libentry in c[0]:
            fname = (os.path.split(libentry.path))[1]
            fnameOnly = _isSomething(fname, 'lib')
            if fnameOnly != '': ourLibSet.add(fnameOnly+'Lib')
            
    for cxt in cxts:
        #print 'swig library list in cxt is: ', cxt[0]
        
        for libentry in cxt[0]:
            myLibName = (os.path.split(libentry.path))[1]
            # ignore non-libraries (e.g. manifest). Must end in ".dll"
            myLibNameOnly =  _isSomething(myLibName, 'dll') 
            if myLibNameOnly != '':
                targ = myLibNameOnly + 'Lib' + env['MSVSPROJECTSUFFIX']
                buildtarg = myLibName
                
                # Swig libraries only depend on one source: the .i file
                #sources = env.FindSourceFiles(node=cxt[0])
                absmisc = []
                

                cxt[1]['outdir'] = libdirstring
                projectFile=cxt[1].MSVSProject(target=targ,
                                               misc=absmisc,
                                               packageroot=pkgroot,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='swigdll',
                                               auto_build_solution=0)
                                              
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                    
                # Compute installed abs path; needed for sln file
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))
                if cxt[1].has_key('LIBS'):
                    libList = []  
                    libList += cxt[1].get('LIBS') 
                    libSet = set(libList) - env['extlibSet']
                    libSets[myLibNameOnly+'Lib'] = libSet
                    #print 'Libset for ', cxt[0], ' is ', libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            # if extraLib isn't one of pkg libs..
                            eLibIx = libList.index(extraLib)
                            eLibSet=set(libList[eLibIx+1:]) - env['extlibSet']
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)
                    
                env.Alias(kw.get('package'), projectInstalled)
                env.Default(projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
                env.Alias('projectFiles', projectInstalled)
                env.Alias('StudioFiles', projectInstalled)

    # root shared libraries. Have to get the rootcint command in there
    cxts = kw.get('rootcintSharedCxts','')
    for c in cxts:
        for libentry in c[0]:
            fname = (os.path.split(libentry.path))[1]
            fnameOnly = _isSomething(fname, 'lib')
            if fnameOnly != '': ourLibSet.add(fnameOnly+'Lib')
            
    #  Copied from "regular" library handling; adjusted.
    #  May need further editing!!
    for cxt in cxts:
        if not cxt[1].has_key('rootcint_node'):
            print 'Cannot build root library without specifying rootcint node'
            continue
        for libentry in cxt[0]:

            myLibName = (os.path.split(libentry.path))[1]
            # ignore non-libraries (e.g. manifest). Must end in ".dll"
            myLibNameOnly =  _isSomething(myLibName, 'dll') 
            if myLibNameOnly != '':
                targ = myLibNameOnly + 'Lib' + env['MSVSPROJECTSUFFIX']
                buildtarg = myLibName
                if installHandled == False:
                    instscript = str(installscriptPath)
                    installHandled = True
                else: instscript = ''
                cxt[1]['packageName'] = pkgname
                cxt[1]['outdir'] = libdirstring
                projectFile=cxt[1].MSVSProject(target=targ,
                                               incs=absincs,
                                               packageroot=pkgroot,
                                               misc=absmisc,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='rootcintdll',
                                               rootcintnode=cxt[1]['rootcint_node'],
                                               installScript = instscript,
                                               auto_build_solution=0)
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                if instscript != '':
                    env.Depends(projectFile, installscript)

                # Compute installed abs path; needed for sln file
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))
                if cxt[1].has_key('LIBS'):
                    libList = []    
                    libList += cxt[1].get('LIBS')
                    libSet = set(libList) - env['extlibSet']
                    libSets[myLibNameOnly+'Lib'] = libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            # if extraLib isn't one of pkg libs..
                            eLibIx = libList.index(extraLib)
                            eLibSet = set(libList[eLibIx+1:]) - env['extlibSet']
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)
                    
                env.Alias(kw.get('package'), projectInstalled)
                env.Default(projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
                env.Alias('projectFiles', projectInstalled)
                env.Alias('StudioFiles', projectInstalled)

    # end rootcint shared

    # root static libraries. Have to get the rootcint command in there
    cxts = kw.get('rootcintStaticCxts','')
    for c in cxts:
        for libentry in c[0]:
            fname = (os.path.split(libentry.path))[1]
            fnameOnly = _isSomething(fname, 'lib')
            if fnameOnly != '': ourLibSet.add(fnameOnly+'Lib')
            
    #  Copied from "regular" library handling; adjusted.
    #  May need further editing!!
    for cxt in cxts:
        if not cxt[1].has_key('rootcint_node'):
            print 'Cannot build root library without specifying rootcint node'
            continue
        for libentry in cxt[0]:

            myLibName = (os.path.split(libentry.path))[1]
            # ignore non-libraries (e.g. manifest). Must end in ".lib"
            myLibNameOnly =  _isSomething(myLibName, 'lib') 
            if myLibNameOnly != '':
                targ = myLibNameOnly + 'Lib' + env['MSVSPROJECTSUFFIX']
                buildtarg = myLibName
                ## Probably need to add arg. for rootcint node
                cxt[1]['packageName'] = pkgname
                cxt[1]['outdir'] = libdirstring
                if installHandled == False:
                    instscript = str(installscriptPath)
                    installHandled = True
                else: instscript = ''

                projectFile=cxt[1].MSVSProject(target=targ,
                                               packageroot=pkgroot,
                                               incs=absincs,
                                               misc=absmisc,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='rootcintlib',
                                               rootcintnode=cxt[1]['rootcint_node'],
                                               installScript = instscript,
                                               auto_build_solution=0)
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                if instscript != '':
                    env.Depends(projectFile, installscript)
    
                #Compute installed abs path; needed for sln file
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))
                if cxt[1].has_key('LIBS'):
                    libList = [] 
                    libList += cxt[1].get('LIBS')
                    libSet = set(libList) - env['extlibSet']
                    libSets[myLibNameOnly+'Lib'] = libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            # if extraLib isn't one of pkg libs..
                            eLibIx = libList.index(extraLib)
                            eLibSet = set(libList[eLibIx+1:]) - env['extlibSet']
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)
                    
                env.Alias(kw.get('package'), projectInstalled)
                env.Default(projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
                env.Alias('projectFiles', projectInstalled)
                env.Alias('StudioFiles', projectInstalled)

    # end rootcint static

    # Do the same thing for binaries as for testApps
    # For now don't specify output dir for apps
    # env['outdir'] = ''
    cxts = []
    if kw.get('testAppCxts','') != '':
        cxts.extend(kw.get('testAppCxts'))
    if kw.get('binaryCxts', '') != '':
        cxts.extend(kw.get('binaryCxts'))
    for cxt in cxts:
        # ignore anything which isn't an actual app (e.g. manifest)
        # must end in ".exe"

        for appentry in cxt[0]:
            myAppName = (os.path.split(appentry.path))[1]
            myAppNameOnly = _isSomething(myAppName, 'exe')
            if myAppNameOnly != '':

                targ = myAppNameOnly + env['MSVSPROJECTSUFFIX']
                buildtarg = myAppName
                
                cxt[1]['outdir'] = env['BINDIR']
                projectFile=cxt[1].MSVSProject(target=targ,
                                               incs=absincs,
                                               packageroot=pkgroot,
                                               misc=absmisc,
                                               variant=env['VISUAL_VARIANT'],
                                               buildtarget=buildtarg,
                                               targettype='exe',
                                               auto_build_solution=0)
                projectInstalled = env.Install(env['STUDIODIR'], projectFile)
                targsrc = os.path.join(str(env['STUDIODIR']), targ)
                targetNames.append(env.subst(targsrc))

                if cxt[1].has_key('LIBS'):
                    libList = []
                    libList += cxt[1]['LIBS']
                    libSet = set(libList) - env['extlibSet']
                    #print 'after removing externals: ', libSet
                    libSets[myAppNameOnly] = libSet
                    # Look for 'extra' projects which sln has to refer to,
                    # whose project files will be built by other packages
                    for extraLib in libSet:
                        extraLibProj = extraLib + 'Lib'
                        if extraLibProj not in set(extras).union(ourLibSet):
                            eLibIx = libList.index(extraLib)
                            eLibSet = (set(libList[eLibIx+1:]) - env['extlibSet'])
                            libSets[extraLibProj] = eLibSet
                            extras.append(extraLibProj)

                env.Alias(kw.get('package'), projectInstalled)
                env.Alias('projectFiles', projectInstalled)
                env.Alias('StudioFiles', projectInstalled)
                env.Alias(pkgname+'-StudioFiles', projectInstalled)
    
    if installHandled == False:   #need to make special project file
        targ = pkgname + 'InstallSrc' + env['MSVSPROJECTSUFFIX']

        projectFile = env.MSVSProject(target=targ,
                                      packageroot =pkgroot,
                                      incs=absincs,
                                      misc=absmisc,
                                      variant=env['VISUAL_VARIANT'],
                                      #buildtarget = ["*DUMMY*"],
                                      targettype="install",
                                      installScript = str(installscriptPath),
                                      auto_build_solution=0)
        env.Depends(projectFile, installscript)
        projectInstalled = env.Install(env['STUDIODIR'], projectFile)
        env.Alias(pkgname, projectInstalled)
        env.Alias('projectFiles', projectInstalled)
        env.Alias('StudioFiles', projectInstalled)
        env.Alias(pkgname+'-StudioFiles', projectInstalled)
        targsrc = os.path.join(str(env['STUDIODIR']), targ)
        targetNames.append(env.subst(targsrc))

    slnTarget = kw.get('package') + env['MSVSSOLUTIONSUFFIX']
        
    # Last thing:  add entries to projects for the extras.
    # These entries need full absolute path, so prepend dir, append .vcproj
    for extra in extras:
        # if supersede area, then don't include references to packages
        # *not* in the supersede area.  They're like externals.
        if env.GetOption('supersede') != '.':
            extraPck = extra[:len(extra) - 3] # take off Lib suffix
            #print "Found extraPck ", extraPck
            if not (extraPck in env['packageNameList']): 
                #print "Did not find ", extraPck, " in packageNameList"
                continue
            #else: print extraPck, " was in packageNameList"

        extra += env.subst(env['MSVSPROJECTSUFFIX'])
        targetNames.append(os.path.join(str(env['STUDIODIR']), extra) )
        print "Just appended ", extra

        
    #print 'targetNames count when including extras: ', len(targetNames)
    #print 'For package ', pkgname, 'projects are: ', targetNames
    #print 'libSets are: ', libSets

    # Note MSVSSolution has to extract filename (only; no dir, no extension)
    # to use to look up assoc. libs in libSets
    if len(targetNames) > 0:
        #print "Target (projects) count is ", len(targetNames)
        fdebug('For package %s  about to make solution file with target count = %s '  % (pkgname, len(targetNames)))
        slnFile=env.MSVSSolution(target=slnTarget, projects=targetNames,
                                 variant=env['VISUAL_VARIANT'], libs=libSets,
                                 installDir=str(env['STUDIODIR']))

        slnsrc = os.path.join(str(env['STUDIODIR']), slnTarget)
        slnInstalled = env.Install(env['STUDIODIR'], slnTarget)

        env.Alias(kw.get('package'), slnInstalled)
        env.Alias('slns', slnInstalled)
        env.Alias('StudioFiles', slnInstalled)
        env.Alias(pkgname+'-StudioFiles', slnInstalled)

def handleLib(libentry='', libtype=''):
    # Would be nice to factor out some of the common lib-handling code
    # for dll's, static libs, swig libs, root libs
    return 1;

def exists(env):
    return 1;
