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
#include "observationSim/FitsTable.h"
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

   /// @param filename The name of the output FITS file.
   /// @param useA1fmt A flag to use the format that the A1 tool expects.
   ScDataContainer(const std::string &filename, bool useA1fmt=false) :
      m_useA1fmt(useA1fmt) {init(filename);}

   ScDataContainer(char *filename, bool useA1fmt=false) :
      m_useA1fmt(useA1fmt) {init(std::string(filename));}

   ~ScDataContainer() {writeScData(); delete m_scDataTable;}

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param spacecraft A pointer to the object that provides methods
   ///        for accessing spacecraft orbit and attitude info.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        ScData and then flush the buffers.
   void addScData(EventSource *event, Spacecraft *spacecraft, 
                  bool flush=false);

   /// The simulation time of the most recently added data.
   double simTime() {return m_scData[m_scData.size()-1].time();}

private:

   bool m_useA1fmt;

   long m_fileNum;

   FitsTable *m_scDataTable;
   
   std::vector<ScData> m_scData;

   void init(const std::string &filename);

   void writeScData();

};

} // namespace observationSim

#endif // observationSim_ScDataContainer_h
