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

#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"

#include "Likelihood/Aeff.h"
#include "Likelihood/Psf.h"

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

EventContainer::EventContainer(const std::string &filename, bool useA1fmt) 
   : m_useA1fmt(useA1fmt) {

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
      Hep3Vector sourceDir = rotationMatrix(launchDir);
      Hep3Vector zAxis = rotationMatrix(Hep3Vector(0., 0., 1.));
      Hep3Vector xAxis = rotationMatrix(Hep3Vector(1., 0., 0.));
   
      Likelihood::Aeff *aeff = Likelihood::Aeff::instance();
      GPS *gps = GPS::instance();
      gps->getPointingCharacteristics(time);
      astro::EarthCoordinate earthCoord(gps->lon(), gps->lat());

      double inclination = sourceDir.angle(zAxis)*180./M_PI;
      if ( inclination < Likelihood::Response::incMax() 
           && energy > 31.623 
           && RandFlat::shoot() < ((*aeff)(energy, inclination)
                                   /event->totalArea()/1e4)
           && !earthCoord.insideSAA() ) {
         Hep3Vector appDir = apparentDir(energy, sourceDir, zAxis); 
                                                        
         m_events.push_back(Event(time, energy, 
                                  astro::SkyDir(appDir), 
                                  astro::SkyDir(sourceDir),
                                  astro::SkyDir(zAxis), 
                                  astro::SkyDir(xAxis),
                                  astro::SkyDir(ScZenith(time))));
         if (flush) writeEvents();
         return 1;
      }
   }
   if (flush) writeEvents();
   return 0;
}

Hep3Vector EventContainer::ScZenith(double time) {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   double lon_zenith = gps->RAZenith()*M_PI/180.;
   double lat_zenith = gps->DECZenith()*M_PI/180.;
   return Hep3Vector(cos(lat_zenith)*cos(lon_zenith),
                     cos(lat_zenith)*sin(lon_zenith),
                     sin(lat_zenith));
}

Hep3Vector EventContainer::apparentDir(double energy, 
                                       const Hep3Vector &sourceDir, 
                                       const Hep3Vector &zAxis) {
// This routine returns the apparent photon direction in the same
// coordinate system used by sourceDir and zAxis.

// Access the Psf data
   Likelihood::Psf *psf = Likelihood::Psf::instance();

// Compute the inclination of the source wrt the instrument axis.
   double inclination = sourceDir.angle(zAxis)*180./M_PI;

// The GLAST25 PSF parameters:
   std::vector<double> psfParams;
   try {
      psf->fillPsfParams(energy, inclination, psfParams);
   } catch(Likelihood::LikelihoodException &eObj) {
      std::cerr << eObj.what() << std::endl;
      std::cerr << energy << "  "
                << inclination << std::endl;
   }
   double sig1 = psfParams[0];
   double sig2 = psfParams[1];
   double wt = psfParams[2];

// Draw the apparent direction in photon coordinates.
   double xi = RandFlat::shoot();
   double sig;
   if (xi <= wt) {
      sig = sig1*M_PI/180.;
   } else {
      sig = sig2*M_PI/180.;
   }
   xi = RandFlat::shoot();
   double mu = 1. + sig*sig*log(1. - xi*(1. - exp(-2./sig/sig)));
   double theta = my_acos(mu);

   xi = RandFlat::shoot();
   double phi = 2.*M_PI*xi;

// Create an arbitrary x-direction about which to perform the theta
// rotation.
   Hep3Vector xDir = sourceDir.cross(zAxis);

   Hep3Vector appDir = sourceDir;

   appDir.rotate(theta, xDir).rotate(phi, sourceDir);

   return appDir;
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
