/** \file table_types.h

    \brief Forward declarations and typedefs for all table classes.

    \author James Peachey, HEASARC
*/
#ifndef table_table_types_h
#define table_table_types_h

namespace table {

  /** \brief Type used to identify field index.
  */
  typedef signed int FieldIndex_t;

  /** \brief Type used for differences in table row numbers.
  */
  typedef signed long IndexDiff_t;

  /** \brief Type used for table row numbers.
  */
  typedef signed long Index_t;
}

#endif
