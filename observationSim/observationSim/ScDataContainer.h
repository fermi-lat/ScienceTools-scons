/**
 * @file ScDataContainer.h
 * @brief Declaration for ScDataContainer class.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_ScDataContainer_h
#define observationSim_ScDataContainer_h

#include <fstream>
#include <string>
#include <vector>

#include "astro/SkyDir.h"

#include "observationSim/ScData.h"
#include "observationSim/Spacecraft.h"

class EventSource;

namespace observationSim {

/**
 * @class ScDataContainer
 * @brief Stores and writes ScData to a FITS file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class ScDataContainer {

public:

   /// @param filename The root name of the output FITS file.
   /// @param maxNumEntries The maximum number of entries in the ScData
   ///        buffer before a FITS file is written.
   ScDataContainer(const std::string &filename, 
                   int maxNumEntries=20000) :
      m_filename(filename), m_fileNum(0),
      m_maxNumEntries(maxNumEntries) {init();}

   ~ScDataContainer();

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param spacecraft A pointer to the object that provides methods
   ///        for accessing spacecraft orbit and attitude info.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        ScData and then flush the buffers.
   void addScData(EventSource *event, Spacecraft *spacecraft, 
                  bool flush=false);

   /// The simulation time of the most recently added entry.
   double simTime() {return m_scData[m_scData.size()-1].time();}

private:

   /// Root name for the FITS binary table output files.
   std::string m_filename;

   /// Template file for FT2 data.
   std::string m_ft2Template;

   /// The current index number fo the FITS output files.
   long m_fileNum;

   /// The maximum number of entries in the m_scData vector.
   unsigned int m_maxNumEntries;

   /// The ScData buffer.
   std::vector<ScData> m_scData;

   /// This routine contains the constructor implementation.
   void init();

   /// This routine unpacks and writes the ScData to a FT2 file.
   void writeScData();

   /// Return an output filename, based on the root name, m_filename,
   /// and the counter index, m_fileNum.
   std::string outputFileName() const;

};

} // namespace observationSim

#endif // observationSim_ScDataContainer_h
