/**
 * @file makeFT2.cxx
 * @brief Convert Root D2 data from Gleam to FT2 format using Goodi.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/Header.h"

#include "fitsGen/Util.h"

using namespace fitsGen;

int main(int iargc, char * argv[]) {
   try {
      std::string rootFile("merit.root");
      std::string fitsFile("myScData.fits");

      Util::getFileNames(iargc, argv, rootFile, fitsFile);

      tip::Table * pointingTable = 
         tip::IFileSvc::instance().editTable(rootFile, "Exposure");
      long nrows = pointingTable->getNumRecords();
      tip::Table::Iterator pointing_iter = pointingTable->begin();
      tip::Table::Record & pointing = *pointing_iter;

// Create the FT2 file.
      std::string ft2Template = std::getenv("FITSGENROOT")
         + std::string("/data/ft2.tpl");
      tip::IFileSvc::instance().createFile(fitsFile, ft2Template);
      tip::Table * scDataTable =
         tip::IFileSvc::instance().editTable(fitsFile, "Ext1");
      scDataTable->setNumRecords(nrows);
      tip::Table::Iterator ft2_iter = scDataTable->begin();
      tip::Table::Record & ft2 = *ft2_iter;

      for (; pointing_iter != pointingTable->end(); 
           ++pointing_iter, ++ft2_iter) {
         ft2["start"].set(pointing["elapsed_time"].get());
         tip::Table::Vector<float> scPosition = ft2["sc_position"];
// @todo check units (FT2 specifies sc_position in m)
         scPosition[0] = pointing["posx"].get();
         scPosition[1] = pointing["posy"].get();
         scPosition[2] = pointing["posz"].get();
         ft2["lat_geo"].set(pointing["lat"].get());
         ft2["lon_geo"].set(pointing["lon"].get());
// @todo check units (FT2 specifies rad_geo in km)
         ft2["rad_geo"].set(pointing["alt"].get());
         ft2["ra_scz"].set(pointing["raz"].get());
         ft2["dec_scz"].set(pointing["decz"].get());
         ft2["ra_scx"].set(pointing["rax"].get());
         ft2["dec_scx"].set(pointing["decx"].get());
      }

// Get stop times. This is complicated since Table::Iterator is not
// random access.
      pointing_iter = pointingTable->begin();
      ++pointing_iter;
      ft2_iter = scDataTable->begin();
      double start_time = ft2["start"].get();
      for (; pointing_iter != pointingTable->end(); 
           ++pointing_iter, ++ft2_iter) {
         ft2["stop"].set(pointing["elapsed_time"].get());
      }
// Compute the last stop time based on the last interval of start times.
      pointing_iter = pointingTable->end();
      --pointing_iter;
      --pointing_iter;
      double t1 = pointing["elapsed_time"].get();
      ++pointing_iter;
      double t2 = pointing["elapsed_time"].get();
      double stop_time = 2.*t2 - t1;
      ft2_iter = scDataTable->end();
      --ft2_iter;
      ft2["stop"].set(stop_time);

      Util::writeDateKeywords(scDataTable, start_time, stop_time);
      delete scDataTable;
   } catch (std::exception &eObj) {
      std::cout << eObj.what() << std::endl;
      std::exit(1);
   }
}
