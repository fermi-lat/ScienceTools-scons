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

#include "astro/JulianDate.h"

#include "st_facilities/Env.h"
#include "st_facilities/Util.h"

#include "fitsGen/FtFileBase.h"
#include "fitsGen/Util.h"

namespace fitsGen {

astro::JulianDate FtFileBase::s_missionStart(2001, 1, 1, 0);

void FtFileBase::setMissionStart(int year, int month, int day, int sec) {
   s_missionStart = astro::JulianDate(year, month, day, sec);
}

FtFileBase::FtFileBase(const std::string & outfile, long nrows) : 
   m_outfile(outfile), m_table(0), m_nrows(nrows),
   m_startTime(-1), m_stopTime(-1) {
}

void FtFileBase::init(const std::string & templateFile, 
                      const std::string & extname) {
   std::string ft_template(templateFile);
   if (templateFile == "ft1.tpl" || templateFile == "ft2.tpl") {
      ft_template = st_facilities::Env::appendFileName(
         st_facilities::Env::getDataDir("fitsGen"), templateFile);
   } 
   tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
   if (ft_template != "") {
      fileSvc.createFile(m_outfile, ft_template);
   } else {
      fileSvc.appendTable(m_outfile, extname);
   }
   m_table = fileSvc.editTable(m_outfile, extname);
   setNumRows(m_nrows);
   m_it = m_table->begin();
}

FtFileBase::~FtFileBase() {
   close();
}

void FtFileBase::close() {
   if (m_table) {
      Util::writeDateKeywords(m_table, m_startTime, m_stopTime, true,
                              s_missionStart);
      delete m_table;
      m_table = 0;

      tip::Image * phdu(tip::IFileSvc::instance().editImage(m_outfile, ""));
      Util::writeDateKeywords(phdu, m_startTime, m_stopTime, false,
                              s_missionStart);
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

const std::vector<std::string> & FtFileBase::getFieldNames() const {
   return m_table->getValidFields();
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
