/** \file RootExtensionManager.h

    \brief Utilities to help manage Root specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef table_RootExtensionManager_h
#define table_RootExtensionManager_h

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "TTree.h"

#include "table/IExtensionManager.h"
#include "table/TableException.h"
#include "table/table_types.h"

class TBranch;
class TFile;
class TLeaf;

namespace table {

  class IData;
  class IHeaderData;
  class ITabularData;

  class LeafBuffer {
    public:
      LeafBuffer(TTree * tree, const std::string & leaf_name, const std::string & leaf_type);

      ~LeafBuffer();

      template <typename T>
      void get(T & val) const;

    private:
      LeafBuffer(const LeafBuffer &); // Make sure nobody copies one of these.
      std::string m_leaf_name;
      TTree * m_tree;
      void * m_buf;
  };

  template <typename T>
  inline void LeafBuffer::get(T & val) const { val = T(*static_cast<const double *>(m_buf)); };

  /** \class RootExtensionManager

      \brief Low level interface to Root format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Root access. It also
      acts as a factory for creating Root-specific header and data objects, which refer back to the
      RootExtensionManager object which created them.
  */
  class RootExtensionManager : public IExtensionManager {
    public:
      /** \brief Create an object to provide low-level access to the given Root extension.
          \param file_name The name of the Root file.
          \param ext_name The name of the Root extension.
      */
      RootExtensionManager(const std::string & file_name, const std::string & ext_name);

      /** \brief Destructor. Closes file if it is open.
      */
      virtual ~RootExtensionManager();

      /** \brief Create a header object which refers to this file. Caller is responsible for deleting
          the header object.
      */
      virtual IHeaderData * getHeaderData();

      /** \brief Create a data object (table or image) which refers to this file. Caller is responsible
          for deleting the header object.
      */
      virtual ITabularData * getTabularData();

      // General support for Root files:
      /** \brief Open the Root file.
      */
      void open();

      /** \brief Close the Root file.
      */
      void close();

      // Header/keyword-specific support:
      /** \brief Templated function which can get keywords from a FITS table, converted to any data type.
          \param name The name of the keyword.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      // Table-specific support:
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
      mutable std::map<std::string, FieldIndex_t> m_branch_lookup;
      mutable std::vector<LeafBuffer *> m_leaves;
      Index_t m_num_records;
      TFile * m_fp;
      mutable TTree * m_tree;
      IHeaderData * m_header;
      IData * m_data;
  };

  // Getting keywords.
  template <typename T>
//  inline void RootExtensionManager::getKeywordGeneric(const std::string & name, T & value) const {
  inline void RootExtensionManager::getKeywordGeneric(const std::string &, T &) const {
    throw TableException("Keyword access not yet implemented for Root files.");
  }

  // Getting columns.
  template <typename T>
  inline void RootExtensionManager::getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end,
    T * dest_begin) const {
    m_tree->GetEntry(record_index);
    if (src_begin + 1 != src_end) throw TableException("Getting vectors from Root files not working yet.");
    m_leaves[col_num]->get(*dest_begin);
  }

  // Setting columns.
  template <typename T>
//  inline void RootExtensionManager::setCellGeneric(int col_num, Index_t record_index, Index_t src_begin,
//    T * dest_begin, T * dest_end) {
  inline void RootExtensionManager::setCellGeneric(int , Index_t , Index_t ,
    T *, T *) {
    throw TableException("Write access not yet implemented for Root files.");
  }

}

#endif
