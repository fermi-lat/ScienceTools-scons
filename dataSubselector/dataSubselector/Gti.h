/**
 * @file Gti.h
 * @brief Handle GTIs. Derives from evtbin::Gti, adding a constructor and 
 * an accept() method.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_Gti_h
#define dataSubselector_Gti_h

#include "evtbin/Gti.h"

namespace tip {
   class Table;
}

namespace dataSubselector {

/**
 * @class Gti
 *
 */

class Gti : public evtbin::Gti {

public:

   Gti() : evtbin::Gti() {}

   Gti(const std::string & filename, const std::string & extension="GTI") 
      : evtbin::Gti(filename, extension) {}

   Gti(const tip::Table & gtiTable);

   bool accept(double time) const;

   void writeExtension(const std::string & filename) const;

};

} // namespace dataSubselector

#endif // dataSubselector_Gti_h
