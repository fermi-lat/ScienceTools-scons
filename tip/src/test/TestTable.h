/** \file TestTable.h
    \brief Declaration for class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestTable_h
#define tip_TestTable_h

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

      /// \brief Test iterator access:
      void readWriteFieldTest();

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

    private:
      Table * m_fits_table;
      Table * m_root_table;
  };

}

#endif
