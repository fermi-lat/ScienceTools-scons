/**
 * @file makeFT1.cxx
 * @brief Convert merit ntuple to FT1 format.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <stdexcept>
#include <string>

#include "astro/JulianDate.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/Header.h"

void getEventFlags(tip::Table::Record & merit, short & isGamma, 
                   short & goodPsf, short & goodEnergy);

void writeDateKeywords(tip::Table * table, double start_time, 
                       double stop_time);

astro::JulianDate currentTime();

int main(int iargc, char * argv[]) {

   try {
      std::string rootFile;
      std::string fitsFile("myLatData.fits");

      if (iargc == 1) {
         std::string fitsGenRoot = std::getenv("FITSGENROOT");
         rootFile = fitsGenRoot + "/data/merit.root";
      } else if (iargc == 2) {
         if (!std::strcmp(argv[1], "-h")) {
            std::cout << "usage: " << argv[0] << ": "
                      << "<root input file> " 
                      << "<fits input file> " << std::endl;
            return 0;
         } else {
            rootFile = std::string(argv[1]);
         }
      } else if (iargc == 3) {
         rootFile = std::string(argv[1]);
         fitsFile = std::string(argv[2]);     
      }

      tip::Table * meritTable = 
         tip::IFileSvc::instance().editTable(rootFile, "MeritTuple");
      long nrows = meritTable->getNumRecords();
      tip::Table::Iterator merit_iter = meritTable->begin();
      tip::Table::Record & merit = *merit_iter;

// Create the FT1 file.
      std::string ft1Template = std::getenv("TIPROOT") 
         + std::string("/data/ft1.tpl");
      tip::IFileSvc::instance().createFile(fitsFile, ft1Template);
      tip::Table * eventTable =
         tip::IFileSvc::instance().editTable(fitsFile, "EVENTS");
      eventTable->setNumRecords(nrows);
      tip::Table::Iterator ft1_iter = eventTable->begin();
      tip::Table::Record & ft1 = *ft1_iter;
   
      for ( ; merit_iter != meritTable->end(); ++merit_iter, ++ft1_iter) {
         ft1["energy"].set(merit["FT1Energy"].get());
         ft1["ra"].set(merit["FT1Ra"].get());
         ft1["dec"].set(merit["FT1Dec"].get());
         ft1["theta"].set(merit["FT1Theta"].get());
         ft1["phi"].set(merit["FT1Phi"].get());
         ft1["zenith_angle"].set(merit["FT1ZenithTheta"].get());
         ft1["earth_azimuth_angle"].set(merit["FT1EarthAzimuth"].get());
         ft1["time"].set(merit["elapsed_time"].get());
         ft1["event_id"].set(static_cast<int>(merit["FT1EventId"].get()));
         ft1["conversion_layer"].
            set(static_cast<short>(merit["FT1ConvLayer"].get()));
         ft1["imgoodcalprob"].set(merit["IMgoodCalProb"].get());
         ft1["imvertexprob"].set(merit["IMvertexProb"].get());
         ft1["imcoreprob"].set(merit["IMcoreProb"].get());
         ft1["impsferrpred"].set(merit["IMpsfErrPred"].get());
         ft1["calenergysum"].set(merit["CalEnergySum"].get());
         ft1["caltotrln"].set(merit["CalTotRLn"].get());
         ft1["imgammaprob"].set(merit["IMgammaProb"].get());

         short isGamma, goodPsf, goodEnergy;
         getEventFlags(merit, isGamma, goodPsf, goodEnergy);
      }

      merit_iter = meritTable->begin();
      double start_time = merit["elapsed_time"].get();
      merit_iter = meritTable->end();
      --merit_iter;
      double stop_time = merit["elapsed_time"].get();
      tip::Table * gtiTable = 
         tip::IFileSvc::instance().editTable(fitsFile, "GTI");
      gtiTable->setNumRecords(1);
      tip::Table::Iterator it = gtiTable->begin();
      (*it)["start"].set(start_time);
      (*it)["stop"].set(stop_time);

      writeDateKeywords(eventTable, start_time, stop_time);
      writeDateKeywords(gtiTable, start_time, stop_time);

      delete meritTable;
      delete gtiTable;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      return 1;
   }
}

void getEventFlags(tip::Table::Record & merit, short & isGamma, 
                   short & goodPsf, short & goodEnergy) {
   
   isGamma = 0;
   goodPsf = 0;
   goodEnergy = 0;

   bool good_energy_cut = merit["IMgoodCalProb"].get() > 0.2;
   bool zdir_cut = merit["Tkr1ZDir"].get() < -0.2;
   bool no_cal_cut = (merit["CalEnergySum"].get() < 5. 
                      || merit["CalTotRLn"].get() < 2.);
   bool thin_cut = (merit["Tkr1FirstLayer"].get() != 0 
                    && merit["Tkr1FirstLayer"].get() < 15);

   bool global_cut = good_energy_cut && zdir_cut && !no_cal_cut;

   bool psf_filter = (merit["IMcoreProb"].get() > 0.2 
                      && merit["IMpsfErrPred"].get() < 3.);

//    bool background_cut = (merit["TkrNumTracks"].get() > 0 
//                           && merit["GltWord"].get() > 3
//                           && merit["IMcoreProb"].get() > 0.2);
//    bool veto(true);

//    if (background_cut) {
//       if (merit["VtxAngle"].get() > 0) {
//          if (merit["EvtEnergySumOpt"].get() > 3500.) {
//             veto = false;
//          } else if (merit["EvtEnergySumOpt"].get() > 350.) {
//             veto = false;
//          } else {
//             if (merit["Tkr1TotFirst"].get() > 4.5 
//                 || merit["Tkr1TotAve"].get() > 3.5
//                 || merit["AcdTotalEnergy"] > 0.25
//                 || merit["VtxAngle"] > 0.4) {
//                veto = true;
//             } else {
//                veto = false;
//             }
//          }
//       } else {
//          if (merit["EvtEnergySumOpt"].get() > 3500.) {
//             veto = false;
//          } else if (merit["EvtEnergySumOpt"].get() > 350.) {
//             if (merit["Tkr1TotAve"] > 3. 
//                 || merit["AcdTotalEnergy"] > 5.
//                 || merit["EvtTkrEComptonRatio"] < 1) {
//                veto = true;
//             } else {
//                veto = false;
//             }
//          } else {
//             if (merit["Tkr1TotAve"] > 3. 
//                 || merit["AcdTileCount"] > 0
//                 || merit["AcdRibbonActDist"] > -300.
//                 || merit["EvtTkrEComptonRatio"] < 1.05
//                 || merit["FilterStatus_HI"] > 3.) {
//                veto = true;
//             } else {
//                veto = false;
//             }
//          } // if EvtEnergySumOpt > 3500
//       } // if VtxAngle > 0
//    } // if background_cut

// For DC1, IMgammaProb directly passes the background cut.
   if (merit["IMgammaProb"].get() > 0.5) isGamma = 1;
   if (psf_filter) goodPsf = 1;
   if (global_cut) goodEnergy = 1;
}

void writeDateKeywords(tip::Table * table, double start_time, 
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

astro::JulianDate currentTime() {
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
