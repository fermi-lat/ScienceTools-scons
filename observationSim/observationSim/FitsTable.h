/**
 * @file FitsTable.h
 * @brief Declaration of class to write FITS binary tables.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_FitsTable_h
#define observationSim_FitsTable_h

#include <vector>
#include <string>
#include "fitsio.h"

namespace observationSim {

/**
 * @class FitsTable
 * @brief A class to write FITS binary tables using cfitsio.
 *
 * The data for these tables are passed as doubles, but each column is
 * cast into the desired data-type ("1E", "1D", "1I", etc.), using the
 * format vector.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class FitsTable {

public:

   /// @param filename The name of the output FITS file.
   /// @param extName The name of this binary table extension.
   /// @param columnNames The names of the columns in the FITS binary table.
   /// @param format The data type of the column data: "1I", "1E", "1D".
   /// @param unit The units of the data, e.g., "seconds", "degrees", etc.
   FitsTable(const std::string filename, const std::string extName,
             const std::vector<std::string> &columnNames, 
             const std::vector<std::string> &format,
             const std::vector<std::string> &unit);

   ~FitsTable();

   void writeTableData(const std::vector<std::vector<double> > &data);

private:

   fitsfile *m_fptr;

   char *m_extName;

   int m_tfields;

// These arrays are passed to the cfitsio routines and contain type,
// format, and unit information.  (ANSI C++ requires a literal be used
// to set the size of these arrays.)
   char *m_ttype[100];
   char *m_tform[100];
   char *m_tunit[100];

};

} // namespace observationSim

#endif // observationSim_FitsTable_h
