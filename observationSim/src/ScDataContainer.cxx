/**
 * @file ScDataContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include <sstream>

#include "CLHEP/Geometry/Vector3D.h"

#include "astro/EarthCoordinate.h"

// #include "FluxSvc/../src/EventSource.h"
// #include "FluxSvc/../src/GPS.h"

#include "flux/EventSource.h"
#include "flux/GPS.h"

#include "observationSim/ScDataContainer.h"

namespace observationSim {

void ScDataContainer::init() {
   m_scData.clear();
}

void ScDataContainer::addScData(EventSource *event, Spacecraft *spacecraft,
                                bool flush) {
   
   double time = event->time();

   astro::SkyDir zAxis = spacecraft->zAxis(time);
   astro::SkyDir xAxis = spacecraft->xAxis(time);

   m_scData.push_back(ScData(time, zAxis.ra(), zAxis.dec(), 
                             spacecraft->EarthLon(time), 
                             spacecraft->EarthLat(time),
                             zAxis, xAxis, spacecraft->inSaa(time)));
   if (flush || m_scData.size() >= m_maxNumEntries) writeScData();
}

void ScDataContainer::writeScData() {

   makeFitsTable();
   if (!m_useA1fmt) {
      std::vector<std::vector<double> > data(6);
      for (std::vector<ScData>::const_iterator it = m_scData.begin();
           it != m_scData.end(); it++) {
         data[0].push_back(it->time());
         data[1].push_back(it->raz());
         data[2].push_back(it->decz());
         data[3].push_back(it->lon());
         data[4].push_back(it->lat());
         data[5].push_back(static_cast<double>(it->inSaa()));
      }
      m_scDataTable->writeTableData(data);
   } else {
      std::vector<std::vector<double> > data(10);
// pre-allocate the memory for each vector
      for (std::vector<std::vector<double> >::iterator vec_it = data.begin();
           vec_it != data.end(); vec_it++)
         vec_it->reserve(m_scData.size());
      for (std::vector<ScData>::const_iterator it = m_scData.begin();
           it != m_scData.end(); it++) {
         data[0].push_back(it->xAxis().dir().x());
         data[1].push_back(it->xAxis().dir().y());
         data[2].push_back(it->xAxis().dir().z());
         data[3].push_back(it->zAxis().dir().x());
         data[4].push_back(it->zAxis().dir().y());
         data[5].push_back(it->zAxis().dir().z());
         data[6].push_back(it->lon());
         data[7].push_back(it->lat());
         data[8].push_back(it->time());
         data[9].push_back(static_cast<double>(it->inSaa()));
      }
      m_scDataTable->writeTableData(data);
   }
// Delete the old table...
   delete m_scDataTable;

// flush the buffer...
   m_scData.clear();

// and update the m_fileNum index.
   m_fileNum++;
}

void ScDataContainer::makeFitsTable() {

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

   if (!m_useA1fmt) { // the default
      colName.push_back("time");fmt.push_back("1E");unit.push_back("seconds");
      colName.push_back("RA_z");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("Dec_z");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("lon");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("lat");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("inSAA");fmt.push_back("1I");unit.push_back("integer");
   } else {
      colName.push_back("SC_x0");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x1");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x2");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_x");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_y");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("SC_z");fmt.push_back("1E");unit.push_back("dir cos");
      colName.push_back("lon");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("lat");fmt.push_back("1E");unit.push_back("degrees");
      colName.push_back("time");fmt.push_back("1D");unit.push_back("s");
      colName.push_back("SAA_flag");fmt.push_back("1I");unit.push_back("int");
   }

// Create the FITS filename using the root name, m_filename, and
// m_fileNum.
   std::ostringstream outputfile;
   outputfile << m_filename;
   if (m_fileNum < 10) {
      outputfile << "_000";
   } else if (m_fileNum < 100) {
      outputfile << "_00";
   } else if (m_fileNum < 1000) {
      outputfile << "_0";
   } else {
      std::cerr << "Too many ScData output files." << std::endl;
      exit(-1);
   }
   outputfile << m_fileNum << ".fits";

   m_scDataTable = new FitsTable(outputfile.str(), "LAT_PLM_summary",
                                 colName, fmt, unit);
}

} // namespace observationSim
