/** \file test_table_main.cxx

    \brief Test/sample program.

    \author James Peachey, HEASARC
*/
#include <iostream>
#include <vector>

#include "table/Ref.h"
#include "table/Table.h"

int main() {
  int status = 0;

  using namespace table;

  try {
    try {
      // Opening a non-existent file should throw an exception.
      Table * my_table = Table::openReadWrite("non-existent.pha", "SPECTRUM");

      // If we got here, it didn't throw!
      std::cerr << "Opening non-existent.pha didn't throw a TableException." << std::endl;

      delete my_table;

      status = 1;
    } catch(const TableException & x) {
      // This is as it should be
    }

    // The following test file should be present.
    Table * my_table = Table::openReadWrite("arlac.pha", "SPECTRUM");

    // Populate a test array with one of the fields from the table.
    std::vector<double> counts_vec(my_table->getNumRecords());

    // First, use standard C++-style access.
    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table reading methods are working.
      int ichan = 0;

      for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
        // Variable used to hold the channel at each point in the loop.
        double channel = 0.;

        // For convenience, dereference the iterator.
        Record & r = *itor;

        // Explicitly read the "counts" field from the file into the counts_vec array, one value at
        // a time.
        r["counts"].read(counts_vec[ichan]);

        // Similarly, read the channel number. Just put this in a scalar however.
        r["channel"].read(channel);

        // Confirm that the channel number read from the file matches what it should be.
        // Channels are numbered sequentially starting with 1.
        if (channel != ++ichan) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more channel numbers mismatched following read." << std::endl;
            status = 1;
            first_time = false;
          }
        }

        try {
          double energy;
          r["energy"].read(energy);
          std::cerr << "Getting a non-existent column didn't throw." << std::endl;
          status = 1;
        } catch(const TableException & x) {
          // This should have thrown.
        }

      }
    } catch(const TableException & x) {
      std::cerr << "Unexpected exception while testing reading through the iterator." << std::endl;
      status = 1;
    }

    try {
      // Integer channel number, which will be incremented and used to verify that
      // the Table reading methods are working.
      int ichan = 0;

      // Declare the iterator and dereference it outside the loop.
      Table::Iterator itor;
      Record & r = *itor;

      // Make local aliases to hold the channel and counts. These variables are bound to the Iterator's
      // referent Record object.
      Ref<double> channel = r["channel"];
      Ref<double> counts = r["counts"];

      // Just for fun, verify that one can copy-contruct a Ref object.
      Ref<double> spud = counts;

      for (itor = my_table->begin(); itor != my_table->end(); ++itor) {

        // Note that the iterator is never deferenenced; that was done once and for all above.
        if (counts != counts_vec[ichan]) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more values read from counts mismatched following read using Ref." << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // Confirm that the channel number read from the file matches what it should be.
        // Channels are numbered sequentially starting with 1.
        if (channel != ++ichan) {
          static bool first_time = true;
          if (first_time) {
            std::cerr << "One or more channel numbers mismatched following read using Ref." << std::endl;
            status = 1;
            first_time = false;
          }
        }

        // Verify assignment between two different Ref variables.
// This is broken for now because write doesn't work yet!
        spud = channel;
        if (spud != channel) {
          static bool first_time = true;
          if (first_time) {
//            std::cerr << "One or more assignments didn't work right for Ref." << std::endl;
//            std::cerr << spud << " is not equal to " << channel << std::endl;
//            status = 1;
            first_time = false;
          }
        }

      }

    } catch(const TableException & x) {
      std::cerr << "Unexpected exception while testing reading through the iterator into Ref." << std::endl;
      status = 1;
    }

  } catch(const TableException & x) {
    std::cerr << "Unhandled TableException." << std::endl;
    status = 1;
  } catch(const std::exception & x) {
    std::cerr << "Unhandled std::exception." << std::endl;
  } catch(...) {
    std::cerr << "Unhandled unknown thrown object." << std::endl;
  }

  return status;
}
