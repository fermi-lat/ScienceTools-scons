/** \file FitsTabularData.h

    \brief Low level interface to Fits format tabular data.

    \author James Peachey, HEASARC
*/
#ifndef FitsTabularData_h
#define FitsTabularData_h

#include <exception>
#include <map>
#include <string>

#include "fitsio.h"

#include "table/table_types.h"
#include "table/ITabularData.h"

namespace table {

  /** \class FitsException

      \brief Exceptions thrown by FitsTabularData.
  */
  class FitsException : public std::exception {
    public:
      virtual const char * what() const throw() { return "FitsError"; }
  };

  /** \class FitsTabularData
      \brief Low level abstract interface to Fits format tabular data.
  */
  class FitsTabularData : public ITabularData {
    public:
      FitsTabularData(const std::string & file_name, const std::string & table_name);

      FitsTabularData(const FitsTabularData & fits_data);

      virtual ~FitsTabularData();

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const;

      /** \brief Read a value from the current tabular data object.
          \param field The name of the field (column) to read.
          \param record_index The index whose value to read.
          \param value The output value.
      */
      virtual void read(const std::string & field, Index_t record_index, double & value) const;

    protected:
      // Iternal method to open the FITS file.
      void open();

      // Iternal method to close the FITS file.
      void close();

    private:
      std::map<std::string, int> m_col_info;
      std::string m_file_name;
      std::string m_table_name;
      fitsfile * m_fp;
      Index_t m_num_records;
  };

}

#endif
