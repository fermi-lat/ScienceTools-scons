/**
 * @file makeFT1.cxx
 * @brief Convert merit ntuple to FT1 format using Goodi.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <vector>
#include <valarray>
#include <string>
#include <algorithm>
#include <functional>
#include <sstream>

#include "assert.h"

#include "Goodi/GoodiConstants.h"
#include "Goodi/DataIOServiceFactory.h"
#include "Goodi/DataFactory.h"
#include "Goodi/IDataIOService.h"
#include "Goodi/IData.h"
#include "Goodi/IEventData.h"

//#include "Goodi/../src/LatEventsData.h"    // meh.

#include "rootTuple/RootTuple.h"

int main(int iargc, char * argv[]) {


   std::string rootFile;
   std::string fitsFile("!myLatData.fits");

   if (iargc == 1) 
     {
       std::string fitsGenRoot = ::getenv("FITSGENROOT");
       rootFile = fitsGenRoot + "/data/merit.root";
     } 
   else if (iargc == 2) 
       {
	 if (!strcmp(argv[1],"-h")) {
	   std::cout << "usage: " << argv[0] << ": "
		     << "<root input file>" 
		     << "<fits input file>" << std::endl;
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

// Merit tuple is read to retrieve general and FT1 info
   RootTuple::RootTuple meritTuple(rootFile, "MeritTuple");

   std::vector<std::string> colNames;

   colNames.push_back("elapsed_time");
   colNames.push_back("FT1Energy");
   colNames.push_back("FT1Ra");
   colNames.push_back("FT1Dec");
   colNames.push_back("FT1Theta");
   colNames.push_back("FT1Phi");
   colNames.push_back("FT1ZenithTheta");
   colNames.push_back("FT1EarthAzimuth");
   colNames.push_back("FT1EventId");
   colNames.push_back("FT1ConvPointX");
   colNames.push_back("FT1ConvPointY");
   colNames.push_back("FT1ConvPointZ");
   colNames.push_back("FT1ConvLayer");
   colNames.push_back("IMgoodCalProb");
   colNames.push_back("IMvertexProb");
   colNames.push_back("IMcoreProb");
   colNames.push_back("IMpsfErrPred");
   colNames.push_back("IMgammaProb");
   colNames.push_back("GltWord");
   colNames.push_back("CalEnergySum");
   colNames.push_back("CalTotRLn");
   colNames.push_back("CalMIPDiff");
   colNames.push_back("CalCsIRLn");
   colNames.push_back("CalLRmsRatio");
   colNames.push_back("Tkr1ZDir");
   colNames.push_back("Tkr1FirstLayer");
   colNames.push_back("Tkr1ToTFirst");
   colNames.push_back("Tkr1ToTAve");
   colNames.push_back("TkrNumTracks");
   colNames.push_back("AcdTotalEnergy");
   colNames.push_back("AcdRibbonActDist");
   colNames.push_back("AcdTileCount");
   colNames.push_back("FilterStatus_HI");
   colNames.push_back("EvtEnergySumOpt");
   colNames.push_back("EvtTkrEComptonRatio");
   colNames.push_back("VtxAngle");

   std::string query("");
   int nentries(0);

// Read in all of the columns for the FT1 and Exposure branches.
//   colNames = meritTuple.branchNames();
   meritTuple.readTree(colNames, query, nentries);

   std::cout<<"colName size "<<colNames.size()<<std::endl;
// Goodi setup.

// Create the IOService and Data factories.
   Goodi::DataIOServiceFactory iosvcCreator;
   Goodi::DataFactory dataCreator;

// Set the type of data to be generated and the mission.
   Goodi::DataType datatype = Goodi::Evt;
   Goodi::Mission  mission  = Goodi::Lat;

   Goodi::IEventData *data = 
     dynamic_cast<Goodi::IEventData *>(dataCreator.create(datatype, mission));

   std::vector<double> vect_times = meritTuple("elapsed_time");
   std::stable_sort(vect_times.begin(),vect_times.end());
   std::vector<std::pair<double,double> > gti;
   gti.push_back(std::make_pair(vect_times.front(),vect_times.back()));

   unsigned int nevts = vect_times.size();
   std::cout << "Number of events: " << nevts << std::endl;

// Fill the EventData object with data from the FT1 tree:
   
// ENERGY
   std::vector<double> energy = meritTuple("FT1Energy");
// Convert from MeV to eV, which is what Goodi wants.
   std::transform( energy.begin(), energy.end(), energy.begin(), 
                   std::bind2nd(std::multiplies<double>(), 1e6) );
   data->setEnergy(energy);

// Goodi wants angles in radians.
   std::vector<double> angles(nevts);
// RA
   std::transform( meritTuple("FT1Ra").begin(), 
                   meritTuple("FT1Ra").end(), 
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setRA(angles);
//   data->setRA(meritTuple("ra"));

// DEC
   std::transform( meritTuple("FT1Dec").begin(), 
                   meritTuple("FT1Dec").end(), 
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setDec(angles);
//   data->setDec(meritTuple("dec"));

// Colatitude and azimuthal angle in instrument coordinates.
// THETA
  std::transform( meritTuple("FT1Theta").begin(), 
                   meritTuple("FT1Theta").end(), 
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setTheta(angles);
//   data->setTheta( meritTuple("theta") );

// PHI
   std::transform( meritTuple("FT1Phi").begin(), 
                   meritTuple("FT1Phi").end(), 
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setPhi(angles);
//   data->setPhi(meritTuple("phi"));

// ZENITH_ANGLE
   std::transform( meritTuple("FT1ZenithTheta").begin(), 
                   meritTuple("FT1ZenithTheta").end(),
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setZenithAngle(angles);
//   data->setZenithAngle(meritTuple("zenith_angle"));

// EARTH_AZIMUTH
   std::transform( meritTuple("FT1EarthAzimuth").begin(), 
                   meritTuple("FT1EarthAzimuth").end(),
                   angles.begin(), 
                   std::bind2nd(std::multiplies<double>(), M_PI/180.) );
   data->setAzimuth(angles);
//   data->setAzimuth(meritTuple("earth_azimuth"));

// TIME
   data->setTime(meritTuple("elapsed_time"));

// EVENT_ID
// Using std::copy complains about type conversion; passing
// static_cast<long>() as the unary function to transform does not to
// compile under gcc 2.95.3.  This kludge seems to work without
// complaint.
   std::vector<long> eventId(nevts);
   std::transform( meritTuple("FT1EventId").begin(), 
                   meritTuple("FT1EventId").end(),
                   eventId.begin(), 
                   std::bind2nd(std::multiplies<long>(), 1) );
   data->setEventID(eventId);

// Set the sizes of the valarray data for the multiword columns,
    std::vector< std::valarray<float>  >      convPoint(nevts);
// GEO_OFFSET, BARY_OFFSET, etc.. and fill where possible.
//    std::vector< std::valarray<double> >      geoOffset(nevts);
//    std::vector< std::valarray<double> >     baryOffset(nevts);
//    std::vector< std::valarray<long>   >    acdTilesHit(nevts);
    std::vector< std::valarray<int>    >   calibVersion(nevts);
    for (unsigned int i = 0; i < nevts; i++) {
       convPoint[i].resize(3);
//       geoOffset[i].resize(3);
//       baryOffset[i].resize(3);
//       acdTilesHit[i].resize(3);
       calibVersion[i].resize(3);
    }
   
// CONVERSION_POINT
   std::vector<double>::const_iterator pConvPtX 
      = meritTuple("FT1ConvPointX").begin();
   std::vector<double>::const_iterator pConvPtY 
      = meritTuple("FT1ConvPointY").begin();
   std::vector<double>::const_iterator pConvPtZ 
      = meritTuple("FT1ConvPointZ").begin();
   std::vector< std::valarray<float> >::iterator pConvPoint 
      = convPoint.begin();
   while (pConvPoint != convPoint.end()) {
       (*pConvPoint)[0] = static_cast<float>(*pConvPtX);
       (*pConvPoint)[1] = static_cast<float>(*pConvPtY);
       (*pConvPoint)[2] = static_cast<float>(*pConvPtZ);

       pConvPoint++;
       pConvPtX++;
       pConvPtY++;
       pConvPtZ++;
   }
   data->setConvPoint(convPoint);

// CONVERSION_LAYER
   std::vector<int> convLayer(nevts);
   std::transform( meritTuple("FT1ConvLayer").begin(), 
                   meritTuple("FT1ConvLayer").end(),
                   convLayer.begin(), 
                   std::bind2nd(std::multiplies<int>(), 1) );
   data->setConvLayer( convLayer );

// IM variables
   assert(meritTuple("IMgoodCalProb").size() == nevts);

   std::vector<float> floatVector(nevts);
   std::copy( meritTuple("IMgoodCalProb").begin(),
              meritTuple("IMgoodCalProb").end(),
              floatVector.begin() );
   data->setGoodCalProb( floatVector );

   std::copy( meritTuple("IMvertexProb").begin(),
              meritTuple("IMvertexProb").end(),
              floatVector.begin() );
   data->setVertexProb( floatVector );

   std::copy( meritTuple("IMcoreProb").begin(),
              meritTuple("IMcoreProb").end(),
              floatVector.begin() );
   data->setCoreProb( floatVector );

   std::copy( meritTuple("IMpsfErrPred").begin(),
              meritTuple("IMpsfErrPred").end(),
              floatVector.begin() );
   data->setPsferrpred( floatVector );

   std::copy( meritTuple("IMgammaProb").begin(),
              meritTuple("IMgammaProb").end(),
              floatVector.begin() );

   // ignore IMgammaProb at this point  12/6/2003 RD.
   data->setGammaProb( floatVector );

// // GltLayer (no doubt the same as ConvLayer)
//    std::vector<short> gltLayer(nevts);
//    std::transform( meritTuple("GltLayer").begin(), 
//                    meritTuple("GltLayer").end(),
//                    gltLayer.begin(), 
//                    std::bind2nd(std::multiplies<short>(), 1) );
//     data->setGltLayer( gltLayer );

// CalEnergySum and CalTotRLn
   std::copy( meritTuple("CalEnergySum").begin(),
              meritTuple("CalEnergySum").end(),
              floatVector.begin() );
   data->setCalEnergySum( floatVector );

   std::copy( meritTuple("CalTotRLn").begin(),
              meritTuple("CalTotRLn").end(),
              floatVector.begin() );
   data->setCalToTrln( floatVector );

// Set methods in IEventData that have no corresponding branches in
// the FT1 tree:
//    data->setWeight();
//    data->setMJD();
//    data->setTimeOfDay();
//    data->setEnergyUncert();
//    data->setPulsePhase();
//    data->setGeoTime();
//    data->setBaryTime();
//    data->setQuality();
//    data->setQualParams();
//    data->setReconVersion();
//    data->setMultiplicity();
//    data->setSubsysFlag();
//    data->setStartTime();
//    data->setStopTime();
   
// GTI infos: START and STOP times
   data->setGTI(gti);

   //HEADER KEYS in GTI for DC1
   std::string date_start = "2005-07-18T00:00:00.0000";
   std::string date_stop  = "2005-07-19T00:00:00.0000";
   data->setKey("DATE-OBS", date_start);
   data->setKey("DATE-END", date_stop);
   double duration = gti.front().second - gti.front().first;
   data->setKey("TSTART", gti.front().first);
   data->setKey("TSTOP", gti.front().second);
   data->setKey("ONTIME", duration);
   data->setKey("TELAPSE", duration);

// These also are not implemented in the FT1 tree, but Goodi seems to
// want these columns set.
//   data->setGeoOffset(geoOffset);
//   data->setBaryOffset(baryOffset);
//   data->setAcdTilesHit(acdTilesHit);


   std::vector<double> gltword        = meritTuple("GltWord");
   std::vector<double> imgoodcalprob  = meritTuple("IMgoodCalProb");
   std::vector<double> tkr1zdir      = meritTuple("Tkr1ZDir");
   std::vector<double> calenergysum   = meritTuple("CalEnergySum");
   std::vector<double> calmipdiff     = meritTuple("CalMIPDiff");
   std::vector<double> calcsirln      = meritTuple("CalCsIRLn");
   std::vector<double> caltotrln      = meritTuple("CalTotRLn");
   std::vector<double> callrmsratio   = meritTuple("CalLRmsRatio");
   std::vector<double> tkr1firstlayer = meritTuple("Tkr1FirstLayer");
   std::vector<double> tkrnumtracks   = meritTuple("TkrNumTracks");
   std::vector<double> imcoreprob     = meritTuple("IMcoreProb");
   std::vector<double> imgammaprob     = meritTuple("IMgammaProb");
   std::vector<double> impsfprederr   = meritTuple("IMpsfErrPred");

   std::vector<double> acdtotalenergy = meritTuple("AcdTotalEnergy");
   std::vector<double> acdribbonactdist = meritTuple("AcdRibbonActDist");

   std::vector<double> tkr1totfirst = meritTuple("Tkr1ToTFirst");
   std::vector<double> tkr1totave = meritTuple("Tkr1ToTAve");

   std::vector<double> filterstatus_hi = meritTuple("FilterStatus_HI");

   std::vector<double> evtenergysumopt = meritTuple("EvtEnergySumOpt");
   std::vector<double> evttkrecomptonratio = meritTuple("EvtTkrEComptonRatio");
   std::vector<double> vtxangle       = meritTuple("VtxAngle");
   std::vector<double> acdtilecount    = meritTuple("AcdTileCount");

   for (unsigned int i = 0; i < nevts; i++) {

     (calibVersion[i])[0] = 0.;
     (calibVersion[i])[1] = 0.;
     (calibVersion[i])[2] = 0.;


     bool good_energy_cut   = (imgoodcalprob[i]>0.2); 
     bool zdir_cut          = (tkr1zdir[i]<-0.2);      
     bool no_cal_cut        = (calenergysum[i]<5.0)||(caltotrln[i]<2.0);
     bool thin_cut          = (tkr1firstlayer[i] != 0.0) && (tkr1firstlayer[i]<15.0);

     bool global_cut = good_energy_cut && zdir_cut && !(no_cal_cut);
     
     bool psf_filter =   (imcoreprob[i]>0.2) && (impsfprederr[i]<3.0);

     bool background_cut = (tkrnumtracks[i]>0) && (gltword[i]>3) && (imcoreprob[i]>0.2);
     bool veto = 1;
     if(background_cut){
       if(vtxangle[i]>0.0)
	 {
	   if( evtenergysumopt[i]>3500.0)
	     {
	       veto=0.0;
	     }
	   if( evtenergysumopt[i]>350.0 && evtenergysumopt[i]<3500.0)
	     {
	       veto=0.0;
	     }
 	   if(evtenergysumopt[i]<=350.0) 
	      if(tkr1totfirst[i]>4.5||tkr1totave[i]>3.5||
		 acdtotalenergy[i]>0.25||vtxangle[i]>0.4)
		{
		  veto=1.0;
		}
	      else {
		veto = 0.0;
	      }
	 }
       else
	 {
	   if( evtenergysumopt[i]>3500.0)
	     {
	       veto=0.0;
	     }

 	   if( evtenergysumopt[i]>350.0 && evtenergysumopt[i]<=3500.0)
	       if (tkr1totave[i]>3.0||acdtotalenergy[i]>5.0||
		   evttkrecomptonratio[i]<1.0) {
		 veto = 1.0;
	       }
	       else veto=0.0;
		
	   if(evtenergysumopt[i]<=350.0) 
	     if (tkr1totave[i]>3.0 || acdtilecount[i]>0.0 || 
		 acdribbonactdist[i]>-300.0 ||
		 evttkrecomptonratio[i]<1.05 || filterstatus_hi[i]>3.0) 
	     {  
	       veto=1.0;
	     }
	   else veto = 0.0; 
	   
	 }
     }
     //FOR DC1, now, IMgammaProb directly passes the background cut....
     //     if(!veto)           (calibVersion[i])[0] = 1.;
     (calibVersion[i])[0] = imgammaprob[i];
     if(psf_filter) 	 (calibVersion[i])[1] = 1.;
     if(global_cut) 	 (calibVersion[i])[2] = 1.;
     
   }

   data->setCalibVersion(calibVersion);


// Write the data to a FITS file.
   Goodi::IDataIOService *ioService = iosvcCreator.create();

   data->write(ioService, fitsFile);

   delete ioService;
}




