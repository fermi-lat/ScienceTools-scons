/**
 * @file CutParameters.cxx
 * @brief  Method definitions for the CutParameters class
 *
 * $Header$
 */

#include <cmath>

#include "st_app/AppParGroup.h"

#include "dataSubselector/CutParameters.h"
// #include "dataSubselector/Cuts.h"
// namespace dataSubselector {

CutParameters::CutParameters(st_app::AppParGroup & pars) : 
   m_query(""),  m_headerString("") {
   
   m_RA = pars["ra"];
   m_Dec = pars["dec"];
   m_radius = pars["rad"];
   if (m_RA == 0 && m_Dec == 0 && m_radius == 0) {
      m_RA = -1;
      std::cout << "resetting default RA" << std::endl;
   }
   m_lonMin = pars["lonMin"];
   m_lonMax = pars["lonMax"];
   m_latMin = pars["latMin"];
   m_latMax = pars["latMax"];
   std::string coordSys = pars["coordSys"];
   m_coordSys = coordSys;
   m_tmin = pars["tmin"];
   m_tmax = pars["tmax"];
   m_emin = pars["emin"];
   m_emax = pars["emax"];
   m_phimin = pars["phimin"];
   m_phimax = pars["phimax"];
   m_thetamin = pars["thetamin"];
   m_thetamax = pars["thetamax"];
   m_gammaProbMin = pars["gammaProbMin"];
   m_gammaProbMax = pars["gammaProbMax"];
   m_zmin = pars["zmin"];
   m_zmax = pars["zmax"];
   m_convLayerMin = pars["convLayerMin"];
   m_convLayerMax = pars["convLayerMax"];
   m_calibVersion[0] = pars["bgcut"];
   m_calibVersion[1] = pars["psfcut"];
   m_calibVersion[2] = pars["erescut"];
   setFilterExpression();
}

bool CutParameters::acceptRow(tip::ConstTableRecord & row) const {
   double ra, dec;
   row["RA"].get(ra);
   row["DEC"].get(dec);
   return withinCoordLimits(ra, dec);
}


/**
 * @brief Builds the FITS row filtering expression from the cut parameters
 * 
 * This function builds a valid FITS row filtering expression based on
 * the cut parameters specified by the user.  It also store both the
 * FITS expression and a string to be entered into the header file
 * history in member variables.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  3 Dec 2003
 */

void CutParameters::setFilterExpression() {
   static double DEG_TO_RAD = M_PI/180.;
   if (m_RA >= 0) {
// We have a new area, so apply haversine search on the area.
      std::ostringstream q, hs;
      q << "((2*asin(min(1,sqrt(max(0,(sin((DEC-" << m_Dec << ")*" 
        << DEG_TO_RAD/2;
      q << ")*sin((DEC-" << m_Dec << ")*" << DEG_TO_RAD/2 
        << "))+(cos(DEC*" <<DEG_TO_RAD;
      q << ")*" << std::cos(m_Dec*DEG_TO_RAD) 
        << "*sin((RA-" << m_RA << ")*" << DEG_TO_RAD/2;
      q << ")*sin((RA-" << m_RA << ")*" << DEG_TO_RAD/2 << ")))))))<" 
        << m_radius*DEG_TO_RAD << ")";
      m_query = q.str();

      hs << "CIRCLE(" << m_RA << "," << m_Dec << "," << m_radius << ",RA,DEC)";
      m_headerString = hs.str();
   }
   
   if (m_tmin) addParameterToQuery("TIME>=", m_tmin); 
   if (m_tmax) addParameterToQuery("TIME<=", m_tmax); 
   if (m_emin) addParameterToQuery("ENERGY>=", m_emin); 
   if (m_emax) addParameterToQuery("ENERGY<=", m_emax);
   if (m_phimin) addParameterToQuery("PHI>=", m_phimin); 
   if (m_phimax) addParameterToQuery("PHI<=", m_phimax);
   if (m_thetamin) addParameterToQuery("THETA>=", m_thetamin); 
   if (m_thetamax) addParameterToQuery("THETA<=", m_thetamax);
   if (m_gammaProbMin) addParameterToQuery("IMGAMMAPROB>=", m_gammaProbMin); 
   if (m_gammaProbMax) addParameterToQuery("IMGAMMAPROB>=", m_gammaProbMax);
   if (m_zmin) addParameterToQuery("ZENITH_ANGLE>=", m_zmin); 
   if (m_zmax) addParameterToQuery("ZENITH_ANGLE<=", m_zmax);
   if (m_convLayerMin != 0) addParameterToQuery("CONVERSION_LAYER>=", 
                                                m_convLayerMin);
   if (m_convLayerMax != 15) addParameterToQuery("CONVERSION_LAYER<=", 
                                                 m_convLayerMax);

// These check the background cuts
   if (m_calibVersion[0]) addParameterToQuery("CALIB_VERSION[1]==", 
                                              m_calibVersion[0]);
   if (m_calibVersion[1]) addParameterToQuery("CALIB_VERSION[2]==", 
                                              m_calibVersion[1]);
   if (m_calibVersion[2]) addParameterToQuery("CALIB_VERSION[3]==", 
                                              m_calibVersion[2]);
// Add query delimiters   
   m_headerString = "@" + m_headerString + "@";
}

/**
 * @brief This function adds the data subspace keywords to the event header
 * 
 * The addDataSubspaceKeywords() function adds the relevant data
 * subspace keywords to the Events header to desginate the type of
 * search that was done on the data to create the file.  This version
 * is modified from the one in the D1 Database
 * 
 * @param ios  The ioservice to write the subspace keywords to
 * 
 * @author Tom Stephens
 * @date Created:  30 Oct 2003
 * @date Last Modified:  3 Dec 2003
 */

void CutParameters::addDataSubspaceKeywords(tip::Table * table) {
  
   tip::Header & header = table->getHeader();

// First read in the number of DS keys already in the fits file
   int nKeys(0);
   try {
      header["NDSKEYS"].get(nKeys);
   } catch (tip::TipException) {
// If the keyword doesn't exist then the file didn't come from the D1 database.
// We'll let the user know and continue on.
      std::cout << "No NDSKEYS keyword in event header.  "
                << "This file not generated by\n"
                << "the D1 Database.  Continuing anyway.\n";  
   }
   
   if (m_RA >= 0) {  // position header keyword
      std::string type("POS(RA,DEC)");
      std::string unit("deg");
      std::ostringstream val, key1, key2, key3;
      val << "CIRCLE(" << m_RA << "," << m_Dec << "," << m_radius << ")";
      nKeys++;
      key1 << "DSTYP" << nKeys;
      key2 << "DSUNI" << nKeys;
      key3 << "DSVAL" << nKeys;

      header[key1.str()].set(type);
      header[key2.str()].set(unit);
      header[key3.str()].set(val.str());
   }
   if (m_tmin||m_tmax) 
      addDSKeywordEntry("TIME", "s", m_tmin, m_tmax, nKeys, header);
   if (m_emin||m_emax) 
      addDSKeywordEntry("ENERGY", "MeV", m_emin, m_emax, nKeys, header);
   if (m_phimin||m_phimax)
      addDSKeywordEntry("PHI", "deg", m_phimin, m_phimax, nKeys, header);
   if (m_thetamin||m_thetamax)
      addDSKeywordEntry("THETA", "deg", m_thetamin, m_thetamax, nKeys, header);
   if (m_gammaProbMin||m_gammaProbMax) 
      addDSKeywordEntry("IMGAMMAPROB", "dimensionless", m_gammaProbMin, 
                        m_gammaProbMax, nKeys, header);
   if (m_zmin||m_zmax) 
      addDSKeywordEntry("ZENTIH_ANGLE", "deg", m_zmin, m_zmax, nKeys, header);
   if (m_calibVersion[0]) 
      addDSKeywordEntry("CALIB_VERSION[1]", "dimensionless",
                        m_calibVersion[0], nKeys, header);
   if (m_calibVersion[1]) 
      addDSKeywordEntry("CALIB_VERSION[2]", "dimensionless",
                        m_calibVersion[1], nKeys, header);
   if (m_calibVersion[2]) 
      addDSKeywordEntry("CALIB_VERSION[3]", "dimensionless",
                        m_calibVersion[2], nKeys, header);
   
   // update the NDSKEYS keyword with the new value
   header["NDSKEYS"].set(nKeys);
}

//}
