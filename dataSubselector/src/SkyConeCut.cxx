/**
 * @file SkyConeCut.cxx
 *
 */

#include <stdexcept>

#include "facilities/Util.h"

#include "dataSubselector/Cuts.h"

namespace dataSubselector {

Cuts::SkyConeCut::SkyConeCut(const std::string & type,
                             const std::string & unit, 
                             const std::string & value) {
   if (unit.find("deg") != 0) {
      throw std::runtime_error("dataSubselector::Cuts::SkyConeCut:\n" +
                               std::string("Unsupported unit: ") + unit);
   }
   std::vector<std::string> coordSys;
   getArgs(type, coordSys);
   std::vector<std::string> coords;
   getArgs(value, coords);
   double lon = std::atof(coords.at(0).c_str());
   double lat = std::atof(coords.at(1).c_str());
   m_radius = std::atof(coords.at(2).c_str());
   if (coordSys.at(0) == "RA") {
      m_coneCenter = astro::SkyDir(lon, lat, astro::SkyDir::EQUATORIAL);
      m_ra = lon;
      m_dec = lat;
   } else if (coordSys.at(0) == "GLON") {
      m_coneCenter = astro::SkyDir(lon, lat, astro::SkyDir::GALACTIC);
      m_ra = m_coneCenter.ra();
      m_dec = m_coneCenter.dec();
   } else {
      throw std::runtime_error("dataSubselector::Cuts::SkyConeCut:\n" +
                               std::string("Unsupported type: ") + type);
   }
}

void Cuts::SkyConeCut::getArgs(const std::string & value, 
                               std::vector<std::string> & args) const {
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(value, "()", tokens);
   facilities::Util::stringTokenize(tokens.at(1), ",", args);
}

bool Cuts::SkyConeCut::accept(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
   return accept(ra, dec);
}

bool Cuts::
SkyConeCut::accept(const std::map<std::string, double> & params) const {
   std::map<std::string, double>::const_iterator ra;
   std::map<std::string, double>::const_iterator dec;
   if ( (ra = params.find("RA")) != params.end() &&
        (dec = params.find("DEC")) != params.end() ) {
      return accept(ra->second, dec->second);
   }
   return true;
}

bool Cuts::SkyConeCut::operator==(const CutBase & arg) const {
   try {
      SkyConeCut & rhs = 
         dynamic_cast<SkyConeCut &>(const_cast<CutBase &>(arg));
      return (m_ra == rhs.m_ra && m_dec == rhs.m_dec &&
              m_radius == rhs.m_radius);
   } catch (...) {
      return false;
   }
}

void Cuts::SkyConeCut::
getKeyValues(std::string & type, std::string & unit,
             std::string & value, std::string & ref) const {
   (void)(ref);
   std::ostringstream val;
   val << "CIRCLE(" 
       << m_coneCenter.ra() << "," 
       << m_coneCenter.dec() << ","
       << m_radius << ")";
   type = "POS(RA,DEC)";
   unit = "deg";
   value = val.str();
}

bool Cuts::SkyConeCut::accept(double ra, double dec) const {
   double separation = m_coneCenter.difference(astro::SkyDir(ra, dec));
   return separation*180./M_PI <= m_radius;
}

} // namespace dataSubselector
