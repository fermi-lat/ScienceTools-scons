/**
 * @file Ft1File.h
 * @brief Declaration of FT1 file abstraction
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_Ft1File_h
#define fitsGen_Ft1File_h

#include "tip/Table.h"

namespace tip {
   class Header;
}

namespace fitsGen {

/**
 * @class Ft1File
 * @brief Abstraction/interface layer for using tip to write FT1
 * files.
 *
 * @author J. Chiang
 */

class Ft1File {

public:

   Ft1File(const std::string & outfile, long nrows=0);

   ~Ft1File();

   void next();

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

   tip::Table::Iterator itor();

   /// @return The EVENTS extension FITS header.
   tip::Header & header();

private:

   tip::Table * m_table;
   tip::Table::Iterator m_it;
   long m_nrows;

};

} // namespace fitsGen

#endif // fitsGen_Ft1File_h
