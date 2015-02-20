/**
 * @file BitMaskCut.cxx
 * @brief Cuts based on a bit mask.  This is used for filtering on
 * EVENT_CLASS for Pass 7 IRFs, and also on EVENT_TYPE for Pass 8 and
 * later.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iomanip>
#include <sstream>

#include "tip/Table.h"

#include "dataSubselector/BitMaskCut.h"

namespace {
unsigned int bitPosition(unsigned int mask) {
   return static_cast<unsigned int>(std::log(mask)/std::log(2.));
}
}

namespace dataSubselector {

BitMaskCut::BitMaskCut(const std::string & colname,
                       unsigned int mask,
                       const std::string & pass_ver) 
   : CutBase("bit_mask"), m_colname(colname), m_mask(mask),
     m_pass_ver(pass_ver), m_post_P7(post_P7(pass_ver)) {
}

bool BitMaskCut::accept(tip::ConstTableRecord & row) const {
   unsigned int value;
   row[m_colname].get(value);
   return accept(value);
}

bool BitMaskCut::accept(const std::map<std::string, double> & params) const {
   const std::map<std::string, double>::const_iterator value
      = params.find(m_colname);
   if (value != params.end()) {
      return accept(static_cast<unsigned int>(value->second));
   }
   return true;
}

bool BitMaskCut::accept(unsigned int value) const {
   bool result((value & m_mask) != 0);
   return result;
}

bool BitMaskCut::supercedes(const CutBase & cut) const {
   if (cut.type() != "bit_mask") {
      return false;
   }
   BitMaskCut & bitMaskCut = 
      dynamic_cast<BitMaskCut &>(const_cast<CutBase &>(cut));
   if (bitMaskCut.colname() != m_colname) {
      return false;
   }
   if (m_colname == "EVENT_TYPE") {
      // Supercedes if the and-ed bits match the candidate mask.
      // Event class selections don't behave this way so need to
      // handle both as special cases.
      if ((m_mask & bitMaskCut.mask()) == m_mask) {
         return true;
      }
   }
   if (m_colname == "EVENT_CLASS" && !m_post_P7) {
      // For P7 and P7REP:
      // This test assumes the event class cuts are hierarchical.
      if (m_mask > bitMaskCut.mask()) {
         return true;
      }
   }
   return false;
}

std::string BitMaskCut::filterString() const {
   std::ostringstream filter;
   if (m_post_P7) {
      std::ostringstream octal_rep;
      octal_rep << "o" << std::setbase(8) << m_mask;
      filter << "((" << m_colname << "&" 
             << octal_rep.str() << ") != o0)";
   } else {
      filter << "((" << m_colname << "/" 
             << ::bitPosition(m_mask) 
             << ")%2 == 1)";
   }
   return filter.str();
}

bool BitMaskCut::equals(const CutBase & arg) const {
   try {
      BitMaskCut &rhs = dynamic_cast<BitMaskCut &>(const_cast<CutBase &>(arg));
      bool result = (m_colname == rhs.m_colname && m_mask == rhs.m_mask);
      return result;
   } catch(...) {
      // Failed downcast, so do nothing and return false by default.
   }
   return false;
}

void BitMaskCut::getKeyValues(std::string & type, 
                              std::string & unit, 
                              std::string & value,
                              std::string & ref) const {
   (void)(ref);
   std::ostringstream typ;
   typ << "BIT_MASK(" << m_colname;
   if (m_post_P7) {
      typ << "," << m_mask;
   } else {
      typ << "," << ::bitPosition(m_mask);
   }
   if (m_pass_ver != "") {
      typ << "," << m_pass_ver;
   }
   typ << ")";
   type = typ.str();
   unit = "DIMENSIONLESS";
   value = "1:1";
}

bool BitMaskCut::post_P7(const std::string & pass_ver) {
   static size_t nvers(6);
   char * old_pass_vers[] = {const_cast<char *>(""),
                             const_cast<char *>("NONE"),
                             const_cast<char *>("P6"),
                             const_cast<char *>("P7V6"),
                             const_cast<char *>("P7REP"),
                             const_cast<char *>("P8R0")};
   for (size_t i(0); i < nvers; i++) {
      if (pass_ver == old_pass_vers[i]) {
         return false;
      }
   }
   return true;
}

} // namespace dataSubselector 
