/**
 * @file RangeCut.cxx
 * @brief Cut on a column value in a given range of values.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>

#include "facilities/Util.h"

#include "tip/Table.h"

#include "dataSubselector/RangeCut.h"

namespace dataSubselector {

RangeCut::RangeCut(const std::string & colname, const std::string & unit,
                         double minVal, double maxVal, IntervalType type,
                         unsigned int indx)
   : CutBase("range"), m_colname(colname), m_unit(unit),
     m_min(minVal), m_max(maxVal), m_type(type), m_index(indx),
     m_fullName(colname) {
   setFullName();
}

RangeCut::RangeCut(const std::string & type,
                         const std::string & unit, 
                         const std::string & value,
                         unsigned int indx) 
   : CutBase("range"), m_colname(type), m_unit(unit), m_index(indx),
     m_fullName(type) {
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
   setFullName();
}

bool RangeCut::accept(tip::ConstTableRecord & row) const {
   double value = extractValue(row);
   return accept(value);
}

bool RangeCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator it;
   if ( (it = params.find(m_fullName)) != params.end()) {
      double value = it->second;
      return accept(value);
   }
   return true;
}

bool RangeCut::operator==(const CutBase & arg) const {
   try {
      RangeCut & rhs = dynamic_cast<RangeCut &>(const_cast<CutBase &>(arg));
      return (m_colname == rhs.m_colname && m_unit == rhs.m_unit &&
              m_min == rhs.m_min && m_max == rhs.m_max &&
              m_type == rhs.m_type && m_index == rhs.m_index);
   } catch (...) {
      return false;
   }
}

bool RangeCut::supercedes(const CutBase & cut) const {
   if (cut.type() != "range") {
      return false;
   }
   RangeCut & rangeCut = dynamic_cast<RangeCut &>(const_cast<CutBase &>(cut));
/// @todo Need to handle open ranges.
   if (rangeCut.colname() != colname() || rangeCut.m_type != m_type) {
      return false;
   }
   if (rangeCut.minVal() <= minVal() && maxVal() <= rangeCut.maxVal()) {
      return true;
   }
   return false;
}

void RangeCut::getKeyValues(std::string & type, std::string & unit,
                            std::string & value, std::string & ref) const {
   (void)(ref);
   std::ostringstream val;
   if (m_type == MINONLY) {
      val << m_min << ":";
   } else if (m_type == MAXONLY) {
      val << ":" << m_max;
   } else {
      val << m_min << ":" << m_max;
   }
   type = m_fullName;
   unit = m_unit;
   value = val.str();
}

bool RangeCut::accept(double value) const {
   if (m_type == MINONLY) {
      return m_min <= value;
   } else if (m_type == MAXONLY) {
      return value <= m_max;
   }
   return m_min <= value && value <= m_max;
}

double RangeCut::extractValue(tip::ConstTableRecord & row) const {
   if (m_index) {
      std::vector<double> tableVector;
      row[m_colname].get(tableVector);
      return tableVector.at(m_index-1);
   }
   double value;
   row[m_colname].get(value);
   return value;
}

void RangeCut::setFullName() {
   if (m_index) {
      std::ostringstream name;
      name << m_colname << "[" << m_index << "]";
      m_fullName = name.str();
   } else {
      m_fullName = m_colname;
   }
}

} // namespace dataSubselector
