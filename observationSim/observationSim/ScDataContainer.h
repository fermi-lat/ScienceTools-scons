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

#include "FluxSvc/../src/EventSource.h"
#include "FluxSvc/../src/FluxMgr.h"
#include "observationSim/ScData.h"
#include "observationSim/FitsTable.h"

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
   ScDataContainer(const std::string &filename, bool useA1fmt=false);

   ~ScDataContainer() {writeScData(); delete m_scDataTable;}

   /// @param event A pointer to the current EventSource object
   ///        that was provided by the FluxMgr object.
   /// @param fm The FluxMgr object.
   /// @param flush A flag to indicate whether to write the accumulated
   ///        ScData and then flush the buffers.
   void addScData(EventSource *event, FluxMgr &fm, bool flush=false);

private:

   bool m_useA1fmt;

   long m_fileNum;

   FitsTable *m_scDataTable;
   
   std::vector<ScData> m_scData;

   void writeScData();

};

} // namespace observationSim

#endif // observationSim_ScDataContainer_h
