/**
 * @file MeritFile.cxx
 * @brief Implementation for merit tuple file abstraction using tip.
 * @author J. Chiang
 *
 * $Header$
 */

#include "tip/IFileSvc.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"

#include "fitsGen/MeritFile.h"

namespace fitsGen {

MeritFile::MeritFile(const std::string & meritfile,
                     const std::string & tree,
                     const std::string & filter) :
   m_table(tip::IFileSvc::instance().readTable(meritfile, tree, filter)),
   m_it(m_table->begin()),
   m_row(*m_it),
   m_nrows(m_table->getNumRecords()),
   m_gti(new dataSubselector::Gti()),
   m_goodEvent1(new dataSubselector::Cuts()),
   m_goodEvent3(new dataSubselector::Cuts())
{
   try {
      double start(m_row["EvtElapsedTime"].get());
      m_it = end();
      --m_it;
      double stop(m_row["EvtElapsedTime"].get());
      m_gti->insertInterval(start, stop);
      m_it = begin();
      
      // CTBCORE > 0.1
      m_goodEvent1->addRangeCut("CTBCORE", "N/A", 0.1, 1.1);
      // CTBBestEnergyProb > 0.1
      m_goodEvent1->addRangeCut("CTBBestEnergyProb", "N/A", 0.1, 1.1);
      // CTBGAM > 0.35
      m_goodEvent1->addRangeCut("CTBGAM", "N/A", 0.35, 1.1);

      // CTBCORE > 0.35
      m_goodEvent3->addRangeCut("CTBCORE", "N/A", 0.35, 1.1);
      // CTBBestEnergyProb > 0.35
      m_goodEvent3->addRangeCut("CTBBestEnergyProb", "N/A", 0.35, 1.1);
      // CTBGAM > 0.50
      m_goodEvent3->addRangeCut("CTBGAM", "N/A", 0.50, 1.1);
   } catch (...) { 
// assume there is no EvtElapsedTime, so do not insert any intervals
   }
}

MeritFile::~MeritFile() {
   delete m_goodEvent1;
   delete m_goodEvent3;
   delete m_gti;
   delete m_table;
}

void MeritFile::next() {
   ++m_it;
}

void MeritFile::prev() {
   --m_it;
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

tip::Table::ConstIterator & MeritFile::itor() {
   return m_it;
}

const dataSubselector::Gti & MeritFile::gti() const {
   return *m_gti;
}

short int MeritFile::eventType() const {
   if (m_goodEvent1->accept(m_row)) {
      if (m_goodEvent3->accept(m_row)) { // Class A event
         return conversionType();
      } else {                           // Class B
         return conversionType() + 2;
      }
   }
   return -1;
}

short int MeritFile::conversionType() const {
   if (17 - m_row["Tkr1FirstLayer"].get() < 11.5) { // Front converting
      return 0;
   } else { // Back converting
      return 1;
   }
}

} // namespace fitsGen
