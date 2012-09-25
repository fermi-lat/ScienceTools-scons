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

#include "facilities/commonUtilities.h"
#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include "dataSubselector/BitMaskCut.h"
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

dataSubselector::RangeCut * 
mergeRangeCuts(const std::vector<dataSubselector::RangeCut *> & cuts) {
   double minValue;
   double maxValue;
   bool haveMin(false);
   bool haveMax(false);
   for (size_t i = 0; i < cuts.size(); i++) {
      if (cuts.at(i)->intervalType() == dataSubselector::RangeCut::CLOSED ||
          cuts.at(i)->intervalType() == dataSubselector::RangeCut::MINONLY) {
         if (!haveMin) {
            minValue = cuts.at(i)->minVal();
            haveMin = true;
         } else if (cuts.at(i)->minVal() > minValue) {
            minValue = cuts.at(i)->minVal();
         }
      }
      if (cuts.at(i)->intervalType() == dataSubselector::RangeCut::CLOSED ||
          cuts.at(i)->intervalType() == dataSubselector::RangeCut::MAXONLY) {
         if (!haveMax) {
            maxValue = cuts.at(i)->maxVal();
            haveMax = true;
         } else if (cuts.at(i)->maxVal() < maxValue) {
            maxValue = cuts.at(i)->maxVal();
         }
      }
   }
   dataSubselector::RangeCut::IntervalType 
      type(dataSubselector::RangeCut::CLOSED);
   if (!haveMin) {
      type = dataSubselector::RangeCut::MAXONLY;
   } else if (!haveMax) {
      type = dataSubselector::RangeCut::MINONLY;
   }
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(cuts.at(0)->colname(), "[]", tokens);
   return new dataSubselector::RangeCut(tokens.at(0),
                                        cuts.at(0)->unit(),
                                        minValue, maxValue, type,
                                        cuts.at(0)->index());
}

void joinPaths(const std::string & components,
               std::string & final_path) {
   std::vector<std::string> tokens; 
   facilities::Util::stringTokenize(components, " ", tokens);
   if (tokens.size() == 0) {
      throw std::runtime_error("dataSubselector::Cuts: joinPaths: "
                               "empty string input");
   }
   if (tokens.size() == 1) {
      final_path = tokens[0];
      return;
   }
   final_path = facilities::commonUtilities::joinPath(tokens[0], tokens[1]);
   for (size_t i(2); i < tokens.size(); i++) {
      final_path = facilities::commonUtilities::joinPath(final_path,tokens[i]);
   }
   return;
}

} // anonymous namespace

namespace dataSubselector {

Cuts::Cuts(const std::vector<std::string> & eventFiles,
           const std::string & extname, bool check_columns,
           bool skipTimeRangeCuts, bool skipEventClassCuts) {
   std::vector<Cuts> my_cuts;
   for (size_t i = 0; i < eventFiles.size(); i++) {
      my_cuts.push_back(Cuts(eventFiles.at(i), extname, check_columns,
                             skipTimeRangeCuts, skipEventClassCuts));
      if (i > 0) {
         if (!my_cuts.front().compareWithoutGtis(my_cuts.back())) {
            std::ostringstream message;
            message << "DSS keywords in " << eventFiles.at(i)
                    << " do not match those in " << eventFiles.front();
            throw std::runtime_error(message.str());
         }
      }
   }
   if (!my_cuts.empty()) {
      *this = mergeGtis(my_cuts);
   }
   set_irfName(eventFiles.at(0), extname);
}

Cuts Cuts::mergeGtis(std::vector<Cuts> & cuts_vector) {
// Gather non-Gti cuts.
   dataSubselector::Cuts my_cuts;
   const dataSubselector::Cuts & firstCuts(cuts_vector.front());
   for (size_t i = 0; i < firstCuts.size(); i++) {
      if (firstCuts[i].type() != "GTI") {
         my_cuts.addCut(firstCuts[i]);
      }
   }
   
// Merge all of the GTIs into one, taking the union of the intervals.
   dataSubselector::Gti merged_gti;
   std::vector<const dataSubselector::GtiCut *> gtiCuts;
   for (size_t i = 0; i < cuts_vector.size(); i++) {
      cuts_vector.at(i).getGtiCuts(gtiCuts);
      for (size_t j = 0; j < gtiCuts.size(); j++) {
         if (i == 0 && j == 0) {
            merged_gti = gtiCuts.at(j)->gti();
         } else {
            merged_gti = merged_gti | gtiCuts.at(j)->gti();
         }
      }
   }

   if (merged_gti.getNumIntervals() > 0) {
      my_cuts.addGtiCut(merged_gti);
   }
   return my_cuts;
}

void Cuts::getGtiCuts(std::vector<const GtiCut *> & gtiCuts) {
   gtiCuts.clear();
   for (size_t i = 0; i < m_cuts.size(); i++) {
      if (m_cuts.at(i)->type() == "GTI") {
         gtiCuts.push_back(dynamic_cast<GtiCut *>(m_cuts.at(i)));
      }
   }
}

Cuts::Cuts(const std::string & eventFile, const std::string & extname,
           bool check_columns, bool skipTimeRangeCuts,
           bool skipEventClassCuts) {
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
      for (size_t i = 0; i < colnames.size(); i++) {
         ::toUpper(colnames[i]);
      }
   }

   const tip::Header & header = ext->getHeader();

// NB: The .get(...) method does not work for unsigned int arguments.
   int nkeys;
   header["NDSKEYS"].get(nkeys);

   std::string type, unit, value, ref("");

   for (int keynum = 1; keynum <= nkeys; keynum++) {
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
         if ((type != "TIME" || !skipTimeRangeCuts) &&
             (type != "EVENT_CLASS" || !skipEventClassCuts)) {
            m_cuts.push_back(new RangeCut(colname, unit, value, indx));
         }
      } else if (type == "TIME" && value == "TABLE") {
         m_cuts.push_back(new GtiCut(eventFile));
      } else if (type.substr(0, 8) == "BIT_MASK") {
         std::vector<std::string> tokens;
         facilities::Util::stringTokenize(type, "(),", tokens);
         unsigned int bit_position = std::atoi(tokens[2].c_str());
         m_cuts.push_back(new BitMaskCut(tokens[1], bit_position));
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
   set_irfName(eventFile, extname);
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

unsigned int Cuts::addBitMaskCut(const std::string & colname,
                                 unsigned int bitPosition) {
   return addCut(new BitMaskCut(colname, bitPosition));
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

unsigned int Cuts::mergeRangeCuts() {
   std::vector<RangeCut *> rangeCuts;
   for (size_t j = 0; j < m_cuts.size(); j++) {
      if (m_cuts.at(j)->type() == "range") {
         RangeCut * rangeCut(dynamic_cast<RangeCut *>(m_cuts.at(j)));
         rangeCuts.push_back(rangeCut);
      }
   }

   std::map<std::string, int> colnames;
   for (size_t j = 0; j < rangeCuts.size(); j++) {
      colnames[rangeCuts.at(j)->colname()] = 1;
   }

   for (std::map<std::string, int>::const_iterator colname(colnames.begin());
        colname != colnames.end(); ++colname) {
      removeRangeCuts(colname->first, rangeCuts);
      m_cuts.push_back(::mergeRangeCuts(rangeCuts));
      for (size_t i = 0; i < rangeCuts.size(); i++) {
         delete rangeCuts.at(i);
      }
   }

   return m_cuts.size();
}

unsigned int Cuts::removeRangeCuts(const std::string & colname,
                                   std::vector<RangeCut *> & removedCuts) {
   removedCuts.clear();
   for (size_t j = 0; j < m_cuts.size(); j++) {
      if (m_cuts.at(j)->type() == "range") {
         RangeCut * rangeCut(dynamic_cast<RangeCut *>(m_cuts.at(j)));
         if (rangeCut->colname() == colname) {
            removedCuts.push_back(rangeCut);
            m_cuts.at(j) = 0;
         }
      }
   }
   std::vector<CutBase *> held_cuts;
   for (size_t j = 0; j < m_cuts.size(); j++) {
      if (m_cuts.at(j)) {
         held_cuts.push_back(m_cuts.at(j));
      }
   }
   m_cuts = held_cuts;
   return m_cuts.size();
}

void Cuts::writeDssKeywords(tip::Header & header) const {
   removeDssKeywords(header);
   int ndskeys(0);
   for (size_t i = 0; i < m_cuts.size(); i++) {
      if (!isTimeCut(*m_cuts.at(i)) || m_cuts.at(i)->type() == "GTI") {
         ndskeys++;
         m_cuts[i]->writeDssKeywords(header, ndskeys);
      }
   }
   header["NDSKEYS"].set(ndskeys);
}

void Cuts::writeDssTimeKeywords(tip::Header & header) const {
   removeDssKeywords(header);

   std::vector<CutBase *> my_time_cuts;
   for (size_t i = 0; i < m_cuts.size(); i++) {
      if (isTimeCut(*m_cuts.at(i))) {
         my_time_cuts.push_back(m_cuts.at(i));
      }
   }

   int ndskeys = my_time_cuts.size();
   header["NDSKEYS"].set(ndskeys);
   for (unsigned int i = 0; i < my_time_cuts.size(); i++) {
      my_time_cuts.at(i)->writeDssKeywords(header, i + 1);
   }
}

bool Cuts::isTimeCut(const CutBase & cut) {
   if (cut.type() == "GTI" || 
       (cut.type() == "range" && 
        dynamic_cast<RangeCut &>(const_cast<CutBase &>(cut)).colname() 
        == "TIME")) {
      return true;
   }
   return false;
}

void Cuts::removeDssKeywords(tip::Header & header) const {
   int ndskeys(0);
   try {
      header["NDSKEYS"].get(ndskeys);
      char * dskeys[] = {"DSTYP", "DSUNI", "DSVAL", "DSREF"};
      for (int i = 0; i < ndskeys; i++) {
         for (int j = 0; j < 4; j++) {
            std::ostringstream keyname;
            keyname << dskeys[j] << i+1;
            tip::Header::Iterator keyword = header.find(keyname.str());
            if (keyword != header.end()) {
               header.erase(keyword);
            }
         }
      }
      header.erase("NDSKEYS");
   } catch (tip::TipException & eObj) {
      if (!st_facilities::Util::expectedException(eObj, "keyword not found")) {
         throw;
      }
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

void Cuts::writeCuts(std::ostream & stream, bool suppressGtis) const {
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      if (!suppressGtis || m_cuts.at(i)->type() != "GTI") {
         m_cuts.at(i)->writeCut(stream, i + 1);
      } else {
         stream << "DSTYP" << i+1 << ": TIME\n"
                << "DSUNI" << i+1 << ": s\n"
                << "DSVAL" << i+1 << ": TABLE\n"
                << "DSREF" << i+1 << ": :GTI\n\n"
                << "GTIs: (suppressed)\n\n";
      }
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

const std::string & Cuts::irfName() const {
   return m_irfName;
}

void Cuts::
read_bitmask_mapping(const std::string & pass_ver,
                     std::map<unsigned int, std::string> & irfs) const{
   irfs.clear();
   std::string sub_path;
   ::joinPaths("data glast lat irf_index.fits", sub_path);
   std::string irf_index = facilities::commonUtilities::joinPath(
      facilities::commonUtilities::getEnvironment("CALDB"), sub_path);
   const tip::Table * irf_map 
      = tip::IFileSvc::instance().readTable(irf_index, "BITMASK_MAPPING");
   tip::Table::ConstIterator it(irf_map->begin());
   tip::ConstTableRecord & row = *it;
   for ( ; it != irf_map->end(); ++it) {
      std::string event_class;
      int bitpos;
      row["event_class"].get(event_class);
      row["bitposition"].get(bitpos);
      size_t evlen(event_class.length() - 1);   // neglect termination char
      std::string my_pass_ver(event_class.substr(0, 2) + 
                              event_class.substr(evlen-1, 2));
      if (my_pass_ver == pass_ver) {
         irfs[bitpos] = event_class;
      }
   }
   delete irf_map;
   if (irfs.size() == 0) {
      std::ostringstream message;
      message << "dataSubselector::Cuts::read_bitmask_mapping: "
              << "PASS_VER not found in irf_index.fits: "
              << pass_ver;
      throw std::runtime_error(message.str());
   }
}

void Cuts::set_irfName(const std::string & infile, 
                       const std::string & ext) {
/// @todo Improve error handling when PASS_VER does not exist or does not
/// have "P#V#" format.
   std::auto_ptr<const tip::Extension> 
      events(tip::IFileSvc::instance().readExtension(infile, ext));
   std::string pass_ver;
   try {
      events->getHeader()["PASS_VER"].get(pass_ver);
   } catch (tip::TipException & eObj) {
      if (st_facilities::Util::expectedException(eObj,"Cannot read keyword")) {
         // Do nothing for now.
         return;
      }
   }
   if (pass_ver == "NONE") {
      // Do nothing for now.
      return;
   }

   std::map<unsigned int, std::string> irfs;
   read_bitmask_mapping(pass_ver, irfs);

   std::ostringstream irf_name;

   for (size_t i(0); i < m_cuts.size(); i++) {
      dataSubselector::BitMaskCut * bit_mask_cut 
         = dynamic_cast<dataSubselector::BitMaskCut *>(
            const_cast<dataSubselector::CutBase *>(m_cuts[i]));
      if (bit_mask_cut) {
         irf_name << irfs[bit_mask_cut->bitPosition()];
         continue;
      }

      dataSubselector::RangeCut * convtype_cut
         = dynamic_cast<dataSubselector::RangeCut *>(
            const_cast<dataSubselector::CutBase *>(m_cuts[i]));
      if (convtype_cut && convtype_cut->colname() == "CONVERSION_TYPE") {
         if (convtype_cut->minVal() == 0 && 
             convtype_cut->maxVal() == 0) {
            irf_name << "::FRONT";
         } else if (convtype_cut->minVal() == 1 && 
                    convtype_cut->maxVal() == 1) {
            irf_name << "::BACK";
         }
      }
   }
   m_irfName = irf_name.str();
}

} // namespace dataSubselector
