/**
 * @file cutParameters.cxx
 * 
 * @brief  Method definitions for the cutParameters class
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  $Date$
 * @version $Revision$
 * 
 * $Id$
 */
#include "../include/cutParameters.h"

/**
 * @brief Default constructor
 * 
 * This is the default constructor function for the cutParameters class.  It
 * creates an empty structure with the values all set to zero except m_RA which
 * is set to -1 as a flag for a new ra/dec cut.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  3 Dec 2003
 */
cutParameters::cutParameters(){

  m_inFile="";
  m_outFile="";
  m_RA = -1;
  m_Dec = 0;
  m_radius = 0;
  m_tmin = 0;
  m_tmax = 0;
  m_emin = 0;
  m_emax = 0;
  m_phimin = 0;
  m_phimax = 0;
  m_thetamin = 0;
  m_thetamax = 0;
  m_gammaProbMin = 0;
  m_gammaProbMax = 0;
  m_zmin = 0;
  m_zmax = 0;
  m_calibVersion[0]=0;
  m_calibVersion[1]=0;
  m_calibVersion[2]=0;
  m_query="";
  m_headerString="";
  if(DEBUG) std::cout << "cutParameters initalized\n";
}

/**
 * @brief Primary constructor
 * 
 * This is main constructor function for the cutParameters class.  It
 * creates an empty structure with the values all set to zero and then
 * reads the parameter file using hoops and fills in the data values.
 * 
 * @param argc the number of command line arguments
 * @param argv array of pointers holding the individual arguments
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  15 Dec 2003
 * 
 * Revisions:
 *   - 15/12/03 - Added hoops functionality
 */
cutParameters::cutParameters(int argc, char **argv){

  m_inFile="";
  m_outFile="";
  m_RA = -1;
  m_Dec = 0;
  m_radius = 0;
  m_tmin = 0;
  m_tmax = 0;
  m_emin = 0;
  m_emax = 0;
  m_phimin = 0;
  m_phimax = 0;
  m_thetamin = 0;
  m_thetamax = 0;
  m_gammaProbMin = 0;
  m_gammaProbMax = 0;
  m_zmin = 0;
  m_zmax = 0;
  m_calibVersion[0]=0;
  m_calibVersion[1]=0;
  m_calibVersion[2]=0;
  m_query="";
  m_headerString="";
  
  strcpy(argv[0],"dataSubselector");
  // start using hoops here  - stole this straight from likelihood
  hoops::IParFile *pf = hoops::PILParFileFactory().NewIParFile(argv[0]);
  try {
    pf->Load();
    m_prompter = hoops::PILParPromptFactory().NewIParPrompt(argc, argv);
    m_prompter->Prompt();
    
    pf->Group() = m_prompter->Group();
    pf->Save();
    delete pf;
  } catch (hoops::Hexception &eObj) {
    if (eObj.Code() == -3003) {
      std::cout << "dataSubselector cutParamters .par file " << argv[0]
                << " is not found.  Check your PFILES directory." 
                << std::endl;
    }
    throw;
  }
  // Parameters are read in, not let's set the data values
  std::string tstring;
  getParam("input_file",tstring);
  m_inFile=tstring;
  std::cout << m_inFile << " is the filename" << std::endl;
  getParam("output_file",tstring);
  m_outFile=tstring;
  std::cout << m_outFile << " is the filename" << std::endl;
  getParam("ra",m_RA);
  getParam("dec",m_Dec);
  getParam("rad",m_radius);
  if(m_RA==0&&m_Dec==0&&m_radius==0) {
    m_RA=-1;
    std::cout << "resetting default RA" << std::endl;
  }
  getParam("tmin",m_tmin);
  getParam("tmax",m_tmax);
  getParam("emin",m_emin);
  getParam("emax",m_emax);
  getParam("phimin",m_phimin);
  getParam("phimax",m_phimax);
  getParam("thetamin",m_thetamin);
  getParam("thetamax",m_thetamax);
  getParam("gammaProbMin",m_gammaProbMin);
  getParam("gammaProbMax",m_gammaProbMax);
  getParam("zmin",m_zmin);
  getParam("zmax",m_zmax);
  getParam("bgcut",m_calibVersion[0]);
  getParam("psfcut",m_calibVersion[1]);
  getParam("erescut",m_calibVersion[2]);
  
  
  if(DEBUG){
    std::cout << "cutParameters initalized:\n";
    std::cout << "m_inFile: " << m_inFile << std::endl;
    std::cout << "m_outFile: " << m_outFile << std::endl;
    std::cout << "m_RA: " << m_RA << std::endl;
    std::cout << "m_Dec: " << m_Dec << std::endl;
    std::cout << "m_radius: " << m_radius << std::endl;
    std::cout << "m_tmin: " << m_tmin << std::endl;
    std::cout << "m_tmax: " << m_tmax << std::endl;
    std::cout << "m_emin: " << m_emin << std::endl;
    std::cout << "m_emax: " << m_emax << std::endl;
    std::cout << "m_phimin: " << m_phimin << std::endl;
    std::cout << "m_phimax: " << m_phimax << std::endl;
    std::cout << "m_thetamin: " << m_thetamin << std::endl;
    std::cout << "m_thetamax: " << m_thetamax << std::endl;
    std::cout << "m_gammaProbMin: " << m_gammaProbMin << std::endl;
    std::cout << "m_gammaProbMax: " << m_gammaProbMax << std::endl;
    std::cout << "m_zmin: " << m_zmin << std::endl;
    std::cout << "m_zmax: " << m_zmax << std::endl;
    std::cout << "m_calibVersion: " << m_calibVersion << std::endl;
  }
  
}

/**
 * @brief Default destructor
 * 
 * This is the default destructor function for the cutParameters class.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  17 Oct 2003
 */
cutParameters::~cutParameters() {
  delete m_prompter;
  if(DEBUG) std::cout << "cutParameters destroyed\n";
}

/**
 * @brief Builds the FITS row filtering expression from the cut parameters
 * 
 * This function builds a valid FITS row filtering expression based on the 
 * cut parameters specified by the user.  It returns a string containing the
 * filtering command that can then be passed to the Goodi::FitsService::filter()
 * function to open the file and do the row filtering.  It also store both the
 * FITS expression and a string to be entered into the header file history in
 * member variables.
 * 
 * @author Tom Stephens
 * @date Created:  17 Oct 2003
 * @date Last Modified:  3 Dec 2003
 */
/* Revisions:
 *   25/11/03 - Changed OVERALL_QUALITY keyword to IMGAMMAPROB keyword -TS
 *   03/12/03 - Added cuts on CALIB_VERSION - TS
 */
std::string *cutParameters::getFilterExpression(){
  bool first=true;
  
  m_query="";
  m_headerString="";
  if(m_RA>=0){  // we have a new area so apply haversine search on the area
    std::ostringstream q,hs;
    q << "((2*asin(min(1,sqrt(max(0,(sin((DEC-" << m_Dec << ")*" << DEG_TO_RAD/2;
    q << ")*sin((DEC-" << m_Dec << ")*" << DEG_TO_RAD/2 << "))+(cos(DEC*" <<DEG_TO_RAD;
    q << ")*" << std::cos(m_Dec*DEG_TO_RAD) << "*sin((RA-" << m_RA << ")*" << DEG_TO_RAD/2;
    q << ")*sin((RA-" << m_RA << ")*" << DEG_TO_RAD/2 << ")))))))<" << m_radius*DEG_TO_RAD << ")";
    m_query=q.str();
    if(DEBUG) std::cout << m_query << std::endl;
  
    hs << "circle(" << m_RA << "," << m_Dec << "," << m_radius << ",RA,DEC)";
    m_headerString=hs.str();
    if(DEBUG) std::cout << m_headerString << std::endl;

    first=false;
  }
  
  if(m_tmin) first = addParameterToQuery(first,"TIME>=",m_tmin); 
  if(m_tmax) first = addParameterToQuery(first,"TIME<=",m_tmax); 
  if(m_emin) first = addParameterToQuery(first,"ENERGY>=",m_emin); 
  if(m_emax) first = addParameterToQuery(first,"ENERGY<=",m_emax);
  if(m_phimin) first = addParameterToQuery(first,"PHI>=",m_phimin); 
  if(m_phimax) first = addParameterToQuery(first,"PHI<=",m_phimax);
  if(m_thetamin) first = addParameterToQuery(first,"THETA>=",m_thetamin); 
  if(m_thetamax) first = addParameterToQuery(first,"THETA<=",m_thetamax);
  if(m_gammaProbMin) first = addParameterToQuery(first,"IMGAMMAPROB>=",m_gammaProbMin); 
  if(m_gammaProbMax) first = addParameterToQuery(first,"IMGAMMAPROB>=",m_gammaProbMax);
  if(m_zmin) first = addParameterToQuery(first,"ZENITH_ANGLE>=",m_zmin); 
  if(m_zmax) first = addParameterToQuery(first,"ZENITH_ANGLE<=",m_zmax);
  // these check the background cuts
  if(m_calibVersion[0]) first = addParameterToQuery(first,"CALIB_VERSION[1]== ",m_calibVersion[0]);
  if(m_calibVersion[1]) first = addParameterToQuery(first,"CALIB_VERSION[2]== ",m_calibVersion[1]);
  if(m_calibVersion[2]) first = addParameterToQuery(first,"CALIB_VERSION[3]== ",m_calibVersion[2]);
  
  m_headerString="@"+m_headerString+"@";  // add query delimiters
  
  return new std::string(m_query);
}

/**
 * @brief This function adds the data subspace keywords to the event header
 * 
 * The addDataSubspaceKeywords() function adds the relevant data subspace
 * keywords to the Events header to desginate the type of search that was done
 * on the data to create the file.  This version is modified from the one in the
 * D1 Database
 * 
 * @param ios  The ioservice to write the subspace keywords to
 * 
 * @author Tom Stephens
 * @date Created:  30 Oct 2003
 * @date Last Modified:  3 Dec 2003
 */
void cutParameters::addDataSubspaceKeywords(Goodi::IDataIOService *ios){
  std::string type,unit;
  std::ostringstream val,key1,key2,key3;
  int nKeys=0;
  std::string comment,test="NDSKEYS";
  
  // first read in the number of DS keys already in the fits file
  try{
    ios->readKey("NDSKEYS",nKeys,comment);
  }
  catch(...){
    // if the keyword doesn't exist then the file didn't come from the D1 database
    // We'll let the user know and continue on
    std::cout << "No NDSKEYS keyword in event header.  This file not generated by\n";
    std::cout << "the D1 Database.  Continuing anyway.\n";  
  }
  
  if (m_RA>=0){  // position header keyword
    type=std::string("POS(RA,DEC)");
    unit=std::string("deg");
    val << "circle(" << m_RA << "," << m_Dec << "," << m_radius << ")";
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
  if(m_tmin||m_tmax) addDSKeywordEntry("TIME","s",m_tmin,m_tmax,nKeys,ios);
  if(m_emin||m_emax) addDSKeywordEntry("ENERGY","MeV",m_emin,m_emax,nKeys,ios);
  if(m_phimin||m_phimax) addDSKeywordEntry("PHI","deg",m_phimin,m_phimax,nKeys,ios);
  if(m_thetamin||m_thetamax) addDSKeywordEntry("THETA","deg",m_thetamin,m_thetamax,nKeys
      ,ios);
  if(m_gammaProbMin||m_gammaProbMax) addDSKeywordEntry("IMGAMMAPROB","dimensionless"
      ,m_gammaProbMin,m_gammaProbMax,nKeys,ios);
  if(m_zmin||m_zmax) addDSKeywordEntry("ZENTIH_ANGLE","deg",m_zmin,m_zmax,nKeys,ios);
  if(m_calibVersion[0]) addDSKeywordEntry("CALIB_VERSION[1]","dimensionless"
      ,m_calibVersion[0],nKeys,ios);
  if(m_calibVersion[1]) addDSKeywordEntry("CALIB_VERSION[2]","dimensionless"
      ,m_calibVersion[1],nKeys,ios);
  if(m_calibVersion[2]) addDSKeywordEntry("CALIB_VERSION[3]","dimensionless"
      ,m_calibVersion[2],nKeys,ios);
  
  // update the NDSKEYS keyword with the new value
  ios->writeKey("NDSKEYS",nKeys,"Number of data subspace keywords in header");
}

