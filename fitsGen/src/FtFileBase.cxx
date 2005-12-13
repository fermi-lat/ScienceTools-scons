/**
 * @file FtFileBase.cxx
 * @brief Implementation of FT1/2 file base class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Image.h"

#include "fitsGen/FtFileBase.h"
#include "fitsGen/Util.h"

namespace fitsGen {

FtFileBase::FtFileBase(const std::string & outfile, long nrows) : 
   m_outfile(outfile), m_table(0), m_nrows(nrows),
   m_startTime(-1), m_stopTime(-1) {
}

void FtFileBase::init(const std::string & templateFile, 
                      const std::string & extname) {
   std::string ft_template(std::getenv("FITSGENROOT") + std::string("/data/") 
                            + templateFile);
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   fileSvc.createFile(m_outfile, ft_template);
   m_table = fileSvc.editTable(m_outfile, extname);
   setNumRows(m_nrows);
   m_it = m_table->begin();
}

FtFileBase::~FtFileBase() {
   close();
}

void FtFileBase::close() {
   if (m_table) {
      Util::writeDateKeywords(m_table, m_startTime, m_stopTime);
      delete m_table;
      m_table = 0;

      tip::Image * phdu(tip::IFileSvc::instance().editImage(m_outfile, ""));
      Util::writeDateKeywords(phdu, m_startTime, m_stopTime, false);
      delete phdu;
   }
}

void FtFileBase::next() {
   ++m_it;
}

void FtFileBase::prev() {
   --m_it;
}

void FtFileBase::setNumRows(long nrows) {
   m_table->setNumRecords(nrows);
   m_nrows = nrows;
}

void FtFileBase::appendField(const std::string & colname,
                          const std::string & format) {
   m_table->appendField(colname, format);
}

tip::Table::Iterator FtFileBase::begin() {
   return m_table->begin();
}

tip::Table::Iterator FtFileBase::end() {
   return m_table->end();
}

tip::Table::Iterator & FtFileBase::itor() {
   return m_it;
}

tip::Header & FtFileBase::header() {
   return m_table->getHeader();
}

void FtFileBase::setObsTimes(double start, double stop) {
   m_startTime = start;
   m_stopTime = stop;
}

} // namespace fitsGen
