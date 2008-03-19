#include "facilities/commonUtilities.h"
#include <iostream>
#include <algorithm>

#ifndef WIN32
extern char **environ;
#endif

namespace facilities {
  std::string commonUtilities::getPackagePath(const std::string &package){
#ifdef SCons
    return joinPath(commonUtilities::getPackageRoot(package), package);
#else
    return commonUtilities::getPackageRoot(package);
#endif
  }

  std::string commonUtilities::getDataPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string dataPath = joinPath(packageRoot, "data");
    return joinPath(dataPath, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\data";
#else
    return packageRoot+"/data";
#endif
#endif
  }

  std::string commonUtilities::getXmlPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string xmlLocation = joinPath(packageRoot, "xml");
    return joinPath(xmlLocation, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\xml";
#else
    return packageRoot+"/xml";
#endif
#endif
  }

  std::string commonUtilities::getPfilesPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string pfilesLocation = joinPath(packageRoot, "pfiles");
    return joinPath(pfilesLocation, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\pfiles";
#else
    return packageRoot+"/pfiles";
#endif
#endif
  }

  void commonUtilities::setEnvironment(const std::string &name, const std::string &value, bool overwrite){
    if(getenv(name.c_str())==NULL || overwrite){
#ifdef WIN32
      _putenv((name+"="+value).c_str());
#else
      setenv(name.c_str(), value.c_str(),1);
#endif
    }
  }

  std::string commonUtilities::getEnvironment(const std::string &name){
    const char *env = getenv(name.c_str());
    std::string toReturn;
    if(env!=NULL)
      toReturn = env;
    return toReturn;
  }

  std::string commonUtilities::getPackageRoot(const std::string &package){
    std::string packageRoot;
#ifdef SCons
    const char *env = getenv("INST_DIR");
    if(env != NULL)
      packageRoot = env;
#else
    std::string upperCase=package;
    transform(upperCase.begin(),upperCase.end(),upperCase.begin(),(int(*)(int)) toupper);
    upperCase=upperCase+"ROOT";
    const char *env = getenv(upperCase.c_str());
    if(env!=NULL)
      packageRoot = env;
#endif
    //  For now insist there be a translation for package root
    /*
      if(packageRoot == ""){
      env = getenv("INST_DIR");
      if(env!=NULL)
        packageRoot = env;
      }
    */
    //if(packageRoot == ""){
    //  std::cerr<<"Unable to determine data path for "<<package<<std::endl;
    //}
    return packageRoot;
  }

  std::string commonUtilities::joinPath(const std::string &first, const std::string &second){
#ifdef WIN32
    return first+"\\"+second;
#else
    return first+"/"+second;
#endif
  }

  void commonUtilities::setupEnvironment(){
#ifdef SCons
#ifdef ScienceTools
    setEnvironment("CALDB", joinPath(joinPath(joinPath(getDataPath("caldb"), "data"), "glast"), "lat"));
    setEnvironment("CALDBCONFIG", joinPath(joinPath(joinPath(getDataPath("caldb"), "software"), "tools"), "caldb.config"));
    setEnvironment("CALDBALIAS", joinPath(joinPath(joinPath(getDataPath("caldb"), "software"), "tools"), "alias_config.fits"));
#endif
#else
    if(environ!=NULL){
      int counter=0;
      while(environ[counter]!=NULL){
	std::string env = environ[counter];
	std::string::size_type pos = env.find_first_of('=');
	if(pos != std::string::npos){
	  env = env.substr(0, pos);
	  pos = env.rfind("ROOT");
	  if(pos != std::string::npos && pos+4 == env.size()){
	    env = env.substr(0, pos);
	    if(env != ""){
	      setEnvironment(env+"XMLPATH", getXmlPath(env));
	      setEnvironment(env+"DATAPATH", getDataPath(env));
	      setEnvironment(env+"PFILESPATH", getPfilesPath(env));
	    }
	  }
	}
	counter++;
      }
    }
#endif
  }
}

