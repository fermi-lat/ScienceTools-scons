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

ScDataContainer::ScDataContainer(const std::string &filename, bool useA1fmt) :
   m_useA1fmt(useA1fmt) {

   std::vector<std::string> colName;
   std::vector<std::string> fmt;
   std::vector<std::string> unit;

   if (!useA1fmt) { // the default
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
      colName.push_back("time");fmt.push_back("1D");unit.push_back("s");
      colName.push_back("SAA_flag");fmt.push_back("1I");unit.push_back("int");
   }

   m_scDataTable = new FitsTable(filename, "LAT_PLM_summary",
                                 colName, fmt, unit);

   m_scData.clear();
}

void ScDataContainer::addScData(EventSource *event, FluxMgr &fm, bool flush) {
   
   std::string name = event->fullTitle();
   if (name.find("TimeTick") != std::string::npos) {
      double time = event->time();

// The GPS instance keeps track of spacecraft data.
      GPS *gps = GPS::instance();
      gps->getPointingCharacteristics(time);

// Use FluxMgr to get the rotation matrix from instrument to Galactic
// coordinates.
      HepRotation rotationMatrix = fm.transformGlastToGalactic(time);
      Hep3Vector zAxis = rotationMatrix(Hep3Vector(0., 0., 1.));
      Hep3Vector xAxis = rotationMatrix(Hep3Vector(1., 0., 0.));

// Determine if the spacecraft is in the SAA.  GPS should provide this
// since it has a m_earthOrbit data member, but it doesn't, so we must
// do it by hand using an astro::EarthCoordinate object.
      astro::EarthCoordinate earthCoord(gps->lon(), gps->lat());

      m_scData.push_back(ScData(time, gps->RAZ(), gps->DECZ(), 
                                gps->lon(), gps->lat(), zAxis, xAxis,
                                static_cast<int>(earthCoord.insideSAA())));
   }
   if (flush) writeScData();
}

void ScDataContainer::writeScData() {

   if (!m_useA1fmt) {
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
   } else {
      std::vector<std::vector<double> > data(8);
      for (std::vector<ScData>::const_iterator it = m_scData.begin();
           it != m_scData.end(); it++) {
         data[0].push_back(it->m_xAxis.x());
         data[1].push_back(it->m_xAxis.y());
         data[2].push_back(it->m_xAxis.z());
         data[3].push_back(it->m_zAxis.x());
         data[4].push_back(it->m_zAxis.y());
         data[5].push_back(it->m_zAxis.z());
         data[6].push_back(it->m_time);
         data[7].push_back(static_cast<double>(it->m_inSaa));
      }
      m_scDataTable->writeTableData(data);
   }

   m_scData.clear();
}

} // namespace observationSim
