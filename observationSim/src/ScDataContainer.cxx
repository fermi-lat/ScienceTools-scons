/**
 * @file ScDataContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include <sstream>

#include "CLHEP/Geometry/Vector3D.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "astro/EarthCoordinate.h"

#include "flux/EventSource.h"

#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"

namespace observationSim {

ScDataContainer::~ScDataContainer() {
   if (m_scData.size() > 0) writeScData();
}

void ScDataContainer::init() {
   m_scData.clear();

   std::string rootPath(std::getenv("OBSERVATIONSIMROOT"));
   m_ft2Template = rootPath + "/data/ft2.tpl";
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

   if (m_useFT2) {
      std::string ft2File = outputFileName();
      tip::IFileSvc::instance().createFile(ft2File, m_ft2Template);
      tip::Table * my_table = 
         tip::IFileSvc::instance().editTable(ft2File, "Ext1");
      int npts = m_scData.size();
      my_table->setNumRecords(npts);
      tip::Table::Iterator it = my_table->begin();
      tip::Table::Record & row = *it;
      std::vector<ScData>::const_iterator sc = m_scData.begin();
      double start_time = m_scData.begin()->time();
      double stop_time = 2.*m_scData[npts-1].time() - m_scData[npts-2].time();
      for ( ; it != my_table->end(), sc != m_scData.end(); ++it, ++sc) {
         row["start"].set(sc->time());
         if (sc+1 != m_scData.end()) {
            row["stop"].set((sc+1)->time());
            row["livetime"].set((sc+1)->time() - sc->time());
         } else {
            row["stop"].set(stop_time);
            row["livetime"].set(stop_time - sc->time());
         }
         row["lat_geo"].set(sc->lat());
         row["lon_geo"].set(sc->lon());
         row["ra_scz"].set(sc->zAxis().ra());
         row["dec_scz"].set(sc->zAxis().dec());
         row["ra_scx"].set(sc->xAxis().ra());
         row["dec_scx"].set(sc->xAxis().dec());
      }
      EventContainer::writeDateKeywords(my_table, start_time, stop_time);
      delete my_table;
   } else { // Use the old A1 format.
      makeFitsTable();
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

// Delete the old table.
      delete m_scDataTable;
   }

// Flush the buffer...
   m_scData.clear();

// and update the m_fileNum index.
   m_fileNum++;
}

void ScDataContainer::makeFitsTable() {

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

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

   std::string outputfile = outputFileName();
   m_scDataTable = new FitsTable(outputfile, "LAT_PLM_summary",
                                 colName, fmt, unit);
}

std::string ScDataContainer::outputFileName() const {
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
   return outputfile.str();
}

} // namespace observationSim
