/**
 * @file makeFT2.cxx
 * @brief Convert Root D2 data from Gleam to FT2 format using Goodi.
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

#include "rootTuple/RootTuple.h"

int main(int iargc, char * argv[]) {

   std::string d2File;
   if (iargc == 1) {
      std::string fitsGenRoot = ::getenv("FITSGENROOT");
      d2File = fitsGenRoot + "/data/pointing_history.root";
   } else if (iargc == 2) {
      if (argv[1] == "-h") {
         std::cout << "usage: " 
                   << argv[0] 
                   << " <D2 root file>" 
                   << std::endl;
         return 0;
      } else {
         d2File = std::string(argv[1]);
      }
   }

   RootTuple::RootTuple exposure(d2File, "Exposure");
   std::vector<std::string> colNames;
   std::string query("");
   int nentries(0);
   colNames = exposure.branchNames();
   exposure.readTree(colNames, query, nentries);

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
   unsigned int npts = exposure.nrows();

// Start and stop times.
   std::vector<double> startTime(npts);
   std::vector<double> stopTime(npts);
   std::copy(exposure("PtTime").begin(), 
             exposure("PtTime").end(), 
             startTime.begin());
   std::copy(exposure("PtTime").begin()+1, 
             exposure("PtTime").end(), 
             stopTime.begin());
   stopTime[npts-1] = startTime[npts-1] 
      + (startTime[npts-1] - startTime[npts-2]);
   scData->setStartTime(startTime);
   scData->setStopTime(stopTime);

//    scData->setGTI(gti);

// Spacecraft position.
   std::vector< std::valarray<float> > scPosition(npts);
   std::vector< std::valarray<float> >::iterator scPosIt = scPosition.begin();
   std::vector<double>::const_iterator posx = exposure("posx").begin();
   std::vector<double>::const_iterator posy = exposure("posy").begin();
   std::vector<double>::const_iterator posz = exposure("posz").begin();
   for ( ; scPosIt != scPosition.end(); scPosIt++) {
      (*scPosIt).resize(3);
      (*scPosIt)[0] = static_cast<float>(*posx++);
      (*scPosIt)[1] = static_cast<float>(*posy++);
      (*scPosIt)[2] = static_cast<float>(*posz++);
   }
   scData->setSCposition(scPosition);

// Ground point longitude and latitude.  Convert to radians.
   std::vector<float> lonGeo(npts);
   std::transform( exposure("PtLon").begin(), 
                   exposure("PtLon").end(),
                   lonGeo.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setLonGeo(lonGeo);
   std::vector<float> latGeo(npts);
   std::transform( exposure("PtLat").begin(), 
                   exposure("PtLat").end(),
                   latGeo.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setLatGeo(latGeo);

// Assume this is the same as altitude.  Convert from km to m.
   std::vector<double> radGeo(npts);
   std::transform( exposure("PtAlt").begin(), 
                   exposure("PtAlt").end(),
                   radGeo.begin(), 
                   std::bind2nd(std::multiplies<double>(), 1e3) );
   scData->setRadGeo(radGeo);

// // Zenith in Celestial coordinates.
//    std::vector<float> raZenith(npts);
//    std::copy(exposure("").begin(), exposure("").begin(), raZenith.begin());
//    scData->setRAZenith(raZenith);
//    std::vector<float> decZenith(npts);
//    std::copy(exposure("").begin(), exposure("").begin(), decZenith.begin());
//    scData->setDECZenith(decZenith);

// // Geomagnetic quantities.
//    std::vector<float> bMcIlwain(npts);
//    std::copy(exposure("").begin(), exposure("").begin(), bMcIlwain.begin());
//    scData->setBMcIlwain(bMcIlwain);
//    std::vector<float> lMcIlwain(npts);
//    std::copy(exposure("").begin(), exposure("").begin(), lMcIlwain.begin());
//    scData->setLMcIlwain(lMcIlwain);

// Spacecraft z-axis in Celestial coordinates.
   std::vector<float> raSCZ(npts);
   std::transform( exposure("PtRaz").begin(), 
                   exposure("PtRaz").end(),
                   raSCZ.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setRAscz(raSCZ);

   std::vector<float> decSCZ(npts);
   std::transform( exposure("PtDecz").begin(), 
                   exposure("PtDecz").end(),
                   decSCZ.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setDECscz(decSCZ);

// Spacecraft x-axis in Celestial coordinates.
   std::vector<float> raSCX(npts);
   std::transform( exposure("PtRax").begin(), 
                   exposure("PtRax").end(),
                   raSCX.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setRAscx(raSCX);

   std::vector<float> decSCX(npts);
   std::transform( exposure("PtDecx").begin(), 
                   exposure("PtDecx").end(),
                   decSCX.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setDECscx(decSCX);

//    std::vector< std::pair<double, double> > gti(npts);
//    std::vector<int> latMode(npts);
//    scData->setLatMode(latMode);
//    std::vector<double> livetime(npts);
//    scData->setLivetime(livetime);
//    std::vector<double> deadtime(npts);
//    scData->setDeadtime(deadtime);

// Goodi I/O service object.
   Goodi::IDataIOService *goodiIoService = iosvcCreator.create();

   std::string outputFile = "!myScData.fits";
   scData->write(goodiIoService, outputFile);

   delete goodiIoService;

}
