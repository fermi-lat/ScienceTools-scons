/**
 * @file SimpleTransient.h
 * @brief A flaring source defined by a single interval and constant
 * photon flux and spectral index during that interval.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef mySpectrum_SimpleTransient_h
#define mySpectrum_SimpleTransient_h

#include "flux/Spectrum.h"

/**
 * @class SimpleTransient
 * 
 * @brief A flaring source defined by a single interval and constant
 * photon flux and spectral index during that interval.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class SimpleTransient : public Spectrum {

public:

   SimpleTransient(const std::string &params);

   virtual ~SimpleTransient() {}

   /// @return Particle energy in MeV.
   /// @param xi Uniform random deviate on the unit interval.
   virtual float operator() (float xi) const;

   /// @return Particle type, "gamma".
   virtual const char * particleName() const {return "gamma";}

   /// @return Title describing the spectrum.
   virtual std::string title() const {return "SimpleTransient";}

   /// @return Interval to the next event (seconds)
   virtual double interval(double time);

   /// @return Photon energy (MeV).
   virtual double energy(double time);

protected:

   /// Disable these virtual functions since they are not used by
   /// this source.
   virtual double flux(double) const {return 0;}
   virtual double solidAngle() const {return 0;}

   /// galactic_dir or celestial_dir tags will give the
   /// photon directions.
   virtual std::pair<double, double> dir(double) {
      return std::make_pair(0, 0);
   }

private:

   double m_flux;
   double m_gamma;
   double m_tstart;
   double m_tstop;
   double m_emin;
   double m_emax;

   unsigned int m_nextEvent;
   std::vector<double> m_eventTimes;
   void createEventTimes();

};

#endif // mySpectrum_SimpleTransient_h
