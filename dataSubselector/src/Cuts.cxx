/**
 * @file Cuts.cxx
 * @brief Handle data selections and DSS keyword management.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "facilities/Util.h"

#include "tip/IFileSvc.h"

#include "dataSubselector/Cuts.h"

namespace {
   void toUpper(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::toupper(*it);
      }
   }
}

namespace dataSubselector {

Cuts::Cuts(const std::string & eventFile, const std::string & extension) {
   const tip::Table * table =
      tip::IFileSvc::instance().readTable(eventFile, extension);

   std::vector<std::string> colnames = table->getValidFields();
// FITS column names are in CAPS, not lowercase, so undo the damage
// wrought by tip:
   for (unsigned int i = 0; i < colnames.size(); i++) {
      ::toUpper(colnames[i]);
   }

   const tip::Header & header = table->getHeader();
   double nkeys_value;
   header["NDSKEYS"].get(nkeys_value);
// This is incredibly lame....one might as well use CCFits.
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
      if (value == "TABLE") {
         key4 << "DSREF" << keynum;
         header[key4.str()].get(ref);
      }
      std::string colname;
      unsigned int indx = parseColname(type, colname);
      if (std::find(colnames.begin(), colnames.end(), colname) 
          != colnames.end() && value != "TABLE") {
         m_cuts.push_back(new RangeCut(colname, unit, value, indx));
      } else if (value.find("CIRCLE") == 0) {
         m_cuts.push_back(new SkyConeCut(type, unit, value));
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
                               RangeType type, unsigned int indx) {
   m_cuts.push_back(new Cuts::RangeCut(colname, unit, minVal, maxVal, 
                                       type, indx));
   return m_cuts.size();
}

unsigned int Cuts::addGtiCut(const tip::Table & table) {
   m_cuts.push_back(new Cuts::GtiCut(table));
   return m_cuts.size();
}

unsigned int Cuts::addSkyConeCut(double ra, double dec, double radius) {
   m_cuts.push_back(new Cuts::SkyConeCut(ra, dec, radius));
   return m_cuts.size();
}

void Cuts::writeDssKeywords(tip::Header & header) const {
   header["NDSKEYS"].set(m_cuts.size());
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts[i]->writeDssKeywords(header, i + 1);
   }
}      

bool Cuts::operator==(const Cuts & rhs) const {
   if (size() != rhs.size()) {
      return false;
   }
// Note that the ordering of the cuts must be the same.
   for (unsigned int i = 0; i < size(); i++) {
      if (!(*m_cuts.at(i) == *rhs.m_cuts.at(i))) {
         return false;
      }
   }
   return true;
}

void Cuts::writeCuts(std::ostream & stream) const {
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts.at(i)->writeCut(stream, i + 1);
   }
}

void Cuts::CutBase::
writeCut(std::ostream & stream, unsigned int keynum) const {
   std::string type, unit, value, ref("");
   getKeyValues(type, unit, value, ref);
   stream << "DSTYP" << keynum << ": " << type << "\n"
          << "DSUNI" << keynum << ": " << unit << "\n"
          << "DSVAL" << keynum << ": " << value << "\n";
   if (ref != "") {
      stream << "DSREF" << keynum << ": " << ref << "\n";
   }
   stream << std::endl;
}

void Cuts::CutBase::writeDssKeywords(tip::Header & header,
                                     unsigned int keynum) const {
   std::string type, unit, value, ref("");
   getKeyValues(type, unit, value, ref);
   writeDssKeywords(header, keynum, type, unit, value, ref);
}

void Cuts::CutBase::writeDssKeywords(tip::Header & header, 
                                     unsigned int keynum,
                                     const std::string & type, 
                                     const std::string & unit,
                                     const std::string & value,
                                     const std::string & ref) const {
   std::ostringstream key1, key2, key3;
   key1 << "DSTYP" << keynum;
   header[key1.str()].set(type);
   key2 << "DSUNI" << keynum;
   header[key2.str()].set(unit);
   key3 << "DSVAL" << keynum;
   header[key3.str()].set(value);
   if (ref != "") {
      std::ostringstream key4;
      key4 << "DSREF" << keynum;
      header[key4.str()].set(ref);
   }
}

} // namespace dataSubselector
