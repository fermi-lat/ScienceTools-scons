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

#include "flux/EventSource.h"
#include "flux/FluxMgr.h"

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

   /// @param filename The root name of the output FITS file.
   /// @param useA1fmt A flag to use the format that the A1 tool expects.
   /// @param maxNumEvents The maximum size of the Event buffer before
   ///        a FITS file is written.
   EventContainer(const std::string &filename, bool useA1fmt=false,
                  int maxNumEvents=20000) : 
      m_filename(filename), m_useA1fmt(useA1fmt), m_fileNum(0), 
      m_maxNumEvents(maxNumEvents), m_prob(1) {init();}

   /// This version is provided for SWIG since it does not presently
   /// have full support for std::string.
   EventContainer(char *filename, bool useA1fmt=false,
                  int maxNumEvents=20000) : 
      m_filename(filename), m_useA1fmt(useA1fmt), m_fileNum(0), 
      m_maxNumEvents(maxNumEvents), m_prob(1) {init();}

   ~EventContainer() {if (m_events.size() > 0) writeEvents();}

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param respObj A container of pointers to the standard response 
   ///        functions.
   /// @param spacecraft A pointer to an object that provides methods 
   ///        for accessing spacecraft orbit and attitude information.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        Event data and then flush the buffers.
   /// @param alwaysAccept If true, the event is accepted without
   ///        regard to the response info, i.e., true energies and 
   ///        directions are saved.
   int addEvent(EventSource *event, latResponse::Irfs &respObj, 
                Spacecraft *spacecraft, bool flush=false, 
                bool alwaysAccept=false);

   /// The number of events in the container.
   long numEvents() {return m_events.size();}

   /// The acceptance probability for any event is typically the ratio
   /// of the livetime to elapsed time for a given observation
   /// interval.
   void setAcceptanceProb(double prob) {m_prob = prob;}

   /// Return a const reference to m_events for processing by Python
   /// of the data contained therein.
   const std::vector<Event> &getEvents() {return m_events;}  

private:

   /// Root name for the FITS binary table output files.
   std::string m_filename;

   /// Flag to indicate that A1 (Likelihood prototype) formatting of
   /// the FITS file will be used.
   bool m_useA1fmt;

   /// The current index number of the FITS file to be written.  This
   /// number is formatted appropriately and appended to the root
   /// filename given in the constructor.
   long m_fileNum;

   /// The maximum number of Events to accumulate before the Events
   /// are written to a FITS file and the Event buffer is flushed.
   int m_maxNumEvents;

   /// The prior probability that an event will be accepted.
   /// Typically this is set to be the ratio of livetime to elapsed
   /// time for a given observation interval.
   double m_prob;

   /// The FITS binary table object that steers the cfitsio routines.
   FitsTable *m_eventTable;

   /// The Event buffer.
   std::vector<Event> m_events;

   /// This routine contains the constructor implementation (such as
   /// it is).
   void init();

   /// A routine to create the FITS binary table object.
   void makeFitsTable();

   /// Return the zenith for the current spacecraft location.
   astro::SkyDir ScZenith(double time);

   /// A routine to unpack the Event buffer, m_events, into an
   /// appropriate data vector.  After unpacking, this routine calls
   /// the writeTableData(...) method of the m_eventTable object.
   void writeEvents();

};

} // namespace observationSim

#endif // observationSim_EventContainer_h
