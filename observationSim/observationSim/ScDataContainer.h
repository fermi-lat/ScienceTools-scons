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

   /// @param filename The root name of the output FITS file.
   /// @param useA1fmt A flag to use the format that the A1 tool expects.
   /// @param maxNumEntries The maximum number of entries in the ScData
   ///        buffer before a FITS file is written.
   ScDataContainer(const std::string &filename, bool useA1fmt=false,
                   int maxNumEntries=20000) :
      m_filename(filename), m_useA1fmt(useA1fmt), m_fileNum(0),
      m_maxNumEntries(maxNumEntries) {init();}

   /// This version is provided for SWIG/Python.
   ScDataContainer(char *filename, bool useA1fmt=false,
                   int maxNumEntries=20000) :
      m_filename(filename), m_useA1fmt(useA1fmt), m_fileNum(0),
      m_maxNumEntries(maxNumEntries) {init();}

   ~ScDataContainer() {if (m_scData.size() > 0) writeScData();}

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

   /// Flag to indicate that A1 (Likelihood prototype) formatting of
   /// the FITS files will be used.
   bool m_useA1fmt;

   /// The current index number fo the FITS output files.
   long m_fileNum;

   /// The maximum number of entries in the m_scData vector.
   int m_maxNumEntries;

   /// The FITS binary table object that steers the cfitsio routines.
   FitsTable *m_scDataTable;
   
   /// The ScData buffer.
   std::vector<ScData> m_scData;

   /// This routine contains the constructor implementation.
   void init();

   /// A routine the create the FITS binary table object.
   void makeFitsTable(); 

   /// This routine unpacks m_scData and calls the writeTableData(...)
   /// method of the m_scDataTable object.
   void writeScData();

};

} // namespace observationSim

#endif // observationSim_ScDataContainer_h
