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

   std::string rootFile;
   std::string fitsFile("!myScData.fits");

   if (iargc == 1) 
     {
       std::string fitsGenRoot = ::getenv("FITSGENROOT");
       rootFile = fitsGenRoot + "/data/merit.root";
     } 
   else if (iargc == 2) 
       {
	 if (!strcmp(argv[1],"-h")) {
	   std::cout << "usage: " << argv[0] << ": "
		     << "<D2 input root file>" 
		     << "<D2 output fits file>" << std::endl;
	   return 0;
	 } else {
	     rootFile = std::string(argv[1]);
	 }
       } 
   else if (iargc == 3) 
	 {
	   rootFile   = std::string(argv[1]);
	   fitsFile   = std::string(argv[2]);     
	 }

   RootTuple::RootTuple exposure(rootFile, "Exposure");
   
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

   // Set some general header entries:
   std::string date_start = "2005-07-18T00:00:00.0000";
   std::string date_stop  = "2005-07-19T00:00:00.0000";
   scData->setKey("DATE-OBS", date_start);
   scData->setKey("DATE-END", date_stop);


// Read the columns into Goodi.
   unsigned int npts = exposure.nrows();

   std::cout << "Number of events: " << npts << std::endl;

// Start and stop times.
   std::vector<double> startTime(npts);
   std::vector<double> stopTime(npts);
   std::copy(exposure("elapsed_time").begin(), 
             exposure("elapsed_time").end(), 
             startTime.begin());
   std::copy(exposure("elapsed_time").begin()+1, 
             exposure("elapsed_time").end(), 
             stopTime.begin());
   stopTime[npts-1] = startTime[npts-1] 
      + (startTime[npts-1] - startTime[npts-2]);

   std::vector<std::pair<double,double> > gti;
   for(int i=0;i<npts;i++)
     {
       gti.push_back(std::make_pair<double,double>(startTime[i],stopTime[i]));
     }
       scData->setGTI(gti);
       //   scData->setStartTime(startTime);
       //   scData->setStopTime(stopTime);

   std::vector<double> vect_times = exposure("elapsed_time");
   std::stable_sort(vect_times.begin(),vect_times.end());
   //   gti.push_back(std::make_pair(vect_times.front(),vect_times.back()));
   //   scData->setGTI(gti);

   std::cout<<vect_times.front()<<" "<<vect_times.back()<<std::endl;
   scData->setKey("TSTART", vect_times.front());
   scData->setKey("TSTOP", vect_times.back());

// Spacecraft position.
   std::vector< std::valarray<float> > scPosition(npts);
   std::vector< std::valarray<float> >::iterator scPosIt = scPosition.begin();
   std::vector<double>::const_iterator posx = exposure("PtPosx").begin();
   std::vector<double>::const_iterator posy = exposure("PtPosy").begin();
   std::vector<double>::const_iterator posz = exposure("PtPosz").begin();
   for ( ; scPosIt != scPosition.end(); scPosIt++) {
      (*scPosIt).resize(3);
      (*scPosIt)[0] = static_cast<float>(*posx++);
      (*scPosIt)[1] = static_cast<float>(*posy++);
      (*scPosIt)[2] = static_cast<float>(*posz++);
   }
   scData->setSCposition(scPosition);

// Ground point longitude and latitude.  Convert to radians.
   std::vector<float> lonGeo(npts);
   std::transform( exposure("lon").begin(), 
                   exposure("lon").end(),
                   lonGeo.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setLonGeo(lonGeo);
   std::vector<float> latGeo(npts);
   std::transform( exposure("lat").begin(), 
                   exposure("lat").end(),
                   latGeo.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setLatGeo(latGeo);

// Assume this is the same as altitude.  Convert from km to m.
   std::vector<double> radGeo(npts);
   std::transform( exposure("alt").begin(), 
                   exposure("alt").end(),
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
   std::transform( exposure("raz").begin(), 
                   exposure("raz").end(),
                   raSCZ.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setRAscz(raSCZ);

   std::vector<float> decSCZ(npts);
   std::transform( exposure("decz").begin(), 
                   exposure("decz").end(),
                   decSCZ.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setDECscz(decSCZ);

// Spacecraft x-axis in Celestial coordinates.
   std::vector<float> raSCX(npts);
   std::transform( exposure("rax").begin(), 
                   exposure("rax").end(),
                   raSCX.begin(), 
                   std::bind2nd(std::multiplies<float>(), M_PI/180.) );
   scData->setRAscx(raSCX);

   std::vector<float> decSCX(npts);
   std::transform( exposure("decx").begin(), 
                   exposure("decx").end(),
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

   scData->write(goodiIoService, fitsFile);

   delete goodiIoService;

}
