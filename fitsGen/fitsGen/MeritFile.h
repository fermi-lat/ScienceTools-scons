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
             const std::string & tree="MeritTuple");

   ~MeritFile();

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

   /// @return A Gti object containing the GTIs for this merit file.
   /// This comprises just the beginning and end times for the data.
   const dataSubselector::Gti & gti() const; 

private:

   const tip::Table * m_table;
   tip::Table::ConstIterator m_it;
   tip::ConstTableRecord & m_row;
   long m_nrows;

   dataSubselector::Gti * m_gti;
};

} // namespace fitsGen

#endif // fitsGen_MeritFile_h
