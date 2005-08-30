/** \file test_tip_main.cxx

    \brief Test/sample program.

    \author James Peachey, HEASARC
*/
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <string>
#include <vector>

#include "TestColumn.h"
#include "TestExtensionData.h"
#include "TestException.h"
#include "TestFileManager.h"
#include "TestFileSummary.h"
#include "TestFilter.h"
#include "TestImage.h"
#include "TestInterpolation.h"
#include "TestKeyRecord.h"
#include "TestTable.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

// Thorough test of IExtensionData and its subclasses.
int TestExtensionData(const std::string & data_dir, int currentStatus);

int main() {
  int status = 0;

  using namespace tip;

  Table * my_table = 0;

  try {
    // Figure out runtime environment.
    std::string tiproot;
    std::string data_dir;

    // Read TIPROOT environment variable.
    const char * tiproot_cp = getenv("TIPROOT");

    // Use it to set name of data directory:
    if (0 != tiproot_cp) {
      tiproot = tiproot_cp;
#ifdef WIN32
      data_dir = tiproot + "\\data\\";
#else
      data_dir = tiproot + "/data/";
#endif
    }

    // Perform startup initialization.
    IFileSvc::globalInit();

    try {
      // Opening a non-existent file should throw an exception.
      my_table = IFileSvc::instance().editTable("non-existent.pha", "SPECTRUM");

      // If we got here, it didn't throw!
      std::cerr << "Unexpected: opening non-existent.pha didn't throw a TipException." << std::endl;

      status = 1;
    } catch(const TipException & x) {
      // This is as it should be
      std::cerr << "Expected: opening non-existent.pha threw exception: " << x.what() << std::endl;
    }

    delete my_table; my_table = 0;

    try {
      // Opening an existent file which is neither FITS nor Root should throw an exception.
      my_table = IFileSvc::instance().editTable(data_dir + "ft1.tpl", "");

      // If we got here, it didn't throw!
      std::cerr << "Unexpected: opening ft1.tpl didn't throw a TipException." << std::endl;

      status = 1;
    } catch(const TipException & x) {
      // This is as it should be
      std::cerr << "Expected: opening ft1.tpl threw exception: " << x.what() << std::endl;
    }

    delete my_table; my_table = 0;

    try {
      // Opening an existent file which is really a directory should throw an exception.
      my_table = IFileSvc::instance().editTable(data_dir, "");

      // If we got here, it didn't throw!
      std::cerr << "Unexpected: opening directory didn't throw a TipException." << std::endl;

      status = 1;
    } catch(const TipException & x) {
      // This is as it should be
      std::cerr << "Expected: opening directory threw exception: " << x.what() << std::endl;
    }

    delete my_table; my_table = 0;

    try {
      // Opening an existent FITS file but asking for a non-existent extension should throw an exception.
      my_table = IFileSvc::instance().editTable(data_dir + "a1.pha", "NON_EXIS");

      // If we got here, it didn't throw!
      std::cerr << "Unexpected: opening extension NON_EXIS in a1.pha didn't throw a TipException." << std::endl;

      status = 1;
    } catch(const TipException & x) {
      // This is as it should be
      std::cerr << "Expected: opening extension NON_EXIS in a1.pha threw exception: " << x.what() << std::endl;
    }

    delete my_table; my_table = 0;

    try {
      // Opening an existent Root file but asking for a non-existent extension should throw an exception.
      my_table = IFileSvc::instance().editTable(data_dir + "merit.root", "NON_EXIS");

      // If we got here, it didn't throw!
      std::cerr << "Unexpected: opening extension NON_EXIS in merit.root didn't throw a TipException." << std::endl;

      status = 1;
    } catch(const TipException & x) {
      // This is as it should be
      std::cerr << "Expected: opening extension NON_EXIS in merit.root threw exception: " << x.what() << std::endl;
    }

    delete my_table; my_table = 0;

    // Test opening some extensions in a1.pha, both with and without write access.
    try {
      const Extension * ext = IFileSvc::instance().readExtension(data_dir + "a1.pha", "");
      delete ext;
      std::cerr << "Expected: reading primary extension in a1.pha did not throw an exception." << std::endl;
    } catch (const TipException & x) {
      std::cerr << "Unexpected: reading primary extension in a1.pha threw exception: " << x.what() << std::endl;
      status = 1;
    }

    try {
      const Extension * ext = IFileSvc::instance().readTable(data_dir + "a1.pha", "SPECTRUM");
      delete ext;
      std::cerr << "Expected: reading SPECTRUM extension in a1.pha did not throw an exception." << std::endl;
    } catch (const TipException & x) {
      std::cerr << "Unexpected: reading SPECTRUM extension in a1.pha threw exception: " << x.what() << std::endl;
      status = 1;
    }

    try {
      Extension * ext = IFileSvc::instance().editExtension(data_dir + "a1.pha", "");
      delete ext;
      std::cerr << "Expected: editing primary extension in a1.pha did not throw an exception." << std::endl;
    } catch (const TipException & x) {
      std::cerr << "Unexpected: editing primary extension in a1.pha threw exception: " << x.what() << std::endl;
      status = 1;
    }

    try {
      Extension * ext = IFileSvc::instance().editTable(data_dir + "a1.pha", "SPECTRUM");
      delete ext;
      std::cerr << "Expected: editing SPECTRUM extension in a1.pha did not throw an exception." << std::endl;
    } catch (const TipException & x) {
      std::cerr << "Unexpected: editing SPECTRUM extension in a1.pha threw exception: " << x.what() << std::endl;
      status = 1;
    }

    // The following test file should be present.
    my_table = IFileSvc::instance().editTable(data_dir + "a1.pha", "SPECTRUM", "#row<100");

    // Populate a test array with one of the fields from the table.
    std::vector<double> counts_vec(my_table->getNumRecords());

    // Confirm the size.
    if (99 != counts_vec.size())
      std::cerr << "Selecting #row < 100 resulted in " << counts_vec.size() << " rows, not 99" << std::endl;

    // First, use standard C++-style access.
    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table access methods are working.
      int ichan = 0;

      for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
        // Variable used to hold the channel at each point in the loop.
        double channel = 0.;

        // For convenience, dereference the iterator.
        Table::Record r = *itor;

        // Similarly, get the channel number. Just put this in a scalar however.
        // Get this field using mixed case to verify case insensitivity.
        r["CHANnel"].get(channel);

        // Confirm that the channel number obtained from the file matches what it should be.
        // Channels are numbered sequentially starting with 0.
        if (channel != ichan++) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more channel numbers mismatched following get." << std::endl;
            status = 1;
            first_time = false;
          }
        }

        try {
          double energy;
          r["energy"].get(energy);
          std::cerr << "Getting a non-existent column didn't throw." << std::endl;
          status = 1;
        } catch(const TipException &) {
          // This should have thrown.
        }

      }
    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while testing TableIterator: " << x.what() << std::endl;
      status = 1;
    }

    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table reading methods are working.
      int ichan = 0;

      // TODO 8: 4/2/2004: This pattern was originally to facilitate using a functor which
      // has a Table::Record, and could thus have fields bound to member Table::Scalars or
      // Table::Cell. This binding could be in Table::Record's constructor, thus making
      // the functor's operator more efficient. But this doesn't work because for_each
      // copy constructs the functor. Solution is not known at present.

      Table::Iterator itor = my_table->begin();

      // Define a table record object.
      Table::Record & r = *itor;

      // Make local aliases to hold the channel and counts. These variables are bound to the record object.
      Table::Scalar<double> channel = r["channel"];

      // Just for fun, verify that one can copy-contruct a Scalar object.
      Table::Scalar<double> spud = channel;

      // An aside: for completeness, make a const iterator.
      const Table::Iterator citor = my_table->begin();

      // An aside continued: dereference the const iterator.
      const Table::Record & r1 = *citor;

      // An aside concluded: get a const Cell from the const Record:
      const Table::Cell & cell = r1["channel"];

      // Confirm that another way of reading the channel number matches what it should be.
      double cell_channel = cell.get();
      if (0. != cell_channel) {
        std::cerr << "Cell::get() returned " << cell_channel << ", not 0" << std::endl;
        status = 1;
      }

      // Now try getting the same value as a long.
      Table::Scalar<long> lchannel = r["channel"];

      for (; itor != my_table->end(); ++itor) {

        // Dereference the iterator. After this, channel and counts both refer to the current record.
//        r = *itor;

        // Confirm that the channel number read from the file matches what it should be.
        // Channels are numbered sequentially starting with 0.
        if (channel != ichan++) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more channel numbers mismatched following get using Scalar." << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // Verify assignment between two different Scalar variables.
        spud = channel;
        if (spud != channel) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more assignments didn't work right for Scalar." << std::endl;
            std::cerr << spud << " is not equal to " << channel << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // Verify other assignments.
        long orig_channel = channel;
        channel = my_table->getNumRecords() - channel;

        if (my_table->getNumRecords() - orig_channel != channel) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "Assignment from double or comparison to double didn't work right for Scalar." << std::endl;
            std::cerr << "channel == " << channel << " not " << my_table->getNumRecords() - orig_channel << std::endl;
            status = 1;
            first_time = false;
          }
        }

        if (lchannel != channel) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "Same value obtained as a double and a long got different results" << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // Try getting counts as a vector-valued column.
        Table::Vector<double> vcounts = r["Counts"];
        if (4096 != vcounts.getNumElements()) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "Size of counts when read into a vector is " <<
              vcounts.getNumElements() << " not " << 4096 << std::endl;
            status = 1;
            first_time = false;
          }
        }

        double save_counts = vcounts[37];
        vcounts[37] = save_counts + 37.; 
        if (save_counts + 37. != vcounts[37]) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "37th counts value as a vector after assignment is " <<
              double(vcounts[37]) << " not " << save_counts + 37. << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // And reset them:
        vcounts[37] = save_counts;
        if (save_counts != vcounts[37]) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "37th counts value as a vector after being reset to original value is " <<
              double(vcounts[37]) << " not " << save_counts << std::endl;
            status = 1;
            first_time = false;
          }
        }

#ifdef print_vector
        if (itor == my_table->begin()) {
          std::cout << "First counts vector is:" << std::endl;
          for (int ii = 0; ii < 4096; ++ii) {
            std::cout << "\t" << vcounts[ii] << std::endl;
          }
        }
#endif
      }

    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while testing reading through the iterator into Scalar: " <<
        x.what() << std::endl;
      status = 1;
    }

    try {
      Header & header = my_table->getHeader();
      double src_thet = 0.;
      header["src_thet"].get(src_thet);
      if (-999. != src_thet) {
        std::cerr << "Keyword src_thet was read to be " << src_thet << " not " << -999. << std::endl;
        status = 1;
      }

      std::string telescop;
      header["telescop"].get(telescop);
      if (0 != telescop.compare("SWIFT")) {
        std::cerr << "Keyword telescop was read to be " << telescop << " not " << "SWIFT" << std::endl;
        status = 1;
      }

      // Test keyword writing.
      header["new_bool"].set(true);
      header["new_double"].set(137.);
      header["new_string"].set("a string keyword");

    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while testing direct keyword access: " << x.what() << std::endl;
      status = 1;
    }

    try {
      const Header & const_header = my_table->getHeader();
      
      // Test getting key-value pairs from header. All should be found except NON_EXIS.
      const char * keys [] = { "POISSERR", "GROUPING", "NON_EXIS", "QUALITY", 0 };
      Header::KeyValCont_t key_vals;

      // Get the container of key-value pairs from the const_header.
      const_header.get(keys, key_vals);

      // Make sure the correct number were found.
      if (3 != key_vals.size()) {
        status = 1;
        std::cerr << "Unexpected: got " << key_vals.size() << " key-value pairs, instead of 3" << std::endl;
      }

      std::string s;
      for (Header::KeyValCont_t::iterator itor = key_vals.begin(); itor != key_vals.end(); ++itor)
        s += "\t" + itor->first + " = " + itor->second + "\n";

      std::string s_should_be("\tPOISSERR = T\n\tGROUPING = 0\n\tQUALITY = 0\n");
      if (0 != s_should_be.compare(s)) {
        status = 1;
        std::cerr << "Unexpected: matching key-value pairs were:\n" << s << "not\n" << s_should_be << std::endl;
      }

      // Add a keyword which was not in the original file.
      key_vals.push_back(Header::KeyValPair_t("NON_EXIS", "this should not get written"));

      // Modify a keyword.
      key_vals[0].second = "F"; 

      Header & header = my_table->getHeader();
      header.update(key_vals); // Should update existing keywords only.

      // Check whether update had the correct effect.
      try {
        std::string value;
        header[key_vals[0].first].get(value);
        if (0 != value.compare("F")) {
          status = 1;
          std::cerr << "Unexpected: key-value pair did not modify value of keyword " << key_vals[0].first << std::endl;
        }
      } catch (const TipException & x) {
        status = 1;
        std::cerr << "Unexpected exception while retrieving an existing keyword: " << x.what() << std::endl;
      }
      
      try {
        std::string value;
        header["NON_EXIS"].get(value);
        status = 1;
        std::cerr << "Unexpected: exception not thrown after a non-existent keyword was updated using key-value pair." << std::endl;
      } catch (const TipException &) {
        // This should happen, actually!
      }
      
      try {
        struct tm some_time;
        some_time.tm_sec = 12;
        some_time.tm_min = 12;
        some_time.tm_hour = 11;
        some_time.tm_mday = 14;
        some_time.tm_mon = 6;
        some_time.tm_year = 104;
        some_time.tm_wday = 3;
        some_time.tm_yday = 195;
        some_time.tm_isdst = 0;
        
        time_t made_time = mktime(&some_time);
        if (-1 == made_time) {
          throw std::runtime_error("mktime returned -1 when trying to construct a test time");
        }

        std::string formatted_time = header.formatTime(made_time);
        if (0 != formatted_time.compare("2004-07-14T12:12:12")) {
          status = 1;
          std::cerr << "Unexpected: Header::formatTime formatted time string as " << formatted_time <<
            ", not 2004-07-14T12:12:12" << std::endl;
        } else {
          std::cerr << "Expected: Header::formatTime formatted a time correctly" << std::endl;
        }

      } catch (const std::exception & x) {
        status = 1;
        std::cerr << "Unexpected exception while testing Header::formatTime: " << x.what() << std::endl;
      }
  
    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while testing direct key-value pair access: " << x.what() << std::endl;
      status = 1;
    }

    delete my_table; my_table = 0;

    // Now test Root file access.
    my_table = IFileSvc::instance().editTable(data_dir + "merit.root", "1", "McEnergy < 2000. && McEnergy > 50.");

    // This should work:
    Header & header = my_table->getHeader();

    // Accessing keywords should not work:
    try {
      double x;
      header["keyword"].get(x);
    } catch(const TipException &) {
      // This is as it should be.
    }

    try {
      double x = 7.;
      header["keyword"].set(x);
    } catch(const TipException &) {
      // This is as it should be.
    }

    try {
      int recordNum = 0;

      Table::Iterator itor = my_table->begin();

      Table::Record & r = *itor;

      // Make local aliases to hold two fields from the file. These variables are bound to the Iterator's
      // referent Table::Record object.
      Table::Scalar<double> McEnergy = r["McEnergy"];
      Table::Scalar<double> McCharge = r["McCharge"];

      // Show the columns.
      for (;itor != my_table->end(); ++itor) {
        std::cout << "*     " << recordNum++ << " *      " << McEnergy << " *      " << McCharge << std::endl;
      }

    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while reading through the iterator into Scalar for Root file: " <<
        x.what() << std::endl;
      status = 1;
    }

    try {
      // Test creating a new (FITS) file using ft1.tpl:
      IFileSvc::instance().createFile("new_ft1.fits", data_dir + "ft1.tpl");

      delete my_table; my_table = 0;

      // Open the newly created table:
      my_table = IFileSvc::instance().editTable("new_ft1.fits", "EVENTS");

      // Test history and comment facilities.
      try {
        my_table->getHeader().addComment("This comment was added by test_tip");
        std::cerr << "Expected: writing a comment string to a newly created header succeeded" << std::endl;

        my_table->getHeader().addHistory("This history was added by test_tip");
        std::cerr << "Expected: writing a history string to a newly created header succeeded" << std::endl;
      } catch (const TipException & x) {
        std::cerr << "Unexpected exception while writing comment or history to a newly created header: " << x.what() << std::endl;
      }

      // Insert some records:
      Index_t num_rec = 128;
      my_table->setNumRecords(num_rec);

      // Confirm that this worked.
      num_rec = my_table->getNumRecords();

      if (128 != num_rec) {
        std::cerr << "Number of records after resizing table is " << num_rec << " not 128" << std::endl;
      }

      // Write something into the records:
      double time = 0.;
      for (Table::Iterator it = my_table->begin(); it != my_table->end(); ++it) {
        (*it)["time"].set(++time);
      }

      // Truncate the table:
      my_table->setNumRecords(64);

      // Then increase its size:
      my_table->setNumRecords(128);

      // Write something else into the records:
      long event_id = 0;
      for (Table::Iterator it = my_table->begin(); it != my_table->end(); ++it) {
        (*it)["event_id"].set(++event_id);
      }

      // Confirm that this worked: time should only be set for first 64 records, but event_id for all 128 records.
      for (Table::Iterator it = my_table->begin(); it != my_table->end(); ++it) {
        (*it)["event_id"].get(event_id);
        (*it)["time"].get(time);
        if (event_id > 64 && time != 0.) {
          std::cerr << "After truncating and then augmenting table, event number " << event_id <<
            " showed a time of " << time << ", not 0." << std::endl;
        }
      }

    } catch(const TipException & x) {
      std::cerr << "Unexpected exception while creating and populating a new table: " << x.what() << std::endl;
      status = 1;
    }
    
    // Test reading and writing string valued columns.
    try {
      // Create an output file using a file for the template which has some string-valued columns.
      IFileSvc::instance().createFile("new_groD4-dc2v1.fits", data_dir + "groD4-dc2v1.fits");

      // Open the input file for reading.
      const Table * in_table = IFileSvc::instance().readTable(data_dir + "groD4-dc2v1.fits", "SPIN_PARAMETERS");

      // Open the output file for writing.
      Table * out_table = IFileSvc::instance().editTable("new_groD4-dc2v1.fits", "SPIN_PARAMETERS");

      // Populate the output file with the contents of the input file.
      Table::Iterator out_itor = out_table->begin();
      for (Table::ConstIterator in_itor = in_table->begin(); in_itor != in_table->end(); ++in_itor, ++out_itor) {
        std::string ipsrname;
        std::string opsrname;
        (*in_itor)["PSRNAME"].get(ipsrname);
        (*out_itor)["PSRNAME"].set(ipsrname);

        // Compare the two values to make sure the one written matches the one read.
        (*out_itor)["PSRNAME"].get(opsrname);
        if (ipsrname != opsrname) {
          status = 1;
          std::cerr << "Unexpected: input string \"" << ipsrname << "\" was written as \"" << opsrname << "\"" << std::endl;
        }
      }

      // Clean up.
      delete out_table;
      delete in_table;
      
    } catch(const TipException & x) {
      status = 1;
      std::cerr << "Unexpected exception while creating or populating a new table to test column string access: "
        << x.what() << std::endl;
    }

    // Test IExtensionData and its subclasses. New tests should not be added here, but rather
    // to the test object class below. The tests in the TestExtensionData function should be
    // eventually superceded by the tests in tip::TestExtensionData class.
    status = ::TestExtensionData(data_dir, status);

    // Test IExtensionData and its subclasses. This uses the new style of tests. 
    tip::TestExtensionData ext_data_test;
    status = ext_data_test.test(status);

    // Test table interface-related classes.
    TestTable table_test;
    status = table_test.test(status);

    // Test file management-related classes.
    TestFileManager file_manager_test;
    status = file_manager_test.test(status);

    // Test image access.
    TestImage image_test;
    status = image_test.test(status);

    // Test file access.
    TestFileSummary file_summary_test;
    status = file_summary_test.test(status);

    // Test column abstractions.
    TestColumn column_test;
    status = column_test.test(status);

    // Test interpolation abstractions.
    TestInterpolation interpolation_test;
    status = interpolation_test.test(status);

    // Test key record abstraction.
    TestKeyRecord key_record_test;
    status = key_record_test.test(status);

    // Test filter capabilities.
    TestFilter filter_test;
    status = filter_test.test(status);

    // Test Tip exception classes.
    TestException exception_test;
    status = exception_test.test(status);

  } catch(const TipException & x) {
    std::cerr << "Unhandled TipException: " << x.what() << std::endl;
    status = 1;
  } catch(const std::exception & x) {
    std::cerr << "Unhandled std::exception: " << x.what() << std::endl;
  } catch(...) {
    std::cerr << "Unhandled unknown thrown object." << std::endl;
  }

  delete my_table;

  return status;
}
