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

#include "latResponse/ResponseFiles.h"
#include "latResponse/../src/AeffGlast25.h"
#include "latResponse/../src/PsfGlast25.h"

#include "observationSim/Container.h"
#include "observationSim/Event.h"
#include "observationSim/FitsTable.h"

namespace observationSim {

/**
 * @class EventContainer
 * @brief Stores and writes Events to a FITS file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class EventContainer : public Container {

public:

   /// @param filename The name of the output FITS file.
   /// @param glast25Data Object containing the full paths to the GLAST25
   ///        instrument response files.
   /// @param useA1fmt A flag to use the format that the A1 tool expects.
   EventContainer(const std::string &filename, 
                  const latResponse::ResponseFiles &glast25Data,
                  bool useA1fmt=false) : 
      m_useA1fmt(useA1fmt) {init(filename, glast25Data);}

   EventContainer(char *filename, 
                  const latResponse::ResponseFiles &glast25Data,
                  bool useA1fmt=false) : 
      m_useA1fmt(useA1fmt) {init(filename, glast25Data);}

   ~EventContainer() 
      {writeEvents(); delete m_eventTable; 
      delete m_aeff; delete m_psf;}

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        Event data and then flush the buffers.
   int addEvent(EventSource *event, bool flush=false);

private:

   bool m_useA1fmt;

   long m_fileNum;

   latResponse::AeffGlast25 *m_aeff;
   latResponse::PsfGlast25 *m_psf;
   
   FitsTable *m_eventTable;

   std::vector<Event> m_events;

   void init(const std::string &filename, 
             const latResponse::ResponseFiles &glast25Data);

   /// Return the zenith for the current spacecraft location.
   astro::SkyDir ScZenith(double time);

   void writeEvents();

};

} // namespace observationSim

#endif // observationSim_EventContainer_h
