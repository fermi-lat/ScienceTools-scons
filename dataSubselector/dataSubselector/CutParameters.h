/**
 * @file CutParameters.h
 * @brief Header file for the CutParameters class
 *
 * $Header$
 */
 
#ifndef dataFilter_CUTPARAMETERS_H
#define dataFilter_CUTPARAMETERS_H

#include <iostream>
#include <string>
#include <sstream>

#include "astro/SkyDir.h"

#include "tip/Table.h"
#include "tip/Header.h"

// flag for debug printing
#define DEBUG 0

namespace st_app {
   class AppParGroup;
}

/**
 * @class CutParameters
 * @brief definition of CutParameters class
 * 
 * This class holds the parameters of the cut to be applied to the data.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  25 Nov 2003
 * @version $Revision$
 * 
 * $Header$
 */

class CutParameters {
  
public:

   CutParameters(st_app::AppParGroup & pars);

   ~CutParameters() {}

   void addDataSubspaceKeywords(tip::Table * table);
   
   const std::string & fitsQueryString() const {return m_query;}

   const std::string & headerString() const {return m_headerString;}

   bool withinCoordLimits(double ra, double dec) {
      if (m_coordSys == "GAL") {
         astro::SkyDir dir(ra, dec);
         return m_lonMin <= dir.l() && dir.l() <= m_lonMax && 
            m_latMin <= dir.b() && dir.b() <= m_latMax;
      } else {
         return m_lonMin <= ra && ra <= m_lonMax && 
            m_latMin <= dec && dec <= m_latMax;
      }
      return false;
   }
  
private:
   
   float m_RA;
   float m_Dec;
   float m_radius;
   float m_lonMin;
   float m_lonMax;
   float m_latMin;
   float m_latMax;
   std::string m_coordSys;
   double m_tmin;
   double m_tmax;
   float m_emin;
   float m_emax;
   float m_phimin;
   float m_phimax;
   float m_thetamin;
   float m_thetamax;
   float m_gammaProbMin;
   float m_gammaProbMax;
   float m_zmin;
   float m_zmax;

   std::string m_query;
   std::string m_headerString;
   short m_calibVersion[3];

   void setFilterExpression();

/**
 * @brief adds the condition for the given parameter to the query and
 * header string
 * 
 * The addParameterToQuery() function adds the input text and cut
 * parameter to the cfitsio query string and header string.  It is a
 * template function so it can handle any type of parameter value.
 * 
 * @param text string holding the FITS column name to test against
 * @param param the CutParameters data member to include in the cut
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  void addParameterToQuery(std::string text, T &param) {
    // make a pair of stringstreams to facilitate writing out the data
    std::ostringstream q;
    std::ostringstream hs;
    
    if (m_query != "") {
       q << "&&";
       hs << "&&";
    }
    // add the text and parameter value to the stream
    q << text << param;
    hs << text << param;
    // append the values of the stringstreams back to the data strings
    m_query += q.str();
    m_headerString += hs.str();
  }

/**
 * @brief Adds the Data Subspace keywords for the given parameters to
 * the FITS header
 * 
 * The addDSKeywordEntry() function adds the individual lines of the
 * Data Subspace Keyword entry into the FITS header.  It determines
 * the ranges based on the values of the cut parameters, build the
 * data strings and writes them to the file.
 * 
 * @param type The keyword that was selected on
 * @param unit The unit of that keyword
 * @param min The minimum value from the cuts
 * @param max The maximum value from the cuts
 * @param nKeys The number of DS Keys currently in the file.  <b>Note:
 * this value is updated in the function.</b>
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  void addDSKeywordEntry(std::string type, std::string unit, T min, T max,
                         int& nKeys, tip::Header & header) {
     std::ostringstream val, key1, key2, key3;
    
     if (min && max){
        val << min << ":" << max;
     } else {
        if(min){
           val << min << ":";
        } else {
           val << ":" << max;
        }
     }
     nKeys++;
     key1 << "DSTYP" << nKeys;
     key2 << "DSUNI" << nKeys;
     key3 << "DSVAL" << nKeys;
     header[key1.str()].set(type);
     header[key2.str()].set(unit);
     header[key3.str()].set(val.str());
  }  

/**
 * @brief Overloaded version to handle only one input parameter
 * 
 * The addDSKeywordEntry() function adds the individual lines of the
 * Data Subspace Keyword entry into the FITS header.  This version
 * simply takes a single input parameter and uses it to set the min
 * and max values.
 * 
 * @param type The keyword that was selected on
 * @param unit The unit of that keyword
 * @param min The minimum value from the cuts
 * @param nKeys The number of DS Keys currently in the file.  <b>Note:
 * this value is updated in the fuction.</b>
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  void addDSKeywordEntry(std::string type, std::string unit, T min,
                         int& nKeys, tip::Header & header) {
    std::ostringstream key1, key2, key3;
    std::ostringstream val;
    // set up the limits
    val << min << ':' << min;
    // build the keynames
    nKeys++;
    key1 << "DSTYP" << nKeys;
    key2 << "DSUNI" << nKeys;
    key3 << "DSVAL" << nKeys;
    header[key1.str()].set(type);
    header[key2.str()].set(unit);
    header[key3.str()].set(val.str());
  }  
};

#endif //dataFilter_CUTPARAMETERS_H
