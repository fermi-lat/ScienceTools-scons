/**
 * @file EventContainer.h
 * @brief Declaration for EventContainer class.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_EventContainer_h
#define observationSim_EventContainer_h

#include <fstream>
#include <string>
#include <vector>

#include "FluxSvc/../src/EventSource.h"
#include "FluxSvc/../src/FluxMgr.h"

#include "astro/SkyDir.h"

#include "latResponse/Irfs.h"

#include "observationSim/Event.h"
#include "observationSim/FitsTable.h"
#include "observationSim/Spacecraft.h"

namespace observationSim {

/**
 * @class EventContainer
 * @brief Stores and writes Events to a FITS file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class EventContainer {

public:

   /// @param filename The name of the output FITS file.
   /// @param useA1fmt A flag to use the format that the A1 tool expects.
   EventContainer(const std::string &filename, bool useA1fmt=false) : 
      m_useA1fmt(useA1fmt) {init(filename); m_prob = 1;}

   EventContainer(char *filename, bool useA1fmt=false) : 
      m_useA1fmt(useA1fmt) {init(filename); m_prob = 1;}

   ~EventContainer() 
      {writeEvents(); delete m_eventTable;}

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param respObj A container of pointers to the standard response 
   ///        functions.
   /// @param spacecraft A pointer to an object that provides methods 
   ///        for accessing spacecraft orbit and attitude information.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        Event data and then flush the buffers.
   int addEvent(EventSource *event, latResponse::Irfs &respObj, 
                Spacecraft *spacecraft, bool flush=false);

   /// The number of events in the container.
   long numEvents() {return m_events.size();}

   /// The acceptance probability for any event is typically the ratio
   /// of the livetime to elapsed time for a given observation
   /// interval.
   void setAcceptanceProb(double prob) {m_prob = prob;}

private:

   bool m_useA1fmt;

   long m_fileNum;

   double m_prob;
   
   FitsTable *m_eventTable;

   std::vector<Event> m_events;

   void init(const std::string &filename);

   /// Return the zenith for the current spacecraft location.
   astro::SkyDir ScZenith(double time);

   void writeEvents();

};

} // namespace observationSim

#endif // observationSim_EventContainer_h
