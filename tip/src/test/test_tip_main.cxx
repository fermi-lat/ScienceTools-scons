/** \file test_table_main.cxx

    \brief Test/sample program.

    \author James Peachey, HEASARC
*/
#include <iostream>
#include <vector>

#include "table/Header.h"
#include "table/IFileSvc.h"
#include "table/Table.h"

int main() {
  int status = 0;

  using namespace table;

  Table * my_table = 0;

  try {
    try {
      // Opening a non-existent file should throw an exception.
      my_table = IFileSvc::getSvc().editTable("non-existent.pha", "SPECTRUM");

      // If we got here, it didn't throw!
      std::cerr << "Opening non-existent.pha didn't throw a TableException." << std::endl;

      status = 1;
    } catch(const TableException & x) {
      // This is as it should be
    }

    delete my_table;

    // The following test file should be present.
    my_table = IFileSvc::getSvc().editTable("a1.pha", "SPECTRUM");

    // Populate a test array with one of the fields from the table.
    std::vector<double> counts_vec(my_table->getNumRecords());

    // First, use standard C++-style access.
    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table access methods are working.
      int ichan = 0;

      for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
        // Variable used to hold the channel at each point in the loop.
        double channel = 0.;

        // For convenience, dereference the iterator.
        Table::Record & r = *itor;

        // Explicitly get the "counts" field from the file into the counts_vec array, one value at
        // a time.
        r["counts"].get(counts_vec[ichan]);

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
        } catch(const TableException & x) {
          // This should have thrown.
        }

      }
    } catch(const TableException & x) {
      std::cerr << "Unexpected exception while testing TableIterator: " << x.what() << std::endl;
      status = 1;
    }

    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table reading methods are working.
      int ichan = 0;

      // Declare the iterator and dereference it outside the loop.
      Table::Iterator itor;
      Table::Record & r = *itor;

      // Make local aliases to hold the channel and counts. These variables are bound to the Iterator's
      // referent Table::Record object.
      Table::Scalar<double> channel = r["channel"];
      Table::Scalar<double> counts = r["counts"];

      // Just for fun, verify that one can copy-contruct a Scalar object.
      Table::Scalar<double> spud = counts;

      // An aside: for completeness, make a const iterator.
      const Table::Iterator citor = my_table->begin();

      // An aside continued: dereference the const iterator.
      const Table::Record & r1 = *citor;

      // An aside concluded: get a const Cell from the const Record:
      const Table::Cell & cell = r1["channel"];

      // Now try getting the same value as a long.
      Table::Scalar<long> lchannel = r["channel"];

      for (itor = my_table->begin(); itor != my_table->end(); ++itor) {

        // Note that the iterator is never deferenenced; that was done once and for all above.
        if (counts != counts_vec[ichan]) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more values read from counts mismatched following get using Scalar." << std::endl;
            status = 1;
            first_time = false;
          }
        }

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

        double dcounts = 1000.;
        dcounts = counts;

        if (dcounts != counts) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "Assignment to double or comparison to double didn't work right for Scalar." << std::endl;
            std::cerr << "dcounts == " << dcounts << " not " << counts << std::endl;
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
        if (counts != vcounts[0]) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "First Counts value when read into a vector is " <<
              double(vcounts[0]) << " not " << counts << std::endl;
            status = 1;
            first_time = false;
          }
        }
      }

    } catch(const TableException & x) {
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
      if (telescop.compare("SWIFT")) {
        std::cerr << "Keyword telescop was read to be " << telescop << " not " << "SWIFT" << std::endl;
        status = 1;
      }
    } catch(const TableException & x) {
      std::cerr << "Unexpected exception while testing direct keyword access: " << x.what() << std::endl;
      status = 1;
    }

    delete my_table; my_table = 0;

    // Now test Root file access.
    my_table = IFileSvc::getSvc().editTable("merit.root", "1");

    try {
      int recordNum = 0;

      // Declare an iterator and dereference it outside the loop.
      Table::Iterator itor;
      Table::Record & r = *itor;

      // Make local aliases to hold two fields from the file. These variables are bound to the Iterator's
      // referent Table::Record object.
      Table::Scalar<double> McEnergy = r["McEnergy"];
      Table::Scalar<double> McCharge = r["McCharge"];

      // Show the columns.
      for (itor = my_table->begin(); itor != my_table->end(); ++itor) {
        std::cout << "*     " << recordNum++ << " *      " << McEnergy << " *      " << McCharge << std::endl;
      }

    } catch(const TableException & x) {
      std::cerr << "Unexpected exception while reading through the iterator into Scalar for Root file: " <<
        x.what() << std::endl;
      status = 1;
    }


  } catch(const TableException & x) {
    std::cerr << "Unhandled TableException:" << x.what() << std::endl;
    status = 1;
  } catch(const std::exception & x) {
    std::cerr << "Unhandled std::exception." << x.what() << std::endl;
  } catch(...) {
    std::cerr << "Unhandled unknown thrown object." << std::endl;
  }

  delete my_table;

  return status;
}
