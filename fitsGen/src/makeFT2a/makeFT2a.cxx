/**
 * @file makeFT2.cxx
 * @brief Convert ascii D2 data from Gleam to FT2 format using Goodi.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/Header.h"

#include "astro/EarthCoordinate.h"

#include "fitsGen/Util.h"

using namespace fitsGen;

namespace {

void getFileNames(int iargc, char * argv[], std::string & rootFile,
                  std::string & fitsFile) {
   if (iargc != 3) {
      std::cout << "usage: " 
                << facilities::Util::basename(argv[0]) << " "
                << "<pointing history file> " 
                << "<FITS output file> " << std::endl;
      std::exit(0);
   } else {
      rootFile = std::string(argv[1]);
      fitsFile = std::string(argv[2]);     
   }
}

unsigned long count_lines(const std::string & infile) {
   std::ifstream file(infile.c_str());
   unsigned long nlines(0);
   std::string line;
   while (std::getline(file, line, '\n')) {
      nlines++;
   }
   file.close();
   return nlines;
}

} // unnamed namespace

int main(int iargc, char * argv[]) {
   try {
      std::string pointingFile;
      std::string fitsFile;
      
      ::getFileNames(iargc, argv, pointingFile, fitsFile);
      unsigned long nrows = ::count_lines(pointingFile);

// Create the FT2 file.
      std::string ft2Template = std::getenv("FITSGENROOT")
         + std::string("/data/ft2.tpl");
      tip::IFileSvc::instance().createFile(fitsFile, ft2Template);
      tip::Table * scDataTable =
         tip::IFileSvc::instance().editTable(fitsFile, "Ext1");
      scDataTable->setNumRecords(nrows);
      tip::Table::Iterator ft2_iter = scDataTable->begin();
      tip::Table::Record & ft2 = *ft2_iter;

      std::ifstream d2(pointingFile.c_str());
      std::string line;
      std::vector<std::string> dataFields;
      while (std::getline(d2, line, '\n')) {
         facilities::Util::stringTokenize(line, "\t ", dataFields);
         ft2["start"].set(std::atof(dataFields[0].c_str()));
         tip::Table::Vector<float> scPosition = ft2["sc_position"];
// convert the spacecraft position from km to meters.
         scPosition[0] = std::atof(dataFields[1].c_str())*1e3;
         scPosition[1] = std::atof(dataFields[2].c_str())*1e3;
         scPosition[2] = std::atof(dataFields[3].c_str())*1e3;
         ft2["ra_scz"].set(std::atof(dataFields[4].c_str()));
         ft2["dec_scz"].set(std::atof(dataFields[5].c_str()));
         ft2["ra_scx"].set(std::atof(dataFields[6].c_str()));
         ft2["dec_scx"].set(std::atof(dataFields[7].c_str()));
         ft2["ra_zenith"].set(std::atof(dataFields[8].c_str()));
         ft2["dec_zenith"].set(std::atof(dataFields[9].c_str()));
         double lonGeo = std::atof(dataFields[10].c_str());
         double latGeo = std::atof(dataFields[11].c_str());
         double radGeo = std::atof(dataFields[12].c_str());
         ft2["lon_geo"].set(lonGeo);
         ft2["lat_geo"].set(latGeo);
         ft2["rad_geo"].set(radGeo);
         astro::EarthCoordinate earthCoord(latGeo, lonGeo, radGeo);
         if (earthCoord.insideSAA()) {
            ft2["in_saa"].set(true);
         } else {
            ft2["in_saa"].set(false);
         }
         ++ft2_iter;
      }
// Get stop times. This is painful and ugly since Table::Iterator is
// not random access.
      ft2_iter = scDataTable->begin();
      ++ft2_iter;
      double start_time;
      for ( ; ft2_iter != scDataTable->end(); ++ft2_iter) {
         start_time = ft2["start"].get();
         --ft2_iter;
         ft2["stop"].set(start_time);
         ++ft2_iter;
      }
      ft2_iter = scDataTable->end();
      --ft2_iter;
      --ft2_iter;
      double time_step = start_time - ft2["start"].get();
      ft2_iter = scDataTable->end();
      --ft2_iter;
      double stop_time = start_time + time_step;
      ft2["stop"].set(stop_time);

// sigh, *now* we can fill in the livetime
      ft2_iter = scDataTable->begin();
      for ( ; ft2_iter != scDataTable->end(); ++ft2_iter) {
         double full_interval(ft2["stop"].get() - ft2["start"].get());
         double fraction(0.90);
         ft2["livetime"].set(fraction*full_interval);
         ft2["deadtime"].set(full_interval*(1. - fraction));
         bool in_saa;
         ft2["in_saa"].get(in_saa);
         if (true == in_saa) {
            ft2["livetime"].set(0);
            ft2["deadtime"].set(full_interval);
         }
      }

      Util::writeDateKeywords(scDataTable, start_time, stop_time);
      delete scDataTable;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      std::exit(1);
   }
}
