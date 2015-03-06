/**
 * @file FtFileBase.cxx
 * @brief Implementation of FT1/2 file base class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "astro/JulianDate.h"

#include "facilities/commonUtilities.h"

#include "fitsGen/FtFileBase.h"

namespace fitsGen {

astro::JulianDate FtFileBase::s_missionStart(2001, 1, 1, 0);

void FtFileBase::setMissionStart(int year, int month, int day, int sec) {
   s_missionStart = astro::JulianDate(year, month, day, sec);
}

FtFileBase::FtFileBase(const std::string & outfile, tip::Index_t nrows) : 
   m_outfile(outfile), m_table(0), m_nrows(nrows),
   m_startTime(-1), m_stopTime(-1) {
}

void FtFileBase::init(const std::string & templateFile, 
                      const std::string & extname) {
   std::string ft_template(templateFile);
   if (templateFile == "ft1.tpl" || 
       templateFile == "ft1_p7.tpl" || 
       templateFile == "ft2.tpl" ||
       templateFile == "lle.tpl") {
      ft_template = facilities::commonUtilities::joinPath(
         facilities::commonUtilities::getDataPath("fitsGen"), templateFile);
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
      writeDateKeywords(m_table, m_startTime, m_stopTime,
                        true, s_missionStart);
      delete m_table;
      m_table = 0;

      tip::Image * phdu(tip::IFileSvc::instance().editImage(m_outfile, ""));
      writeDateKeywords(phdu, m_startTime, m_stopTime, 
                        false, s_missionStart);
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

/// Copied from st_facilities::Util in order to make fitsGen part of
/// GlastRelease without dragging in other dependencies.
void FtFileBase::writeDateKeywords(tip::Extension * table, double start_time, 
                                   double stop_time, bool extension,
                                   const astro::JulianDate & mission_start) {
   (void)(extension);
   static double secsPerDay(8.64e4);
   tip::Header & header = table->getHeader();
   astro::JulianDate current_time = currentTime();
   try {
      header["DATE"].set(current_time.getGregorianDate());
   } catch (...) {
   }
// The official mission start time is Jan 1 2001:
   astro::JulianDate date_start(mission_start + start_time/secsPerDay);
   astro::JulianDate date_stop(mission_start + stop_time/secsPerDay);
   try {
      header["DATE-OBS"].set(date_start.getGregorianDate());
      header["DATE-END"].set(date_stop.getGregorianDate());
   } catch (...) {
   }
   try {
      header["TSTART"].set(start_time);
      header["TSTOP"].set(stop_time);
   } catch (...) {
   }
// Update TELAPSE keyword if it exists
   if (table->getHeader().find("TELAPSE") != table->getHeader().end()) {
      header["TELAPSE"].set(stop_time - start_time);
   }
}

astro::JulianDate FtFileBase::currentTime() {
   std::time_t my_time = std::time(0);
   std::tm * now = std::gmtime(&my_time);
   if (now != 0) {
      double hours = now->tm_hour + now->tm_min/60. + now->tm_sec/3600.;
      astro::JulianDate current_time(now->tm_year + 1900, now->tm_mon + 1,
                                     now->tm_mday, hours);
      return current_time;
   } else {
      throw std::runtime_error("currentTime:\n"
                               + std::string("cannot be ascertained, ")
                               + "std::time returns a null value.");
   }
}

} // namespace fitsGen
