/**
 * @file makeFT2.cxx
 * @brief Convert Root D2 data from Gleam to FT2 format.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/Header.h"

#include "fitsGen/Ft2File.h"
#include "fitsGen/MeritFile.h"
#include "fitsGen/Util.h"

using namespace fitsGen;

int main(int iargc, char * argv[]) {
   try {
      std::string rootFile("merit.root");
      std::string fitsFile("myScData.fits");

      Util::getFileNames(iargc, argv, rootFile, fitsFile);

      fitsGen::MeritFile pointing(rootFile, "Exposure");
      fitsGen::Ft2File ft2(fitsFile, pointing.nrows());

      for ( ; pointing.itor() != pointing.end(); pointing.next(), ft2.next()) {
         ft2["start"].set(pointing["elapsed_time"]);
         tip::Table::Vector<float> scPosition = ft2["sc_position"];
// @todo check units (FT2 specifies sc_position in m)
         scPosition[0] = pointing["posx"];
         scPosition[1] = pointing["posy"];
         scPosition[2] = pointing["posz"];
         ft2["lat_geo"].set(pointing["lat"]);
         ft2["lon_geo"].set(pointing["lon"]);
// @todo check units (FT2 specifies rad_geo in km)
         ft2["rad_geo"].set(pointing["alt"]);
         ft2["ra_scz"].set(pointing["raz"]);
         ft2["dec_scz"].set(pointing["decz"]);
         ft2["ra_scx"].set(pointing["rax"]);
         ft2["dec_scx"].set(pointing["decx"]);
      }

// Get stop times. This is complicated since Table::Iterator is not
// random access.
      pointing.itor() = pointing.begin();
      pointing.next();
      ft2.itor() = ft2.begin();
      double start_time = ft2["start"].get();
      for (; pointing.itor() != pointing.end();  pointing.next(), ft2.next()) {
         ft2["stop"].set(pointing["elapsed_time"]);
      }
// Compute the last stop time based on the last interval of start times.
      pointing.itor() = pointing.end();
      pointing.prev();
      pointing.prev();
      double t1 = pointing["elapsed_time"];
      pointing.next();
      double t2 = pointing["elapsed_time"];
      double stop_time = 2.*t2 - t1;
      ft2.itor() = ft2.end();
      ft2.prev();
      ft2["stop"].set(stop_time);

      ft2.setObsTimes(start_time, stop_time);
   } catch (std::exception &eObj) {
      std::cout << eObj.what() << std::endl;
      std::exit(1);
   }
}
