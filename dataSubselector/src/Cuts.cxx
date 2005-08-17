/**
 * @file Cuts.cxx
 * @brief Handle data selections and DSS keyword management.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cctype>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/GtiCut.h"
#include "dataSubselector/RangeCut.h"
#include "dataSubselector/SkyConeCut.h"

namespace {
   void toUpper(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::toupper(*it);
      }
   }
}

namespace dataSubselector {

Cuts::Cuts(const std::vector<std::string> & eventFiles,
           const std::string & extname, bool check_columns) {
   std::vector<Cuts> my_cuts;
   for (unsigned int i = 0; i < eventFiles.size(); i++) {
      my_cuts.push_back(Cuts(eventFiles.at(i), extname, check_columns));
      if (i > 0) {
         if (!my_cuts.front().compareWithoutGtis(my_cuts.back())) {
            std::ostringstream message;
            message << "DSS keywords in " << eventFiles.at(i)
                    << " do not match those in " << eventFiles.front();
            throw std::runtime_error(message.str());
         }
      }
   }
   *this = mergeGtis(my_cuts);
}

Cuts Cuts::mergeGtis(std::vector<Cuts> & cuts_vector) {
// Gather non-Gti cuts.
   dataSubselector::Cuts my_cuts;
   const dataSubselector::Cuts & firstCuts(cuts_vector.front());
   for (unsigned int i = 0; i < firstCuts.size(); i++) {
      if (firstCuts[i].type() != "GTI") {
         my_cuts.addCut(firstCuts[i]);
      }
   }
   
// Merge all of the GTIs into one, taking the union of the intervals.
   dataSubselector::Gti merged_gti;
   std::vector<const dataSubselector::GtiCut *> gtiCuts;
   for (unsigned int i = 0; i < cuts_vector.size(); i++) {
      cuts_vector.at(i).getGtiCuts(gtiCuts);
      for (unsigned int j = 0; j < gtiCuts.size(); j++) {
         if (i == 0 && j == 0) {
            merged_gti = gtiCuts.at(j)->gti();
         } else {
            merged_gti = merged_gti | gtiCuts.at(j)->gti();
         }
      }
   }

   my_cuts.addGtiCut(merged_gti);
   return my_cuts;
}

void Cuts::getGtiCuts(std::vector<const GtiCut *> & gtiCuts) {
   gtiCuts.clear();
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      if (m_cuts.at(i)->type() == "GTI") {
         gtiCuts.push_back(dynamic_cast<GtiCut *>(m_cuts.at(i)));
      }
   }
}

Cuts::Cuts(const std::string & eventFile, const std::string & extname,
           bool check_columns) {
   const tip::Extension * ext(0);
   try {
      ext = tip::IFileSvc::instance().readTable(eventFile, extname);
   } catch (tip::TipException & eObj) {
      if (!st_facilities::Util::expectedException(eObj, "HDU is not a")) {
         throw;
      }
      ext = tip::IFileSvc::instance().readImage(eventFile, extname);
   }

   std::vector<std::string> colnames;
   if (check_columns) {
      const tip::Table * table 
         = dynamic_cast<tip::Table *>(const_cast<tip::Extension *>(ext));
      colnames = table->getValidFields();
// FITS column names are in CAPS, not lowercase, so undo what tip has wrought
      for (unsigned int i = 0; i < colnames.size(); i++) {
         ::toUpper(colnames[i]);
      }
   }

   const tip::Header & header = ext->getHeader();
// The .get(...) method does not work for unsigned int arguments, so
// we are force to use a double as a temporary variable:
   double nkeys_value;
   header["NDSKEYS"].get(nkeys_value);
   unsigned int nkeys = static_cast<unsigned int>(nkeys_value);

   std::string type, unit, value, ref("");

   for (unsigned int keynum = 1; keynum <= nkeys; keynum++) {
      std::ostringstream key1, key2, key3, key4;
      key1 << "DSTYP" << keynum;
      header[key1.str()].get(type);
      key2 << "DSUNI" << keynum;
      header[key2.str()].get(unit);
      key3 << "DSVAL" << keynum;
      header[key3.str()].get(value);
      ::toUpper(value);

      if (value == "TABLE") {
         key4 << "DSREF" << keynum;
         header[key4.str()].get(ref);
      }
      std::string colname;
      unsigned int indx = parseColname(type, colname);
      if (value.find("CIRCLE") == 0) {
         m_cuts.push_back(new SkyConeCut(type, unit, value));
      } else if ( (!check_columns || 
                   std::find(colnames.begin(), colnames.end(), colname) 
                   != colnames.end())
                  && value != "TABLE" ) {
         m_cuts.push_back(new RangeCut(colname, unit, value, indx));
      } else if (type == "TIME" && value == "TABLE") {
         m_cuts.push_back(new GtiCut(eventFile));
      } else {
         std::ostringstream message;
         message << "FITS extension contains an unrecognized DSS filtering "
                 << "scheme.\n"
                 << key1.str() << " = " << type << "\n"
                 << key2.str() << " = " << unit << "\n"
                 << key3.str() << " = " << value << "\n";
         if (value == "TABLE") {
            message << key4.str() << " = " << ref << "\n";
         }
         throw std::runtime_error(message.str());
      }
   }
   delete ext;
}

unsigned int Cuts::parseColname(const std::string & colname,
                                std::string & col) const {
   if (colname.find("[") == std::string::npos) {
      col = colname;
      return 0;
   }
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(colname, "[]", tokens);
   col = tokens.at(0);
   return std::atoi(tokens.at(1).c_str());
}

unsigned int Cuts::find(const CutBase * cut) const {
   for (unsigned int i = 0; i < size(); i++) {
      if (*cut == *m_cuts[i]) {
         return i;
      }
   }
   return size();
}

bool Cuts::hasCut(const CutBase * newCut) const {
   for (unsigned int i = 0; i < size(); i++) {
      if (*newCut == *m_cuts[i]) {
         return true;
      }
   }
   return false;
}

Cuts::Cuts(const Cuts & rhs) {
   m_cuts.reserve(rhs.size());
   for (unsigned int i = 0; i < rhs.size(); i++) {
      m_cuts.push_back(rhs.m_cuts[i]->clone());
   }
}

Cuts::~Cuts() {
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      delete m_cuts[i];
   }
}

Cuts & Cuts::operator=(const Cuts & rhs) {
   if (*this != rhs) {
      std::vector<CutBase *>::reverse_iterator cut = m_cuts.rbegin();
      for ( ; cut != m_cuts.rend(); cut++) {
         delete *cut;
      }
      m_cuts.clear();
      for (unsigned int i = 0; i < rhs.size(); i++) {
         m_cuts.push_back(rhs.m_cuts.at(i)->clone());
      }
   }
   return *this;
}

bool Cuts::accept(tip::ConstTableRecord & row) const {
   bool ok(true);
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      ok = ok && m_cuts[i]->accept(row);
   }
   return ok;
}

bool Cuts::accept(const std::map<std::string, double> & params) const {
   bool ok(true);
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      ok = ok && m_cuts[i]->accept(params);
   }
   return ok;
}

unsigned int Cuts::addRangeCut(const std::string & colname,
                               const std::string & unit,
                               double minVal, double maxVal, 
                               RangeCut::IntervalType type,
                               unsigned int indx) {
   return addCut(new RangeCut(colname, unit, minVal, maxVal, type, indx));
}

unsigned int Cuts::addGtiCut(const tip::Table & table) {
   return addCut(new GtiCut(table));
}

unsigned int Cuts::addGtiCut(const Gti & gti) {
   return addCut(new GtiCut(gti));
}

unsigned int Cuts::addSkyConeCut(double ra, double dec, double radius) {
   return addCut(new SkyConeCut(ra, dec, radius));
}

unsigned int Cuts::addCut(CutBase * newCut) {
   if (hasCut(newCut)) {
      delete newCut;
   } else {
      for (unsigned int j = 0; j != size(); j++) {
         if (newCut->supercedes(*(m_cuts[j]))) {
            delete m_cuts[j];
            m_cuts[j] = newCut;
            return size();
         }
         if (m_cuts[j]->supercedes(*newCut)) {
            delete newCut;
            return size();
         }
      }
      m_cuts.push_back(newCut);
   }
   return size();
}

void Cuts::writeDssKeywords(tip::Header & header) const {
   header["NDSKEYS"].set(m_cuts.size());
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts[i]->writeDssKeywords(header, i + 1);
   }
}      

void Cuts::writeGtiExtension(const std::string & filename) const {
   const dataSubselector::Gti * gti(0);
   for (unsigned int i = 0; i < size(); i++) {
      if (m_cuts[i]->type() == "GTI") {
         gti = &(dynamic_cast<GtiCut *>(m_cuts[i])->gti());
         break;  // assume there is at most one GTI
      }
   }
   if (gti) {
      gti->writeExtension(filename);
   }
}

bool Cuts::operator==(const Cuts & rhs) const {
   if (size() != rhs.size()) {
      return false;
   }
   for (unsigned int i = 0; i < size(); i++) {
      unsigned int place = find(rhs.m_cuts.at(i));
      if (place == size()){
         return false;
      }
   }
   return true;
}

bool Cuts::compareWithoutGtis(const Cuts & rhs) const {
   if (size() != rhs.size()) {
      return false;
   }
   for (unsigned int i = 0; i < size(); i++) {
      if (rhs.m_cuts.at(i)->type() != "GTI") {
         unsigned int place = find(rhs.m_cuts.at(i));
         if (place == size()){
            return false;
         }
      }
   }
   return true;
}

void Cuts::writeCuts(std::ostream & stream) const {
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts.at(i)->writeCut(stream, i + 1);
   }
}

std::string Cuts::filterString() const {
   std::string filter("");
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      std::string my_filter = m_cuts.at(i)->filterString();
      if (my_filter != "") {
         if (filter != "") {
            filter += " && ";
         }
         filter += my_filter;
      }
   }
   return filter;
}

} // namespace dataSubselector
