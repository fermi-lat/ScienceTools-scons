/** \file RootTabularData.cxx

    \brief Low level implementation for Root format tabular data.

    \author James Peachey, HEASARC
*/

#include "table/TableException.h"
#include "RootExtensionUtils.h"
#include "RootTabularData.h"

namespace table {

  RootTabularData::RootTabularData(RootExtensionUtils * root_utils): ITabularData(),
    m_root_utils(root_utils) {
    if (!m_root_utils) throw TableException("RootTabularData::RootTabularData(RootExtensionUtils *): "
      "Cannot create RootTabularData object with NULL RootExtensionUtils pointer");
    m_root_utils->openTable();
  }

  Index_t RootTabularData::getNumRecords() const { return m_root_utils->getNumRecords(); }

  FieldIndex_t RootTabularData::getFieldIndex(const std::string & field_name) const {
    return m_root_utils->getFieldIndex(field_name);
  }

  Index_t RootTabularData::getFieldNumElements(FieldIndex_t field_index) const {
    return m_root_utils->getFieldNumElements(field_index);
  }

  // getCell overloads.
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    bool * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    double * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    float * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    char * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed char * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed short * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed int * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed long * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned char * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned short * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned int * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void RootTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned long * dest_begin) const {
    m_root_utils->getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }

  // set methods
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    bool * dest_begin, bool * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    double * dest_begin, double * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    float * dest_begin, float * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    char * dest_begin, char * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed char * dest_begin, signed char * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed short * dest_begin, signed short * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed int * dest_begin, signed int * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed long * dest_begin, signed long * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned char * dest_begin, unsigned char * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned short * dest_begin, unsigned short * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned int * dest_begin, unsigned int * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void RootTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned long * dest_begin, unsigned long * dest_end) {
    m_root_utils->setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }

}
