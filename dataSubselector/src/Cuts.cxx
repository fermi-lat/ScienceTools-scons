/**
 * @file Cuts.cxx
 * @brief Handel data selections and DSS keyword management.
 * @author J. Chiang
 *
 * $Header$
 */

#include "dataSubselector/Cuts.h"

//namespace dataSubselector {

Cuts::Cuts(tip::Header & header) : m_header(header) {
}

Cuts::~Cuts() {
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      m_cuts[i]->writeDssKeywords(m_header);
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
                         double minVal, double maxVal) {
   m_cuts.append(new Cuts::RangeCut(keyword, minVal, maxVal));
   return m_cuts.size();
}

unsigned int Cuts::addTableCut(const std::string & keyword,
                         tip::ConstTableRecord & table) {
   m_cuts.append(new Cuts::TableCut(keyword, table));
   return m_cuts.size();
}

unsigned int Cuts::addSkyConeCut(double ra, double dec, double radius) {
   m_cuts.append(new Cuts::SkyConeCut(ra, dec, radius));
   return m_cuts.size();
}

bool Cuts::RangeCut::accept(tip::ConstTableRecord & row) const {
   double value;
   row[m_keyword].get(value);
   return m_min <= value && value <= m_max;
}

void Cuts::RangeCut::writeDssKeywords(tip::Header & header, 
                                      unsigned int keynum) const {
   std::ostringstream 
}

bool Cuts::TableCut::accept(tip::ConstTableRecord & row) const {
   double value;
   row[m_keyword].get(value);
/// @todo implement this
   return true;
}

bool Cuts::SkyConeCut::accept(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
   double separation = m_coneCenter.difference(astro::SkyDir(ra, dec));
   return separation*180./M_PI <= m_radius;
}

} // namespace dataSubselector

