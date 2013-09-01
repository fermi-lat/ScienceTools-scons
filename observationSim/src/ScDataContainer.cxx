/**
 * @file ScDataContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include "CLHEP/Geometry/Vector3D.h"

#include "astro/EarthCoordinate.h"

#include "Likelihood/Aeff.h"
#include "Likelihood/Psf.h"
#include "observationSim/ScDataContainer.h"

namespace observationSim {

ScDataContainer::ScDataContainer(const std::string &filename) {

//    m_outfile = new std::ofstream(filename.c_str());
//    if (!m_outfile) {
//       std::cerr << "ScDataContainer::ScDataContainer:\n"
//                 << "Failed to open output file "
//                 << filename << std::endl;
//       exit(-1);
//    }

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

   colName.push_back("time"); fmt.push_back("1E"); unit.push_back("seconds");
   colName.push_back("RA_z"); fmt.push_back("1E"); unit.push_back("degrees");
   colName.push_back("Dec_z"); fmt.push_back("1E"); unit.push_back("degrees");
   colName.push_back("lon"); fmt.push_back("1E"); unit.push_back("degrees");
   colName.push_back("lat"); fmt.push_back("1E"); unit.push_back("degrees");
   colName.push_back("inSAA"); fmt.push_back("1I"); unit.push_back("integer");

   m_scDataTable = new FitsTable(filename, "Spacecraft Data", 
                                 colName, fmt, unit);

   m_scData.clear();
}

void ScDataContainer::addScData(EventSource *event, bool flush) {
   
   std::string name = event->fullTitle();
   if (name.find("TimeTick") != std::string::npos) {
      double time = event->time();

// The GPS instance keeps track of spacecraft data.
      GPS *gps = GPS::instance();
      gps->getPointingCharacteristics(time);

// Determine if the spacecraft is in the SAA.  GPS should provide this
// since it has a m_earthOrbit data member, but it doesn't, so we must
// do it by hand using an astro::EarthCoordinate object.
      astro::EarthCoordinate earthCoord(gps->lon(), gps->lat());

      m_scData.push_back(ScData(time, gps->RAZ(), gps->DECZ(), 
                                gps->lon(), gps->lat(),
                                static_cast<int>(earthCoord.insideSAA())));
   }
   if (flush) writeScData();
}

void ScDataContainer::writeScData() {
// // Write as ASCII until FITS handler is implemented.
//    for (std::vector<ScData>::const_iterator it = m_scData.begin();
//         it != m_scData.end(); it++) {
//       *m_outfile << it->m_time << "  "
//                  << it->m_RAz << "  "
//                  << it->m_Decz << "  "
//                  << it->m_lon << "  "
//                  << it->m_lat << "  "
//                  << it->m_inSaa << std::endl;
//    }

   std::vector<std::vector<double> > data(6);

   for (std::vector<ScData>::const_iterator it = m_scData.begin();
        it != m_scData.end(); it++) {
      data[0].push_back(it->m_time);
      data[1].push_back(it->m_RAz);
      data[2].push_back(it->m_Decz);
      data[3].push_back(it->m_lon);
      data[4].push_back(it->m_lat);
      data[5].push_back(static_cast<double>(it->m_inSaa));
   }

   m_scDataTable->writeTableData(data);

   m_scData.clear();
}

} // namespace observationSim
