/**
 * @file egret2Ft1.cxx
 * @brief EGRET SMDB to FT1 converter
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>

#include "fitsGen/Ft1File.h"

#include "EgretSmdb.h"

int main(int iargc, char * argv[]) {
   if (iargc != 3) {
      std::cout << "usage: egret2FT1 <smdb file> <output FT1 file>\n";
      std::exit(1);
   }

   EgretSmdb egret(argv[1]);
   fitsGen::Ft1File ft1(argv[2], egret.nrows());

   ft1.header()["MJDREF"].set(EgretSmdb::mjdref());
   ft1.appendField("ARRIVAL_TJD", "D");

   long nrows(0);
   for ( ; egret.itor() != egret.end(); egret.next()) {
      if (egret.tascIn()) {
         ft1["energy"].set(egret["gamma_ray_energy"]);
         ft1["ra"].set(egret["right_ascension"]*180./M_PI);
         ft1["dec"].set(egret["declination"]*180./M_PI);
         ft1["l"].set(egret["galactic_longitude"]*180./M_PI);
         ft1["b"].set(egret["galactic_latitude"]*180./M_PI);
         ft1["zenith_angle"].set(egret["zenith_direction"]*180./M_PI);
         ft1["earth_azimuth_angle"].set(egret["azimuth_direction"]*180./M_PI);
         ft1["time"].set(egret.arrivalTime());
         ft1["arrival_tjd"].set(egret.arrivalTjd());
         ft1["event_class"].set(egret.eventClass());
         ft1.next();
         nrows++;
      }
   }
   ft1.setNumRows(nrows);
}
