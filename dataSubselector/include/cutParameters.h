/**
 * @file cutParameters.h
 * @brief Header file for the cutParameters class
 */
 
/**
 * @class cutParameters
 * @brief definition of cutParameters class
 * 
 * This class holds the parameters of the cut to be applied to the data.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  25 Nov 2003
 * @version $Revision$
 * 
 * $ID$
 */

#ifndef CUTPARAMETERS_H
#define CUTPARAMETERS_H

#include "fitsio.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "Goodi/IDataIOService.h"
#include "hoops/hoops.h"
#include "hoops/hoops_limits.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_pil.h"
#include "hoops/hoops_exception.h"

/// conversion factor from degress to radians
#define DEG_TO_RAD 0.0174532925199
/// flag for debug printing
#define DEBUG 0

class cutParameters {
  
private:
  /// File name of input FT1 file
  std::string m_inFile;
  /// File name of output FT1 file
  std::string m_outFile;
  /// RA of new search center
  float m_RA;
  /// Dec of new search center
  float m_Dec;
  /// radius of new search area
  float m_radius;
  /// start time of new time interval
  double m_tmin;
  /// ending time of new time interval
  double m_tmax;
  /// low energy cut
  float m_emin;
  /// high energy cut
  float m_emax;
  /// minimum phi cut
  float m_phimin;
  /// maximum phi cut
  float m_phimax;
  /// minimum theta cut
  float m_thetamin;
  /// maximum theta cut
  float m_thetamax;
  /// minimum IMGAMMAPROB value desired
  float m_gammaProbMin;
  /// maximium IMGAMMAPROB value desired
  float m_gammaProbMax;
  /// minimum ZENITH_ANGLE value desired
  float m_zmin;
  /// maximum ZENITH_ANGLE value desired
  float m_zmax;
  /// string to hold fits row filtering query command
  std::string m_query;
  /// string to hold header comment for search parameters
  std::string m_headerString;
  /// calib version entries to check for cuts
  short m_calibVersion[3];
  /// pointer to hoops IParPrompter object
  hoops::IParPrompt * m_prompter;
  
public:
  cutParameters();
  cutParameters(int argc, char **argv);
  ~cutParameters();
  std::string *getFilterExpression();
  void addDataSubspaceKeywords(Goodi::IDataIOService *ios);
  // inline function go here
  std::string *getInputFilename() {return new std::string(m_inFile);}
  std::string *getOutputFilename() {return new std::string(m_outFile);}
  std::string *getFITSQueryString() {return new std::string(m_query);}
  std::string *getHeaderString() {return new std::string(m_headerString);}
  
/****** Templated Function Definitions ***********/

/**
 * @brief Returns the parameter values from parameter file
 * 
 * The getParam() function extracts the value for the parameter from
 * the parameter file and returns it to the program.  This was copied from
 * RunParams class that Jim wrote for Likelihood.  It is a templated function
 * and can be used on any variable type.
 * 
 * @param name The name of the parameter as contained in the paramter file
 * @param value a pointer to the variable to hold the returned data.
 * 
 * @author Jim Chang
 * @date Last Modified 15 Dec 2003
 */
  template <typename T> 
  void getParam(const std::string &name, T &value) const {
    try {
      T my_value = (m_prompter->Group())[name];
      value = my_value;
    } catch (hoops::Hexception &eObj) {
      std::cout << "HOOPS exception: " << eObj.Msg() << "\n"
                << "Code: " << eObj.Code() << std::endl;
      assert(false);
    } catch (...) {
      std::cout << name << std::endl;
      assert(false);
    }
  }

/**
 * @ brief adds the condition for the given parameter to the query and header string
 * 
 * The addParameterToQuery() function adds the input text and cut parameter to the
 * cfitsio query string and header string.  It is a template function so it can handle
 * and type of parameter value
 * 
 * @param first boolean flag to signify if this is the first parameter in the string
 * @param text string holding the FITS column name to test against
 * @param param the cutParameters data member to include in the cut
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  bool addParameterToQuery(bool first, std::string text, T &param) {
    // make a pair of stringstreams to facilitate writing out the data
    std::ostringstream q;
    std::ostringstream hs;
    
    if(!first){  // if we're not the first we need to add an "and"
      q << "&&";
      hs << "&&";
    } else {
      first=false;
    }
    // add the text and parameter value to the stream
    q << text << param;
    hs << text << param;
    // append the values of the stringstreams back to the data strings
    m_query+=q.str();
    m_headerString+=hs.str();
    
    return first;
  }

/**
 * @brief Adds the Data Subspace keywords for the given parameters
 *   to the FITS header
 * 
 * The addDSKeywordEntry() function adds the individual lines of the Data Subspace
 * Keyword entry into the FITS header.  It determines the ranges based on the
 * values of the cut parameters, build the data strings and writes them to the file.
 * 
 * @param type The keyword that was selected on
 * @param unit The unit of that keyword
 * @param min The minimum value from the cuts
 * @param max The maximum value from the cuts
 * @param nKeys The number of DS Keys currently in the file.  <b>Note: this value is
 * updated in the fuction.</b>
 * @param ios The Goodi IO Service that points to the file
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  void addDSKeywordEntry(std::string type, std::string unit, T min, T max,
                         int& nKeys, Goodi::IDataIOService *ios){
    std::ostringstream val,key1,key2,key3;
    
    if(min&&max){
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
    if(DEBUG) std::cout << "******KEYS are: "<< key1.str() << "," << key2.str() 
                        << "," << key3.str();
    ios->writeKey(key1.str(),type);
    ios->writeKey(key2.str(),unit);
    ios->writeKey(key3.str(),val.str());

  }  
/**
 * @brief Overloaded version to handle only one input parameter
 * 
 * The addDSKeywordEntry() function adds the individual lines of the Data Subspace
 * Keyword entry into the FITS header.  This version simply takes a single input parameter
 * and uses it to set the min and max values.
 * 
 * @param type The keyword that was selected on
 * @param unit The unit of that keyword
 * @param min The minimum value from the cuts
 * @param nKeys The number of DS Keys currently in the file.  <b>Note: this value is
 * updated in the fuction.</b>
 * @param ios The Goodi IO Service that points to the file
 * 
 * @author Tom Stephens
 * @date Created:  Jan 02, 2004
 * @date Last Modified:  Jan 02, 2004
 * 
 */  
  template <typename T>
  void addDSKeywordEntry(std::string type, std::string unit, T min,
                         int& nKeys, Goodi::IDataIOService *ios){
    std::ostringstream key1,key2,key3;
    std::ostringstream val;
    // set up the limits
    val << min << ':' << min;
    // build the keynames
    nKeys++;
    key1 << "DSTYP" << nKeys;
    key2 << "DSUNI" << nKeys;
    key3 << "DSVAL" << nKeys;
    if(DEBUG) std::cout << "******KEYS are: "<< key1.str() << "," << key2.str() 
                        << "," << key3.str();
    ios->writeKey(key1.str(),type);
    ios->writeKey(key2.str(),unit);
    ios->writeKey(key3.str(),val.str());

  }  
  
};

#endif CUTPARAMETERS_H
