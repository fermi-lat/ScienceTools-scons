/** \file TestTable.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "FitsPrimProps.h"
#include "FitsTable.h"
#include "TestTable.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

#ifndef BUILD_WITHOUT_ROOT
#include "RootTable.h"
#endif

#define MAKE_COMPILATION_FAIL (0)

namespace {
  template <typename T>
  bool is_equal(const T & t1, const T & t2) {
    bool equal = t1 == t2;
    if (!equal) {
      if (0 != t1) equal = std::fabs(double(t2 - t1) / t1) < std::numeric_limits<T>::epsilon();
      else if (0 != t2) equal = std::fabs(double(t1 - t2) / t2) < std::numeric_limits<T>::epsilon();
    }
    return equal;
  }
}

namespace tip {

  TestTable::TestTable(): m_fits_table(0), m_root_table(0), m_root_ft2(0) {}

  TestTable::~TestTable() throw() {}

  int TestTable::test(int status) {
    // Use inherited status to set initial status
    setStatus(status);

    // Test Table's constructors:
    TableTest();

    // Test new browsing capabilities:
    getValidFieldsTest();

    // Test iterator access:
    readWriteFieldTest();

    // Test iterator access to vector columns:
    readWriteVectorFieldTest();

    // Test appending a field to an existing table.
    appendFieldTest();

    // Test copying records from one table to another.
    copyFieldTest();

    // Test that bug when only one column is present was corrected.
    singleFieldBugTest();

    // Test that unsigned integers are handled correctly.
    unsignedIntTest();

    // Test that Root version of FT2 file is read correctly.
    rootFt2Test();

    // Test that large files are handled correctly.
    largeFileTest();

    // Clean up.
    delete m_root_ft2; m_root_ft2 = 0;
    delete m_root_table; m_root_table = 0;
    delete m_fits_table; m_fits_table = 0;

    return getStatus();
  }

  void TestTable::TableTest() {
    std::string msg;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test constructing a FITS table:
    msg = std::string("opening SPECTRUM extension of ") + data_dir + "a1.pha";
    try {
      m_fits_table = getTable();
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("FITS table tests will be skipped!");
    }

#ifndef BUILD_WITHOUT_ROOT
    // Test constructing a Root table:
    msg = std::string("opening TTree \"1\" extension of ") + data_dir + "merit.root";
    try {
      Table * data = new RootTable(data_dir + "merit.root", "1");
      m_root_table = data;
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("Root table tests will be skipped!");
    }

    // Test constructing a Root FT2 table:
    msg = std::string("opening TTree \"pointing_history\" extension of ") + data_dir + "FT2.root";
    try {
      Table * data = new RootTable(data_dir + "FT2.root", "pointing_history");
      m_root_ft2 = data;
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("Root FT2 table tests will be skipped!");
    }
#endif
  }

  void TestTable::getValidFieldsTest() {
    std::string msg;
    if (0 != m_fits_table) {
      msg = "getting field container from FITS table";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_fits_table->getValidFields();

        int num_fields = 0;
        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
          // Don't count "new_chan" which is a field added during the test process.
          if (0 != it->compare("new_chan")) ++num_fields;
        }

        // Test file has 2 fields:
        if (2 == num_fields) ReportExpected(msg + " succeeded");
        else ReportUnexpected(msg + " got " + toString(num_fields) + " fields, not 2");

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
    if (0 != m_root_table) {
      msg = "getting field container from Root table";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_root_table->getValidFields();

        int num_fields = 0;
        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
          ++num_fields;
        }

        // Test file has 224 fields:
        if (224 == num_fields) ReportExpected(msg + " succeeded");
        else ReportUnexpected(msg + " got " + toString(num_fields) + " fields, not 224");

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
  }

  void TestTable::readWriteFieldTest() {
    // Test FITS field read/write for channel field:
    readWriteFieldTest(m_fits_table, "FITS", "chaNNel");

    if (0 == m_root_table) return;

    // Test Root field read only for McEnergy field; note that this doesn't test whether the
    // values were read correctly.
    double expected_prim[] = {
      9881.1264038085956,
      374.32735403145591, 1152.7569293975816, 225.12541611949609, 18.767785214795236, 339.41182494163502, 10.787301655186937,
      797.71661758422795, 14.06410336494446, 36.623448133468628, 217.75630116462719, 23.318689316511175, 460.38362383842457,
      132.9941302537917, 33.655513077974319, 909.00588035583507, 54.750926792621613, 36540.122985839967, 22.067859768867493,
      377.05758213996899, 327.8842270374297, 10.945985093712842, 5289.381504058837, 22.08190038800231, 8162.9266738891656,
      855.15505075454632, 68.108297884463497, 128.2227784395219, 18.153037875890789, 175.14409124851559, 14083.505630493151,
      378.89450788494082, 66.376052796840654, 14.711145311594038, 497.05943465232849, 25.426756590604821, 28.807224705815244,
      372.79221415519714, 45.064602047204126, 9686.2659454345721, 13036.463737487768, 3482.7575683593755, 138.82616162300511,
      34.407161176204681, 979.49832677841164, 33.445887267588958, 13607.332229614267, 1098.5789299011228, 10577.536582946796,
      39.210032671689987, 122.55537509918202, 78.723251819610596, 55.001884698867798, 484.89898443222, 7445.1961517334166,
      60.068890452384949, 10977.50663757324, 14.0966046601534, 48.188492655754089, 212.06137537956226, 1121.369361877441,
      67.464247345924647, 45.730490237474271, 1120.1473474502566, 25014.816284179677, 864.57240581512406, 34759.517669677713,
      4227.282047271734, 19845.930099487312, 39.487283782755362, 20481.880187988296, 725.81541538238537, 48.921696841716198,
      8337.3651504516802, 10748.487472534172, 12337.617874145524, 57.304948568344116, 58.529380708931932, 105.42155802249897,
      21.78704924881459, 18.594179302453984, 131.47398829460133, 441.02409482002258, 744.8423504829409, 424.8797297477721,
      439.89828228950512, 257.54529237747192, 631.42776489257801, 307.97910690307629, 916.03666543960583, 39685.558319092001,
      445.65033912658703, 105.19703477621152, 18603.65104675293, 860.60708761215233, 636.98887825012196, 23.482421413064074,
      14332.509040832481, 58.536481112240828, 272.88678288459755, 19.715877249836996, 148.36083352565799, 62.212664633988936,
      28734.207153320436, 59.966348111628768, 28381.13403320306, 19393.611907959028, 45.317552983760535, 17956.829071044936,
      10.328903855784141, 25.998368859291141, 19211.191177368193, 51.203172653913498, 13509.49573516849, 76.188392937183679,
      10.321938432753091, 459.03411507602431, 12.923626229166985, 67.820891737937927, 50.406116992234182, 26.325548067688942,
      12195.323944091791, 502.07096338272095, 45.203171670436859, 28.993524610996246, 717.13972091674816, 1923.8300323486335,
      957.36962556839001
    };
    std::vector<double> expected_mc_energy(expected_prim, expected_prim + sizeof(expected_prim) / sizeof(double));
    try {
      std::vector<double> mc_energy;
      readFieldTest(m_root_table, "McEnergy", mc_energy);
      if (mc_energy != expected_mc_energy) {
        ReportUnexpected("reading McEnergy field from Root table did not return expected result:");
        if (mc_energy.size() != expected_mc_energy.size()) {
          std::ostringstream os;
          os << "number of values read was: " << mc_energy.size() << ", not " << expected_mc_energy.size() << " as expected";
          ReportUnexpected(os.str());
        } else {
          for(std::vector<double>::size_type ii = 0; ii != mc_energy.size(); ++ii) {
            if (mc_energy[ii] != expected_mc_energy[ii]) {
              std::ostringstream os;
              os.precision(std::numeric_limits<long double>::digits10);
              os << "mc_energy[" << ii << "] was " << mc_energy[ii] << ", not " << expected_mc_energy[ii] << " as expected";
              ReportUnexpected(os.str());
            }
          }
        }
      } else {
        ReportExpected("reading McEnergy field from Root table succeeded");
      }
    } catch (const TipException & x) {
      ReportUnexpected("reading McEnergy field from Root table failed", x);
    }

    if (0 == m_root_table) {
      ReportUnexpected("pointer to root table is null, skipping some tests.");
      return;
    }
    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_table->begin(); itor != m_root_table->end(); ++itor) {
        float mc_energy;
        (*itor)["McEnergy"].get(mc_energy);
        if (!is_equal<float>(mc_energy, expected_mc_energy[0])) {
          std::ostringstream os;
          os << "float(mc_energy) was " << mc_energy << ", not " << float(expected_mc_energy[0]) << ", as expected";
          ReportUnexpected(os.str());
        } else {
          ReportExpected("Reading McEnergy field in FT2.root as a float succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading McEnergy field in FT2.root as a float failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_table->begin(); itor != m_root_table->end(); ++itor) {
        int mc_energy;
        (*itor)["McEnergy"].get(mc_energy);
        if (mc_energy != int(expected_mc_energy[0])) {
          std::ostringstream os;
          os << "int(mc_energy) was " << mc_energy << ", not " << int(expected_mc_energy[0]) << ", as expected";
          ReportUnexpected(os.str());
        } else {
          ReportExpected("Reading McEnergy field in FT2.root as a int succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading McEnergy field in FT2.root as a int failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      typedef unsigned int uint;
      for (Table::Iterator itor = m_root_table->begin(); itor != m_root_table->end(); ++itor) {
        uint mc_energy;
        (*itor)["McEnergy"].get(mc_energy);
        if (mc_energy != uint(expected_mc_energy[0])) {
          std::ostringstream os;
          os << "uint(mc_energy) was " << mc_energy << ", not " << uint(expected_mc_energy[0]) << ", as expected";
          ReportUnexpected(os.str());
        } else {
          ReportExpected("Reading McEnergy field in FT2.root as a uint succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading McEnergy field in FT2.root as a uint failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_table->begin(); itor != m_root_table->end(); ++itor) {
        long mc_energy;
        (*itor)["McEnergy"].get(mc_energy);
        if (mc_energy != long(expected_mc_energy[0])) {
          std::ostringstream os;
          os << "long(mc_energy) was " << mc_energy << ", not " << long(expected_mc_energy[0]) << ", as expected";
          ReportUnexpected(os.str());
        } else {
          ReportExpected("Reading McEnergy field in FT2.root as a long succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading McEnergy field in FT2.root as a long failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      typedef unsigned long ulong;
      for (Table::Iterator itor = m_root_table->begin(); itor != m_root_table->end(); ++itor) {
        ulong mc_energy;
        (*itor)["McEnergy"].get(mc_energy);
        if (mc_energy != ulong(expected_mc_energy[0])) {
          std::ostringstream os;
          os << "ulong(mc_energy) was " << mc_energy << ", not " << ulong(expected_mc_energy[0]) << ", as expected";
          ReportUnexpected(os.str());
        } else {
          ReportExpected("Reading McEnergy field in FT2.root as a ulong succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading McEnergy field in FT2.root as a ulong failed", x);
    }

  }

  void TestTable::readWriteFieldTest(Table * table, const std::string & format, const std::string & field_name) {
    std::string msg;
    std::vector<double> orig;
    std::vector<double> modified;
    std::vector<double> read_modified;
    bool no_error = true;
    if (0 != table) {
      // Read original values from table into orig array:
      msg = std::string("testing reading ") + format + " table";
      try {
        readFieldTest(table, field_name, orig);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping the rest of its tests");
        return;
      }

      // Fill modified array, trying to make sure the contents are distinct from the orig array:
      modified.reserve(orig.size());
      srand(1023);
      for (std::vector<double>::iterator itor = orig.begin(); itor != orig.end(); ++itor) {
        double new_val = short(double(rand()) * std::numeric_limits<short>::max() / RAND_MAX);
        modified.push_back(new_val);
      }

      // Write new array to the table:
      msg = std::string("testing writing ") + format + " table";
      try {
        writeFieldTest(table, field_name, modified);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping some tests");
        no_error = false;
      }

      if (no_error) {
        // Read and check new values from table:
        msg = std::string("testing reading ") + format + " table values which were just written";
        try {
          readFieldTest(table, field_name, read_modified);

          if (modified == read_modified) ReportExpected(msg + " succeeded");
          else {
            ReportUnexpected("discrepancies found between values which were written and then read");
            ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
          }
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
          ReportWarning("readWriteFieldTest is skipping some tests");
          no_error = false;
        }
      }

      // Rewrite orig array to the table:
      msg = std::string("testing restoring ") + format + " table to its original state";
      try {
        writeFieldTest(table, field_name, orig);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping some tests");
        no_error = false;
      }

      if (no_error) {
        // Read and check whether original values were successfully restored to the table:
        msg = "testing reading restored values";
        try {
          readFieldTest(table, field_name, read_modified);

          if (orig == read_modified) ReportExpected(msg + " succeeded");
          else {
            ReportUnexpected("discrepancies found between original values and those which were restored");
            ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
          }
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
          ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
        }
      }

    }
  }

  void TestTable::readFieldTest(const Table * table, const std::string & field_name, std::vector<double> & field_values) {
    if (0 != table) {
      std::string msg;

      // Test error cases:
      msg = std::string("getting scalar-valued \"") + field_name + "\" cell into a local vector variable";
      try {
        // Try to read scalar-valued column into a vector.
        double vec[1];
        (*table->begin())[field_name].get(0, 1, vec);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      // Find out how many records are in the table:
      Index_t num_records = -1;
      try {
        num_records = table->getNumRecords();
      } catch (const TipException &) {
        // Don't report this exception: not testing getNumRecords here.
      }

      if (0 > num_records) {
        ReportWarning("readFieldTest had trouble preparing to run; test will be skipped!");
      } else if (0 == num_records) {
        ReportWarning("readfieldTest called for a table with no records; test will be skipped!");
      } else {
        // Clear out whatever's in field_values before filling it.
        field_values.clear();

        // Resize the output array so it has enough room:
        field_values.reserve(num_records);

        // Loop over table, copying all values into output array:
        for (Table::ConstIterator itor = table->begin(); itor != table->end(); ++itor) {
          field_values.push_back((*itor)[field_name].get());

#if MAKE_COMPILATION_FAIL
          throw TipException("SHOULD NOT HAVE COMPILED! Assigning to a const Table::Cell object");
          (*itor)[field_name].set(1.);
#endif

        }
      }
    }
  }

  void TestTable::writeFieldTest(Table * table, const std::string & field_name, const std::vector<double> & field_values) {
    if (0 != table) {
      std::string msg;

      // Test error cases:
      msg = std::string("writing scalar-valued \"") + field_name + "\" cell from a local vector variable";
      try {
        // Try to write scalar-valued column into a vector.
        double vec[1];
        (*table->begin())[field_name].set(vec, vec + 1, 0);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      // Set the number of records in the table to match the array:
      Index_t num_records = -1;
      try {
        Index_t tmp_num = field_values.size();
        table->setNumRecords(tmp_num);
        num_records = tmp_num;
      } catch (const TipException &) {
        // Don't report this exception: not testing setNumRecords here.
      }

      if (0 > num_records) {
        ReportWarning("writeFieldTest had trouble preparing to run; test will be skipped!");
      } else if (0 == num_records) {
        ReportWarning("writefieldTest called to write an array with no elements; test will be skipped!");
      } else {
        // Loop over array and table, copying input array into table:
        Table::Iterator out = table->begin();
        for (std::vector<double>::const_iterator itor = field_values.begin(); itor != field_values.end(); ++itor, ++out) {
          (*out)[field_name].set(*itor);
        }
      }
    }
  }

  void TestTable::readWriteVectorFieldTest() {
    if (m_fits_table) {
      Table * table = m_fits_table;
      const Table * const_table = m_fits_table;

      std::string msg;
      std::string vector_field = "cOUnts";

      // Test error cases:
      msg = std::string("getting vector-valued \"") + vector_field + "\" cell into a local scalar variable";
      try {
        // Try to read vector-valued column into a scalar.
        double scalar;
        (*const_table->begin())[vector_field].get(scalar);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      msg = std::string("setting vector-valued \"") + vector_field + "\" cell from a local scalar variable";
      try {
        // Try to write vector-valued column from a scalar.
        double scalar;
        (*table->begin())[vector_field].set(scalar);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      msg = std::string("getting vector-valued \"") + vector_field + "\" cell as a vector<string> variable";
      try {
        std::vector<std::string> result;
        (*const_table->begin())[vector_field].get(result);
        bool error = false;
        if (4096 != result.size()) {
          std::ostringstream os;
          os << msg << " returned vector of size " << result.size() << ", not 4096 as expected";
          error = true;
          ReportUnexpected(os.str());
        } else {
          // Check some values.
          for (std::vector<std::string>::size_type index = 0; index != 1989; ++index) {
            if ("0" != result[index]) {
              error = true;
              break;
            }
          }
          if ("1" != result[1989] || "1" != result[2021] || "1" != result[2063] || "53" != result[2050] || "70" != result[2043])
            error = true;
          for (std::vector<std::string>::size_type index = 2069; index != result.size(); ++index) {
            if ("0" != result[index]) {
              error = true;
              break;
            }
          }
        }
        if (error) ReportUnexpected(msg + ", some values were not as expected");
        else ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
  }

  // Test appending a field to an existing table.
  void TestTable::appendFieldTest() {
    std::string msg;
    if (0 != m_root_table) {
      msg = "appending field to Root table";
      try {
        m_root_table->appendField("new_chan", "1I");
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }
    }

    if (0 != m_fits_table) {
      msg = "appending field to FITS table";
      try {
        m_fits_table->appendField("NEW_chan", "1I");
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }

      msg = "appending vector bool field to FITS table";
      try {
        // Add a vector bool field.
        m_fits_table->appendField("NEW_bool", "8L");
        
        // Create a state with alternating T/F.
        bool ramp = false;
        std::vector<bool> bool_state(8, false);
        for (std::vector<bool>::iterator itor = bool_state.begin(); itor != bool_state.end(); ++itor, ramp = !ramp)
          (*itor) = ramp;
    
        // Assign the state to each row in the new field.
        for (Table::Iterator itor = m_fits_table->begin(); itor != m_fits_table->end(); ++itor) {
          (*itor)["NEW_bool"].set(bool_state);
        }

        // Overwrite the first entry, setting it to all empty strings (interpreted as undefined).
        // Start off with 8 blanks.
        std::vector<std::string> state(8);
        (*m_fits_table->begin())["New_bool"].set(state);

        char * prim_correct[] = { "F", "T", "F", "T", "F", "T", "F", "T" };
        // Read back in the new column to make sure it's correct.
        bool error = false;
        std::vector<std::string> correct_state(8, FitsPrimProps<char *>::undefined());
        for (Table::Iterator itor = m_fits_table->begin(); itor != m_fits_table->end(); ++itor) {
          state.clear();
          (*itor)["NEW_bool"].get(state);
          if (state != correct_state) {
            ReportUnexpected("after " + msg + " state read was:");
            std::ostringstream os;
            const char * delim = "\t(";
            for (std::vector<std::string>::iterator s_itor = state.begin(); s_itor != state.end(); ++s_itor) {
              os << delim << *s_itor;
              delim = ", ";
            }
            os << ")";
            ReportUnexpected(os.str());
            ReportUnexpected("but the state read should have been:");
            os.str("");
            delim = "\t(";
            for (std::vector<std::string>::iterator s_itor = correct_state.begin(); s_itor != correct_state.end(); ++s_itor) {
              os << delim << *s_itor;
              delim = ", ";
            }
            os << ")";
            ReportUnexpected(os.str());
            error = true;
          }
          correct_state.assign(prim_correct, prim_correct+ sizeof(prim_correct) / sizeof(char *));
        }
        if (!error) ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }

      msg = "appending vector short field to FITS table";
      try {
        // Add a vector short field.
        m_fits_table->appendField("NEW_short", "8I");
        
        // Create a vector which should evaluate to all INDEF.
        std::vector<short> short_state(8, std::numeric_limits<short>::min());

        // Start with the first row of the new field, and set it as undefined.
        Table::Iterator table_itor = m_fits_table->begin();
        (*table_itor)["New_short"].set(short_state);

        // Recreate the state with ramp.
        short ramp = 0;
        for (std::vector<short>::iterator itor = short_state.begin(); itor != short_state.end(); ++itor, ++ramp)
          (*itor) = ramp;
    
        // Assign the state to each remaining row in the new field.
        for (++table_itor; table_itor != m_fits_table->end(); ++table_itor) {
          (*table_itor)["NEW_short"].set(short_state);
        }

        // Overwrite the first entry, setting it to all empty strings (interpreted as undefined).
        // Start off with 8 blanks.
        std::vector<std::string> state(8);

        char * prim_correct[] = { "0", "1", "2", "3", "4", "5", "6", "7" };
        // Read back in the new column to make sure it's correct.
        bool error = false;
        std::vector<std::string> correct_state(8, FitsPrimProps<char *>::undefined());
        for (Table::Iterator itor = m_fits_table->begin(); itor != m_fits_table->end(); ++itor) {
          state.clear();
          (*itor)["NEW_short"].get(state);
          if (state != correct_state) {
            ReportUnexpected("after " + msg + " state read was:");
            std::ostringstream os;
            const char * delim = "\t(";
            for (std::vector<std::string>::iterator s_itor = state.begin(); s_itor != state.end(); ++s_itor) {
              os << delim << *s_itor;
              delim = ", ";
            }
            os << ")";
            ReportUnexpected(os.str());
            ReportUnexpected("but the state read should have been:");
            os.str("");
            delim = "\t(";
            for (std::vector<std::string>::iterator s_itor = correct_state.begin(); s_itor != correct_state.end(); ++s_itor) {
              os << delim << *s_itor;
              delim = ", ";
            }
            os << ")";
            ReportUnexpected(os.str());
            error = true;
          }
          correct_state.assign(prim_correct, prim_correct+ sizeof(prim_correct) / sizeof(char *));
        }
        if (!error) ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }

      msg = "appending string field to FITS table";
      try {
        m_fits_table->appendField("new_str", "A16");
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
      

      // Error case:
      msg = "appending field which already exists to FITS table";
      try {
        m_fits_table->appendField("new_Chan", "1D");
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }
    }
  }

  void TestTable::copyFieldTest() {
#if MAKE_COMPILATION_FAIL
    throw TipException("SHOULD NOT HAVE COMPILED! Assigning to a ConstTableRecord object");
    Table::ConstIterator itor1;
    Table::ConstIterator itor2;
    *itor1 = *itor2;
#endif
    if (0 != m_fits_table) {
      const Table * in_table = m_fits_table;
      Table * out_table = getTable();
      try {
        // Initialize output table to all zeros.
        setToZero(out_table);

        // Iterate over input and output, copying the input to the output using Cell::operator =.
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = out_table->begin();
        for (; in_itor != in_table->end(); ++in_itor, ++out_itor) {
          (*out_itor)["channel"] = (*in_itor)["channel"];
          (*out_itor)["counts"] = (*in_itor)["counts"];
        }

        // Confirm the copy worked.
        if (confirmEqual(in_table, out_table)) 
          ReportExpected("copyFieldTest() succeeded copying one table's fields to another using Cell::operator =");
        else
          ReportUnexpected("copyFieldTest() failed to copy one table's fields to another using Cell::operator =");

      } catch (const TipException & x) {
        ReportUnexpected("copyFieldTest() failed", x);
      }

      try {
        // Initialize output table to all zeros.
        setToZero(out_table);

        // Iterate over input and output, copying the input to the output using Record::operator =.
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = out_table->begin();
        for (; in_itor != in_table->end(); ++in_itor, ++out_itor) {
          (*out_itor) = (*in_itor);
        }

        // Confirm the copy worked.
        if (confirmEqual(in_table, out_table)) 
          ReportExpected("copyFieldTest() succeeded copying one table's fields to another using Record::operator =");
        else
          ReportUnexpected("copyFieldTest() failed to copy one table's fields to another using Record::operator =");

      } catch (const TipException & x) {
        ReportUnexpected("copyFieldTest() failed", x);
      }
      delete out_table;
    }

    // Try copying to a Root file; this should fail.
    if (0 != m_fits_table && 0 != m_root_table) {
      const Table * in_table = m_fits_table;
      try {
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = m_root_table->begin();

        (*out_itor)["McEnergy"] = (*in_itor)["channel"];

        ReportUnexpected("copyFieldTest() did not fail when copying a cell to a Root file");
      } catch (const TipException & x) {
        ReportExpected("copyFieldTest() failed to copy a cell to a Root file", x);
      }
      try {
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = m_root_table->begin();

        (*out_itor) = (*in_itor);

        ReportUnexpected("copyFieldTest() did not fail when copying a record to a Root file");
      } catch (const TipException & x) {
        ReportExpected("copyFieldTest() failed to copy a record to a Root file", x);
      }
    }
  }

  void TestTable::singleFieldBugTest() {
    try {
      // Create an empty table.
      IFileSvc::instance().appendTable("single_column.fits", "DUMMY");

      // Open the table, and add a column.
      std::auto_ptr<Table> table(IFileSvc::instance().editTable("single_column.fits", "DUMMY"));
      table->appendField("COLUMN1", "1D");
      delete table.release();
      
      // The following should just hang if the bug is present.
      table.reset(IFileSvc::instance().editTable("single_column.fits", "DUMMY"));
      ReportExpected("TestTable::singleFieldBugTest had no problem editing a table containing a single column");
    } catch (const TipException & x) {
      ReportUnexpected("TestTable::singleFieldBugTest had a problem with a table containing a single column", x);
    }
    remove("single_column.fits");
  }

  void TestTable::unsignedIntTest() {
    try {
      remove("unsigned_int.fits");

      // Create an empty table.
      IFileSvc::instance().appendTable("unsigned_int.fits", "DUMMY");

      // Open the table.
      std::auto_ptr<Table> table(IFileSvc::instance().editTable("unsigned_int.fits", "DUMMY"));

      // Add 16 bit unsigned int column.
      table->appendField("USHORT", "1U");

      // Add 16 bit signed int column.
      table->appendField("SHORT", "1I");

      // Add 32 bit unsigned int column.
      table->appendField("UINT", "1V");

      // Add 32 bit signed int column.
      table->appendField("INT", "1J");

      // Add float column.
      table->appendField("FLOAT", "1E");

      // Add scaled int column.
      table->appendField("SCALED", "1I");
      table->getHeader()["TSCAL6"].set(1.01);
      table->getHeader()["TZERO6"].set(1<<16);

      // Make room for a record.
      table->setNumRecords(1);

      // Write a value to the new column.
      unsigned int expected = std::numeric_limits<unsigned int>::max();
      Table::Iterator itor = table->begin();
      (*itor)["UINT"].set(expected);

      // Close and re-open the table.
      delete table.release();
      
      table.reset(IFileSvc::instance().editTable("unsigned_int.fits", "DUMMY"));
      itor = table->begin();
      unsigned int actual;
      (*itor)["UINT"].get(actual);
      if (expected == actual) {
        ReportExpected("TestTable::unsignedIntTest reading and writing unsigned int is consistent.");
      } else {
        std::ostringstream os;
        os << "TestTable::unsignedIntTest wrote value " << expected << " to \"unsigned_int.fits[DUMMY]\", but read it back as " <<
          actual;
        ReportUnexpected(os.str());
      }

      // Confirm format of the columns.
      if ("1I" == table->getColumn(table->getFieldIndex("SHORT"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: SHORT column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: SHORT column format read disagrees with the format used to create column");
      }
      if ("1U" == table->getColumn(table->getFieldIndex("USHORT"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: USHORT column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: USHORT column format read disagrees with the format used to create column");
      }
      if ("1V" == table->getColumn(table->getFieldIndex("UINT"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: UINT column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: UINT column format read disagrees with the format used to create column");
      }
      if ("1J" == table->getColumn(table->getFieldIndex("INT"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: INT column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: INT column format read disagrees with the format used to create column");
      }
      if ("1E" == table->getColumn(table->getFieldIndex("FLOAT"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: FLOAT column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: FLOAT column format read disagrees with the format used to create column");
      }
      if ("1I" == table->getColumn(table->getFieldIndex("SCALED"))->getFormat()) {
        ReportExpected("TestTable::unsignedIntTest: SCALED column format read agrees with the format used to create column");
      } else {
        ReportUnexpected("TestTable::unsignedIntTest: SCALED column format read disagrees with the format used to create column");
      }

    } catch (const TipException & x) {
      ReportUnexpected("TestTable::unsignedIntTest had a problem", x);
    }
  }

  void TestTable::rootFt2Test() {
    if (0 == m_root_ft2) return;

    double expected_prim[] = {
      -5181385, 3659535.75, 2811837, -5283849, 3464278.25, 2867339, -5380638.5, 3265300.25, 2919752.5,
      -5471648.5, 3062814.25, 2969022, -5556780.5, 2857038.25, 3015093.5, -5635943.5, 2648194, 3057916.25,
      -5709051.5, 2436504.75, 3097445.25, -5776026.5, 2222197.5, 3133637, -5836793.5, 2005502.375, 3166453,
      -5891289.5, 1786652.625, 3195856.5, -5939454.5, 1565882.75, 3221816.75, -5981235, 1343430, 3244304.25,
      -6016586.5, 1119533.25, 3263294.75, -6045470.5, 894432.4375, 3278767.75, -6067855, 668370.3125, 3290705.5,
      -6083715.5, 441589.53125, 3299095.75, -6093033.5, 214333.796875, 3303928.75, -6095798, -13152.41015625, 3305199,
      -6092007, -240624.5625, 3302903.75, -6081663, -467837.9375, 3297046.25, -6064775.5, -694548, 3287632,
      -6041363, -920510.9375, 3274671, -6011450, -1145483.25, 3258176, -5975066.5, -1369223.375, 3238164.75,
      -5932252, -1591488.75, 3214658, -5883050.5, -1812041.5, 3187682.5, -5827515.5, -2030644, 3157264.75,
      -5765706, -2247059.25, 3123437.5, -5209305.5, 3607880.5, 2826961.5, -5310269, 3411608.75, 2881646.5,
      -5405528.5, 3211672.75, 2933228, -5494982.5, 3008286.5, 2981650.5, -5578533, 2801669, 3026861.25,
      -5656090.5, 2592042, 3068810.75, -5727572, 2379630.5, 3107454.5, -5792899, 2164661.75, 3142750.5,
      -5852001, 1947367.125, 3174660.25, -5904815, 1727980.125, 3203149, -5951283, 1506736, 3228186.25,
      -5991354, 1283872.5, 3249743.75, -6024984.5, 1059628.75, 3267798.25, -6052138.5, 834245.5625, 3282330.25,
      -6072785.5, 607965.625, 3293323.25, -6086903, 381032, 3300765.5, -6094474, 153688.546875, 3304649,
      -6095490.5, -73820.15625, 3304968.25, -6089951, -301249.53125, 3301722.5, -6077860, -528354.8125, 3294915.5,
      -6059230, -754891.6875, 3284554, -6034080.5, -980616.375, 3270648.5, -6002438, -1205285.875, 3253213.75,
      -5964334.5, -1428658.25, 3232267.75, -5919811, -1650492.25, 3207832.75, -5868914, -1870549.875, 3179935.5,
      -5811698.5, -2088593.875, 3148604.5, -5748224.5, -2304388.25, 3113873.5, -5223119, 3581946, 2834443.75,
      -5323328.5, 3385172.75, 2888719, -5417821, 3184763.25, 2939882.75, -5506493.5, 2980932.75, 2987880.25,
      -5589251, 2773900, 3032659.25, -5666003.5, 2563887.75, 3074170.75, -5736669, 2351121, 3112370.75,
      -5801170.5, 2135827.75, 3147217.75, -5859438, 1918239.5, 3178673.5, -5911409.5, 1698590, 3206703.75,
      -5957027.5, 1477115.125, 3231278.75, -5996242.5, 1254052.625, 3252370.5, -6029011.5, 1029641.8125, 3269956.75,
      -6055299.5, 804123.8125, 3284017.5, -6075077, 577741.5, 3294537.75, -6088322, 350737.90625, 3301506,
      -6095019.5, 123357.140625, 3304914.25, -6095161.5, -104156.296875, 3304758, -6088748, -331557.71875, 3301037,
      -6075784, -558602.5, 3293755.25, -6056283, -785046.25, 3282920.25, -6030265.5, -1010645.25, 3268543.25,
      -5997759, -1235156.875, 3250638.75, -5958796.5, -1458339, 3229225.75, -5913419, -1679950.75, 3204327.25,
      -5861676, -1899754.5, 3175970.25, -5803621.5, -2117512.75, 3144183.25, -5739317.5, -2332990.5, 3109001.5,
      -6007002, -1175394.125, 3255726.25, -5969758, -1398952.5, 3235247.5, -5926088.5, -1621004.375, 3211276.25,
      -5876039, -1841311.625, 3183839.75, -5819663.5, -2059637, 3152965, -5757021, -2275743.75, 3118685.75,
      -5688178.5, -2489400.5, 3081039, -5613209.5, -2700376, 3040064, -5532193.5, -2908442.25, 2995805,
      -5445217.5, -3113375.25, 2948309.75, -5352373, -3314953.75, 2897629.5, -5253761.5, -3512958.75, 2843817.5,
      -5149486.5, -3707178.75, 2786931.75, -5039660.5, -3897402.25, 2727033, -4924401, -4083424.25, 2664186.25,
      -4803832, -4265042.5, 2598458.75, -4678082, -4442062, 2529922, -4547287, -4614291, 2458648.75,
      -4411586.5, -4781542, 2384716.25, -4271126.5, -4943635, 2308203.5, -4126057.75, -5100394.5, 2229193.75,
      -3976537.5, -5251649.5, 2147771.5, -3822724.75, -5397237.5, 2064025.25, -3664786.5, -5537000.5, 1978044.125,
      -3502891.5, -5670786.5, 1889923.375, -3337215.75, -5798450, 1799755.375, -3167936, -5919853.5, 1707639.5,
      -2995235.75, -6034865, 1613674.125
    };

    std::vector<double> expected_sc_pos(expected_prim, expected_prim + sizeof(expected_prim) / sizeof(double));

    // Failure case: reading vetor as a scalar.
    try {
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        (*itor)["sc_position"].get();
      }
      ReportUnexpected("Reading sc_position field in FT2.root as a scalar succeeded");
    } catch (const TipException & x) {
      ReportExpected("Reading sc_position field in FT2.root as a scalar failed", x);
    }

    // Test correctness of native read.
    try {
      std::vector<double>::size_type index = 0;
      bool mismatch = false;
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<double> sc_position(3, 0.);
        (*itor)["sc_position"].get(sc_position);
        if (sc_position[0] != expected_sc_pos[index]) {
          std::ostringstream os;
          os << "In row" << index % 3 << ", sc_position[0] was " << sc_position[0] << ", not " << expected_sc_pos[index];
          mismatch = true;
          ReportUnexpected(os.str());
        }
        ++index;
        if (sc_position[1] != expected_sc_pos[index]) {
          std::ostringstream os;
          os << "In row" << index % 3 << ", sc_position[1] was " << sc_position[1] << ", not " << expected_sc_pos[index];
          mismatch = true;
          ReportUnexpected(os.str());
        }
        ++index;
        if (sc_position[2] != expected_sc_pos[index]) {
          std::ostringstream os;
          os << "In row" << index % 3 << ", sc_position[2] was " << sc_position[2] << ", not " << expected_sc_pos[index];
          mismatch = true;
          ReportUnexpected(os.str());
        }
        ++index;
      }
      if (!mismatch) ReportExpected("sc_position from FT2.root had expected values");
    } catch (const TipException & x) {
      ReportUnexpected("TestTable::rootFt2Test had unexpected problem", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<float> sc_position;
        (*itor)["sc_position"].get(sc_position);
        bool mismatch = false;
        if (sc_position[0] != float(expected_sc_pos[0])) {
          mismatch = true;
          std::ostringstream os;
          os << "float(sc_position[0]) was " << sc_position[0] << ", not " << float(expected_sc_pos[0]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[1] != float(expected_sc_pos[1])) {
          mismatch = true;
          std::ostringstream os;
          os << "float(sc_position[1]) was " << sc_position[1] << ", not " << float(expected_sc_pos[1]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[2] != float(expected_sc_pos[2])) {
          mismatch = true;
          std::ostringstream os;
          os << "float(sc_position[2]) was " << sc_position[2] << ", not " << float(expected_sc_pos[2]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (!mismatch) {
          ReportExpected("Reading sc_position field in FT2.root as a vector<float> succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading sc_position field in FT2.root as a vector<float> failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<int> sc_position;
        (*itor)["sc_position"].get(sc_position);
        bool mismatch = false;
        if (sc_position[0] != int(expected_sc_pos[0])) {
          mismatch = true;
          std::ostringstream os;
          os << "int(sc_position[0]) was " << sc_position[0] << ", not " << int(expected_sc_pos[0]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[1] != int(expected_sc_pos[1])) {
          mismatch = true;
          std::ostringstream os;
          os << "int(sc_position[1]) was " << sc_position[1] << ", not " << int(expected_sc_pos[1]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[2] != int(expected_sc_pos[2])) {
          mismatch = true;
          std::ostringstream os;
          os << "int(sc_position[2]) was " << sc_position[2] << ", not " << int(expected_sc_pos[2]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (!mismatch) {
          ReportExpected("Reading sc_position field in FT2.root as a vector<int> succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading sc_position field in FT2.root as a vector<int> failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      typedef unsigned int uint;
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<uint> sc_position;
        (*itor)["sc_position"].get(sc_position);
        bool mismatch = false;
        if (sc_position[0] != uint(expected_sc_pos[0])) {
          mismatch = true;
          std::ostringstream os;
          os << "uint(sc_position[0]) was " << sc_position[0] << ", not " << uint(expected_sc_pos[0]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[1] != uint(expected_sc_pos[1])) {
          mismatch = true;
          std::ostringstream os;
          os << "uint(sc_position[1]) was " << sc_position[1] << ", not " << uint(expected_sc_pos[1]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[2] != uint(expected_sc_pos[2])) {
          mismatch = true;
          std::ostringstream os;
          os << "uint(sc_position[2]) was " << sc_position[2] << ", not " << uint(expected_sc_pos[2]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (!mismatch) {
          ReportExpected("Reading sc_position field in FT2.root as a vector<uint> succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading sc_position field in FT2.root as a vector<uint> failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<long> sc_position;
        (*itor)["sc_position"].get(sc_position);
        bool mismatch = false;
        if (sc_position[0] != long(expected_sc_pos[0])) {
          mismatch = true;
          std::ostringstream os;
          os << "long(sc_position[0]) was " << sc_position[0] << ", not " << long(expected_sc_pos[0]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[1] != long(expected_sc_pos[1])) {
          mismatch = true;
          std::ostringstream os;
          os << "long(sc_position[1]) was " << sc_position[1] << ", not " << long(expected_sc_pos[1]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[2] != long(expected_sc_pos[2])) {
          mismatch = true;
          std::ostringstream os;
          os << "long(sc_position[2]) was " << sc_position[2] << ", not " << long(expected_sc_pos[2]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (!mismatch) {
          ReportExpected("Reading sc_position field in FT2.root as a vector<long> succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading sc_position field in FT2.root as a vector<long> failed", x);
    }

    // Test type conversions, just the first value of each loop.
    try {
      typedef unsigned long ulong;
      for (Table::Iterator itor = m_root_ft2->begin(); itor != m_root_ft2->end(); ++itor) {
        std::vector<ulong> sc_position;
        (*itor)["sc_position"].get(sc_position);
        bool mismatch = false;
        if (sc_position[0] != ulong(expected_sc_pos[0])) {
          mismatch = true;
          std::ostringstream os;
          os << "ulong(sc_position[0]) was " << sc_position[0] << ", not " << ulong(expected_sc_pos[0]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[1] != ulong(expected_sc_pos[1])) {
          mismatch = true;
          std::ostringstream os;
          os << "ulong(sc_position[1]) was " << sc_position[1] << ", not " << ulong(expected_sc_pos[1]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (sc_position[2] != ulong(expected_sc_pos[2])) {
          mismatch = true;
          std::ostringstream os;
          os << "ulong(sc_position[2]) was " << sc_position[2] << ", not " << ulong(expected_sc_pos[2]) << ", as expected";
          ReportUnexpected(os.str());
        }
        if (!mismatch) {
          ReportExpected("Reading sc_position field in FT2.root as a vector<ulong> succeeded");
        }
        break;
      }
    } catch (const TipException & x) {
      ReportUnexpected("Reading sc_position field in FT2.root as a vector<ulong> failed", x);
    }

  }

  void TestTable::largeFileTest() {
    // Add two to the maximum number expressible as an unsigned long. This is so that
    // the number of rows and the index of the last row are both not expressible as a unsigned long.
    Index_t rec_to_add = Index_t(std::numeric_limits<unsigned long>::max()) + 2;
    try {
      bool success = true;

      // Create an empty table.
      IFileSvc::instance().createFile("large_file.fits", getDataDir() + "large_file.tpl");

      // Open the table, and add a number of records that should overflow a signed 32 bit index as well as
      // exceed 4GB in size.
      std::auto_ptr<Table> table(IFileSvc::instance().editTable("large_file.fits", "LARGE"));
      table->setNumRecords(rec_to_add);
      
      // Read the number of records
      Index_t num_records = table->getNumRecords();

      // Confirm correct.
      if (rec_to_add != num_records) {
        success = false;
        std::ostringstream os;
        os << "TestTable::largeFileTest did not find " << rec_to_add << " records after adding them";
        ReportUnexpected(os.str());
      }

      // Try reading and writing the last row.
      if (success) {
        IColumn * column = table->getColumn(0);

        char read_value = 0;
        // Read the last row, probably just a zero.
        column->get(num_records - 1, read_value);

        // Write something different to the last row. 
        char written_value = read_value + 65;
        column->set(num_records - 1, written_value);

        // Read the last row again to make sure this worked.
        column->get(num_records - 1, read_value);
        if (read_value != written_value) {
          success = false;
          std::ostringstream os;
          os << "TestTable::largeFileTest read a value of " << read_value << " from the last record, not " <<
             written_value << ", as expected";
          ReportUnexpected(os.str());
        }
      }

      if (success) {
        ReportExpected("TestTable::largeFileTest succeeded in creating, reading and writing a large file");
      }
    } catch (const TipException & x) {
      std::ostringstream os;
      os << "TestTable::largeFileTest had trouble creating file with " << rec_to_add << " records";
      ReportUnexpected(os.str(), x);
    } catch (...) {
      // Make sure clean-up occurs no matter what.
      remove("large_file.fits");
      throw;
    }
    remove("large_file.fits");
  }

  Table * TestTable::getTable() {
    return new FitsTable(getDataDir() + "a1.pha", "SPECTRUM", "#row > 0", false);
  }

  void TestTable::setToZero(Table * table) {
    short svalue = 0;
    std::vector<long> vvalue(4096, 0);

    // Zero out columns in output table:
    for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {
      (*itor)["Channel"].set(svalue);
      (*itor)["Counts"].set(vvalue);
    }

    // Confirm columns in output table are all zeroes:
    for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {
      // Set dummy variable to a non-0 value.
      short svalue = 1;

      // Fetch value from the channel column.
      (*itor)["Channel"].get(svalue);

      // Check whether it's 0.
      if (0 != svalue) throw TipException("setToZero() failed to set all scalar values in a table to 0");

      // Set dummy vector field to non-0 values.
      vvalue.assign(4096, 1);

      // Fetch vector from counts column.
      (*itor)["Counts"].get(vvalue);

      // Check whether vector is now all zeroes.
      for (std::vector<long>::iterator itor = vvalue.begin(); itor != vvalue.end(); ++itor) {
        if (0 != *itor) throw TipException("setToZero() failed to set all vector values in a table to 0");
      }
    }
  }

  bool TestTable::confirmEqual(const Table * table1, const Table * table2) {
    Table::ConstIterator itor1 = table1->begin();
    Table::ConstIterator itor2 = table2->begin();
    for (; itor1 != table1->end(); ++itor1, ++itor2) {
      short chan1 = 0;
      short chan2 = 0;
      std::vector<long> counts1(4096, 0);
      std::vector<long> counts2(4096, 0);

      (*itor1)["channel"].get(chan1);
      (*itor2)["channel"].get(chan2);

      (*itor1)["counts"].get(counts1);
      (*itor2)["counts"].get(counts2);

      if (chan1 != chan2) return false;
      if (counts1 != counts2) return false;
    }
    return true;
  }
}
