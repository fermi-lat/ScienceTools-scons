/**
 * @file EgretSc.h
 * @brief Declaration for the EGRET spacecraft object.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef observationSim_EgretSc_h
#define observationSim_EgretSc_h

#include "observationSim/Spacecraft.h"

namespace observationSim {

/**
 * @class EgretSc
 *
 * @brief Provide spacecraft attitude and orbital position information
 * for the EGRET instrument.
 *
 * This class mostly just encapsulates spacecraft data obtained from
 * the exposure history file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class EgretSc : public Spacecraft {

public:

   EgretSc(astro::SkyDir &zAxis, astro::SkyDir &xAxis, 
           double earthLon, double earthLat, bool inSaa) :
      m_zAxis(zAxis), m_xAxis(xAxis), m_earthLon(earthLon),
      m_earthLat(earthLat), m_inSaa(inSaa) {}

   EgretSc(double raz, double decz, double rax, double decx,
           double earthLon, double earthLat, bool inSaa) :
      m_zAxis(astro::SkyDir(raz, decz, astro::SkyDir::EQUATORIAL)), 
      m_xAxis(astro::SkyDir(rax, decx, astro::SkyDir::EQUATORIAL)), 
      m_earthLon(earthLon), m_earthLat(earthLat), m_inSaa(inSaa) {}

   virtual ~EgretSc() {}

   virtual astro::SkyDir zAxis(double) {return m_zAxis;}
   virtual astro::SkyDir xAxis(double) {return m_xAxis;}

   virtual double EarthLon(double) {return m_earthLon;}
   virtual double EarthLat(double) {return m_earthLat;}

   virtual HepRotation InstrumentToCelestial(double);

   virtual bool inSaa(double) {return m_inSaa;}

   virtual void getScPosition(double time, std::vector<double> & scPosition);

   virtual void getZenith(double time, double & ra, double & dec);

private:

   astro::SkyDir m_zAxis;
   astro::SkyDir m_xAxis;
   double m_earthLon;
   double m_earthLat;
   bool m_inSaa;

};

} // namespace observationSim

#endif // observationSim_EgretSc_h
