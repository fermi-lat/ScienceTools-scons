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
#include <iostream>
#include "CLHEP/Geometry/Vector3D.h"
#include "flux/FluxMgr.h"

namespace irfInterface {
   class Irfs;
}

#include "observationSim/Spacecraft.h"

class CompositeSource;

namespace observationSim {

class EventContainer;
class ScDataContainer;
class Roi;

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

   Simulator() : m_fluxMgr(0), m_source(0), m_newEvent(0) {}

   /// @param sourceName The name of the source as it appears in the xml file.
   /// @param fileList A vector of xml file names using the source.dtd.
   /// @param totalArea The cross-sectional area of the sphere enclosing
   ///        the instrument in square meters.  The default value is
   ///        maximum effective area appearing in the GLAST25 tables.
   /// @param startTime Absolute starting time of the simulation in seconds.
   Simulator(const std::string &sourceName, 
             const std::vector<std::string> &fileList,
             double totalArea = 1.21,
             double startTime = 0.,
             const std::string &pointingHistory = "",
             double maxSimTime = 3.155e8)
      : m_fluxMgr(0), m_source(0), m_newEvent(0) {
      init(sourceName, fileList, totalArea, startTime, pointingHistory,
           maxSimTime);
   }

   /// @param sourceNames A vector of source names as they appear in 
   ///        the xml file.
   Simulator(const std::vector<std::string> &sourceNames,
             const std::vector<std::string> &fileList,
             double totalArea = 1.21,
             double startTime = 0.,
             const std::string &pointingHistory = "",
             double maxSimTime = 3.155e8)
      : m_fluxMgr(0), m_source(0), m_newEvent(0) {
      init(sourceNames, fileList, totalArea, startTime, pointingHistory,
           maxSimTime);
   }

   ~Simulator();

   /// Set the pointing history file.
   void setPointingHistoryFile(const std::string &filename)
      {m_fluxMgr->setPointingHistoryFile(filename);}   

   /// Specify the rocking strategy from among those defined by
   /// flux::GPS::RockType.  
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
   void generateEvents(double simulationTime, 
                       EventContainer &events,
                       ScDataContainer &scData, 
                       irfInterface::Irfs &response,
                       Spacecraft *spacecraft, 
                       EventContainer *allEvents=0,
                       Roi *roi=0) {
      m_simTime = simulationTime;
      makeEvents(events, scData, response, spacecraft, true, 
                 allEvents, roi);
   }

   /// Generate a specified number of events.
   void generateEvents(long numberOfEvents, 
                       EventContainer &events,
                       ScDataContainer &scData, 
                       irfInterface::Irfs &response,
                       Spacecraft *spacecraft,
                       EventContainer *allEvents=0,
                       Roi *roi=0) {
      m_maxNumEvents = numberOfEvents;
      makeEvents(events, scData, response, spacecraft, false, 
                 allEvents, roi);
   }

   /// Generate photon events for a given elapsed simulation time.
   void generateEvents(double simulationTime, 
                       EventContainer &events,
                       ScDataContainer &scData, 
                       std::vector<irfInterface::Irfs*> &respPtrs,
                       Spacecraft *spacecraft, 
                       EventContainer *allEvents=0,
                       Roi *roi=0) {
      m_simTime = simulationTime;
      makeEvents(events, scData, respPtrs, spacecraft, true, 
                 allEvents, roi);
   }

   /// Generate a specified number of events.
   void generateEvents(long numberOfEvents, 
                       EventContainer &events,
                       ScDataContainer &scData, 
                       std::vector<irfInterface::Irfs*> &respPtrs,
                       Spacecraft *spacecraft,
                       EventContainer *allEvents=0,
                       Roi *roi=0) {
      m_maxNumEvents = numberOfEvents;
      makeEvents(events, scData, respPtrs, spacecraft, false, 
                 allEvents, roi);
   }

protected:

   Simulator(const Simulator &) {}
   Simulator & operator=(const Simulator &) {return *this;}

private:

   FluxMgr *m_fluxMgr;

   CompositeSource *m_source;

   EventSource *m_newEvent;
   double m_interval;
   
   long m_numEvents;
   long m_maxNumEvents;

   double m_absTime;
   double m_simTime;
   double m_elapsedTime;
   bool m_useSimTime;

   double m_maxSimTime;

   void init(const std::string &sourceName, 
             const std::vector<std::string> &fileList,
             double totalArea, double startTime, 
             const std::string &, double);

   void init(const std::vector<std::string> &sourceNames, 
             const std::vector<std::string> &fileList,
             double totalArea, double startTime,
             std::string, double);

   void makeEvents(EventContainer &, ScDataContainer &, 
                   irfInterface::Irfs &, Spacecraft *spacecraft,
                   bool useSimTime, EventContainer *allEvents, Roi *roi);

   void makeEvents(EventContainer &, ScDataContainer &, 
                   std::vector<irfInterface::Irfs *> &, 
                   Spacecraft *spacecraft,
                   bool useSimTime, EventContainer *allEvents, Roi *roi);

   bool done();

};

} // namespace observationSim

#endif // observationSim_Simulator_h
