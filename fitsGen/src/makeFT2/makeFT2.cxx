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

      fitsGen::MeritFile pointing(rootFile, "pointing_history");
      fitsGen::Ft2File ft2(fitsFile, pointing.nrows());

      for ( ; pointing.itor() != pointing.end(); pointing.next(), ft2.next()) {
         ft2["start"].set(pointing["start"]);
         ft2["stop"].set(pointing["stop"]);
         tip::Table::Vector<float> scPosition = ft2["sc_position"];
         const tip::Table::Vector<float> merit_position =
            const_cast<tip::TableCell&>(pointing.row()["sc_position"]);
         scPosition[0] = merit_position[0];
         scPosition[1] = merit_position[1];
         scPosition[2] = merit_position[2];
         ft2["lat_geo"].set(pointing["lat_geo"]);
         ft2["lon_geo"].set(pointing["lon_geo"]);
         ft2["rad_geo"].set(pointing["rad_geo"]*1e3); // convert to m
         ft2["ra_zenith"].set(pointing["ra_zenith"]);
         ft2["dec_zenith"].set(pointing["dec_zenith"]);
         ft2["b_mcilwain"].set(pointing["B_McIlwain"]);
         ft2["l_mcilwain"].set(pointing["L_McIlwain"]);
         ft2["in_saa"].set(static_cast<bool>(pointing["in_saa"]));
         ft2["ra_scz"].set(pointing["ra_scz"]);
         ft2["dec_scz"].set(pointing["dec_scz"]);
         ft2["ra_scx"].set(pointing["ra_scx"]);
         ft2["dec_scx"].set(pointing["dec_scx"]);
         ft2["livetime"].set(pointing["livetime"]);
      }
      ft2.itor() = ft2.begin();
      double start_time(ft2["start"].get());
      ft2.itor() = ft2.end();
      ft2.prev();
      double stop_time(ft2["stop"].get());
      ft2.setObsTimes(start_time, stop_time);
   } catch (std::exception &eObj) {
      std::cout << eObj.what() << std::endl;
      std::exit(1);
   }
}
