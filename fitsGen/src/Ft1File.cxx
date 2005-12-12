/**
 * @file Ft1File.cxx
 * @brief Implementation of FT1 file abstraction.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Image.h"

#include "fitsGen/Ft1File.h"
#include "fitsGen/Util.h"

namespace fitsGen {

Ft1File::Ft1File(const std::string & outfile, long nrows) : 
   m_outfile(outfile), m_table(0), m_startTime(-1), m_stopTime(-1) {
   std::string ft1_template(std::getenv("FITSGENROOT") 
                            + std::string("/data/ft1.tpl"));

   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   fileSvc.createFile(outfile, ft1_template);
   m_table = fileSvc.editTable(outfile, "EVENTS");
   m_table->setNumRecords(nrows);
   m_it = m_table->begin();
}

Ft1File::~Ft1File() {
   close();
}

void Ft1File::close() {
   verifyObsTimes();

   if (m_table) {
      Util::writeDateKeywords(m_table, m_startTime, m_stopTime);
      delete m_table;
      m_table = 0;

      tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

      tip::Table * gtiTable(fileSvc.editTable(m_outfile, "GTI"));
      Util::writeDateKeywords(gtiTable, m_startTime, m_stopTime);
      delete gtiTable;

      tip::Image * phdu(fileSvc.editImage(m_outfile, ""));
      Util::writeDateKeywords(phdu, m_startTime, m_stopTime, false);
      delete phdu;
   }
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

void Ft1File::setObsTimes(double start, double stop) {
   m_startTime = start;
   m_stopTime = stop;
}

void Ft1File::verifyObsTimes() {
// Infer start and stop times from events if necessary.  The entries
// may not be ordered, so we need to loop over the entire dataset.
   double start, stop;
   if (m_startTime < 0 || m_stopTime < 0) {
      m_it = begin();
      start = (*m_it)["TIME"].get();
      stop = start;
      for ( ; m_it != end(); ++m_it) {
         double time((*m_it)["TIME"].get());
         if (time < start) {
            start = time;
         } else if (time > stop) {
            stop = time;
         }
      }
   }
   if (m_startTime < 0) {
      m_startTime = start;
   }
   if (m_stopTime < 0) {
      m_stopTime = stop;
   }
}


} // namespace fitsGen
