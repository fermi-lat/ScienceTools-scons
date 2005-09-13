/**
 * @file CutController.cxx
 *
 * $Header$
 */

#include "facilities/Util.h"
#include "st_app/AppParGroup.h"
#include "tip/Table.h"
#include "astro/SkyDir.h"

#include "dataSubselector/CutController.h"
#include "dataSubselector/Gti.h"

namespace dataSubselector {

CutController * CutController::s_instance(0);

CutController * CutController::instance(st_app::AppParGroup & pars, 
                                        const std::string & eventFile,
                                        const std::string & evtable) {
   if (!s_instance) {
      s_instance = new CutController(pars, eventFile, evtable);
   }
   return s_instance;
}

void CutController::delete_instance() {
   delete s_instance;
   s_instance = 0;
}

CutController::CutController(st_app::AppParGroup & pars, 
                             const std::string & eventFile,
                             const std::string & evtable) 
   : m_pars(pars), m_cuts(eventFile, evtable) {
   double ra = pars["ra"], dec = pars["dec"], radius = pars["rad"];
   if (ra != 0 && dec != 0 && radius !=0) {
      m_cuts.addSkyConeCut(ra, dec, radius);
   }
   addRangeCut("TIME", "s", pars["tmin"], pars["tmax"]);
   addRangeCut("ENERGY", "MeV", pars["emin"], pars["emax"]);
   addRangeCut("PHI", "deg", pars["phimin"], pars["phimax"]);
   addRangeCut("THETA", "deg", pars["thetamin"], pars["thetamax"]);
   addRangeCut("IMGAMMAPROB", "dimensionless",
               pars["gammaProbMin"], pars["gammaProbMax"]);
   addRangeCut("ZENITH_ANGLE", "deg", pars["zmin"], pars["zmax"]);
   unsigned int layerMin = pars["convLayerMin"];
   unsigned int layerMax = pars["convLayerMax"];
   addRangeCut("CONVERSION_LAYER", "dimensionless", layerMin, layerMax);
   if (pars["bgcut"]) {
      addRangeCut("CALIB_VERSION[1]", "dimensionless", 1, 1, 1);
   }                  
   if (pars["psfcut"]) {
      addRangeCut("CALIB_VERSION[2]", "dimensionless", 1, 1, 2);
   }
   if (pars["erescut"]) {
      addRangeCut("CALIB_VERSION[3]", "dimensionless", 1, 1, 3);
   }
//   updateGti(eventFile);
}

bool CutController::accept(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
   return m_cuts.accept(row) && withinCoordLimits(ra, dec);
}

void CutController::addRangeCut(const std::string & colname,
                                const std::string & unit,
                                double minVal, double maxVal, 
                                unsigned int indx) {
   RangeCut::IntervalType type;
   if (minVal == 0 && maxVal == 0) {
      return;
   }
   if (minVal != 0 && maxVal != 0) {
      type = RangeCut::CLOSED;
   } else if (minVal != 0 && maxVal == 0) {
      type = RangeCut::MINONLY;
   } else if (minVal == 0 && maxVal != 0) {
      type = RangeCut::MAXONLY;
   }
   std::vector<std::string> tokens;
   facilities::Util::stringTokenize(colname, "[]", tokens);
   m_cuts.addRangeCut(tokens.at(0), unit, minVal, maxVal, type, indx);
}

bool CutController::withinCoordLimits(double ra, double dec) const {
   double lonMin = m_pars["lonMin"], lonMax = m_pars["lonMax"],
      latMin = m_pars["latMin"], latMax = m_pars["latMax"];
   if (m_pars["coordSys"] == "GAL") {
      astro::SkyDir dir(ra, dec);
      return (lonMin <= dir.l() && dir.l() <= lonMax && 
              latMin <= dir.b() && dir.b() <= latMax);
   } else {
      return (lonMin <= ra && ra <= lonMax && 
              latMin <= dec && dec <= latMax);
   }
   return false;
}

void CutController::updateGti(const std::string & eventFile) const {
   Gti gti(eventFile);
   for (unsigned int i = 0; i < m_cuts.size(); i++) {
      if (m_cuts[i].type() == "range") {
         const RangeCut & my_cut = 
            dynamic_cast<RangeCut &>(const_cast<CutBase &>(m_cuts[i]));
         if (my_cut.colname() == "TIME") {
            if (my_cut.intervalType() == RangeCut::CLOSED) {
               gti = gti.applyTimeRangeCut(my_cut.minVal(), my_cut.maxVal());
            } else if (my_cut.intervalType() == RangeCut::MINONLY) {
               gti = gti.applyTimeRangeCut(my_cut.minVal(), gti.maxValue());
            } else if (my_cut.intervalType() == RangeCut::MAXONLY) {
               gti = gti.applyTimeRangeCut(gti.minValue(), my_cut.maxVal());
            }
         }
      }
   }
   gti.writeExtension(eventFile);
}

std::string CutController::filterString() const {
   std::string filter(m_cuts.filterString());
   if (filter != "") {
      filter += " && ";
   }
   filter += "gtifilter()";
   return filter;
}

} // namespace dataSubselector
