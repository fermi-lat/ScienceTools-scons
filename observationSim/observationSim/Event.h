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
         double flux_phi=0., int eventId=0) :
      m_time(time), m_energy(energy), m_appDir(appDir), m_srcDir(srcDir),
      m_zAxis(zAxis), m_xAxis(xAxis), m_zenith(zenith), m_eventType(eventType),
      m_trueEnergy(trueEnergy), m_flux_theta(flux_theta), 
      m_flux_phi(flux_phi), m_eventId(eventId) {}

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

   void setEventClass(int eventClass) {
      m_eventClass = eventClass;
   }

   int eventClass() const {
      return m_eventClass;
   }

   /// Conversion type (0=front, 1=back), 
   /// Computed from m_eventType assuming IRF event types are ordered 
   /// front, back, front, back,....
   int conversionType() const {
      return m_eventType % 2;
   }

   /// True photon energy in MeV.
   double trueEnergy() const {return m_trueEnergy;}

   /// Apparent inclination wrt spacecraft z-axis (degrees)
   double phi() const {
      CLHEP::Hep3Vector yAxis = zAxis().dir().cross(xAxis().dir());
      double my_phi = atan2(appDir().dir().dot(yAxis),
                            appDir().dir().dot(xAxis().dir()))*180./M_PI;
      if (my_phi < 0) {
         my_phi += 360.;
      }
      return my_phi;
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

   int eventId() const {return m_eventId;}

private:

   double m_time;
   double m_energy;
   astro::SkyDir m_appDir;
   astro::SkyDir m_srcDir;

   astro::SkyDir m_zAxis;
   astro::SkyDir m_xAxis;
   astro::SkyDir m_zenith;

   int m_eventType;
   int m_eventClass;

   double m_trueEnergy;
   double m_flux_theta;
   double m_flux_phi;
   
   int m_eventId;

};

} // namespace observationSim

#endif // observationSim_Event_h
