/**
 * @file testGoodiRead.cxx
 * @brief This program demostrates how to read in FT1 and FT2 files.
 * @author J. Chiang
 *
 * $Header$
 */

#include "Goodi/GoodiConstants.h"
#include "Goodi/DataIOServiceFactory.h"
#include "Goodi/DataFactory.h"
#include "Goodi/IDataIOService.h"
#include "Goodi/IData.h"
#include "Goodi/IEventData.h"

#include "Goodi/../src/Event.h"

int main(int iargc, char* argv[]) {

   std::string eventFile;
   if (iargc == 1) {
      eventFile = "test_events_0000.fits";
   } else {
      eventFile = argv[1];
   }

   Goodi::DataIOServiceFactory iosvcCreator;
   Goodi::DataFactory dataCreator;

   Goodi::IDataIOService *ioService;

   ioService = iosvcCreator.create(eventFile);

   Goodi::DataType datatype = Goodi::Evt; 

   Goodi::IEventData *eventData = dynamic_cast<Goodi::IEventData *>
      (dataCreator.create(datatype, ioService));
      
   bool done = false;
   int i = 0;

   while (!done) {
      const Goodi::Event evt = eventData->nextEvent(ioService, done);
      if (!done) {
         std::cout << i++ << "  "
                   << evt.time() << "  "
                   << evt.energy() << "  "
                   << evt.ra() << "  "
                   << evt.dec() << "  "
                   << evt.phi() << "  "
                   << evt.theta() << "  "
                   << evt.zenithAngle() << "  "
                   << evt.azimuth() << "  "
                   << std::endl;
      }
   }
   delete ioService;
}
