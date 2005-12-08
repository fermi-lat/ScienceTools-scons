/**
 * @file EgretSmdb.h
 * @brief Declaration for EGRET summary database class interface.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_EgretSmdb_h
#define fitsGen_EgretSmdb_h

#include "tip/Table.h"

/**
 * @class EgretSmdb
 * @brief Provide a convenient interface to tip access of EGRET
 * summary database FITS files.
 *
 * @author J. Chiang
 */

class EgretSmdb {

public:

   EgretSmdb(const std::string & smdbfile);

   ~EgretSmdb();

   void next();
   
   double operator[](const std::string & fieldname) const;

   tip::ConstTableRecord & row() const {
      return m_row;
   }

   long nrows() const {
      return m_nrows;
   }

   tip::Table::ConstIterator begin() const;

   tip::Table::ConstIterator end() const;

   tip::Table::ConstIterator itor() const;

   double arrivalTime() const;

   double arrivalTjd() const;

   bool tascIn() const;

   int eventClass() const;
   
   static void setMdjRef(double mjdref);
   
   static double mjdref() {
      return s_mjdref;
   }

private:

   const tip::Table * m_table;
   tip::Table::ConstIterator m_it;
   tip::ConstTableRecord & m_row;
   long m_nrows;

   static double s_mjdref;

};

#endif // fitsGen_EgretSmdb_h
