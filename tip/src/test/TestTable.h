/** \file TestTable.h
    \brief Declaration for class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestTable_h
#define tip_TestTable_h

#include <string>
#include <vector>

#include "TestHarness.h"

namespace tip {

  class Table;

  /** \class TestTable
      \brief Declaration for class to perform detailed testing of Table class.
  */
  class TestTable : public TestHarness {
    public:
      /** \brief Construct test objects needed to test Table class. This will also test Table's constructor
          in the process.
      */
      TestTable();

      /** \brief Destructor.
      */
      virtual ~TestTable() throw();

      /** \brief Perform the detailed test needed by the subobject.
      */
      virtual int test(int status);

      /// \brief Test Table's constructors:
      void TableTest();

      /// \brief Test Table's method for getting list of fields in table:
      void getValidFieldsTest();

      /// \brief Test reading and writing of FITS and Root table.
      void readWriteFieldTest();

      /// \brief Test reading and writing of an arbitrary table.
      void readWriteFieldTest(Table * table, const std::string & format, const std::string & field_name);

      /** \brief Test reading one field from a table.
          \param table The table.
          \param field_name The name of the field.
          \param field_values Array holding the output values.
      */
      void readFieldTest(const Table * table, const std::string & field_name, std::vector<double> & field_values);

      /** \brief Test reading one field from a table.
          \param field_name The name of the field.
          \param field_values Array holding the input values.
          \param table The output table.
      */
      void writeFieldTest(Table * table, const std::string & field_name, const std::vector<double> & field_values);

      /// \brief Test reading and writing vector-valued fields (just for FITS case for now).
      void readWriteVectorFieldTest();

      /// \brief Test copying one table's columns to another.
      void copyFieldTest();

      /// \brief Test that bug when only one column is present was corrected.
      void singleFieldBugTest();

      /** \brief Get a writable table pointer, for the benefit of other tests.
      */
      Table * getTable();

      /** \brief Test appending a field to an existing table.
      */
      void appendFieldTest();

    private:
      void setToZero(Table * table);
      bool confirmEqual(const Table * table1, const Table * table2);

      Table * m_fits_table;
      Table * m_root_table;
  };

}

#endif
