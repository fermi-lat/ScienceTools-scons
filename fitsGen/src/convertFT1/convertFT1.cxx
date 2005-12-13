#include <iostream>

#include "dataSubselector/Gti.h"

#include "fitsGen/Ft1File.h"
#include "fitsGen/MeritFile.h"

int main(int iargc, char * argv[]) {
   if (iargc != 3) {
      std::cout << "usage: " << argv[0] << " <infile> <outfile>\n";
      std::exit(1);
   }

   fitsGen::MeritFile infile(argv[1], "EVENTS");
   fitsGen::Ft1File outfile(argv[2], infile.nrows(), argv[1]);
   
   for ( ; outfile.itor() != outfile.end(); outfile.next(), infile.next()) {
      *(outfile.itor()) = *(infile.itor());
   }

   outfile.appendField("EVENT_CLASS", "I");
   for (outfile.itor() = outfile.begin(); outfile.itor() != outfile.end();
        outfile.next()) {
      if (outfile["conversion_layer"].get() < 11.5) {
         outfile["event_class"].set(0);
      } else {
         outfile["event_class"].set(1);
      }
   }
   dataSubselector::Gti gti(argv[1]);
   outfile.close();
   gti.writeExtension(argv[2]);
}
