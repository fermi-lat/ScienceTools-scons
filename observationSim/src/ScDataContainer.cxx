/**
 * @file ScDataContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include <sstream>
#include <stdexcept>

#include "CLHEP/Geometry/Vector3D.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

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

   char * root_path = std::getenv("OBSERVATIONSIMROOT");
   if (root_path != 0) {
      m_ftTemplate = std::string(root_path) + "/data/ft2.tpl";
   } else {
      throw std::runtime_error(std::string("Environment variable ") 
                               + "OBSERVATIONSIMROOT not set.");
   }
}

void ScDataContainer::addScData(EventSource *event, Spacecraft *spacecraft,
                                bool flush) {
   double time = event->time();
   addScData(time, spacecraft, flush);
}

void ScDataContainer::addScData(double time, Spacecraft * spacecraft, 
                                bool flush) {
   try {
      astro::SkyDir zAxis = spacecraft->zAxis(time);
      astro::SkyDir xAxis = spacecraft->xAxis(time);
      std::vector<double> scPosition;
      spacecraft->getScPosition(time, scPosition);

      m_scData.push_back(ScData(time, zAxis.ra(), zAxis.dec(), 
                                spacecraft->EarthLon(time), 
                                spacecraft->EarthLat(time),
                                zAxis, xAxis, spacecraft->inSaa(time),
                                scPosition));
   } catch (std::exception & eObj) {
      if (!st_facilities::Util::expectedException(eObj,"Time out of Range!")) {
         throw;
      }
   }
   if (flush || m_scData.size() >= m_maxNumEntries) writeScData();
}

void ScDataContainer::writeScData() {

   std::string ft2File = outputFileName();
   tip::IFileSvc::instance().createFile(ft2File, m_ftTemplate);
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
      row["sc_position"].set(sc->position());
   }
   writeDateKeywords(my_table, start_time, stop_time);
   delete my_table;

// Flush the buffer...
   m_scData.clear();

// and update the m_fileNum index.
   m_fileNum++;
}

} // namespace observationSim
