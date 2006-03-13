/**
 * @file Verbosity.cxx
 * @brief Implementation for Singleton class to control screen output
 * for applications
 * @author J. Chiang
 *
 * $Header$
 */

#define ST_DLL_EXPORTS
#include "Verbosity.h"
#undef ST_DLL_EXPORTS

namespace observationSim {

Verbosity * Verbosity::s_instance(0);

bool print_output(unsigned int local_verbosity) {
   return Verbosity::instance()->value() >= local_verbosity;
}

bool clobber() {
   return Verbosity::instance()->clobber();
}

} // namespace observationSim
