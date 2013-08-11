/**
 * @file VersionCut.cxx
 * @brief Cuts based on a single bit mask applied to an unsigned int
 * column.  This is used for filtering on the EVENT_CLASS column for
 * Pass 7 IRFs and later.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>

#include "tip/Table.h"

#include "dataSubselector/VersionCut.h"

namespace dataSubselector {

VersionCut::VersionCut(const std::string & colname,
                       const std::string & version) 
                       
   : CutBase("version"), m_colname(colname), m_version(version) {}

bool VersionCut::supercedes(const CutBase & cut) const {
   if (cut.type() != "version") {
      return false;
   }
   VersionCut & versionCut = 
      dynamic_cast<VersionCut &>(const_cast<CutBase &>(cut));
   if (versionCut.colname() == colname()) {
      return true;
   }
   return false;
}

bool VersionCut::equals(const CutBase & arg) const {
   try {
      VersionCut &rhs = dynamic_cast<VersionCut &>(const_cast<CutBase &>(arg));
      bool result = (m_colname == rhs.m_colname && m_version == rhs.m_version);
      return result;
   } catch(...) {
      // Failed downcast, so do nothing and return false by default.
   }
   return false;
}

void VersionCut::getKeyValues(std::string & type, 
                              std::string & unit, 
                              std::string & value,
                              std::string & ref) const {
   (void)(ref);
   type = m_colname;
   unit = "DIMENSIONLESS";
   value = m_version;
}

} // namespace dataSubselector 
