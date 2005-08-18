/**
 * @file Gti.cxx
 * @brief Derived class of evtbin::Gti. Adds a constructor and 
 * accept() method.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>

#include "fitsio.h"

#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "dataSubselector/Gti.h"

namespace {
   void fitsReportError(int status) {
      fits_report_error(stderr, status);
      if (status != 0) {
         throw std::string("dataSubselector::Gti::writeExtension: " +
                           std::string("cfitsio error."));
      }
   }
   bool gti_comp(const std::pair<double, double> & a, 
                 const std::pair<double, double> & b) {
      return a.first < b.first;
   }
}

namespace dataSubselector {

Gti::Gti(const tip::Table & gtiTable) : evtbin::Gti() {
   tip::Table::ConstIterator it = gtiTable.begin();
   tip::ConstTableRecord & interval = *it;
   for ( ; it != gtiTable.end(); ++it) {
      double start, stop;
      interval["START"].get(start);
      interval["STOP"].get(stop);
      insertInterval(start, stop);
   }
}

bool Gti::accept(double time) const {
   for (ConstIterator it = begin(); it != end(); ++it) {
      if (it->first <= time && time <= it->second) {
         return true;
      }
   }
   return false;
}

void Gti::writeExtension(const std::string & filename) const {
   try {
// Check if the extension exists already. If not, add it.
      std::auto_ptr<const tip::Table> 
         gtiTable(tip::IFileSvc::instance().readTable(filename, "GTI"));
   } catch (tip::TipException & eObj) {
      if (!st_facilities::Util::
          expectedException(eObj, "Could not open FITS extension")) {
         throw;
      }
      int status(0);
      fitsfile * fptr;
      fits_open_file(&fptr, filename.c_str(), READWRITE, &status);
      ::fitsReportError(status);
      
      char *ttype[] = {"START", "STOP"};
      char *tform[] = {"D", "D"};
      char *tunit[] = {"s", "s"};
      fits_create_tbl(fptr, BINARY_TBL, 0, 2, ttype, tform, tunit,
                      "GTI", &status);
      ::fitsReportError(status);
         
      fits_close_file(fptr, &status);
      ::fitsReportError(status);
   }
   std::auto_ptr<tip::Table> 
      gtiTable(tip::IFileSvc::instance().editTable(filename, "GTI"));

// Assume that setting the number of records to zero erases the existing
// data.
   gtiTable->setNumRecords(0);

   gtiTable->setNumRecords(getNumIntervals());
   tip::Table::Iterator it = gtiTable->begin();
   tip::Table::Record & row = *it;
   Gti::ConstIterator interval = begin();
   double ontime(0);
   double tstart, tstop;
   tstart = interval->first;
   tstop = interval->second;
   for ( ; it != gtiTable->end(); ++it, ++interval) {
      row["START"].set(interval->first);
      row["STOP"].set(interval->second);
      ontime += interval->second - interval->first;
      if (interval->first < tstart) {
         tstart = interval->first;
      }
      if (interval->second > tstop) {
         tstop = interval->second;
      }
   }
   double telapse(tstop - tstart);
   tip::Header & header = gtiTable->getHeader();
   header["ONTIME"].set(ontime);
   header["TELAPSE"].set(telapse);
}

Gti Gti::applyTimeRangeCut(double start, double stop) const {
   Gti my_Gti;
   my_Gti.insertInterval(start, stop);
   return Gti(*this & my_Gti);
}

double Gti::minValue() const {
   double min_val = begin()->first;
   for (ConstIterator interval = begin(); interval != end(); ++interval) {
      if (interval->first < min_val) {
         min_val = interval->first;
      }
   }
   return min_val;
}

double Gti::maxValue() const {
   double max_val = begin()->first;
   for (ConstIterator interval = begin(); interval != end(); ++interval) {
      if (interval->first > max_val) {
         max_val = interval->first;
      }
   }
   return max_val;
}

Gti Gti::operator|(const evtbin::Gti & rhs) const {
   std::vector< std::pair<double, double> > my_intervals;
   std::vector< std::pair<double, double> >::const_iterator intvl;
   for (intvl = begin(); intvl != end(); ++intvl) {
      my_intervals.push_back(*intvl);
   }
   for (intvl = rhs.begin(); intvl != rhs.end(); ++intvl) {
      my_intervals.push_back(*intvl);
   }
   std::stable_sort(my_intervals.begin(), my_intervals.end(), ::gti_comp);
   std::vector<double> x1, x2;
   x1.push_back(my_intervals.front().first);
   x2.push_back(my_intervals.front().second);
   for (unsigned int i = 1; i < my_intervals.size(); i++) {
      if (x2.back() < my_intervals.at(i).first) {
         x1.push_back(my_intervals.at(i).first);
         x2.push_back(my_intervals.at(i).second);
      } else {
         if (my_intervals.at(i).second > x2.back()) {
            x2.back() = my_intervals.at(i).second;
         }
      }
   }
   Gti new_gti;
   for (unsigned int i = 0; i < x1.size(); i++) {
      new_gti.insertInterval(x1.at(i), x2.at(i));
   }
   return new_gti;
}

} // namespace dataSubselector
