/**
 * @file EventContainer.cxx
 * @brief Implementation for the class that keeps track of events and
 * when they get written to a FITS file.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <algorithm>
#include <numeric>

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"
#include "astro/GPS.h"

#include "flux/EventSource.h"

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
}

void EventContainer::init() {
   m_events.clear();
   
   if (m_useFT1) {
      std::string tipRoot(std::getenv("TIPROOT"));
      m_ft1Template = tipRoot + "/data/ft1.tpl";
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

   double flux_theta = acos(launchDir.z());
   double flux_phi = atan2(launchDir.y(), launchDir.x());

   LatSc latSpacecraft;
   HepRotation rotMatrix = latSpacecraft.InstrumentToCelestial(time);
   astro::SkyDir sourceDir(rotMatrix(-launchDir), astro::SkyDir::EQUATORIAL);

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
// Turn off SAA for DC1.
//         && !spacecraft->inSaa(time) ) {
      ) {

      astro::SkyDir appDir 
         = respPtr->psf()->appDir(energy, sourceDir, zAxis, xAxis);
      double appEnergy 
         = respPtr->edisp()->appEnergy(energy, sourceDir, zAxis, xAxis);
                                                        
      m_events.push_back( Event(time, appEnergy, 
                                appDir, sourceDir, zAxis, xAxis,
                                ScZenith(time), respPtr->irfID(), 
                                energy, flux_theta, flux_phi) );
//      std::cout << "adding an event: " << m_events.size() << std::endl;
      if (flush || m_events.size() >= m_maxNumEvents) writeEvents();
      return 1;
   }
   if (flush) writeEvents();
   return 0;
}

astro::SkyDir EventContainer::ScZenith(double time) {
//   astro::GPS *gps = astro::GPS::instance();
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   double lon_zenith = gps->RAZenith()*M_PI/180.;
   double lat_zenith = gps->DECZenith()*M_PI/180.;
   return astro::SkyDir(Hep3Vector(cos(lat_zenith)*cos(lon_zenith),
                                   cos(lat_zenith)*sin(lon_zenith),
                                   sin(lat_zenith)),
                        astro::SkyDir::EQUATORIAL);
}

void EventContainer::writeEvents() {

   if (m_useFT1) {
      std::string ft1File = outputFileName();
      tip::IFileSvc::instance().createFile(ft1File, m_ft1Template);
      tip::Table * my_table = 
         tip::IFileSvc::instance().editTable(ft1File, "EVENTS");
      my_table->setNumRecords(m_events.size());
      tip::Table::Iterator it = my_table->begin();
      tip::Table::Record & row = *it;
      std::vector<Event>::iterator evt = m_events.begin();
      for ( ; it != my_table->end(), evt != m_events.end(); ++it, ++evt) {
         row["time"].set(evt->time());
         row["energy"].set(evt->energy());
         row["ra"].set(evt->appDir().ra());
         row["dec"].set(evt->appDir().dec());
         row["theta"].set(evt->theta());
         row["phi"].set(evt->phi());
         row["zenith_angle"].set(evt->zenAngle());
         try {
            row["conversion_layer"].set(evt->convLayer());
         } catch (std::exception &eObj) {
            std::cout << eObj.what() << std::endl;
            exit(-1);
         }
         tip::Table::Vector<int> calibVersion = row["calib_version"];
         for (int i = 0; i < 3; i++) {
//            calibVersion[i] = 1;
         }
      }
      delete my_table;

// // // Header keywords for the GTI extension.
// //       std::string date_start = "2005-07-18T00:00:00.0000";
// // // This needs to be computed....we need a date class.
// //       std::string date_stop  = "2005-07-19T00:00:00.0000";
// //       m_goodiEventData->setKey("DATE-OBS", date_start);
// //       m_goodiEventData->setKey("DATE-END", date_stop);
// //       double duration = gti.front().second - gti.front().first;
// //       m_goodiEventData->setKey("TSTART", gti.front().first);
// //       m_goodiEventData->setKey("TSTOP", gti.front().second);
// //       m_goodiEventData->setKey("ONTIME", duration);
// //       m_goodiEventData->setKey("TELAPSE", duration);
   } else { // Use the old A1 format.
      makeFitsTable();
      std::vector<std::vector<double> > data(20);
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
// Append some columns for all_gamma/IRF tests.
         double theta = it->srcDir().difference(it->zAxis());
         double xhat = it->srcDir().dir().dot(it->xAxis().dir());
         Hep3Vector yAxis = it->zAxis().dir().cross(it->xAxis().dir());
         double yhat = it->srcDir().dir().dot(yAxis);
         double phi = atan2(yhat, xhat);
         data[14].push_back(theta*180./M_PI);
         data[15].push_back(phi*180./M_PI);
         data[16].push_back(it->trueEnergy());
         double separation = it->srcDir().difference(it->appDir());
         data[17].push_back(separation*180./M_PI);
         data[18].push_back(it->fluxTheta()*180./M_PI);
         data[19].push_back(it->fluxPhi()*180./M_PI);
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

// Columns for all_gamma/IRF tests.
   colName.push_back("theta");fmt.push_back("1E"),unit.push_back("deg");
   colName.push_back("phi");fmt.push_back("1E"),unit.push_back("deg");
   colName.push_back("true_energy");fmt.push_back("1E"),unit.push_back("MeV");
   colName.push_back("separation");fmt.push_back("1E"),unit.push_back("deg");
   colName.push_back("flux_theta");fmt.push_back("1E"),unit.push_back("deg");
   colName.push_back("flux_phi");fmt.push_back("1E"),unit.push_back("deg");

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
