/**
 * @file RangeCut.cxx
 *
 */

#include "facilities/Util.h"

#include "dataSubselector/Cuts.h"

namespace dataSubselector {

Cuts::RangeCut::RangeCut(const std::string & type,
                         const std::string & unit, 
                         const std::string & value) 
   : m_keyword(type), m_unit(unit) {
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(value, ":", tokens);
   if (tokens.size() == 2) {
      m_min = std::atof(tokens[0].c_str());
      m_max = std::atof(tokens[1].c_str());
      m_type = CLOSED;
   } else if (value.find(":") == 0) {
      m_max = std::atof(tokens[0].c_str());
      m_type = MAXONLY;
   } else {
      m_min = std::atof(tokens[0].c_str());
      m_type = MINONLY;
   }      
}

bool Cuts::RangeCut::accept(tip::ConstTableRecord & row) const {
   double value;
   row[m_keyword].get(value);
   return accept(value);
}

bool Cuts::
RangeCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator it;
   if ( (it = params.find(m_keyword)) != params.end()) {
      return accept(it->second);
   }
   return true;
}

bool Cuts::RangeCut::operator==(const CutBase & arg) const {
   try {
      RangeCut & rhs = dynamic_cast<RangeCut &>(const_cast<CutBase &>(arg));
      return (m_keyword == rhs.m_keyword && m_unit == rhs.m_unit &&
              m_min == rhs.m_min && m_max == rhs.m_max &&
              m_type == rhs.m_type);
   } catch (...) {
      return false;
   }
}

void Cuts::RangeCut::
getKeyValues(std::string & type, std::string & unit,
             std::string & value, std::string & ref) const {
   (void)(ref);
   std::ostringstream val;
   if (m_type == Cuts::MINONLY) {
      val << m_min << ":";
   } else if (m_type == Cuts::MAXONLY) {
      val << ":" << m_max;
   } else {
      val << m_min << ":" << m_max;
   }
   type = m_keyword;
   unit = m_unit;
   value = val.str();
}

bool Cuts::RangeCut::accept(double value) const {
   if (m_type == Cuts::MINONLY) {
      return m_min <= value;
   } else if (m_type == Cuts::MAXONLY) {
      return value <= m_max;
   }
   return m_min <= value && value <= m_max;
}

} // namespace dataSubselector
