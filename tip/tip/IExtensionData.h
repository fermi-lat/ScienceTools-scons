/** \file IExtensionData.h

    \brief

    \author James Peachey, HEASARC
*/
#ifndef tip_IExtensionData_h
#define tip_IExtensionData_h

#include <string>
#include <vector>

#include "tip/tip_types.h"

namespace tip {

  /** \class IExtensionData

  */
  class IExtensionData {
    public:
      typedef std::vector<std::string> FieldCont;

      virtual ~IExtensionData() {}

      /** \brief Get a keyword from this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const = 0;
      virtual void getKeyword(const std::string & name, double & value) const = 0;
      virtual void getKeyword(const std::string & name, float & value) const = 0;
      virtual void getKeyword(const std::string & name, char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed short & value) const = 0;
      virtual void getKeyword(const std::string & name, signed int & value) const = 0;
      virtual void getKeyword(const std::string & name, signed long & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned char & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned short & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned int & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned long & value) const = 0;
      virtual void getKeyword(const std::string & name, std::string & value) const = 0;

      /** \brief Set a keyword in this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value) = 0;
      virtual void setKeyword(const std::string & name, const double & value) = 0;
      virtual void setKeyword(const std::string & name, const float & value) = 0;
      virtual void setKeyword(const std::string & name, const char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed short & value) = 0;
      virtual void setKeyword(const std::string & name, const signed int & value) = 0;
      virtual void setKeyword(const std::string & name, const signed long & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned char & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned short & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned int & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned long & value) = 0;
      virtual void setKeyword(const std::string & name, const std::string & value) = 0;
      virtual void setKeyword(const std::string & name, const char * const & value) = 0;

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      virtual bool isTable() const = 0;

      /** \brief Return the number of records in the current (tabular) extension object.
      */
      virtual Index_t getNumRecords() const = 0;

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      virtual void setNumRecords(Index_t num_records) = 0;

      /** \brief Return a container of all field names valid for this table:
      */
      virtual const FieldCont & getValidFields() const = 0;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const = 0;

      virtual Index_t getFieldNumElements(FieldIndex_t field_index, Index_t record_index = 0) const = 0;

      /** \brief Get one or more values from the current (tabular) extension object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const = 0;

      /** \brief Set one or more values in the current (tabular) extension object.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to the first element in the output sequence.
      */
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        bool * dest_begin, bool * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        double * dest_begin, double * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        float * dest_begin, float * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        char * dest_begin, char * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed char * dest_begin, signed char * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed short * dest_begin, signed short * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed int * dest_begin, signed int * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed long * dest_begin, signed long * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned char * dest_begin, unsigned char * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned short * dest_begin, unsigned short * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned int * dest_begin, unsigned int * dest_end) = 0;
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned long * dest_begin, unsigned long * dest_end) = 0;
  };

}

#endif
