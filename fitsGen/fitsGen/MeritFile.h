/**
 * @file MeritFile.h
 * @brief Declaration for MeritTuple abstraction.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_MeritFile_h
#define fitsGen_MeritFile_h

namespace dataSubselector {
   class Cuts;
   class Gti;
}

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

   /// @return A Gti object containing the GTIs for this merit file.
   /// This comprises just the beginning and end times for the data.
   const dataSubselector::Gti & gti() const;

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

   dataSubselector::Gti * m_gti;
};

} // namespace fitsGen

#endif // fitsGen_MeritFile_h
