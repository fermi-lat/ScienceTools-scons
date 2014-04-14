/**
 * @file BitMaskCut.cxx
 * @brief Cuts based on a single bit mask applied to an unsigned long
 * column.  This is used for filtering on the EVENT_CLASS column for
 * Pass 7 IRFs and later.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <iomanip>
#include <sstream>

#include "tip/Table.h"

#include "dataSubselector/BitMaskCut.h"

namespace dataSubselector {

BitMaskCut::BitMaskCut(const std::string & colname,
                       unsigned long bitPosition,
                       const std::string & pass_ver) 
   : CutBase("bit_mask"), m_colname(colname), m_bitPosition(bitPosition),
     m_mask(1 << bitPosition), m_pass_ver(pass_ver), m_post_P7(post_P7()) {
   if (m_post_P7) {
      // Pass version is P8 or later, so we assume bit-array columns
      // in the FT1 file and will take bitPosition as the bit-mask
      // itself.
      m_mask = bitPosition;
   }
}

bool BitMaskCut::accept(tip::ConstTableRecord & row) const {
   unsigned long value;
   row[m_colname].get(value);
   return accept(value);
}

bool BitMaskCut::accept(const std::map<std::string, double> & params) const {
   const std::map<std::string, double>::const_iterator value
      = params.find(m_colname);
   if (value != params.end()) {
      return accept(static_cast<unsigned long>(value->second));
   }
   return true;
}

bool BitMaskCut::accept(unsigned long value) const {
   bool result((value & m_mask) != 0);
   return result;
}

bool BitMaskCut::supercedes(const CutBase & cut) const {
   if (cut.type() != "bit_mask") {
      return false;
   }
   if (m_post_P7) {
      // A nested hierarchy of bit masks cannot be assumed, so this
      // should always be false.
      return false;
   }
   // For P7 and P7REP:
   BitMaskCut & bitMaskCut = 
      dynamic_cast<BitMaskCut &>(const_cast<CutBase &>(cut));
   // This test assumes the cuts are hierarchical (nested).
   if (m_bitPosition > bitMaskCut.bitPosition()) {
      return true;
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
      filter << "((" << m_colname << "/" << m_mask << ")%2 == 1)";
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
   // typ << "(" << m_colname << "/POW(2," << m_bitPosition << "))%2";
   typ << "BIT_MASK(" << m_colname 
       << "," << m_bitPosition;
   if (m_pass_ver != "") {
      typ << "," << m_pass_ver;
   }
   typ << ")";
   type = typ.str();
   unit = "DIMENSIONLESS";
   value = "1:1";
}

bool BitMaskCut::post_P7() const {
   static size_t nvers(5);
   char * old_pass_vers[] = {const_cast<char *>(""),
                             const_cast<char *>("NONE"),
                             const_cast<char *>("P6"),
                             const_cast<char *>("P7V6"),
                             const_cast<char *>("P7REP")};
   for (size_t i(0); i < nvers; i++) {
      if (m_pass_ver == old_pass_vers[i]) {
         return false;
      }
   }
   return true;
}

} // namespace dataSubselector 
