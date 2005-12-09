/**
 * @file MeritFile.cxx
 * @brief Implementation for merit tuple file abstraction using tip.
 * @author J. Chiang
 *
 * $Header$
 */

#include "tip/IFileSvc.h"

#include "dataSubselector/Gti.h"

#include "fitsGen/MeritFile.h"

namespace fitsGen {

MeritFile::MeritFile(const std::string & meritfile,
                     const std::string & tree) :
   m_table(tip::IFileSvc::instance().editTable(meritfile, tree)),
   m_it(m_table->begin()),
   m_row(*m_it),
   m_nrows(m_table->getNumRecords()),
   m_gti(new dataSubselector::Gti()) 
{
   try {
      double start(m_row["EvtElapsedTime"].get());
      m_it = end();
      --m_it;
      double stop(m_row["EvtElapsedTime"].get());
      m_gti->insertInterval(start, stop);
      m_it = begin();
   } catch (...) { 
// assume there is no EvtElapsedTime, so do not insert any intervals
   }
}

MeritFile::~MeritFile() {
   delete m_gti;
   delete m_table;
}

void MeritFile::next() {
   ++m_it;
}

double MeritFile::operator[](const std::string & fieldname) const {
   return m_row[fieldname].get();
}

tip::Table::ConstIterator MeritFile::begin() const {
   return m_table->begin();
}

tip::Table::ConstIterator MeritFile::end() const {
   return m_table->end();
}

tip::Table::ConstIterator MeritFile::itor() const {
   return m_it;
}

const dataSubselector::Gti & MeritFile::gti() const {
   return *m_gti;
}

} // namespace fitsGen
