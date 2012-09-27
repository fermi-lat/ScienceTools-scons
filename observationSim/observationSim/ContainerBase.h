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

namespace st_app {
   class AppParGroup;
}

namespace tip {
   class Extension;
   class Header;
}

namespace observationSim {

/**
 * @class ContainerBase
 *
 */

class ContainerBase {

public:
   
   ContainerBase(const std::string & filename,
                 const std::string & tablename,
                 unsigned int maxNumEntries, 
                 const st_app::AppParGroup * pars) 
      : m_filename(filename), m_tablename(tablename),
        m_maxNumEntries(maxNumEntries), m_pars(pars), m_fileNum(0),
        m_appName(""), m_softwareVersion("") {}

   virtual ~ContainerBase() {}

   virtual void setVersion(const std::string & version) {
      m_softwareVersion = version;
   }

   virtual void setAppName(const std::string & appName) {
      m_appName = appName;
   }

   virtual std::string creator() {
      return m_appName + " " + m_softwareVersion;
   }

protected:

   /// Root name for the FITS binary table output files.
   std::string m_filename;

   /// Extension name of FITS binary table
   std::string m_tablename;

   /// The maximum number of rows to accumulate before the data are
   /// written to a FITS file and the data buffer is flushed.
   unsigned int m_maxNumEntries;

   /// Parameters from gtobssim.par for writing to EVENT and SC_DATA
   /// headers of FT1 and FT2 files, respectively.
   const st_app::AppParGroup * m_pars;

   /// The current index number of the FITS file to be written.  This
   /// number is formatted appropriately and appended to the root
   /// filename given in the constructor.
   long m_fileNum;

   /// The name of the application using this class.
   std::string m_appName;

   /// The version of the application.
   std::string m_softwareVersion;

   /// Return an output filename, based on the root name, m_filename,
   /// and the counter index, m_fileNum.
   std::string outputFileName() const;

   void writeParFileParams(tip::Header & header) const;

   /// Set the date keywords in a given header, accesses via the
   /// tip::Extension and tip::Header interface.
   static void writeDateKeywords(tip::Extension * table, double start_time,
                                 double stop_time, bool extension=true);

   /// Return an astro::JulianDate object for the current time.
   static astro::JulianDate currentTime();

private:

   void write_par_as_string(tip::Header & header,
                            const std::string & keyword,
                            const std::string & parname) const;

   void write_par_as_double(tip::Header & header,
                            const std::string & keyword,
                            const std::string & parname) const;

   void write_par_as_int(tip::Header & header,
                         const std::string & keyword,
                         const std::string & parname) const;

   void write_par_as_long(tip::Header & header,
                          const std::string & keyword,
                          const std::string & parname) const;

   void write_par_as_bool(tip::Header & header,
                          const std::string & keyword,
                          const std::string & parname) const;
};

}

#endif // observationSim_ContainerBase_h
