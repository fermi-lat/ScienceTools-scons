/** \file sample.cxx
    \brief Sample application which illustrates the points discussed in the mainpage.h.
    Note that this is only a demonstration of how to use tip. It is not a good example of
    how to write an application! For that see the st_app and st_app_examples packages.
    \author  James Peachey peachey@lheamail.gsfc.nasa.gov
*/

#include <iostream>
#include <typeinfo>

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

int main() {
  using namespace tip;
  const Table * const_table = 0;
  try {
    // Dummy variable used to silence warning about local variables not being used.
    double silence_warning = 0.;

    // Example 1:
    const_table = IFileSvc::instance().readTable("day023.fits", "LAT_Event_Summary");

    // Example 2:
    const Header & header = const_table->getHeader();
    double tstart;
    header["tstart"].get(tstart);

    // Example 3:
    // Loop over all records (rows) and extract values of ph_time column.
    for (Table::ConstIterator itor = const_table->begin(); itor != const_table->end(); ++itor) {

      // Local double variable to hold the value of the field for each row:
      double ph_time_dbl = (*itor)["ph_time"].get();

      // Do something useful with ph_time_dbl's value here ...
      // (The following is just to silence a compiler warning:)
      silence_warning += ph_time_dbl;
    }

    // Example 4:
    // Completely equivalent to Example 3, just provides explicit typenames.
    // Loop over all records (rows) and extract values of ph_time column.
    for (Table::ConstIterator itor = const_table->begin(); itor != const_table->end(); ++itor) {

      // Dereference the iterator and bind it to a local reference:
      Table::ConstRecord & record = *itor;

      // Create a local reference representing the field (ph_time_cell) of
      // interest:
      const Table::Cell & ph_time_cell(record["ph_time"]);

      // Get the current value:
      double ph_time_dbl = ph_time_cell.get();

      // Do something useful with ph_time_dbl's value here ...
      // (The following is just to silence a compiler warning:)
      silence_warning += ph_time_dbl;
    }

  } catch (const std::exception & x) {
    std::cerr << "Caught " << typeid(x).name() << ": " << x.what() << std::endl;
  }
  delete const_table; const_table = 0;

  Table * table = 0;
  try {
    // Example 5:
    using namespace tip;
    table = IFileSvc::instance().editTable("day023.fits", "LAT_Event_Summary");

    Header & header = table->getHeader();
    double tstart;
    header["tstart"].get(tstart);

    // Modify tstart keyword:
    double offset = 86400.;
    tstart -= offset;
    header["tstart"].set(tstart);

    // Modify ph_time column:
    // Loop over all records (rows), extract and modify values of ph_time column.
    for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {

      // Local double variable to hold the value of the field for each row:
      double ph_time_dbl = (*itor)["ph_time"].get();

      // Modify value:
      ph_time_dbl -= offset;

      // Write it back to the table:
      (*itor)["ph_time"].set(ph_time_dbl);
    }

  } catch (const std::exception & x) {
    std::cerr << "Caught " << typeid(x).name() << ": " << x.what() << std::endl;
  }
  delete table; table = 0;

  try {
    // Example 6:
    // Copy events extension.

    // Open input and create output tables.
    const_table = IFileSvc::instance().readTable("D1.fits", "EVENTS");
    IFileSvc::instance().createFile("D1copy.fits", "D1.fits");
    table = IFileSvc::instance().editTable("D1copy.fits", "EVENTS");

    // Change size at the outset; this is more efficient than doing it row by row.
    table->setNumRecords(const_table->getNumRecords());

    // Start at beginning of each table.
    Table::ConstIterator in_itor = const_table->begin();
    Table::Iterator out_itor = table->end();

    // Copy all records.
    for (; in_itor != const_table->end(); ++in_itor, ++out_itor) {
      *out_itor = *in_itor;
    }

  } catch (const std::exception & x) {
    std::cerr << "Caught " << typeid(x).name() << ": " << x.what() << std::endl;
  }
  delete table; table = 0;
  delete const_table; const_table = 0;
  return 0;
}
