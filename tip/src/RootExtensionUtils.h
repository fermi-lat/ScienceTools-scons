/** \file RootExtensionUtils.h

    \brief Utilities to help manage Root specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef table_RootExtensionUtils_h
#define table_RootExtensionUtils_h

#include <cassert>
#include <map>
#include <sstream>
#include <string>

#include "table/TableException.h"
#include "table/table_types.h"

class TFile;
class TTree;

namespace table {

  class IData;
  class IHeaderData;

  /** \class RootExtensionUtils

      \brief Low level interface to Root format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Root access. It also
      acts as a factory for creating Root-specific header and data objects, which refer back to the
      RootExtensionUtils object which created them.
  */
  class RootExtensionUtils {
    public:
      /** \brief Create an object to provide low-level access to the given Root extension.
          \param file_name The name of the Root file.
          \param ext_name The name of the Root extension.
      */
      RootExtensionUtils(const std::string & file_name, const std::string & ext_name);

      /** \brief Destructor. Closes file if it is open.
      */
      ~RootExtensionUtils();

      /** \brief Create a header object which refers to this file. Caller is responsible for deleting
          the header object.
      */
      IHeaderData * createHeader();

      /** \brief Create a data object (table or image) which refers to this file. Caller is responsible
          for deleting the header object.
      */
      IData * createData();

      // General support for Root files:
      /** \brief Open the Root file.
      */
      void open();

      /** \brief Close the Root file.
      */
      void close();

      // Table-specific support:
      /** Struct holding information about a Branch.
      */
      struct ColumnInfo {
          std::string m_name;
          long m_repeat;
          int m_col_num;
          int m_type;
          double m_value;
      };

      /** \brief Open the Root table. Exceptions will be thrown if the extension does not exist, or if
          the extension is not a table (TTree).
      */
      void openTable();

      // Non-virtual helper functions for ITabularData interface:
      
      /** \brief Return the number of records in the current tabular data object (the number of rows
          in the Root file).
      */
      Index_t getNumRecords() const { return m_num_records; }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Return the number of elements in the given field (the number of items in a vector column.
      */
      Index_t getFieldNumElements(FieldIndex_t field_index) const;

      /** \brief Templated function which can get any kind of data from a Root table. This
          method throws an exception if the extension is not a table.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end, T * dest) const;

      /** \brief Templated function which can set any kind of data in a Root table. This
          method throws an exception if the extension is not a table.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell to set.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to one past the last element in the output sequence.
      */
      template <typename T>
      void setCellGeneric(int col_num, Index_t record_index, Index_t src_begin, T * dest_begin, T * dest_end);

    private:
      std::string formatWhat(const std::string & msg) const;
      std::string m_file_name;
      std::string m_ext_name;
      std::map<std::string, ColumnInfo> m_col_name_lookup;
      std::map<int, ColumnInfo> m_col_num_lookup;
      Index_t m_num_records;
      TFile * m_fp;
      TTree * m_tree;
  };

  // Getting columns.
  template <typename T>
//  inline void RootExtensionUtils::getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end,
//    T * dest_begin) const {
  inline void RootExtensionUtils::getCellGeneric(int, Index_t, Index_t, Index_t,
    T *) const {
    assert(0);
  }

  // Getting column values as strings is a special case because Cfitsio gets them as char *.
  // This is tricky and painful to get right because of Cfitsio's way of indicating the width
  // of columns. Since there is no immediate need to manage strings, this is simply not supported
  // for now.
  template <>
//  inline void RootExtensionUtils::getCellGeneric<double>(int col_num, Index_t record_index, Index_t src_begin,
//    Index_t src_end, double * dest) const {
  inline void RootExtensionUtils::getCellGeneric<double>(int, Index_t, Index_t,
    Index_t, double *) const {
    
  }

  // Setting columns.
  template <typename T>
//  inline void RootExtensionUtils::setCellGeneric(int col_num, Index_t record_index, Index_t src_begin,
//    T * dest_begin, T * dest_end) {
  inline void RootExtensionUtils::setCellGeneric(int , Index_t , Index_t ,
    T *, T *) {
    assert(0);
  }

}

#endif
