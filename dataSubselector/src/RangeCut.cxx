/**
 * @file RangeCut.cxx
 *
 */

#include "facilities/Util.h"

#include "dataSubselector/Cuts.h"

namespace dataSubselector {

Cuts::RangeCut::RangeCut(const std::string & type,
                         const std::string & unit, 
                         const std::string & value,
                         unsigned int indx) 
   : m_colname(type), m_unit(unit), m_index(indx) {
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
   double value = extractValue(row);
   return accept(value);
}

bool Cuts::
RangeCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator it;
   std::string colname;
   if (m_index) {
      std::ostringstream col;
      col << m_colname << "[" << m_index << "]";
      colname = col.str();
   } else {
      colname = m_colname;
   }
   if ( (it = params.find(colname)) != params.end()) {
      double value = it->second;
      return accept(value);
   }
   return true;
}

bool Cuts::RangeCut::operator==(const CutBase & arg) const {
   try {
      RangeCut & rhs = dynamic_cast<RangeCut &>(const_cast<CutBase &>(arg));
      return (m_colname == rhs.m_colname && m_unit == rhs.m_unit &&
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
   if (m_index) {
      std::ostringstream typeval;
      typeval << m_colname << "[" << m_index << "]";
      type = typeval.str();
   } else {
      type = m_colname;
   }
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

double Cuts::RangeCut::extractValue(tip::ConstTableRecord & row) const {
   if (m_index) {
      std::vector<double> tableVector;
      row[m_colname].get(tableVector);
      return tableVector.at(m_index-1);
   }
   double value;
   row[m_colname].get(value);
   return value;
}

} // namespace dataSubselector
