/** \file sample.cxx
    \brief Sample application which illustrates the points discussed in the mainpage.h.
    \author  James Peachey peachey@lheamail.gsfc.nasa.gov
*/

#include <iostream>
#include <typeinfo>

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

int main() {
  try {
    // Example 1:
    using namespace tip;
    Table * my_table = IFileSvc::instance().editTable("day023.fits", "LAT_Event_Summary");
  
    // Example 2:
    Header & header = my_table->getHeader();
    double tstart;
    header["tstart"].get(tstart);
  
    // Example 3:
    // Loop over all records (rows) and extract values of ph_time column.
    for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
  
      // Local double variable to hold the value of the field for each row:
      double ph_time_dbl = (*itor)["ph_time"].get();
  
      // Do something useful with ph_time_dbl's value here ...
    }
  
    // Example 4:
    // Completely equivalent to Example 3:
    // Loop over all records (rows) and extract values of ph_time column.
    for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
  
      // Dereference the iterator and bind it to a local reference:
      Table::Record & record = *itor;
  
      // Create a local reference representing the field (ph_time_cell) of
      // interest:
      Table::Cell & ph_time_cell(record["ph_time"]);
  
      // Get the current value:
      double ph_time_dbl = ph_time_cell.get();
  
      // Do something useful with ph_time_dbl's value here ...
    }
  
    // Example 5:
    // Modify tstart keyword:
    double offset = 86400.;
    tstart -= offset;
    header["tstart"].set(tstart);
  
    // Modify ph_time column:
    // Loop over all records (rows), extract and modify values of ph_time column.
    for (Table::Iterator itor = my_table->begin(); itor != my_table->end(); ++itor) {
  
      // Local double variable to hold the value of the field for each row:
      double ph_time_dbl = (*itor)["ph_time"].get();
  
      // Modify value:
      ph_time_dbl -= offset;
  
      // Write it back to the table:
      (*itor)["ph_time"].set(ph_time_dbl);
    }

  } catch (const std::exception & x) {
    std::cerr << "Caught " << typeid(x).name() << ": " << x.what() << std::endl;
  } catch (...) {
    std::cerr << "Caught unknown object." << std::endl;
  }

  return 0;
}
