/** \file ExtensionData.h

    \brief 

    \author James Peachey, HEASARC
*/
#ifndef tip_ExtensionData_h
#define tip_ExtensionData_h

#include <string>

#include "tip/IExtensionData.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class ExtensionData

  */
  template <typename ExtensionManager>
  class ExtensionData : public IExtensionData {
    public:
      ExtensionData(const std::string & file_name, const std::string & extension_name, const std::string & filter = "",
        bool read_only = true):
        m_extension_manager(file_name, extension_name, filter, read_only) {}

      virtual ~ExtensionData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, double & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, float & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed short & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed int & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed long & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned short & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned int & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned long & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, std::string & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }

      /** \brief Set a keyword in this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const double & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const float & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed short & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed int & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed long & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned short & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned int & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned long & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const std::string & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char * const & value)
        { m_extension_manager.setKeywordGeneric(name, value); }

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      virtual bool isTable() const { return m_extension_manager.isTable(); }

      /** \brief Return the number of records in the current extension object.
      */
      virtual Index_t getNumRecords() const { return m_extension_manager.getNumRecords(); }

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      virtual void setNumRecords(Index_t num_records) { m_extension_manager.setNumRecords(num_records); }

      /** \brief Return a container of all field names valid for this table:
      */
      virtual const IExtensionData::FieldCont & getValidFields() const { return m_extension_manager.getValidFields(); }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const
        { return m_extension_manager.getFieldIndex(field_name); }

      virtual Index_t getFieldNumElements(FieldIndex_t field_index, Index_t record_index = 0) const
        { return m_extension_manager.getFieldNumElements(field_index, record_index); }

      /** \brief Get one or more values from the current extension object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const
        { m_extension_manager.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }

      /** \brief Set one or more values in the current extension object.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to the first element in the output sequence.
      */
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        bool * dest_begin, bool * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        double * dest_begin, double * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        float * dest_begin, float * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        char * dest_begin, char * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed char * dest_begin, signed char * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed short * dest_begin, signed short * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed int * dest_begin, signed int * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed long * dest_begin, signed long * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned char * dest_begin, unsigned char * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned short * dest_begin, unsigned short * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned int * dest_begin, unsigned int * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned long * dest_begin, unsigned long * dest_end)
        { m_extension_manager.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }

    private:
      ExtensionManager m_extension_manager;
  };

}

#endif
