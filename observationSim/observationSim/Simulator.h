/**
 * @file Simulator.h
 * @brief Declaration for Simulator class.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_Simulator_h
#define observationSim_Simulator_h

#include <vector>
#include <string>
#include "CLHEP/Geometry/Vector3D.h"
#include "FluxSvc/../src/EventSource.h"
#include "FluxSvc/../src/FluxMgr.h"

namespace observationSim {

class EventContainer;
class ScDataContainer;

/**
 * @class Simulator
 * @brief This class provides an interface to FluxSvc::FluxMgr for
 * generating gamma-ray events in the LAT.
 *
 * In addition to producing photon events, this class manages the
 * output of the spacecraft data and writes the events and spacecraft
 * data to FITS files.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Simulator {

public:

   /// @param sourceName The name of the source as it appears in the xml file.
   /// @param fileList A vector of xml file names using the source.dtd.
   /// @param totalArea The cross-sectional area of the sphere enclosing
   ///        the instrument in square meters.  The default value is
   ///        maximum effective area appearing in the GLAST25 tables.
   /// @param startTime Absolute starting time of the simulation in seconds.
   Simulator(const std::string &sourceName, 
             const std::vector<std::string> &fileList,
             double totalArea = 1.21,
             double startTime = 0.);

   ~Simulator() {delete m_fluxMgr; delete m_source;}

   /// Specify the rocking strategy from among those defined by
   /// FluxSvc::GPS::RockType.  
   /// @param rockType Integer representation of the desired strategy.
   ///        The default value of 3 corresponds to step-rocking, once
   ///        per orbit.
   /// @param angle Rocking angle in degrees.
   void setRocking(int rockType = 3, double angle = 35.)
      {m_fluxMgr->setRockType(rockType, angle);}

   /// List the available sources by xml name for each file in the fileList.
   void listSources() const;
   
   /// List the avaiable particle spectra.
   void listSpectra() const;

   /// Generate photon events for a given elapsed simulation time.
   void generateEvents(double simulationTime, EventContainer &events,
                       ScDataContainer &scData) {
      m_simTime = simulationTime;
      makeEvents(events, scData);
   }

   /// Generate a specified number of events.
   void generateEvents(long numberOfEvents, EventContainer &events,
                       ScDataContainer &scData) {
      m_maxNumEvents = numberOfEvents;
      makeEvents(events, scData, false);
   }

   /// @param caldbPath File path to the files psf_lat.fits and aeff_lat.fits.
   /// @param hdu FITS HDU for the desired response data.  Valid
   ///        values are 2 = Front, 3 = Back, 4 = Combined.  One can also
   ///        enter the Likelihood::Response::HDU enums.
   void readResponseData(const std::string &caldbPath, int hdu);

private:

   FluxMgr *m_fluxMgr;

   EventSource *m_source;

   long m_numEvents;
   long m_maxNumEvents;

   double m_absTime;
   double m_simTime;
   double m_elapsedTime;
   bool m_useSimTime;

   void makeEvents(EventContainer &, ScDataContainer &, bool useSimTime=true);

   void fluxLoad();

   bool done();

};

} // namespace observationSim

#endif // observationSim_Simulator_h
