/**
 * @file FtFileBase.h
 * @brief Declaration of FT1/2 file base class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_FtFileBase_h
#define fitsGen_FtFileBase_h

#include "tip/Table.h"

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

   FtFileBase(const std::string & outfile, long nrows=0);

   virtual ~FtFileBase();

   virtual void close();

   void next();

   void prev();

   tip::TableCell & operator[](const std::string & fieldname) {
      return (*m_it)[fieldname];
   }

   long nrows() const {
      return m_nrows;
   }

   void setNumRows(long nrows);

   void appendField(const std::string & colname, const std::string & format);

   tip::Table::Iterator begin();

   tip::Table::Iterator end();

   tip::Table::Iterator & itor();

   /// @return The EVENTS or SC_DATA extension FITS header.
   tip::Header & header();

   void setObsTimes(double start, double stop);

protected:

   std::string m_outfile;
   tip::Table * m_table;
   tip::Table::Iterator m_it;
   long m_nrows;

   double m_startTime;
   double m_stopTime;

   void init(const std::string & templateFile,
             const std::string & extname);

};

} // namespace fitsGen

#endif // fitsGen_FtFileBase_h
