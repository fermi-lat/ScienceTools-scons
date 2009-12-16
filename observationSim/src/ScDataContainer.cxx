/**
 * @file ScDataContainer.cxx
 * @brief Implementation for class that keeps track of events and when they
 * get written to a FITS file.
 * @author J. Chiang
 * $Header$
 */

#include <cstdlib>
#include <sstream>
#include <stdexcept>

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "astro/EarthCoordinate.h"

#include "fitsGen/Ft2File.h"

#include "flux/EventSource.h"

#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"

namespace {
   double geomag_lat(const std::vector<double> & scPosition,
                     double met) {
      CLHEP::Hep3Vector pos(scPosition.at(0)/1e3, scPosition.at(1)/1e3,
                            scPosition.at(2)/1e3);
      astro::EarthCoordinate coord(pos, met);
      return coord.geolat();
   }
}

namespace observationSim {

ScDataContainer::~ScDataContainer() {
   if (m_scData.size() > 0) writeScData();
}

void ScDataContainer::init() {
   m_scData.clear();
}

void ScDataContainer::addScData(EventSource * event, Spacecraft * spacecraft,
                                bool flush) {
   double time = event->time();
// Kluge required because the flux package is utterly incapable of
// providing an event time of zero, passing instead 1e-30 (for some
// unknown reason):
   if (time < 1.1e-30 && time > 0) {
      time = 0;
   }
   addScData(time, spacecraft, flush);
}

void ScDataContainer::addScData(double time, Spacecraft * spacecraft, 
                                bool flush) {
   try {
      astro::SkyDir zAxis = spacecraft->zAxis(time);
      astro::SkyDir xAxis = spacecraft->xAxis(time);
      std::vector<double> scPosition;
      spacecraft->getScPosition(time, scPosition);
      double raZenith, decZenith;
      spacecraft->getZenith(time, raZenith, decZenith);
      double livetimeFrac = spacecraft->livetimeFrac(time);

      m_scData.push_back(ScData(time, zAxis.ra(), zAxis.dec(), 
                                spacecraft->EarthLon(time), 
                                spacecraft->EarthLat(time),
                                zAxis, xAxis, spacecraft->inSaa(time),
                                scPosition, raZenith, decZenith,
                                livetimeFrac));
   } catch (std::exception & eObj) {
      if (!st_facilities::Util::expectedException(eObj,"Time out of Range!")) {
         throw;
      }
   }
   if (flush || m_scData.size() >= m_maxNumEntries) writeScData();
}

void ScDataContainer::writeScData() {
   if (m_writeData) {
      std::string ft2File = outputFileName();
      long npts(m_scData.size());

      fitsGen::Ft2File ft2(ft2File, npts, m_tablename);

      double start_time = m_scData.begin()->time();
      double stop_time = 2.*m_scData[npts-1].time() - m_scData[npts-2].time();
      ft2.setObsTimes(start_time, stop_time);

      std::vector<ScData>::const_iterator sc = m_scData.begin();
      for ( ; ft2.itor() != ft2.end() && sc != m_scData.end();
            ft2.next(), ++sc) {
         ft2["start"].set(sc->time());
         double interval;
         if (sc+1 != m_scData.end()) {
            ft2["stop"].set((sc+1)->time());
            interval = (sc+1)->time() - sc->time();
         } else {
            ft2["stop"].set(stop_time);
            interval = stop_time - sc->time();
         }
         ft2["livetime"].set(sc->livetimeFrac()*interval);
         ft2["lat_geo"].set(sc->lat());
         ft2["lon_geo"].set(sc->lon());
         double met((ft2["start"].get() + ft2["stop"].get())/2.);
         ft2["geomag_lat"].set(::geomag_lat(sc->position(), met));
//          ft2["ra_scz"].set(sc->zAxis().ra());
//          ft2["dec_scz"].set(sc->zAxis().dec());
//          ft2["ra_scx"].set(sc->xAxis().ra());
//          ft2["dec_scx"].set(sc->xAxis().dec());
         ft2.setScAxes(sc->zAxis().ra(), sc->zAxis().dec(),
                       sc->xAxis().ra(), sc->xAxis().dec());
         ft2["sc_position"].set(sc->position());
         ft2["ra_zenith"].set(sc->raZenith());
         ft2["dec_zenith"].set(sc->decZenith());
         ft2["in_saa"].set(sc->inSaa());
         if (sc->inSaa()) {
            ft2["livetime"].set(0);
         }
      }
      ft2.setPhduKeyword("FILENAME", ft2File);
      ft2.setPhduKeyword("VERSION", 1);
      ft2.setPhduKeyword("CREATOR", creator());

      ft2.close();

      st_facilities::FitsUtil::writeChecksums(ft2File);

      m_fileNum++;
   }

   m_scData.clear();
}

} // namespace observationSim
