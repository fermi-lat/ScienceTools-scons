/**
 * @file Ft1File.cxx
 * @brief Implementation of FT1 file abstraction.
 * @author J. Chiang
 *
 * $Header$
 */

#include <string>

#include "tip/IFileSvc.h"

#include "fitsGen/Ft1File.h"
#include "fitsGen/Util.h"

namespace fitsGen {

Ft1File::Ft1File(const std::string & outfile, long nrows) : m_table(0) {
   std::string ft1_template(std::getenv("FITSGENROOT") 
                            + std::string("/data/ft1.tpl"));

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   fileSvc.createFile(outfile, ft1_template);
   m_table = fileSvc.editTable(outfile, "EVENTS");
   m_table->setNumRecords(nrows);
   m_it = m_table->begin();
}

Ft1File::~Ft1File() {
// Infer start and stop times from events.  The entries may not be
// ordered, so we need to loop over the entire dataset.
   m_it = begin();
   double start((*m_it)["TIME"].get());
   double stop(start);
   for ( ; m_it != end(); ++m_it) {
      double time((*m_it)["TIME"].get());
      if (time < start) {
         start = time;
      } else if (time > stop) {
         stop = time;
      }
   }
   Util::writeDateKeywords(m_table, start, stop);
   delete m_table;
}

void Ft1File::next() {
   ++m_it;
}

void Ft1File::setNumRows(long nrows) {
   m_table->setNumRecords(nrows);
}

void Ft1File::appendField(const std::string & colname,
                          const std::string & format) {
   m_table->appendField(colname, format);
}

tip::Table::Iterator Ft1File::begin() {
   return m_table->begin();
}

tip::Table::Iterator Ft1File::end() {
   return m_table->end();
}

tip::Table::Iterator Ft1File::itor() {
   return m_it;
}

tip::Header & Ft1File::header() {
   return m_table->getHeader();
}

} // namespace fitsGen
