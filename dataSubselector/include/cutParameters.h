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

#include "fitsio.h"
#include <iostream>
#include <string.h>
#include <cmath>
#include "Goodi/IDataIOService.h"

/// conversion factor from degress to radians
#define DEG_TO_RAD 0.0174532925199
/// flag for debug printing
#define DEBUG 0

class cutParameters {
  /// File name of input FT1 file
  char m_inFile[FLEN_FILENAME];
  /// File name of output FT1 file
  char m_outFile[FLEN_FILENAME];
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
  char m_query[FLEN_FILENAME];
  /// string to hold header comment for search parameters
  char m_headerString[FLEN_FILENAME];
  /// calib version entries to check for cuts
  short m_calibVersion[3];
  
public:
  cutParameters();
  cutParameters(int argc, char **argv);
  ~cutParameters();
  void parseCommandLine(int argc, char **argv);
  char *getFilterExpression();
  void addDataSubspaceKeywords(Goodi::IDataIOService *ios);
  // inline function go here
  inline char *getInputFilename();
  inline char *getOutputFilename();
  inline char *getFITSQueryString();
  inline char *getHeaderString();
};

// **********  INLINE FUNCTION DEFINITIONS  ********************

/**
 * @brief Returns the input filename
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified 17 Oct 2003
 */
inline char *cutParameters::getInputFilename(){
  char *name=(char *)malloc(sizeof(m_inFile));
  strcpy(name,m_inFile);
  return name;
}
/**
 * @brief Returns the output filename
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified 17 Oct 2003
 */
inline char *cutParameters::getOutputFilename(){
  char *name=(char *)malloc(sizeof(m_outFile));
  strcpy(name,m_outFile);
  return name;
}

/**
 * @brief Returns the FITS row filtering string
 * 
 * @author Tom Stephens
 * @date Created:  20 Oct 2003
 * @date Last Modified 20 Oct 2003
 */
inline char *cutParameters::getFITSQueryString(){
  char *name=(char *)malloc(sizeof(m_query));
  strcpy(name,m_query);
  return name;
}

/**
 * @brief Returns the header string for FITS file
 * 
 * @author Tom Stephens
 * @date Created:  20 Oct 2003
 * @date Last Modified 20 Oct 2003
 */
inline char *cutParameters::getHeaderString(){
  char *name=(char *)malloc(sizeof(m_headerString));
  strcpy(name,m_headerString);
  return name;
}
