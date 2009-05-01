#include "facilities/commonUtilities.h"
#include <iostream>
#include <algorithm>
#if defined(SCons) || defined(HEADAS)
#include <sstream>
#include "config.h"
#endif

#if defined(__APPLE__)
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#ifndef WIN32
#if !defined(SCons) && !defined(HEADAS)
extern char **environ;
#endif
#endif

namespace facilities {
  std::string commonUtilities::getPackagePath(const std::string &package){
#ifdef SCons
    return joinPath(commonUtilities::getPackageRoot(package), package);
#else
#ifdef HEADAS
    return joinPath(commonUtilities::getPackageRoot(package), package);
#else
    return commonUtilities::getPackageRoot(package);
#endif
#endif
  }

  std::string commonUtilities::getDataPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string dataPath = joinPath(packageRoot, "data");
    return joinPath(dataPath, package);
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string dataPath = joinPath(packageRoot, "refdata");
    dataPath = joinPath(dataPath, "fermi");
    return joinPath(dataPath, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "data");
#endif
#endif
  }

  std::string commonUtilities::getXmlPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string xmlLocation = joinPath(packageRoot, "xml");
    return joinPath(xmlLocation, package);
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string xmlLocation = joinPath(packageRoot, "xml");
    xmlLocation = joinPath(xmlLocation, "fermi");
    return joinPath(xmlLocation, package);
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "xml");
#endif
#endif
  }

  std::string commonUtilities::getPfilesPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string pfilesLocation = joinPath(packageRoot, "pfiles");
    return joinPath(pfilesLocation, package);
#else
#ifdef HEADAS
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string pfilesLocation = joinPath(packageRoot, "syspfiles");
    return pfilesLocation;
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "pfiles");
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
#ifdef HEADAS
    const char *env = getenv("FERMI_INST_DIR");
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
#if defined(SCons) || defined(HEADAS)
    std::stringstream packages;
    packages << PACKAGES;
    while(!packages.eof()){
      std::string package, packageUpper;
      packages>>std::ws>>package>>std::ws;
      packageUpper = package;
      transform(packageUpper.begin(), packageUpper.end(), packageUpper.begin(), (int(*)(int)) toupper);
      setEnvironment(packageUpper+"XMLPATH", getXmlPath(package));
      setEnvironment(packageUpper+"DATAPATH", getDataPath(package));
    }
    setEnvironment("EXTFILESSYS", joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles));
#ifdef ScienceTools
    setEnvironment("CALDB", joinPath(joinPath(joinPath(getDataPath("caldb"), "data"), "glast"), "lat"));
    setEnvironment("CALDBCONFIG", joinPath(joinPath(joinPath(getDataPath("caldb"), "software"), "tools"), "caldb.config"));
    setEnvironment("CALDBALIAS", joinPath(joinPath(joinPath(getDataPath("caldb"), "software"), "tools"), "alias_config.fits"));
#ifdef HEADAS
    setEnvironment("TIMING_DIR", joinPath(getPackageRoot("timeSystem"), "refData"));
#else
    setEnvironment("TIMING_DIR", joinPath(joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles), "jplephem"));
#endif
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

