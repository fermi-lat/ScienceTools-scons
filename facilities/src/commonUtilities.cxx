#include "facilities/commonUtilities.h"
#include <iostream>
#include <algorithm>
#ifndef WIN32
#include <unistd.h>
#endif

namespace facilities {
  std::string commonUtilities::getPackagePath(const std::string &package){
    return commonUtilities::getPackageRoot(package);
  }

  std::string commonUtilities::getDataPath(const std::string &package){
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\data";
#else
    return packageRoot+"/data";
#endif
  }

  std::string commonUtilities::getXmlPath(const std::string &package){
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\xml";
#else
    return packageRoot+"/xml";
#endif
  }

  std::string commonUtilities::getPfilesPath(const std::string &package){
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
#ifdef WIN32
    return packageRoot+"\\pfiles";
#else
    return packageRoot+"/pfiles";
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
    std::string upperCase=package;
    transform(upperCase.begin(),upperCase.end(),upperCase.begin(),(int(*)(int)) toupper);
    upperCase=upperCase+"ROOT";
    const char *env = getenv(upperCase.c_str());
    std::string packageRoot;
    if(env!=NULL)
      packageRoot = env;
    //  For now insist there be a translation for package root
    /*
      if(packageRoot == ""){
      env = getenv("INST_DIR");
      if(env!=NULL)
        packageRoot = env;
      }
    */
    if(packageRoot == ""){
      std::cerr<<"Unable to determine data path for "<<package<<std::endl;
    }
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
    if(environ!=NULL){
      int counter=0;
      while(environ[counter]!=NULL){
	std::string env = environ[counter];
	env = env.substr(0, env.find_first_of('='));
	env = env.substr(0, env.find_last_of('ROOT'));
	setEnvironment(env+"XMLPATH", getXmlPath(env));
	setEnvironment(env+"DATAPATH", getDataPath(env));
	setEnvironment(env+"PFILESPATH", getPfilesPath(env));
	counter++;
      }
    }
  }
}

