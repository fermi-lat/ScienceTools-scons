/** \file FitsTabularData.h

    \brief Low level abstract interface to Fits tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsTabularData_h
#define table_FitsTabularData_h

#include <map>
#include <string>

#include "fitsio.h"

#include "FitsExtensionData.h"
#include "table/ITabularData.h"
#include "table/table_types.h"

namespace table {

  /** \class FitsTabularData

      \brief Low level abstract interface to Fits tabular data.
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

      /** \brief Read a keyword from this extension.
          \param name The name of the keyword to read.
          \param value The output value.
      */
      virtual void readKeyword(const std::string & name, double & value) const;

    private:
      FitsExtensionData m_extension;
      std::map<std::string, int> m_col_info;
      std::string m_file_name;
      std::string m_table_name;
      fitsfile * m_fp;
      Index_t m_num_records;
  };

}

#endif
