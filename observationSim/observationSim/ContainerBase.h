/**
 * @file ContainerBase.h
 * @brief Base class for observationSim Event and ScData containers.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef observationSim_ContainerBase_h
#define observationSim_ContainerBase_h

#include <string>

#include "astro/JulianDate.h"

namespace tip {
   class Extension;
}

namespace observationSim {

/**
 * @class ContainerBase
 *
 * $Header$
 */

class ContainerBase {

public:
   
   ContainerBase(const std::string & filename, unsigned int maxNumEntries) 
      : m_filename(filename), m_maxNumEntries(maxNumEntries), m_fileNum(0) {} 

protected:

   /// Root name for the FITS binary table output files.
   std::string m_filename;

   /// The maximum number of rows to accumulate before the data are
   /// written to a FITS file and the data buffer is flushed.
   unsigned int m_maxNumEntries;

   /// The current index number of the FITS file to be written.  This
   /// number is formatted appropriately and appended to the root
   /// filename given in the constructor.
   long m_fileNum;

   /// Name of the FT1/2 template file.
   std::string m_ftTemplate;

   /// Return an output filename, based on the root name, m_filename,
   /// and the counter index, m_fileNum.
   std::string outputFileName() const;

   /// Set the date keywords in a given header, accesses via the
   /// tip::Extension and tip::Header interface.
   static void writeDateKeywords(tip::Extension * table, double start_time,
                                 double stop_time, bool extension=true);

   /// Return an astro::JulianDate object for the current time.
   static astro::JulianDate currentTime();

};

}

#endif // observationSim_ContainerBase_h
