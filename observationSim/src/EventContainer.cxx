/**
 * @file EventContainer.cxx
 * @brief Implementation for the class that keeps track of events and
 * when they get written to a FITS file.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <sstream>
#include <algorithm>
#include <numeric>

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"

#include "Goodi/GoodiConstants.h"
#include "Goodi/DataIOServiceFactory.h"
#include "Goodi/DataFactory.h"
#include "Goodi/IDataIOService.h"
#include "Goodi/IData.h"
#include "Goodi/IEventData.h"

#include "latResponse/IAeff.h"
#include "latResponse/IPsf.h"
#include "latResponse/IEdisp.h"
#include "latResponse/Irfs.h"

#include "latResponse/../src/Glast25.h"

#include "observationSim/Spacecraft.h"
#include "observationSim/../src/LatSc.h"
#include "observationSim/EventContainer.h"

namespace {
   double my_acos(double mu) {
      if (mu > 1) {
         return 0;
      } else if (mu < -1) {
         return M_PI;
      } else {
         return acos(mu);
      }
   }

   latResponse::Irfs* drawRespPtr(std::vector<latResponse::Irfs*> &respPtrs,
                                  double area, double energy, 
                                  astro::SkyDir &sourceDir,
                                  astro::SkyDir &zAxis,
                                  astro::SkyDir &xAxis) {
   
// Build a vector of effective area accumulated over the vector
// of response object pointers.
//
// First, fill a vector with the individual values.
      std::vector<double> effAreas(respPtrs.size());
      std::vector<double>::iterator eaIt = effAreas.begin();
      std::vector<latResponse::Irfs *>::iterator respIt = respPtrs.begin();
      while (eaIt != effAreas.end() && respIt != respPtrs.end()) {
         *eaIt = (*respIt)->aeff()->value(energy, sourceDir, zAxis, xAxis);
         eaIt++;
         respIt++;
      }

// Compute the cumulative distribution.
      std::partial_sum(effAreas.begin(), effAreas.end(), effAreas.begin());

// The total effective area.
      double effAreaTot = *(effAreas.end() - 1);

// Generate a random deviate from the interval [0, area) to ascertain
// which response object to use.
      double xi = RandFlat::shoot()*area;

      if (xi < effAreaTot) {
// Success. Find the appropriate response functions.
         eaIt = std::lower_bound(effAreas.begin(), effAreas.end(), xi);
         int indx = eaIt - effAreas.begin();
         return respPtrs[indx];
      } else {
// Do not accept this event.
         return 0;
      }
   }

} // unnamed namespace

namespace observationSim {

EventContainer::~EventContainer() {
   if (m_events.size() > 0) writeEvents();
   if (m_useGoodi) {
      delete m_goodiEventData;
   }
}

void EventContainer::init() {
   m_events.clear();

   if (m_useGoodi) {
      Goodi::DataFactory dataCreator;

// Set the type of data to be generated and the mission.
      Goodi::DataType datatype = Goodi::Evt;
      Goodi::Mission mission = Goodi::Lat;

// Create the EventData object.
      m_goodiEventData = dynamic_cast<Goodi::IEventData *>
         (dataCreator.create(datatype, mission));
   }
}

int EventContainer::addEvent(EventSource *event, 
                             std::vector<latResponse::Irfs *> &respPtrs, 
                             Spacecraft *spacecraft,
                             bool flush, bool alwaysAccept) {
   
   std::string particleType = event->particleName();
   double time = event->time();
   double energy = event->energy();
   Hep3Vector launchDir = event->launchDir();

   LatSc latSpacecraft;
   HepRotation rotMatrix = latSpacecraft.InstrumentToCelestial(time);
   astro::SkyDir sourceDir(rotMatrix(-launchDir), astro::SkyDir::CELESTIAL);

   astro::SkyDir zAxis = spacecraft->zAxis(time);
   astro::SkyDir xAxis = spacecraft->xAxis(time);

   if (alwaysAccept) {
      m_events.push_back( Event(time, energy, 
                                sourceDir, sourceDir, zAxis, xAxis,
                                ScZenith(time), 0) );
      if (flush || m_events.size() >= m_maxNumEvents) writeEvents();
      return 1;
   }

   latResponse::Irfs *respPtr;

// Apply the acceptance criteria.
   if ( RandFlat::shoot() < m_prob
        && (respPtr = ::drawRespPtr(respPtrs, event->totalArea()*1e4, 
                                    energy, sourceDir, zAxis, xAxis))
        && !spacecraft->inSaa(time) ) {

      astro::SkyDir appDir 
         = respPtr->psf()->appDir(energy, sourceDir, zAxis, xAxis);
      double appEnergy 
         = respPtr->edisp()->appEnergy(energy, sourceDir, zAxis, xAxis);
                                                        
      m_events.push_back( Event(time, appEnergy, 
                                appDir, sourceDir, zAxis, xAxis,
                                ScZenith(time), respPtr->irfID()) );
//      std::cout << "adding an event: " << m_events.size() << std::endl;
      if (flush || m_events.size() >= m_maxNumEvents) writeEvents();
      return 1;
   }
   if (flush) writeEvents();
   return 0;
}

astro::SkyDir EventContainer::ScZenith(double time) {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   double lon_zenith = gps->RAZenith()*M_PI/180.;
   double lat_zenith = gps->DECZenith()*M_PI/180.;
   return astro::SkyDir(Hep3Vector(cos(lat_zenith)*cos(lon_zenith),
                                   cos(lat_zenith)*sin(lon_zenith),
                                   sin(lat_zenith)),
                        astro::SkyDir::CELESTIAL);
}

void EventContainer::writeEvents() {

   if (m_useGoodi) {
      unsigned int npts = m_events.size();
      std::vector<double> time(npts);
      std::vector<double> energy(npts);
      std::vector<double> ra(npts);
      std::vector<double> dec(npts);
      std::vector<double> theta(npts);
      std::vector<double> phi(npts);
      std::vector<double> zenithAngle(npts);
      std::vector<int> convLayer(npts);

      std::vector<Event>::iterator evtIt = m_events.begin();
      for (int i = 0; evtIt != m_events.end(); evtIt++, i++) {
         time[i] = evtIt->time();
// Goodi wants energies in ergs.
         energy[i] = evtIt->energy()*1e6;
// Goodi wants angles in radians.
         ra[i] = evtIt->appDir().ra()*M_PI/180.;
         dec[i] = evtIt->appDir().dec()*M_PI/180.;
         theta[i] = evtIt->appDir().difference(evtIt->zAxis());
         Hep3Vector yAxis = evtIt->zAxis().dir().cross(evtIt->xAxis().dir());
         phi[i] = atan2( evtIt->appDir().dir().dot(yAxis),
                         evtIt->appDir().dir().dot(evtIt->xAxis().dir()) );
         zenithAngle[i] = evtIt->zenith().difference(evtIt->appDir());
         if (evtIt->eventType() == 0) { // Front
            convLayer[i] = 0;
         } else if (evtIt->eventType() == 1) { // Back
            convLayer[i] = 15;
         } else { // pick at random
            convLayer[i] = static_cast<int>(RandFlat::shoot()*16);
         }
      }
      m_goodiEventData->setTime(time);
      m_goodiEventData->setEnergy(energy);
      m_goodiEventData->setRA(ra);
      m_goodiEventData->setDec(dec);
      m_goodiEventData->setTheta(theta);
      m_goodiEventData->setPhi(phi);
      m_goodiEventData->setZenithAngle(zenithAngle);
      m_goodiEventData->setConvLayer(convLayer);

// Set the sizes of the valarray data for the multiword columns,
// GEO_OFFSET, BARY_OFFSET, etc..
      std::vector< std::valarray<double> > geoOffset(npts);
      std::vector< std::valarray<double> > baryOffset(npts);
      std::vector< std::valarray<float> > convPoint(npts);
      std::vector< std::valarray<long> > acdTilesHit(npts);
      std::vector< std::valarray<int> > calibVersion(npts);
      for (unsigned int i = 0; i < npts; i++) {
         geoOffset[i].resize(3);
         baryOffset[i].resize(3);
         convPoint[i].resize(3);
         acdTilesHit[i].resize(3);
         calibVersion[i].resize(3);
      }

      m_goodiEventData->setGeoOffset(geoOffset);
      m_goodiEventData->setBaryOffset(baryOffset);
      m_goodiEventData->setConvPoint(convPoint);
      m_goodiEventData->setAcdTilesHit(acdTilesHit);
      m_goodiEventData->setCalibVersion(calibVersion);

      Goodi::DataIOServiceFactory iosvcCreator;
      Goodi::IDataIOService *goodiIoService = iosvcCreator.create();

      std::string outputFile = "!" + outputFileName();
      m_goodiEventData->write(goodiIoService, outputFile);
      delete goodiIoService;
   } else { // Use the old A1 format.
      makeFitsTable();
      std::vector<std::vector<double> > data(14);
// pre-allocate the memory for each vector
      for (std::vector<std::vector<double> >::iterator vec_it = data.begin();
           vec_it != data.end(); vec_it++)
         vec_it->reserve(m_events.size());
      for (std::vector<Event>::const_iterator it = m_events.begin();
           it != m_events.end(); it++) {
         data[0].push_back(it->appDir().ra());
         data[1].push_back(it->appDir().dec());
         data[2].push_back(it->appDir().l());
         data[3].push_back(it->appDir().b());
         data[4].push_back(it->energy());
         data[5].push_back(it->time());
         data[6].push_back(it->xAxis().dir().x());
         data[7].push_back(it->xAxis().dir().y());
         data[8].push_back(it->xAxis().dir().z());
         data[9].push_back(it->zAxis().dir().x());
         data[10].push_back(it->zAxis().dir().y());
         data[11].push_back(it->zAxis().dir().z());
         data[12].push_back(
            it->zenith().dir().angle(it->appDir().dir())*180./M_PI);
         data[13].push_back(static_cast<double>(it->eventType()));
      }
      m_eventTable->writeTableData(data);

// Delete the old table.
      delete m_eventTable;      
   }

// Flush the Event buffer...
   m_events.clear();

// and update the m_fileNum index.
   m_fileNum++;
}

void EventContainer::makeFitsTable() {
// Prepare the column name, format, and unit specifiers that are
// required by the FITS binary table format.  This implementation 
// specializes to the (old) A1 format.

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

   colName.push_back("RA"); fmt.push_back("1E"); unit.push_back("deg");
   colName.push_back("DEC"); fmt.push_back("1E"); unit.push_back("deg");
   colName.push_back("GLON"); fmt.push_back("1E"); unit.push_back("deg");
   colName.push_back("GLAT"); fmt.push_back("1E"); unit.push_back("deg");
   colName.push_back("energy"); fmt.push_back("1E"); unit.push_back("MeV");

   colName.push_back("time"); fmt.push_back("1D"); unit.push_back("s");
   colName.push_back("SC_x0");fmt.push_back("1E");unit.push_back("dir cos");
   colName.push_back("SC_x1");fmt.push_back("1E");unit.push_back("dir cos");
   colName.push_back("SC_x2");fmt.push_back("1E");unit.push_back("dir cos");
   colName.push_back("SC_x");fmt.push_back("1E");unit.push_back("dir cos");

   colName.push_back("SC_y");fmt.push_back("1E");unit.push_back("dir cos");
   colName.push_back("SC_z");fmt.push_back("1E");unit.push_back("dir cos");
   colName.push_back("zenith_angle");fmt.push_back("1E");unit.push_back("deg");
   colName.push_back("event_type");fmt.push_back("1I");unit.push_back("int");

   std::string outputFile = outputFileName();
   m_eventTable = new FitsTable(outputFile, "LAT_event_summary", 
                                colName, fmt, unit);
}

std::string EventContainer::outputFileName() const {
   std::ostringstream outputfile;
   outputfile << m_filename;
   if (m_fileNum < 10) {
      outputfile << "_000";
   } else if (m_fileNum < 100) {
      outputfile << "_00";
   } else if (m_fileNum < 1000) {
      outputfile << "_0";
   } else {
      std::cerr << "Too many Event output files." << std::endl;
      exit(-1);
   }
   outputfile << m_fileNum << ".fits";
   return outputfile.str();
}

} // namespace observationSim
