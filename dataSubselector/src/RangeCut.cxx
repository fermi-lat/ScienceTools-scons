/**
 * @file RangeCut.cxx
 * @brief Cut on a column value in a given range of values.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <sstream>

#include "facilities/Util.h"

#include "tip/Table.h"

#include "dataSubselector/RangeCut.h"

namespace dataSubselector {

RangeCut::RangeCut(const std::string & colname, const std::string & unit,
                         double minVal, double maxVal, IntervalType type,
                         unsigned int indx)
   : CutBase("range"), m_colname(colname), m_unit(unit),
     m_min(minVal), m_max(maxVal), m_intervalType(type), m_index(indx),
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
      m_intervalType = CLOSED;
   } else if (value.find(":") == 0) {
      m_max = std::atof(tokens[0].c_str());
      m_intervalType = MAXONLY;
   } else {
      m_min = std::atof(tokens[0].c_str());
      m_intervalType = MINONLY;
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

bool RangeCut::equals(const CutBase & arg) const {
   try {
      RangeCut & rhs = dynamic_cast<RangeCut &>(const_cast<CutBase &>(arg));
      bool result = (m_colname == rhs.m_colname &&
                     m_unit == rhs.m_unit &&
                     m_intervalType == rhs.m_intervalType && 
                     m_index == rhs.m_index);
      if (m_intervalType == CLOSED) {
         result = result && (m_min == rhs.m_min && 
                             m_max == rhs.m_max);
      } else if (m_intervalType == MINONLY) {
         result = result && m_min == rhs.m_min;
      } else if (m_intervalType == MAXONLY) {
         result = result && m_max == rhs.m_max;
      }
      return result;
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
   if (rangeCut.colname() != colname() || 
       rangeCut.m_intervalType != m_intervalType) {
      return false;
   }
   if (rangeCut.minVal() <= minVal() && maxVal() <= rangeCut.maxVal()) {
      return true;
   }
   return false;
}

std::string RangeCut::filterString() const {
   std::ostringstream filter;
   if (m_intervalType == MINONLY) {
      filter << m_min << " <= " << m_fullName;
   } else if (m_intervalType == MAXONLY) {
      filter << m_fullName << " <= " << m_max;
   } else {
      filter << m_min << " <= " << m_fullName << " && "
             << m_fullName << " <= " << m_max;
   }
   return filter.str();
}

void RangeCut::getKeyValues(std::string & type, std::string & unit,
                            std::string & value, std::string & ref) const {
   (void)(ref);
   std::ostringstream val;
   val.precision(20);
   if (m_intervalType == MINONLY) {
      val << m_min << ":";
   } else if (m_intervalType == MAXONLY) {
      val << ":" << m_max;
   } else {
      val << m_min << ":" << m_max;
   }
   type = m_fullName;
   unit = m_unit;
   value = val.str();
}

bool RangeCut::accept(double value) const {
   if (m_intervalType == MINONLY) {
      return m_min <= value;
   } else if (m_intervalType == MAXONLY) {
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
