/**
 * @file Event.h
 * @brief Simple data structure to hold Event data.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_Event_h
#define observationSim_Event_h

#include <cmath>
#include "astro/SkyDir.h"

namespace observationSim {

/**
 * @class Event
 * @brief Simple data structure to hold Event data.
 * @author J. Chiang
 *
 * $Header$
 */

class Event {

public:

   Event(double time, double energy, const astro::SkyDir &appDir, 
         const astro::SkyDir &srcDir, const astro::SkyDir &zAxis, 
         const astro::SkyDir &xAxis, const astro::SkyDir &zenith, 
         int eventType=4, double trueEnergy = 0., double flux_theta=0.,
         double flux_phi=0.) :
      m_time(time), m_energy(energy), m_appDir(appDir), m_srcDir(srcDir),
      m_zAxis(zAxis), m_xAxis(xAxis), m_zenith(zenith), m_eventType(eventType),
      m_trueEnergy(trueEnergy), m_flux_theta(flux_theta), 
      m_flux_phi(flux_phi) {}

   /// Time in seconds (referenced to the zero time of the orbit
   /// calculation in astro::EarthOrbit).
   double time() const {return m_time;}

   /// Photon energy in MeV.
   double energy() const {return m_energy;}

   /// Apparent photon direction in "Celestial" coordinates.
   const astro::SkyDir & appDir() const {return m_appDir;}

   /// True photon direction in "Celestial" coordinates.
   const astro::SkyDir & srcDir() const {return m_srcDir;}

   /// Instrument z-axis in "Celestial" coordinates.
   const astro::SkyDir & zAxis() const {return m_zAxis;}

   /// Instrument x-axis in "Celestial" coordinates.
   const astro::SkyDir & xAxis() const {return m_xAxis;}

   /// Zenith direction at spacecraft location in "Celestial" coordinates.
   const astro::SkyDir & zenith() const {return m_zenith;}

   /// Event type (for ascertaining which set of IRFs to use).
   int eventType() const {return m_eventType;}

   /// True photon energy in MeV.
   double trueEnergy() const {return m_trueEnergy;}

   /// Apparent inclination wrt spacecraft z-axis (degrees)
   double phi() const {
      Hep3Vector yAxis = zAxis().dir().cross(xAxis().dir());
      double my_phi = atan2(appDir().dir().dot(yAxis),
                            appDir().dir().dot(xAxis().dir()));
      return my_phi*180./M_PI;
   }

   /// Apparent azimuthal angle wrt spacecraft x-axis (degrees)
   double theta() const {
      return appDir().difference(zAxis())*180./M_PI;
   }

   /// Apparent angle wrt zenith (degrees)
   double zenAngle() const {
      return zenith().difference(appDir())*180./M_PI;
   }                                 

   /// Pick a conversion layer.
   short convLayer() const {
      if (eventType() == 0) { // Front
         return 0;
      } else if (eventType() == 1) { // Back
         return 15;
      } 
   // Default: pick at random
      return static_cast<int>(rand()*16);
   }

   double fluxTheta() const {return m_flux_theta;}
   double fluxPhi() const {return m_flux_phi;}

private:

   double m_time;
   double m_energy;
   astro::SkyDir m_appDir;
   astro::SkyDir m_srcDir;

   astro::SkyDir m_zAxis;
   astro::SkyDir m_xAxis;
   astro::SkyDir m_zenith;

   int m_eventType;

   double m_trueEnergy;
   double m_flux_theta;
   double m_flux_phi;

};

} // namespace observationSim

#endif // observationSim_Event_h
