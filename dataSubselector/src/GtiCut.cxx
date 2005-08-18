/**
 * @file GtiCut.cxx
 * @brief Encapsulate a GTI.
 * @author J. Chiang
 *
 * $Header$
 */

#include "dataSubselector/GtiCut.h"

namespace dataSubselector {

bool GtiCut::accept(tip::ConstTableRecord & row) const {
   double time;
   row["TIME"].get(time);
   return accept(time);
}

bool GtiCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator it;
   if ( (it = params.find("TIME")) != params.end()) {
      return accept(it->second);
   }
   return true;
}

bool GtiCut::equals(const CutBase & arg) const {
   try {
      GtiCut & rhs = dynamic_cast<GtiCut &>(const_cast<CutBase &>(arg));
      return !(m_gti != rhs.m_gti);
   } catch (...) {
      return false;
   }
}

void GtiCut::getKeyValues(std::string & type, std::string & unit,
                          std::string & value, std::string & ref) const {
   type = "TIME";
   unit = "s";
   value = "TABLE";
   ref = ":GTI";
}

bool GtiCut::accept(double time) const {
   return m_gti.accept(time);
}

void GtiCut::writeCut(std::ostream & stream, unsigned int keynum) const {
   CutBase::writeCut(stream, keynum);
   std::vector< std::pair<double, double> >::const_iterator dt;
   stream << "GTIs:\n";
   for (dt = m_gti.begin(); dt != m_gti.end(); ++dt) {
      stream << dt->first << "  " << dt->second << "\n";
   }
   stream << std::endl;
}

} // namespace dataSubselector
