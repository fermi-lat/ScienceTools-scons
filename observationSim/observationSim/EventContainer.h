/**
 * @file EventContainer.h
 * @brief Declaration for EventContainer class.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_EventContainer_h
#define observationSim_EventContainer_h

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "astro/SkyDir.h"

#include "observationSim/ContainerBase.h"
#include "observationSim/Event.h"
#include "observationSim/Spacecraft.h"

class EventSource;  // from flux package

namespace tip {
   class Table;
}

namespace irfInterface {
   class Irfs;
}

namespace dataSubselector {
   class Cuts;
}

namespace observationSim {

/**
 * @class EventContainer
 * @brief Stores and writes Events to a FITS file.
 *
 */

class EventContainer : public ContainerBase {

public:

   /// @param filename The root name of the output FITS file.
   /// @param maxNumEvents The maximum size of the Event buffer before
   ///        a FITS file is written.
   EventContainer(const std::string & filename, 
                  const std::string & tablename,
                  dataSubselector::Cuts * cuts=0,
                  unsigned int maxNumEvents=20000,
                  double startTime=0, double stopTime=0,
                  bool applyEdisp=true,
                  const st_app::AppParGroup * pars=0) : 
      ContainerBase(filename, tablename, maxNumEvents, pars), m_prob(1), 
      m_cuts(cuts), m_startTime(startTime), m_stopTime(stopTime),
      m_applyEdisp(applyEdisp) {
      init();
   }

   ~EventContainer();

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param respPtrs A vector of pointers to response 
   ///        function containers.  If respPtrs is empty, then
   ///        accept the event unconditionally without applying
   ///        PSF or energy dispersion.
   /// @param spacecraft A pointer to an object that provides methods 
   ///        for accessing spacecraft orbit and attitude information.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        Event data and then flush the buffers.
   bool addEvent(EventSource * event, 
                 std::vector<irfInterface::Irfs *> & respPtrs, 
                 Spacecraft * spacecraft, bool flush=false);

   /// The number of events in the container.
   long numEvents() {return m_events.size();}

   /// The acceptance probability for any event is typically the ratio
   /// of the livetime to elapsed time for a given observation
   /// interval.
   void setAcceptanceProb(double prob) {m_prob = prob;}

   /// Return a const reference to m_events for processing by Python
   /// of the data contained therein.
   const std::vector<Event> & getEvents() const {return m_events;}

   /// struct to contain event summary for a given source
   class SourceSummary {
   public:
      SourceSummary(int idnum=0) : id(idnum), incidentNum(0), acceptedNum(0) {}
      int id;
      unsigned long incidentNum;
      unsigned long acceptedNum;
   };

   /// Access to the map of event IDs.
   const std::map<std::string, SourceSummary> & eventIds() const {
      return m_srcSummaries;
   }

private:

   /// The prior probability that an event will be accepted.
   /// Typically this is set to be the ratio of livetime to elapsed
   /// time for a given observation interval.
   double m_prob;

   dataSubselector::Cuts * m_cuts;

   double m_startTime;
   double m_stopTime;

   bool m_applyEdisp;

   /// The Event buffer.
   std::vector<Event> m_events;

   /// Event summaries keyed by source name.
   std::map<std::string, SourceSummary> m_srcSummaries;

   /// This routine contains the constructor implementation.
   void init();

   /// Set the event ID for the named source, if it does not already exist.
   void setEventId(const std::string & name, int eventId);

   /// Return the zenith for the current spacecraft location.
   astro::SkyDir ScZenith(double time) const;

   /// Return the Earth azimuth angle of the apparent event direction.
   double earthAzimuthAngle(double ra, double dec, double time) const;

   /// A routine to unpack and write the Event buffer to an FT1 file.
   void writeEvents(double obsStopTime=-1.);

};

} // namespace observationSim

#endif // observationSim_EventContainer_h
