#include "facilities/commonUtilities.h"
#include <iostream>
#include <algorithm>
#if defined(SCons) || defined(HEADAS)
#include <sstream>
#include <fstream>
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
    std::string dataPath = joinPath(joinPath(packageRoot, "data"), package);

    if (pathFound(dataPath)) return dataPath;

    // Otherwise, might have supersede and our package could be in base
    const char *env = getenv("BASE_DIR");
    if (env == NULL) return "";
    packageRoot = std::string(env);

    dataPath = joinPath(joinPath(packageRoot, "data"), package);

    return (pathFound(dataPath)) ?  dataPath : "";
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

  std::string commonUtilities::getJobOptionsPath(const std::string &package) {
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string joPath = joinPath(joinPath(packageRoot, "jobOptions"), package);
    if (pathFound(joPath)) return joPath;
    // Otherwise, might have supersede and our package could be in base
    const char *env = getenv("BASE_DIR");
    if (env == NULL) return "";
    packageRoot = std::string(env);

    joPath = joinPath(joinPath(packageRoot, "jobOptions"), package);

    return (pathFound(joPath)) ?  joPath : "";
#else
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    if(packageRoot=="")
      return packageRoot;
    return joinPath(packageRoot, "src");
#endif
  }

  std::string commonUtilities::getXmlPath(const std::string &package){
#ifdef SCons
    std::string packageRoot = commonUtilities::getPackageRoot(package);
    std::string xmlPath = joinPath(joinPath(packageRoot, "xml"), package);

    if (pathFound(xmlPath)) return xmlPath;

    // Otherwise, might have supersede and our package could be in base
    const char *env = getenv("BASE_DIR");
    if (env == NULL) return "";
    packageRoot = std::string(env);

    xmlPath = joinPath(joinPath(packageRoot, "xml"), package);

    return (pathFound(xmlPath)) ?  xmlPath : "";

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
    std::string pfilesLocation = joinPath(packageRoot, "syspfiles");
    return pfilesLocation;
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
    if (env != NULL) {
      packageRoot = env; // ok if SConscript is open or is openable
    }      

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
#ifdef SCons
  bool commonUtilities::pathFound(const std::string& path) {
    // For the time being, don't even try for Windows since
    // it fails even when it shouldn't
#ifdef WIN32
    return true;
#endif
    // Check to see if rel path exists first for INST_DIR
    // if that fails, try BASE_DIR

    std::fstream filestr;
    filestr.open(path.c_str(), std::ios_base::in);
    if (filestr.is_open()) {  // all is well, just close the file
      filestr.close();
      return true;
    }
    return false;
  }
#endif

  void commonUtilities::setupEnvironment(){
#if defined(SCons) || defined(HEADAS)
    std::stringstream packages;
    packages << PACKAGES;
    while(!packages.eof()){
      std::string package, packageUpper;
      packages>>std::ws>>package>>std::ws;
      // 'package' may be of form myPkg-xx-yy-zz, in which case
      // must strip off suffix
      std::string nameOnly = package;
      // If there is a suffix it will be 9 characters long
      int dashPos = package.size() - 9;
      if (dashPos > 0) {
        if (package[dashPos] == '-') {
          if (std::string("0123456789").find(package[dashPos + 1])
              != std::string::npos) {
            nameOnly = std::string(package, 0, dashPos);
          }
        }
      }
      packageUpper = nameOnly;
      transform(packageUpper.begin(), packageUpper.end(), packageUpper.begin(), (int(*)(int)) toupper);
      setEnvironment(packageUpper+"XMLPATH", getXmlPath(nameOnly));
      setEnvironment(packageUpper+"DATAPATH", getDataPath(nameOnly));
#ifdef GlastRelease
      setEnvironment(packageUpper+"JOBOPTIONSPATH",getJobOptionsPath(nameOnly));
#endif
    }


#if defined(SCons)
#ifdef REL_OBFLDPATH
    std::string obfldPath(REL_OBFLDPATH);
    setEnvironment("OBFLDPATH", 
                   joinPath(getEnvironment("GLAST_EXT"), obfldPath));
#endif
#endif

#ifndef HEADAS
#ifdef extFiles
    setEnvironment("EXTFILESSYS", joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles));
#endif
#endif
#ifdef GlastRelease
  // Define environment variable PARAMFILESROOT used by Gaudi to find
  // particleTable.txt.  This value will be overridden if it's set
  // in job options
  // PARMAFILESROOT should be $EXTFILESSYS/gaudi
  std::string tmp = 
    commonUtilities::joinPath(commonUtilities::getEnvironment("EXTFILESSYS"),
                              "gaudi");
    commonUtilities::setEnvironment("PARAMFILESROOT", tmp);

#endif
#ifdef ScienceTools
    std::string calDbData = getDataPath("caldb");
    setEnvironment("CALDB", calDbData);
    setEnvironment("CALDBCONFIG", 
                   joinPath(joinPath(joinPath(calDbData, "software"), 
                                     "tools"), "caldb.config"));
    setEnvironment("CALDBALIAS", 
                   joinPath(joinPath(joinPath(calDbData, "software"), 
                                     "tools"), "alias_config.fits"));
#ifdef HEADAS
    setEnvironment("TIMING_DIR", joinPath(getPackageRoot("timeSystem"), "refData"));
#else
    setEnvironment("TIMING_DIR", joinPath(joinPath(joinPath(getEnvironment("GLAST_EXT"), "extFiles"), extFiles), "jplephem"));
#endif
#endif
    // Not HEADAS; not SCons
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
              setEnvironment(env+"JOBOPTIONSPATH", getJobOptionsPath(env));
	    }
	  }
	}
	counter++;
      }
    }
#endif
  }
}

