/**
 * @file EventContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "astro/EarthCoordinate.h"

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
} // unnamed namespace

namespace observationSim {

void EventContainer::init(const std::string &filename,
                          const latResponse::ResponseFiles &glast25Data) {

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

   if (!m_useA1fmt) { // the default
      colName.push_back("time");fmt.push_back("1E");unit.push_back("seconds");
      colName.push_back("energy");fmt.push_back("1E");unit.push_back("MeV");
      colName.push_back("app_x");fmt.push_back("1E");unit.push_back("dir_cos");
      colName.push_back("app_y");fmt.push_back("1E");unit.push_back("dir_cos");
      colName.push_back("app_z");fmt.push_back("1E");unit.push_back("dir_cos");
      colName.push_back("src_x");fmt.push_back("1E");unit.push_back("dir_cos");
      colName.push_back("src_y");fmt.push_back("1E");unit.push_back("dir_cos");
      colName.push_back("src_z");fmt.push_back("1E");unit.push_back("dir_cos");
   } else {
      colName.push_back("RA"); fmt.push_back("1E"); unit.push_back("deg");
      colName.push_back("DEC"); fmt.push_back("1E"); unit.push_back("deg");
      colName.push_back("energy"); fmt.push_back("1E"); unit.push_back("MeV");
      colName.push_back("time"); fmt.push_back("1D"); unit.push_back("s");
      colName.push_back("SC_x0");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x1");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x2");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_y");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_z");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("zenith_angle"); fmt.push_back("1E");
      unit.push_back("deg");
   }

   m_eventTable = new FitsTable(filename, "LAT_event_summary", 
                                colName, fmt, unit);
   m_events.clear();

   m_aeff = 
      new latResponse::AeffGlast25(glast25Data.aeffFile(), glast25Data.hdu());
   m_psf = 
      new latResponse::PsfGlast25(glast25Data.psfFile(), glast25Data.hdu());
}

int EventContainer::addEvent(EventSource *event, bool flush) {
   
   std::string name = event->fullTitle();
   if (name.find("TimeTick") == std::string::npos) {
      std::string particleType = event->particleName();
      double time = event->time();
      double energy = event->energy();
      Hep3Vector launchDir = event->launchDir();
// Create the rotation matrix from instrument to "Celestial" (J2000?)
// coordinates.
      HepRotation rotationMatrix = glastToCelestial(time);
      astro::SkyDir sourceDir(rotationMatrix(launchDir),
                              astro::SkyDir::CELESTIAL);
      astro::SkyDir zAxis(rotationMatrix(Hep3Vector(0., 0., 1.)),
                          astro::SkyDir::CELESTIAL);
      astro::SkyDir xAxis(rotationMatrix(Hep3Vector(1., 0., 0.)),
                          astro::SkyDir::CELESTIAL);
   
      GPS *gps = GPS::instance();
      gps->getPointingCharacteristics(time);
      astro::EarthCoordinate earthCoord(gps->lon(), gps->lat());

      double inclination = sourceDir.difference(zAxis)*180./M_PI;
      if ( inclination < m_aeff->incMax() 
           && energy > 31.623 
           && RandFlat::shoot() < (*m_aeff)(energy, sourceDir, zAxis, xAxis)
                                   /event->totalArea()/1e4
           && !earthCoord.insideSAA() ) {
         astro::SkyDir appDir = m_psf->appDir(energy, sourceDir, zAxis, xAxis);
                                                        
         m_events.push_back( Event(time, energy, 
                                   appDir, sourceDir, zAxis, xAxis,
                                   ScZenith(time)) );
         if (flush) writeEvents();
         return 1;
      }
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

   if (!m_useA1fmt) {
      std::vector<std::vector<double> > data(8);
      for (std::vector<Event>::const_iterator it = m_events.begin();
           it != m_events.end(); it++) {
         data[0].push_back(it->time());
         data[1].push_back(it->energy());
         data[2].push_back(it->appDir().dir().x());
         data[3].push_back(it->appDir().dir().y());
         data[4].push_back(it->appDir().dir().z());
         data[5].push_back(it->srcDir().dir().x());
         data[6].push_back(it->srcDir().dir().y());
         data[7].push_back(it->srcDir().dir().z());
      }
      m_eventTable->writeTableData(data);
   } else {
      std::vector<std::vector<double> > data(11);
// pre-allocate the memory for each vector
      for (std::vector<std::vector<double> >::iterator vec_it = data.begin();
           vec_it != data.end(); vec_it++)
         vec_it->reserve(m_events.size());
      for (std::vector<Event>::const_iterator it = m_events.begin();
           it != m_events.end(); it++) {
         data[0].push_back(it->appDir().ra());
         data[1].push_back(it->appDir().dec());
         data[2].push_back(it->energy());
         data[3].push_back(it->time());
         data[4].push_back(it->xAxis().dir().x());
         data[5].push_back(it->xAxis().dir().y());
         data[6].push_back(it->xAxis().dir().z());
         data[7].push_back(it->zAxis().dir().x());
         data[8].push_back(it->zAxis().dir().y());
         data[9].push_back(it->zAxis().dir().z());
         data[10].push_back(
            it->zenith().dir().angle(it->appDir().dir())*180./M_PI);
      }
      m_eventTable->writeTableData(data);
   }
   m_events.clear();
}

} // namespace observationSim
