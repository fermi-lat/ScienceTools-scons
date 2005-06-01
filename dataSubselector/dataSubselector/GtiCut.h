/**
 * @file GtiCut.h
 * @brief Describe a GTI cut.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_GtiCut_h
#define dataSubselector_GtiCut_h

#include "tip/Table.h"

#include "dataSubselector/CutBase.h"
#include "dataSubselector/Gti.h"

namespace dataSubselector {

/**
 * @class GtiCut
 * @brief Cut on Good Time Intervals
 * @author J. Chiang
 *
 * $Header$
 */

class GtiCut : public CutBase {

public:

   GtiCut(const std::string & filename, const std::string & ext="GTI") 
      : CutBase("GTI"), m_gti(Gti(filename, ext)) {}

   GtiCut(const tip::Table & gtiTable) : CutBase("GTI"), m_gti(gtiTable) {}

   GtiCut(const Gti & gti) : CutBase("GTI"), m_gti(gti) {}

   virtual ~GtiCut() {}

   virtual bool accept(tip::ConstTableRecord & row) const;

   virtual bool accept(const std::map<std::string, double> & params) const;

   virtual CutBase * clone() const {return new GtiCut(*this);}

   /// @brief A reference to the Gti object.
   const Gti & gti() const {return m_gti;}

protected:

   virtual bool equals(const CutBase & rhs) const;

   virtual void getKeyValues(std::string & type, std::string & unit,
                             std::string & value, std::string & ref) const;

private:

   const Gti m_gti;

   bool accept(double value) const;

};

} // namespace dataSubselector

#endif // dataSubselector_GtiCut_h
