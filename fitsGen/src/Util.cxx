/**
 * @file Util.cxx
 * @brief Utilities for fitsGen applications.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>
#include <cstring>
#include <ctime>

#include <iostream>
#include <stdexcept>

#include "facilities/Util.h"

#include "tip/Table.h"
#include "tip/Header.h"

#include "fitsGen/Util.h"

namespace fitsGen {

void Util::getFileNames(int iargc, char * argv[], std::string & rootFile, 
                        std::string & fitsFile) {
   if (iargc == 1) {
      std::string fitsGenRoot = std::getenv("FITSGENROOT");
      rootFile = fitsGenRoot + "/data/" + rootFile;
   } else if (iargc == 2) {
      std::string app_name = argv[0];
      if (!std::strcmp(argv[1], "-h")) {
         std::cout << "usage: " 
                   << facilities::Util::basename(argv[0]) << " "
                   << "<root input file> " 
                   << "<fits input file> " << std::endl;
         std::exit(0);
      } else {
         rootFile = std::string(argv[1]);
      }
   } else if (iargc == 3) {
      rootFile = std::string(argv[1]);
      fitsFile = std::string(argv[2]);     
   }
}

void Util::writeDateKeywords(tip::Table * table, double start_time, 
                             double stop_time) {
   static double secsPerDay(8.64e4);
   tip::Header & header = table->getHeader();
   astro::JulianDate current_time = currentTime();
   try {
      header["DATE"].set(current_time.getGregorianDate());
   } catch (...) {
   }
   astro::JulianDate mission_start(2005, 7, 18, 0);
   astro::JulianDate date_start(mission_start + start_time/secsPerDay);
   astro::JulianDate date_stop(mission_start + stop_time/secsPerDay);
   try {
      header["DATE-OBS"].set(date_start.getGregorianDate());
      header["DATE-END"].set(date_stop.getGregorianDate());
   } catch (...) {
   }
   double duration = stop_time - start_time;
   try {
      header["TSTART"].set(start_time);
      header["TSTOP"].set(stop_time);
   } catch (...) {
   }
   try {
      header["ONTIME"].set(duration);
      header["TELAPSE"].set(duration);
   } catch (...) {
   }
}

astro::JulianDate Util::currentTime() {
   std::time_t my_time = std::time(0);
   std::tm * now = std::gmtime(&my_time);
   if (now != 0) {
      double hours = now->tm_hour + now->tm_min/60. + now->tm_sec/3600.;
      astro::JulianDate current_time(now->tm_year + 1900, now->tm_mon + 1,
                                     now->tm_mday, hours);
      return current_time;
   } else {
      throw std::runtime_error("currentTime:\n"
                               + std::string("cannot be ascertained, ")
                               + "std::time returns a null value.");
   }
}

} // namespace fitsGen

