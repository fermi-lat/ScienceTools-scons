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

#include <cstdlib>
#include <cmath>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "st_stream/StreamFormatter.h"
#include "facilities/Util.h"
#include "st_facilities/Util.h"
#include "tip/Table.h"

#include "dataSubselector/BitMaskCut.h"

namespace {
unsigned int bitPosition(unsigned int mask) {
   return static_cast<unsigned int>(std::log(mask)/std::log(2.));
}
}

namespace dataSubselector {

BitMaskCut::BitMaskPrecedence * BitMaskCut::s_evclassPrecedence(0);

BitMaskCut::BitMaskPrecedence * BitMaskCut::s_evtypePrecedence(0);

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
      bool valid_mask 
         = s_evtypePrecedence->validMask(*this, bitMaskCut.mask());
      if (!valid_mask) {
         throw std::runtime_error("The requested evtype selection, " +
                                  this->dstype() + " is inconsistent "
                                  + "with the existing evtype selection, "
                                  + bitMaskCut.dstype());
      }
      return true;
   }
   if (m_colname == "EVENT_CLASS") {
      if (m_post_P7) {
         bool valid_mask 
            = s_evclassPrecedence->validMask(*this, bitMaskCut.mask());
         if (!valid_mask) {
            throw std::runtime_error("The requested evclass selection, " +
                                     this->dstype() + " is inconsistent "
                                     + "with the existing evclass selection, "
                                     + bitMaskCut.dstype());
         }
         if (this->mask() != bitMaskCut.mask()) {
            st_stream::StreamFormatter formatter("dataSubselector::BitMaskCut",
                                                 "supercedes", 3);
            formatter.warn(3) << "\nWARNING:\n"
                              << "Over writing existing event class selection,"
                              << "\n  " << bitMaskCut.dstype() << "\nwith\n  "
                              << this->dstype() << "\n" << std::endl;
         }
         return true;
      } else {
         // For P7 and P7REP:
         // This test assumes the event class cuts are hierarchical.
         return (m_mask > bitMaskCut.mask());
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
             << m_mask
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
   type = dstype();
   unit = "DIMENSIONLESS";
   value = "1:1";
}

std::string BitMaskCut::dstype() const {
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
   return typ.str();
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

BitMaskCut::BitMaskPrecedence::
BitMaskPrecedence(const std::string & maskFile) 
   : m_maskFile(maskFile) {
   std::vector<std::string> lines;
   st_facilities::Util::readLines(maskFile, lines, "#", true);
   for (size_t i(0); i < lines.size(); i++) {
      std::vector<std::string> tokens;
      facilities::Util::stringTokenize(lines[i], ",", tokens);
      m_validityMasks[std::atoi(tokens.at(0).c_str())]
         = std::atoi(tokens.at(1).c_str());
   }
}

bool BitMaskCut::BitMaskPrecedence::
validMask(const BitMaskCut & self, unsigned int currentMask) const {
   std::map<unsigned int, unsigned int>::const_iterator it
      = m_validityMasks.find(currentMask);
   if (it == m_validityMasks.end()) {
      std::ostringstream message;
      message << "Current bit mask, " << currentMask 
              << ", not found in validity mask file.";
      throw std::runtime_error(message.str());
   }
   unsigned int masked_value = it->second & self.mask();
   return masked_value == self.mask();
}

const std::string & BitMaskCut::BitMaskPrecedence::maskFile() const {
   return m_maskFile;
}

void BitMaskCut::setValidityMasks(const std::string & evclassFile,
                                  const std::string & evtypeFile) {
   delete s_evclassPrecedence;
   s_evclassPrecedence = new BitMaskCut::BitMaskPrecedence(evclassFile);

   delete s_evtypePrecedence;
   s_evtypePrecedence = new BitMaskCut::BitMaskPrecedence(evtypeFile);
}

} // namespace dataSubselector 
