/**
 * @file Util.cxx
 * @brief
 * @author J. Chiang
 *
 * $Header$
 */

#include <cassert>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "fitsio.h"

#include "facilities/Util.h"

#include "tip/Header.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

namespace {
   bool reverse_cmp(double x, double y) {
      return x > y;
   }
   int findIndex(const std::vector<double> & xx, double x) {
      std::vector<double>::const_iterator ix;
      if (xx.front() < xx.back()) {
         ix = std::upper_bound(xx.begin(), xx.end(), x);
      } else {
         ix = std::upper_bound(xx.begin(), xx.end(), x, reverse_cmp);
      }
      if (ix != xx.end()) {
         return ix - xx.begin();
      } else {
         if (xx.front() < xx.back()) {
            if (x < xx.front()) {
               return 1;
            } else {
               return xx.size() - 1;
            }
         } else {
            if (x < xx.back()) {
               return 1;
            } else {
               return xx.size() - 1;
            }
         }
      }
   }
   void strip_at_sign(std::string & input) {
      if (input.find_first_of("@") == 0) {
         std::string output = "";
         std::string::iterator it = input.begin() + 1;
         for ( ; it != input.end(); ++it) {
            output += *it;
         }
         input = output;
      }
   }
}

namespace st_facilities {

   bool Util::fileExists(const std::string & filename) {
      std::ifstream file(filename.c_str());
      return file.is_open();
   }

   void Util::file_ok(std::string filename) {
      facilities::Util::expandEnvVar(&filename);
      if (fileExists(filename)) {
         return;
      } else {
         throw std::runtime_error("File not found: " + filename);
      }
   }

   void Util::readLines(std::string inputFile, 
                        std::vector<std::string> & lines,
                        const std::string & skip,
                        bool cleanLines) {
      facilities::Util::expandEnvVar(&inputFile);
      std::ifstream file(inputFile.c_str());
      lines.clear();
      std::string line;
      file_ok(inputFile);
      while (std::getline(file, line, '\n')) {
         if (line != "" && line != " "             //skip (most) blank lines 
             && line.find_first_of(skip) != 0) {   //and commented lines
            if (cleanLines) {
               cleanLine(line);
            }
            lines.push_back(line);
         }
      }
   }

   void Util::cleanLine(std::string & line) {
      char CR[1];
      CR[0] = 0x0d;
      if (line.find(CR) != std::string::npos) {
         std::vector<std::string> tokens;
         facilities::Util::stringTokenize(line, CR, tokens);
         line = tokens.front();
      }
   }

   bool Util::isFitsFile(const std::string & infile) {
      fitsfile * fp(0);
      int status(0);
      fits_open_file(&fp, const_cast<char *>(infile.c_str()), 
                     READONLY, &status);
      if (0 != status) {
         return false;
      }
      fits_close_file(fp, &status);
      if (status != 0) {
         throw std::runtime_error("Util::isFitsFile: Error closing file "
                                  + infile);
      }
      return true;
   }

   void Util::resolve_fits_files(std::string filename, 
                                 std::vector<std::string> &files) {
      ::strip_at_sign(filename);
      facilities::Util::expandEnvVar(&filename);
      files.clear();
      if (isFitsFile(filename)) {
         files.push_back(filename);
         return;
      } else {
// filename contains a list of fits files.
         readLines(filename, files);
         return;
      }
   }

   bool Util::isXmlFile(std::string filename) {
      std::vector<std::string> tokens;
      facilities::Util::stringTokenize(filename, ".", tokens);
      if (*(tokens.end()-1) == "xml") {
         return true;
      }
      return false;
   }

   double Util::interpolate(const std::vector<double> &x,
                            const std::vector<double> &y,
                            double xx) {
      if (xx < x.front() || xx > x.back()) {
         std::ostringstream message;
         message << "Util::interpolate:\n"
                 << "abscissa value out-of-range, "
                 << xx << " is not in (" 
                 << x.front() << ", "
                 << x.back() << ")";
         throw std::range_error(message.str());
      }
      std::vector<double>::const_iterator it 
         = std::upper_bound(x.begin(), x.end(), xx) - 1;
      unsigned int indx = it - x.begin();
      double yy;
      if (*(it+1) != *it) {
         yy = (xx - *it)/(*(it+1) - *it)*(y[indx+1] - y[indx]) + y[indx];
      } else {
         yy = (y[indx+1] + y[indx])/2.;
      }
      return yy;
   }

   double Util::bilinear(const std::vector<double> &xx, double x, 
                         const std::vector<double> &yy, double y, 
                         const std::vector<double> &z) {
      int i = ::findIndex(xx, x);
      if (i < 1) {
         i = 1;
      } else if (i > static_cast<int>(xx.size())) {
         i = xx.size() - 1;
      }
      int j = ::findIndex(yy, y);
      if (j < 1) {
         j = 1;
      } else if (j > static_cast<int>(yy.size())) {
         j = yy.size() - 1;
      }

      double tt = (x - xx.at(i-1))/(xx.at(i) - xx.at(i-1));
      double uu = (y - yy.at(j-1))/(yy.at(j) - yy.at(j-1));
      
      double y1 = z[yy.size()*(i-1) + (j-1)];
      double y2 = z[yy.size()*(i) + (j-1)];
      double y3 = z[yy.size()*(i) + (j)];
      double y4 = z[yy.size()*(i-1) + (j)];

      double value = (1. - tt)*(1. - uu)*y1 + tt*(1. - uu)*y2 
         + tt*uu*y3 + (1. - tt)*uu*y4; 
//       assert(value >= 0);
//       if (value < 0.) {
//          std::ostringstream message;
//          message << "st_facilities::Util::bilinear:\n"
//                  << "value = " << value << " < 0\n";
//          message << xx[i-1] << "  " << xx.at(i-1) << "  " 
//                  << x << "  " << xx.at(i) << "\n";
//          message << yy[j-1] << "  " << yy.at(j-1) << "  " 
//                  << y << "  " << yy.at(j) << "\n";
//          message << tt << "  " << uu << "  " 
//                  << y1 << "  " << y2 << "  "
//                  << y3 << "  " << y4;
//          throw std::runtime_error(message.str());
//       }
      return value;
   }

   double Util::bilinear(const std::vector<double> &xx, double x, 
                         const std::vector<double> &yy, double y, 
                         const std::vector< std::vector<double> > &z) {

      std::vector<double>::const_iterator ix;
      if (x < *(xx.begin())) {
         ix = xx.begin() + 1;
      } else if (x >= *(xx.end()-1)) {
         ix = xx.end() - 1;
      } else {
         ix = std::upper_bound(xx.begin(), xx.end(), x);
      }
      int i = ix - xx.begin();
      
      std::vector<double>::const_iterator iy;
      if (y < *(yy.begin())) {
         iy = yy.begin() + 1;
      } else if (y >= *(yy.end()-1)) {
         iy = yy.end() - 1;
      } else {
         iy = std::upper_bound(yy.begin(), yy.end(), y);
      }
      int j = iy - yy.begin();
      
      double tt = (x - *(ix-1))/(*(ix) - *(ix-1));
      double uu = (y - *(iy-1))/(*(iy) - *(iy-1));
      
      double y1 = z.at(i-1).at(j-1);
      double y2 = z.at(i).at(j-1);
      double y3 = z.at(i).at(j);
      double y4 = z.at(i-1).at(j);

      double value = (1. - tt)*(1. - uu)*y1 + tt*(1. - uu)*y2 
         + tt*uu*y3 + (1. - tt)*uu*y4; 
//       assert(value >= 0);
//       if (value < 0.) {
//          std::ostringstream message;
//          message << "st_facilities::Util::bilinear:\n"
//                  << "value = " << value << " < 0\n";
//          message << xx[i-1] << "  " << *(ix-1) << "  " 
//                  << x << "  " << *ix << "\n";
//          message << yy[j-1] << "  " << *(iy-1) << "  " 
//                  << y << "  " << *iy << "\n";
//          message << tt << "  " << uu << "  " 
//                  << y1 << "  " << y2 << "  "
//                  << y3 << "  " << y4;
//          throw std::runtime_error(message.str());
//       }
      return value;
   }

   bool Util::expectedException(const std::exception & eObj, 
                                const std::string & targetMessage) {
      std::string message(eObj.what());
      return message.find(targetMessage.c_str()) 
         != std::string::npos;
   }

   void Util::writeDateKeywords(tip::Extension * table, double start_time, 
                                double stop_time, bool extension,
                                const astro::JulianDate & mission_start) {
      (void)(extension);
      static double secsPerDay(8.64e4);
      tip::Header & header = table->getHeader();
      astro::JulianDate current_time = currentTime();
      try {
         header["DATE"].set(current_time.getGregorianDate());
      } catch (...) {
      }
// The official mission start time is Jan 1 2001:
      astro::JulianDate date_start(mission_start + start_time/secsPerDay);
      astro::JulianDate date_stop(mission_start + stop_time/secsPerDay);
      try {
         header["DATE-OBS"].set(date_start.getGregorianDate());
         header["DATE-END"].set(date_stop.getGregorianDate());
      } catch (...) {
      }
      try {
         header["TSTART"].set(start_time);
         header["TSTOP"].set(stop_time);
      } catch (...) {
      }
// Update TELAPSE keyword if it exists
      if (table->getHeader().find("TELAPSE") != table->getHeader().end()) {
         header["TELAPSE"].set(stop_time - start_time);
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

} // namespace st_facilities
