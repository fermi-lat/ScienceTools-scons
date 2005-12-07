/**
 * @file makeFT1.cxx
 * @brief Convert merit ntuple to FT1 format.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/Header.h"

#include "astro/SkyDir.h"

#include "fitsGen/Util.h"

using namespace fitsGen;

int main(int iargc, char * argv[]) {
   try {
      std::string rootFile("merit.root");
      std::string fitsFile("myLatData.fits");
      
      Util::getFileNames(iargc, argv, rootFile, fitsFile);
      if (iargc == 1) {
         std::cout << "input merit file: " << rootFile << std::endl;
         std::cout << "output FT1 file: " << fitsFile << std::endl;
      }

      tip::Table * meritTable = 
         tip::IFileSvc::instance().editTable(rootFile, "MeritTuple");
      long nrows = meritTable->getNumRecords();
      std::cout << "number of rows in MeritTuple: " << nrows << std::endl;
      tip::Table::Iterator merit_iter = meritTable->begin();
      tip::Table::Record & merit = *merit_iter;

// Create the FT1 file.
      std::string ft1Template = std::getenv("FITSGENROOT") 
         + std::string("/data/ft1.tpl");
      tip::IFileSvc::instance().createFile(fitsFile, ft1Template);
      tip::Table * eventTable =
         tip::IFileSvc::instance().editTable(fitsFile, "EVENTS");
      eventTable->setNumRecords(nrows);
      tip::Table::Iterator ft1_iter = eventTable->begin();
      tip::Table::Record & ft1 = *ft1_iter;
   
      int ncount(0);
      for ( ; merit_iter != meritTable->end(); ++merit_iter, ++ft1_iter) {
         ft1["energy"].set(merit["FT1Energy"].get());
         double ra = merit["FT1Ra"].get();
         double dec = merit["FT1Dec"].get();
         ft1["ra"].set(ra);
         ft1["dec"].set(dec);
         astro::SkyDir dir(ra, dec);
         ft1["l"].set(dir.l());
         ft1["b"].set(dir.b());
         ft1["theta"].set(merit["FT1Theta"].get());
         ft1["phi"].set(merit["FT1Phi"].get());
         ft1["zenith_angle"].set(merit["FT1ZenithTheta"].get());
         ft1["earth_azimuth_angle"].set(merit["FT1EarthAzimuth"].get());
         ft1["time"].set(merit["EvtElapsedTime"].get());
         ft1["event_id"].set(static_cast<int>(merit["FT1EventId"].get()));
         if (17 - merit["Tkr1FirstLayer"].get() < 11.5) { // Front converting
            ft1["event_class"].set(0); 
         } else { // Back converting
            ft1["event_class"].set(1);
         }
//          ft1["conversion_layer"].
// 	   set(static_cast<short>(15-(merit["Tkr1FirstLayer"].get())));
//          ft1["imgoodcalprob"].set(merit["CTgoodCal"].get());
//          ft1["imvertexprob"].set(merit["CTvertex"].get());
//          ft1["imcoreprob"].set(merit["CTgoodPsf"].get());
//          ft1["impsferrpred"].set(merit["CTgoodPsf"].get());
//          ft1["calenergysum"].set(merit["CalEnergyRaw"].get());
//          ft1["caltotrln"].set(merit["CalTotRLn"].get());
//          ft1["imgammaprob"].set(merit["CTgamma"].get());

//          short isGamma, goodPsf, goodEnergy;
//          getEventFlags(merit, isGamma, goodPsf, goodEnergy);
//          tip::Table::Vector<short> calibVersion = ft1["calib_version"];
//          calibVersion[0] = isGamma;
//          calibVersion[1] = goodPsf;
//          calibVersion[2] = goodEnergy;

//          tip::Table::Vector<double> convPoint = ft1["conversion_point"];
//          convPoint[0] = merit["FT1ConvPointX"].get();
//          convPoint[1] = merit["FT1ConvPointY"].get();
//          convPoint[2] = merit["FT1ConvPointZ"].get();
         ncount++;
      }
      std::cout << "number of rows processed: " << ncount << std::endl;

      merit_iter = meritTable->begin();
      double start_time = merit["EvtElapsedTime"].get();
      merit_iter = meritTable->end();
      --merit_iter;
      double stop_time = merit["EvtElapsedTime"].get();
      tip::Table * gtiTable = 
         tip::IFileSvc::instance().editTable(fitsFile, "GTI");
      gtiTable->setNumRecords(1);
      tip::Table::Iterator it = gtiTable->begin();
      (*it)["start"].set(start_time);
      (*it)["stop"].set(stop_time);

      Util::writeDateKeywords(eventTable, start_time, stop_time);
      Util::writeDateKeywords(gtiTable, start_time, stop_time);

      delete meritTable;
      delete gtiTable;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      return 1;
   }
}

