/**
 * @file Cuts.cxx
 * @brief Handle data selections and DSS keyword management.
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>

#include "dataSubselector/Cuts.h"

namespace dataSubselector {

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

unsigned int Cuts::addRangeCut(const std::string & keyword,
                               const std::string & unit,
                               double minVal, double maxVal, 
                               RangeType type) {
   m_cuts.push_back(new Cuts::RangeCut(keyword, unit, minVal, maxVal, type));
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
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts[i]->writeDssKeywords(header, i + 1);
   }
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

bool Cuts::RangeCut::accept(double value) const {
   if (m_type == Cuts::MINONLY) {
      return m_min <= value;
   } else if (m_type == Cuts::MAXONLY) {
      return value <= m_max;
   }
   return m_min <= value && value <= m_max;
}

void Cuts::RangeCut::writeDssKeywords(tip::Header & header, 
                                      unsigned int keynum) const {
   std::ostringstream value;
   if (m_type == Cuts::MINONLY) {
      value << m_min << ":";
   } else if (m_type == Cuts::MAXONLY) {
      value << ":" << m_max;
   } else {
      value << m_min << ":" << m_max;
   }
   CutBase::writeDssKeywords(header, keynum, m_keyword, m_unit, value.str());
}

bool Cuts::GtiCut::accept(tip::ConstTableRecord & row) const {
   double time;
   row["TIME"].get(time);
   return accept(time);
}

bool Cuts::GtiCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator it;
   if ( (it = params.find("TIME")) != params.end()) {
      return accept(it->second);
   }
   return true;
}

bool Cuts::GtiCut::accept(double time) const {
   tip::Table::ConstIterator it = m_table.begin();
   tip::ConstTableRecord & interval = *it;
   for ( ; it != m_table.end(); ++it) {
      double start, stop;
      interval["START"].get(start);
      interval["STOP"].get(stop);
      if (start <= time && time <= stop) {
         return true;
      }
   }
   return false;
}

void Cuts::GtiCut::writeDssKeywords(tip::Header & header,
                                    unsigned int keynum) const {
   const tip::Header & table_header = m_table.getHeader();
   std::string table_name;
   table_header["EXTNAME"].get(table_name);
   CutBase::writeDssKeywords(header, keynum, "TIME", "s", "TABLE", ":GTI");
}

bool Cuts::SkyConeCut::accept(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
   return accept(ra, dec);
}

bool Cuts::
SkyConeCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator ra;
   std::map<std::string, double>::const_iterator dec;
   if ( (ra = params.find("RA")) != params.end() &&
        (dec = params.find("DEC")) != params.end() ) {
      return accept(ra->second, dec->second);
   }
   return true;
}

bool Cuts::SkyConeCut::accept(double ra, double dec) const {
   double separation = m_coneCenter.difference(astro::SkyDir(ra, dec));
   return separation*180./M_PI <= m_radius;
}

void Cuts::SkyConeCut::writeDssKeywords(tip::Header & header,
                                        unsigned int keynum) const {
   std::ostringstream value;
   value << "CIRCLE(" 
         << m_coneCenter.ra() << "," 
         << m_coneCenter.dec() << ","
         << m_radius << ")";
   CutBase::writeDssKeywords(header, keynum, "POS(RA,DEC)", "deg", 
                             value.str());
}

} // namespace dataSubselector
