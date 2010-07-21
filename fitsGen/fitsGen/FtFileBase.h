/**
 * @file FtFileBase.h
 * @brief Declaration of FT1/2 file base class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_FtFileBase_h
#define fitsGen_FtFileBase_h

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include "astro/JulianDate.h"

namespace tip {
   class Header;
}

namespace fitsGen {

/**
 * @class FtFileBase
 * @brief Base class for FT1/2 files.
 * files.
 *
 * @author J. Chiang
 */

class FtFileBase {

public:

   FtFileBase(const std::string & outfile, tip::Index_t nrows=0);

   virtual ~FtFileBase();

   virtual void close();

   void next();

   void prev();

   tip::TableCell & operator[](const std::string & fieldname) {
      return (*m_it)[fieldname];
   }

   tip::Index_t nrows() const {
      return m_nrows;
   }

   void setNumRows(long nrows);

   const std::vector<std::string> & getFieldNames() const;

   void appendField(const std::string & colname, const std::string & format);

   tip::Table::Iterator begin();

   tip::Table::Iterator end();

   tip::Table::Iterator & itor();

   /// @return The EVENTS or SC_DATA extension FITS header.
   tip::Header & header();

   int fieldIndex(const std::string & colname) const {
      return m_table->getFieldIndex(colname) + 1;
   }

   template<class Type>
   void setPhduKeyword(const std::string & keyword,
                       const Type & value) const {
      tip::Image * phdu(tip::IFileSvc::instance().editImage(m_outfile, ""));
      tip::Header & header = phdu->getHeader();
      header[keyword].set(value);
      delete phdu;
   }

   void setObsTimes(double start, double stop);

   static void setMissionStart(int year, int month, int day, int sec);

   static const astro::JulianDate & missionStart() {
      return s_missionStart;
   }

   const std::string & filename() const {
      return m_outfile;
   }

   /// @brief Write the TSTART, TSTOP, DATE-OBS, DATE-END, ONTIME, TELAPSE
   /// keywords in the desired FITS extension.
   /// @param table FITS extension to be modified
   /// @param start_time observation start time in MET seconds
   /// @param stop_time observation stop time in MET seconds
   /// @param extension set to true if this is not the primary FITS HDU,
   ///        otherwise the TSTART, TSTOP, ONTIME, TELAPSE keywords will
   ///        be written
   /// @param mission_start The mission start time, with official date as
   ///        the default value
   static void writeDateKeywords(tip::Extension * table, double start_time,
                                 double stop_time, bool extension=true,
                                 const astro::JulianDate & mission_start
                                 =astro::JulianDate(2001, 1, 1, 0));

#ifndef SWIG   
   /// @return The current time ascertained using the <ctime> standard
   /// library. This is also copied from st_facilities.
   static astro::JulianDate currentTime();
#endif

protected:

   std::string m_outfile;
   tip::Table * m_table;
   tip::Table::Iterator m_it;
   tip::Index_t m_nrows;

   double m_startTime;
   double m_stopTime;

   static astro::JulianDate s_missionStart;

   void init(const std::string & templateFile,
             const std::string & extname);

};

} // namespace fitsGen

#endif // fitsGen_FtFileBase_h
