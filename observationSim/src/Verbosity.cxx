#include "Verbosity.h"

namespace observationSim {

Verbosity * Verbosity::s_instance(0);

unsigned int verbosity() {
   return Verbosity::instance()->value();
}

bool clobber() {
   return Verbosity::instance()->clobber();
}

} // namespace observationSim
