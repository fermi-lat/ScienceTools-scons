/** \file RootExtensionManager.h

    \brief Utilities to help manage Root specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef tip_RootExtensionManager_h
#define tip_RootExtensionManager_h

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "TTree.h"

#include "tip/IColumn.h"
#include "tip/IExtensionData.h"
#include "tip/TipException.h"
#include "tip/tip_types.h"

class TBranch;
class TFile;
class TLeaf;

namespace tip {

  /** \class RootExtensionManager

      \brief Low level interface to Root format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Root access. It also
      acts as a factory for creating Root-specific header and data objects, which refer back to the
      RootExtensionManager object which created them.
  */
  class RootExtensionManager {
    public:
      /** \brief Reset Root's signale handlers so that Root wont interfere with debugging.
      */
      static void resetSigHandlers();

      /** \brief Determine whether given file is a Root file.
          \param file_name The name of the file.
      */
      static bool isValid(const std::string & file_name);

      /** \brief Create an object to provide low-level access to the given Root extension.
          \param file_name The name of the Root file.
          \param ext_name The name of the Root extension.
          \param filter Root compliant filtering expression.
      */
      RootExtensionManager(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "", bool read_only = true);

      /** \brief Destructor. Closes file if it is open.
      */
      virtual ~RootExtensionManager();

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

      /** \brief Templated function which can set keywords in a FITS table.
          \param name The name of the keyword.
          \param value The value to be written.
      */
      template <typename T>
      void setKeywordGeneric(const std::string & name, const T & value);

      // Table-specific support:
      /** \brief Open the Root table. Exceptions will be thrown if the extension does not exist, or if
          the extension is not a table (TTree).
      */
      void openTable();

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      virtual bool isTable() const { return true; }

      // Non-virtual helper functions for ITabularData interface:

      /** \brief Return the number of records in the current tabular data object (the number of rows
          in the Root file).
      */
      Index_t getNumRecords() const { return m_num_records; }

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      void setNumRecords(Index_t num_records);

      /** \brief Return a container of all field names valid for this table:
      */
      const IExtensionData::FieldCont & getValidFields() const;

      virtual IColumn * getColumn(FieldIndex_t field_index) {
        if (0 > field_index || m_leaves.size() <= (unsigned int)(field_index))
          throw TipException(formatWhat("RootExtensionManager::getColumn was passed invalid index"));
        return m_leaves[field_index];
      }

      virtual const IColumn * getColumn(FieldIndex_t field_index) const {
        if (0 > field_index || m_leaves.size() <= (unsigned int)(field_index))
          throw TipException(formatWhat("RootExtensionManager::getColumn const was passed invalid index"));
        return m_leaves[field_index];
      }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Copy a cell from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_field The field identifier in the source data object.
          \param src_record The record identifier in the source data object.
          \param dest_field The field identifier in this object (the destination data object).
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyCell(const IExtensionData * src_ext, FieldIndex_t src_field, Index_t src_record, FieldIndex_t dest_field,
        Index_t dest_record);

      /** \brief Copy a record from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_record The record identifier in the source data object.
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyRecord(const IExtensionData * src_ext, Index_t src_record, Index_t dest_record);

      /** \brief Append a field to the table.
          \param field_name The name of the field to append.
          \param format The format of the field to append, e.g. 1D for scalar double, 8J for vector long, etc.
           See Cfitsio documentation for details.
      */
      void appendField(const std::string & field_name, const std::string & format);

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const;

      /** \brief Set the dimensionality of an image.
          \param dims Array holding the sizes in each dimension.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims);

      /** \brief Get a specific pixel from an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void getPixel(PixOrd_t x, PixOrd_t y, double & pixel) const;

      /** \brief Set a specific pixel in an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void setPixel(PixOrd_t x, PixOrd_t y, const double & pixel);

    private:
      std::string formatWhat(const std::string & msg) const;
      std::string m_file_name;
      std::string m_ext_name;
      std::string m_filter;
      mutable std::map<std::string, FieldIndex_t> m_branch_lookup;
      mutable std::vector<IColumn *> m_leaves;
      IExtensionData::FieldCont m_fields;
      Index_t m_num_records;
      TFile * m_fp;
      mutable TTree * m_tree;
  };

  // Getting keywords.
  template <typename T>
//  inline void RootExtensionManager::getKeywordGeneric(const std::string & name, T & value) const {
  inline void RootExtensionManager::getKeywordGeneric(const std::string &, T &) const {
    throw TipException("Keyword reading not yet implemented for Root extensions.");
  }

  // Setting keywords.
  template <typename T>
//  inline void RootExtensionManager::setKeywordGeneric(const std::string & name, const T & value) {
  inline void RootExtensionManager::setKeywordGeneric(const std::string &, const T &) {
    throw TipException("Keyword writing not yet implemented for Root extensions.");
  }

}

#endif
