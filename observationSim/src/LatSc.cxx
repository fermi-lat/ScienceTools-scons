/**
 * @file LatSc.cxx
 * @brief Implementation of LatSc class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "astro/EarthCoordinate.h"
#include "astro/PointingTransform.h"
#include "astro/GPS.h"

#include "LatSc.h"

namespace observationSim {

LatSc::LatSc(const std::string & ft2file) : Spacecraft() {
   const tip::Table * scData = 
      tip::IFileSvc::instance().readTable(ft2file, "SC_DATA");
   tip::Table::ConstIterator it(scData->begin());
   tip::ConstTableRecord & row(*it);
   for ( ; it != scData->end(); ++it) {
      m_start.push_back(row["START"].get());
      m_stop.push_back(row["STOP"].get());
      double duration(m_stop.back() - m_start.back());
      m_livetimefrac.push_back(row["livetime"].get()/duration);
   }
   m_dt = (m_stop.back() - m_start.front())/m_start.size();
   delete scData;
}

astro::SkyDir LatSc::zAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(0, 0, 1)),
                        astro::SkyDir::EQUATORIAL);
}

astro::SkyDir LatSc::xAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(1, 0, 0)),
                        astro::SkyDir::EQUATORIAL);
}

double LatSc::EarthLon(double time) {
   astro::GPS * gps(astro::GPS::instance());
   gps->time(time);
   return gps->lon();
}

double LatSc::EarthLat(double time) {
   astro::GPS * gps(astro::GPS::instance());
   gps->time(time);
   return gps->lat();
}

HepRotation LatSc::InstrumentToCelestial(double time) {
   astro::GPS * gps(astro::GPS::instance());
   gps->time(time);
   astro::PointingTransform transform(gps->zAxisDir(), gps->xAxisDir());
   return transform.localToCelestial();
}

bool LatSc::inSaa(double time) {
   if (::getenv("DISABLE_SAA")) {
      return false;
   }
   astro::GPS * gps(astro::GPS::instance());
   return gps->earthpos(time).insideSAA();
}

void LatSc::getScPosition(double time, std::vector<double> & position) {
   Hep3Vector pos = astro::GPS::instance()->position(time);
   position.clear();
// GPS returns the position in units of km, but FT2 wants meters so
// we multiply by 10^3.
   double mperkm(1e3);
   position.push_back(pos.x()*mperkm);
   position.push_back(pos.y()*mperkm);
   position.push_back(pos.z()*mperkm);
}

void LatSc::getZenith(double time, double & ra, double & dec) {
   astro::GPS * gps(astro::GPS::instance());
   gps->time(time);
   astro::SkyDir zenithDir(gps->zenithDir());
   ra = zenithDir.ra();
   dec = zenithDir.dec();
}

double LatSc::livetimeFrac(double time) const {
   if (m_start.size() == 0) { // We are not using an FT2 file.
      return Spacecraft::livetimeFrac(time);
   }
   if (time < m_start.front() || time > m_start.back()) {
      return 0;
   }
   size_t indx = (std::upper_bound(m_start.begin(), m_start.end(), time)
                  - m_start.begin() - 1);
   if (m_start.at(indx) <= time && time <= m_stop.at(indx)) {
      return m_livetimefrac.at(indx);
   }
   return 0;
}

} // namespace observationSim
