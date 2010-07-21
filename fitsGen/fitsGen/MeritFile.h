/**
 * @file MeritFile.h
 * @brief Declaration for MeritTuple abstraction.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_MeritFile_h
#define fitsGen_MeritFile_h

#include "tip/Table.h"

namespace fitsGen {

/**
 * @class MeritFile
 * @brief Abstraction/interface layer for using tip to read merit
 * files.
 *
 * @author J. Chiang
 */

class MeritFile {

public:

   MeritFile(const std::string & meritfile,
             const std::string & tree="MeritTuple",
             const std::string & filter="");

   ~MeritFile();

   void next();

   void prev();

   double operator[](const std::string & fieldname) const;

   tip::ConstTableRecord & row() const {
      return m_row;
   }

   tip::Index_t nrows() const {
      return m_nrows;
   }

   tip::Table::ConstIterator begin() const;

   tip::Table::ConstIterator end() const;

   tip::Table::ConstIterator & itor();

   double tstart() const {
      return m_tstart;
   }

   double tstop() const {
      return m_tstop;
   }

   /// @brief Set the start and stop times of the GTI by hand.
   /// This filter will be applied to the data in addition to the 
   /// filter string.
   void setStartStop(double tstart, double tstop);
   
   /// @return Conversion type (e.g., front=0, back=1) of current row.
   short int conversionType() const;

private:

   const tip::Table * m_table;
   tip::Table::ConstIterator m_it;
   tip::ConstTableRecord & m_row;
   tip::Index_t m_nrows;

   bool m_haveTime;
   double m_tstart;
   double m_tstop;
};

} // namespace fitsGen

#endif // fitsGen_MeritFile_h
