/**
 * @file FitsUtil.h
 * @brief Static functions for accessing data from FITS files.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef st_facilities_FitsUtil_h
#define st_facilities_FitsUtil_h

#include <string>
#include <vector>

namespace st_facilities {

/**
 * @class FitsUtil
 *
 * $Header$
 */

class FitsUtil {

public:

   /// Get a vector of values from the specified extension and column.
   static void getTableVector(const std::string & filename,
                              const std::string & extName,
                              const std::string & columnName,
                              std::vector<double> & branchVector);

   /// Get a vector from a given row of a record.
   static void getRecordVector(const std::string & filename,
                               const std::string & extName,
                               const std::string & columnName,
                               std::vector<double> & tableVector,
                               int recordNum = 0);

   /// Get the extension name of a FITS table HDU by extension number.
   static void getFitsHduName(const std::string & filename, int hdu,
                              std::string & hduName);

   /// Get the column names for a FITS table HDU.
   static void getFitsColNames(const std::string & filename, int hdu,
                               std::vector<std::string> & columnNames);

   /// Write checksum and datasum information for all HDUs in a FITS file.
   static void writeChecksums(const std::string & filename);

protected:

   FitsUtil() {}

};

} // namespace st_facilities

#endif // st_facilities_FitsUtil_h
