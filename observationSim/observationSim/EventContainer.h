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

#include "astro/SkyDir.h"
#include "astro/JulianDate.h"

#include "observationSim/Event.h"
#include "observationSim/FitsTable.h"
#include "observationSim/Spacecraft.h"

class EventSource;  // from flux package

namespace tip {
   class Table;
}

//namespace latResponse {
namespace irfInterface {
   class Irfs;
}

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
   /// @param useFT1 A flag to write the data in 
   /// <a href="http://glast.gsfc.nasa.gov/ssc/dev/fits_def/definitionFT1.html">
   /// FT1</a> format. If set to false, the old A1 format is used.
   /// @param maxNumEvents The maximum size of the Event buffer before
   ///        a FITS file is written.
   EventContainer(const std::string &filename, bool useFT1=true, 
                  unsigned int maxNumEvents=20000) : 
      m_filename(filename), m_useFT1(useFT1), m_fileNum(0), 
      m_maxNumEvents(maxNumEvents), m_prob(1) {init();}

   ~EventContainer();

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param respPtrs A vector of pointers to response 
   ///        function containters.
   /// @param spacecraft A pointer to an object that provides methods 
   ///        for accessing spacecraft orbit and attitude information.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        Event data and then flush the buffers.
   /// @param alwaysAccept If true, the event is accepted without
   ///        regard to the response info, i.e., true energies and 
   ///        directions are saved.
   int addEvent(EventSource *event, 
//                std::vector<latResponse::Irfs *> &respPtrs, 
                std::vector<irfInterface::Irfs *> &respPtrs, 
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

   /// Set the date keywords in a given header, accesses via the
   /// tip::Table and tip::Header interface.
   static void writeDateKeywords(tip::Table * table, double start_time,
                                 double stop_time);

   /// Return an astro::JulianDate object for the current time.
   static astro::JulianDate currentTime();

private:

   /// Root name for the FITS binary table output files.
   std::string m_filename;

   /// Flag to indicate that the FT1 format will be used.
   bool m_useFT1;

   /// Name of the FT1 template file.
   std::string m_ft1Template;

   /// The current index number of the FITS file to be written.  This
   /// number is formatted appropriately and appended to the root
   /// filename given in the constructor.
   long m_fileNum;

   /// The maximum number of Events to accumulate before the Events
   /// are written to a FITS file and the Event buffer is flushed.
   unsigned int m_maxNumEvents;

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

   /// Return an output filename, based on the root name, m_filename,
   /// and the counter index, m_fileNum.
   std::string outputFileName() const;

};

} // namespace observationSim

#endif // observationSim_EventContainer_h
