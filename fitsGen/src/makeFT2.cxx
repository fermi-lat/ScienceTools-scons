/**
 * @file makeFT2.cxx
 * @brief Convert ascii D2 data from Gleam to FT2 format using Goodi.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>

#include <vector>
#include <valarray>
#include <string>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>

#include "Goodi/GoodiConstants.h"
#include "Goodi/DataIOServiceFactory.h"
#include "Goodi/DataFactory.h"
#include "Goodi/IDataIOService.h"
#include "Goodi/IData.h"
#include "Goodi/ISpacecraftData.h"

namespace {

// This routine breaks down a string into its components based on the
// characters appearing in the delimiters string.  A vector
// of strings is returned.
   std::vector<std::string> string_split(std::string input, 
                                         const std::string &delimiters) {
      std::vector<std::string> components;
      
      std::string::size_type j;
      while ( (j = input.find_first_of(delimiters)) != std::string::npos ) { 
         if (j != 0) components.push_back(input.substr(0, j));
         input = input.substr(j+1);
      }
      components.push_back(input);
      return components;
   }
} // unnamed namespace

int main(int iargc, char * argv[]) {

   std::string d2File;
   if (iargc == 1) {
      std::string fitsGenRoot = ::getenv("FITSGENROOT");
      d2File = fitsGenRoot + "/data/asciiD2.dat";
   } else if (iargc == 2) {
      if (argv[1] == "-h") {
         std::cout << "usage: " 
                   << argv[0] 
                   << " <D2 file>" 
                   << std::endl;
         return 0;
      } else {
         d2File = std::string(argv[1]);
      }
   }

   std::vector<double> time;
   std::vector< std::valarray<float> > scPosition;
   std::vector<float> latGeo;
   std::vector<float> lonGeo;
   std::vector<double> radGeo;
   std::vector<float> raZenith;
   std::vector<float> decZenith;
//    std::vector<float> bMcIlwain;
//    std::vector<float> lMcIlwain;
   std::vector<float> raSCZ;
   std::vector<float> decSCZ;
   std::vector<float> raSCX;
   std::vector<float> decSCX;
//    std::vector< std::pair<double, double> > gti;
//    std::vector<int> latMode;
//    std::vector<double> livetime;
//    std::vector<double> deadtime;

   ifstream d2(d2File.c_str());
   std::string line;
   std::vector<std::string> dataFields;
   std::valarray<float> pos(3);
   while (std::getline(d2, line, '\n')) {
      dataFields = ::string_split(line, "\t");
      time.push_back(atof(dataFields[0].c_str()));
      pos[0] = atof(dataFields[1].c_str());
      pos[1] = atof(dataFields[2].c_str());
      pos[2] = atof(dataFields[3].c_str());
      scPosition.push_back(pos);
      raSCZ.push_back(atof(dataFields[4].c_str()));
      decSCZ.push_back(atof(dataFields[5].c_str()));
      raSCX.push_back(atof(dataFields[6].c_str()));
      decSCX.push_back(atof(dataFields[7].c_str()));
      raZenith.push_back(atof(dataFields[8].c_str()));
      decZenith.push_back(atof(dataFields[9].c_str()));
      lonGeo.push_back(atof(dataFields[10].c_str()));
      latGeo.push_back(atof(dataFields[11].c_str()));
      radGeo.push_back(atof(dataFields[12].c_str()));
      if (time.size() == 1) {
         for (unsigned int i = 0; i < dataFields.size(); i++) {
            std::cout << dataFields[i] << std::endl;
         }
         std::cout << raSCZ[0] << std::endl;
         std::cout << decSCZ[0] << std::endl;
         std::cout << raSCX[0] << std::endl;
         std::cout << decSCX[0] << std::endl;
         std::cout << raZenith[0] << std::endl;
         std::cout << decZenith[0] << std::endl;
         std::cout << lonGeo[0] << std::endl;
         std::cout << latGeo[0] << std::endl;
         std::cout << radGeo[0] << std::endl;
      }
   }
   unsigned int npts = time.size();
   std::vector<double> startTime(npts);
   std::vector<double> stopTime(npts);
   std::copy(time.begin(), time.end(), startTime.begin());
   std::copy(time.begin()+1, time.end(), stopTime.begin());
   stopTime[npts-1] = startTime[npts-1] 
      + (startTime[npts-1] - startTime[npts-2]);

// Goodi setup.

// Create the IOService and Data factories.
   Goodi::DataIOServiceFactory iosvcCreator;
   Goodi::DataFactory dataCreator;

// Set the type of data to be generated and the mission.
   Goodi::DataType datatype = Goodi::Spacecraft;
   Goodi::Mission  mission  = Goodi::Lat;

// Create the SpacecraftData object.
   Goodi::ISpacecraftData *scData = dynamic_cast<Goodi::ISpacecraftData *>
      (dataCreator.create(datatype, mission));

// Read the columns into Goodi.
   scData->setStartTime(startTime);
   scData->setStopTime(stopTime);
//    scData->setGTI(gti);
//    scData->setSCposition(scPosition);
   scData->setLatGeo(latGeo);
   scData->setLonGeo(lonGeo);
   scData->setRadGeo(radGeo);
   scData->setRAZenith(raZenith);
   scData->setDECZenith(decZenith);
//    scData->setBMcIlwain(bMcIlwain);
//    scData->setLMcIlwain(lMcIlwain);
   scData->setRAscz(raSCZ);
   scData->setDECscz(decSCZ);
   scData->setRAscx(raSCX);
   scData->setDECscx(decSCX);
//    scData->setLatMode(latMode);
//    scData->setLivetime(livetime);
//    scData->setDeadtime(deadtime);

// Goodi I/O service object.
   Goodi::IDataIOService *goodiIoService = iosvcCreator.create();

   std::string outputFile = "!myScData.fits";
   scData->write(goodiIoService, outputFile);

   delete goodiIoService;

}
