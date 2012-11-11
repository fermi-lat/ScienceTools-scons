/**
 * @file Environment.cxx
 * @brief Yet another environment interface.  This provides access to
 * facilities::commonUtilities functions that rely on environment
 * variables.  By implementing as a Singleton and providing access to
 * the underlying functions only via the Singleton object, this class
 * ensures that the facilities::commonUtilities::setupEnvironment()
 * function is called without having to burden the clients with this
 * task.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include "facilities/commonUtilities.h"

#include "st_facilities/Environment.h"

namespace st_facilities {

Environment * Environment::s_instance(0);

Environment & Environment::instance() {
   if (s_instance == 0) {
      s_instance = new Environment();
   }
   return *s_instance;
}

Environment::Environment() {
   facilities::commonUtilities::setupEnvironment();
}

std::string Environment::dataPath(const std::string & package) {
   instance();
   return facilities::commonUtilities::getDataPath(package);
}

std::string Environment::packagePath(const std::string & package) {
   instance();
   return facilities::commonUtilities::getPackagePath(package);
}

} // namespace st_facilities
