/** \file FitsTabularData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include "table/TableException.h"
#include "FitsExtensionUtils.h"
#include "FitsTabularData.h"

namespace table {

  FitsTabularData::FitsTabularData(FitsExtensionUtils * fits_utils): ITabularData(),
    m_fits_utils(fits_utils) {
    if (!m_fits_utils) throw TableException("FitsTabularData::FitsTabularData(FitsExtensionUtils *): "
      "Cannot create FitsTabularData object with NULL FitsExtensionUtils pointer");
    m_fits_utils->openTable();
  }

  Index_t FitsTabularData::getNumRecords() const { return m_fits_utils->getNumRecords(); }

  FieldIndex_t FitsTabularData::getFieldIndex(const std::string & field_name) const {
    return m_fits_utils->getFieldIndex(field_name);
  }

  Index_t FitsTabularData::getFieldNumElements(FieldIndex_t field_index) const {
    return m_fits_utils->getFieldNumElements(field_index);
  }

  // getCell overloads.
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    bool * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    double * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    float * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    char * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed char * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed short * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed int * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed long * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned char * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned short * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned int * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned long * dest_begin) const {
    m_fits_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }

  // set methods
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    bool * dest_begin, bool * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    double * dest_begin, double * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    float * dest_begin, float * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    char * dest_begin, char * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed char * dest_begin, signed char * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed short * dest_begin, signed short * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed int * dest_begin, signed int * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed long * dest_begin, signed long * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned char * dest_begin, unsigned char * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned short * dest_begin, unsigned short * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned int * dest_begin, unsigned int * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned long * dest_begin, unsigned long * dest_end) {
    m_fits_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }

}
