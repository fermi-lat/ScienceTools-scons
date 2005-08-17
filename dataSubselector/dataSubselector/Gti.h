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
 * @brief A more useful and complete implementation of evtbin::Gti
 * @author J. Chiang
 *
 * $Header$
 */

class Gti : public evtbin::Gti {

public:

   Gti() : evtbin::Gti() {}

   Gti(const std::string & filename, const std::string & extension="GTI") 
      : evtbin::Gti(filename, extension) {}

   Gti(const tip::Table & gtiTable);

   Gti(const evtbin::Gti & gti) : evtbin::Gti(gti) {}

   bool accept(double time) const;

   void writeExtension(const std::string & filename) const;

   /// @brief Given a start and stop time this method recomputes the
   /// the GTIs.
   /// @return A new Gti object with the new good-time intervals.
   /// @param start The start time of the time range cut (MET seconds)
   /// @param stop The stop time of the time range cut (MET seconds)
   Gti applyTimeRangeCut(double start, double stop) const;

   /// @return The minimum lower bound of the GTIs (MET seconds)
   double minValue() const;

   /// @return The maximum upper bound of the GTIs (MET seconds)
   double maxValue() const;

   /// @return A Gti that is the union of intervals between this and
   /// the other Gti.  The resulting set of GTIs are disjoint.
   Gti operator|(const evtbin::Gti & rhs) const;

};

} // namespace dataSubselector

#endif // dataSubselector_Gti_h
