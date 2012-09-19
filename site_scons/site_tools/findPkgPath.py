import os,platform,os.path

def generate(env, **kw):
    pkgName = kw.get('package', '')
    if pkgName == '': 
        print 'findPkgPath called with no arg'
        return None
    path = None

    # paths all start with .\ so strip it off
    #if forStatic.first == True: 
    #    print 'findPkgPath called with argument ', pkgName
    for p in env['packageNameList']:
        bname = os.path.basename(str(p[2:]))
        if pkgName == bname: 
            if env.GetOption('supersede') == '.':
                path = '#' + str(p[2:])
            else: path = pkgName
        if pkgName + '-' == bname[:len(bname)+1]:
            if env.GetOption('supersede') == '.': 
                    path = '#' + str(p[2:])       
            else: path = pkgName

    if path == None and env.GetOption('supersede') != '.': # look in base
        for p in env['basePackageNameList']:
            bname = os.path.basename(str(p[2:]))
            ##if pkgName == bname: path = '#' + str(p[2:])
            if pkgName == bname: 
                path = os.path.join(env['absBasePath'], str(p[2:]))
            if pkgName + '-' == bname[:len(bname)+1]: 
                path = os.path.join(env['absBasePath'], str(p[2:]))
    if path != None:
        env.AppendUnique(CPPPATH = [path])

def exists(env):
    return 1