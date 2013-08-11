/**
 * @file VersionCut.h
 * @brief This "cut" is intended as a means of propagating version information
 * that is specified by the IRF selection made by the user when running
 * gtselect.  The DSVAL# field will be read in as a string.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_VersionCut_h
#define dataSubselector_VersionCut_h

#include "dataSubselector/CutBase.h"

namespace dataSubselector {

/**
 * @class VersionCut
 *
 */

class VersionCut : public CutBase {

public:

   VersionCut(const std::string & colname, const std::string & version);

   virtual ~VersionCut() {}

   // This "cut" does not refer to columns in the FT1 table, so cannot
   // filter based on the row contents.  Accordingly, these member
   // functions always return true.
   virtual bool accept(tip::ConstTableRecord & row) const {
      return true;
   }
   virtual bool accept(const std::map<std::string, double> & params) const {
      return true;
   }
   virtual std::string filterString() const {
      return "";  // empty string means no filtering occurs.
   }

   virtual bool supercedes(const CutBase & cut) const;

   virtual CutBase * clone() const {return new VersionCut(*this);}

   const std::string & colname() const {
      return m_colname;
   }
   
   const std::string & version() const {
      return m_version;
   }

protected:

   virtual bool equals(const CutBase & rhs) const;

   virtual void getKeyValues(std::string & type, std::string & unit,
                             std::string & value, std::string & ref) const;

private:

   std::string m_colname;
   std::string m_version;

};

} // namespace dataSubselector

#endif // dataSubselector_VersionCut_h
