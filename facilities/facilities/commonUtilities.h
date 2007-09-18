#ifndef _COMMON_UTILITIES_H_
#define _COMMON_UTILITIES_H_

#include <string>

class commonUtilities{
 public:
  static std::string getDataPath(const std::string &package);
  static std::string getXmlPath(const std::string &package);
  static std::string getPfilesPath(const std::string &package);
  static std::string getPackagePath(const std::string &package);
  static void setEnvironment(const std::string &name, const std::string &value, bool overwrite=false);
  static std::string getEnvironment(const std::string &name);
 private:
  static std::string getPackageRoot(const std::string &package);
};

#endif
