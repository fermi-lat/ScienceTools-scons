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

#include "Goodi/GoodiConstants.h"
#include "Goodi/DataIOServiceFactory.h"
#include "Goodi/DataFactory.h"
#include "Goodi/IDataIOService.h"
#include "Goodi/IData.h"
#include "Goodi/ISpacecraftData.h"

#include "flux/EventSource.h"
#include "flux/GPS.h"

#include "observationSim/ScDataContainer.h"

namespace observationSim {

void ScDataContainer::init() {
   m_scData.clear();

   if (m_useGoodi) {
      Goodi::DataFactory dataCreator;

// Set the type of data to be generated and the mission.
      Goodi::DataType datatype = Goodi::Spacecraft;
      Goodi::Mission mission = Goodi::Lat;

// Create the ScData object.
      m_goodiScData = dynamic_cast<Goodi::ISpacecraftData *>
         (dataCreator.create(datatype, mission));

   }
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

   if (m_useGoodi) {
      unsigned int npts = m_scData.size();
      std::vector<double> startTime(npts);
      std::vector<double> stopTime(npts);
      std::vector< std::pair<double, double> > gti(npts);
      std::vector<float> latGeo(npts);
      std::vector<float> lonGeo(npts);
      std::vector<float> raSCZ(npts);
      std::vector<float> decSCZ(npts);
      std::vector<float> raSCX(npts);
      std::vector<float> decSCX(npts);

      std::vector<ScData>::const_iterator scIt = m_scData.begin();
      for (unsigned int i = 0; scIt != m_scData.end(); scIt++, i++) {
         startTime[i] = scIt->time();
         gti[i].first = startTime[i];
         if (i > 0) {
            stopTime[i] = startTime[i-1];
            gti[i].second = stopTime[i];
         }
         latGeo[i] = scIt->lat();
         lonGeo[i] = scIt->lon();
         raSCZ[i] = scIt->zAxis().ra();
         decSCZ[i] = scIt->zAxis().dec();
         raSCX[i] = scIt->xAxis().ra();
         decSCX[i] = scIt->xAxis().dec();
      }
      m_goodiScData->setStartTime(startTime);
      m_goodiScData->setStopTime(stopTime);
      m_goodiScData->setGTI(gti);
      m_goodiScData->setLatGeo(latGeo);
      m_goodiScData->setLonGeo(lonGeo);
      m_goodiScData->setRAscz(raSCZ);
      m_goodiScData->setDECscz(decSCZ);
      m_goodiScData->setRAscx(raSCX);
      m_goodiScData->setDECscx(decSCX);

// Goodi I/O service object.
      Goodi::DataIOServiceFactory iosvcCreator;
      Goodi::IDataIOService *goodiIoService = iosvcCreator.create();

      std::string outputFile = "!" + outputFileName();
      m_goodiScData->write(goodiIoService, outputFile);
      delete goodiIoService;

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
