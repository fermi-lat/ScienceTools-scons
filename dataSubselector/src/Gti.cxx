/**
 * @file Gti.cxx
 * @brief Derived class of evtbin::Gti. Adds a constructor and 
 * accept() method.
 * @author J. Chiang
 *
 * $Header$
 */

#include "tip/Table.h"

#include "dataSubselector/Gti.h"

namespace dataSubselector {

Gti::Gti(const tip::Table & gtiTable) : evtbin::Gti() {
   tip::Table::ConstIterator it = gtiTable.begin();
   tip::ConstTableRecord & interval = *it;
   for ( ; it != gtiTable.end(); ++it) {
      double start, stop;
      interval["START"].get(start);
      interval["STOP"].get(stop);
      insertInterval(start, stop);
   }
}

bool Gti::accept(double time) const {
   evtbin::Gti::ConstIterator it;
   for (it = begin(); it != end(); ++it) {
      if (it->first <= time && time <= it->second) {
         return true;
      }
   }
   return false;
}

} // namespace dataSubselector
