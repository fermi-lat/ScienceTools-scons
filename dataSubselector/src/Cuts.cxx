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

unsigned int Cuts::addRangeCut(const std::string & keyword,
                               const std::string & unit,
                               double minVal, double maxVal) {
   m_cuts.push_back(new Cuts::RangeCut(keyword, unit, minVal, maxVal));
   return m_cuts.size();
}

unsigned int Cuts::addTableCut(const std::string & keyword,
                               const std::string & unit,
                               const tip::Table & table) {
   m_cuts.push_back(new Cuts::TableCut(keyword, unit, table));
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

void Cuts::CutBase::writeDssKeywords(tip::Header & header, unsigned int keynum,
                                     const std::string & type, 
                                     const std::string & unit,
                                     const std::string & value) const {
   std::ostringstream key1, key2, key3;
   key1 << "DSTYP" << keynum;
   header[key1.str()].set(type);
   key2 << "DSUNI" << keynum;
   header[key2.str()].set(unit);
   key3 << "DSVAL" << keynum;
   header[key3.str()].set(value);
}

bool Cuts::RangeCut::accept(tip::ConstTableRecord & row) const {
   double value;
   row[m_keyword].get(value);
   return m_min <= value && value <= m_max;
}

void Cuts::RangeCut::writeDssKeywords(tip::Header & header, 
                                      unsigned int keynum) const {
   std::ostringstream value;
   value << m_min << ":" << m_max;
   CutBase::writeDssKeywords(header, keynum, m_keyword, m_unit, value.str());
}

bool Cuts::TableCut::accept(tip::ConstTableRecord & row) const {
   double value;
   row[m_keyword].get(value);
/// @todo implement this
   return true;
}

void Cuts::TableCut::writeDssKeywords(tip::Header & header,
                                      unsigned int keynum) const {
   const tip::Header & table_header = m_table.getHeader();
   std::string table_name;
   table_header["EXTNAME"].get(table_name);
   CutBase::writeDssKeywords(header, keynum, m_keyword, m_unit, table_name);
}

bool Cuts::SkyConeCut::accept(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
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
