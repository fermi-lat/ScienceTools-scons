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

  strcpy(m_inFile,"");
  strcpy(m_outFile,"");
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

  strcpy(m_inFile,"");
  strcpy(m_outFile,"");
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
  strcpy(m_inFile,tstring.c_str());
  std::cout << m_inFile << " is the filename" << std::endl;
  getParam("output_file",tstring);
  strcpy(m_outFile,tstring.c_str());
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
char *cutParameters::getFilterExpression(){
  char *expression=(char *)malloc(FLEN_FILENAME);
  bool first=true;
  char tmp[FLEN_FILENAME];
  int i;
  
  m_query[0]='\0';
  m_headerString[0]='\0';

  if(m_RA>=0){  // we have a new area so apply haversine search on the area
    sprintf(m_query,"((2*asin(min(1,sqrt(max(0,(sin((DEC-%g)*%g)*sin((DEC-%g)*%g))+(cos(DEC*%g)*%g*sin((RA-%g)*%g)*sin((RA-%g)*%g)))))))<%g)"
      ,m_Dec,DEG_TO_RAD/2,m_Dec,DEG_TO_RAD/2,DEG_TO_RAD,std::cos(m_Dec*DEG_TO_RAD)
      ,m_RA,DEG_TO_RAD/2,m_RA,DEG_TO_RAD/2,m_radius*DEG_TO_RAD);
    sprintf(m_headerString,"circle(%g,%g,%g,RA,DEC)",m_RA,m_Dec,m_radius);
    printf("%s\n",m_headerString);
    first=false;
  }
  if(m_tmin){ // minimum time
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sTIME>=%.6f",m_query,m_tmin);
    sprintf(m_headerString,"%sTIME>=%.6f",m_headerString,m_tmin);
  }
  if(m_tmax){ // maximum time
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sTIME<=%.6f",m_query,m_tmax);
    sprintf(m_headerString,"%sTIME<=%.6f",m_headerString,m_tmax);
  }
  if(m_emin){ // minimum enegy
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sENERGY>=%g",m_query,m_emin);
    sprintf(m_headerString,"%sENERGY>=%g",m_headerString,m_emin);
  }
  if(m_emax){ // maximum energy
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sENERGY<=%g",m_query,m_emax);
    sprintf(m_headerString,"%sENERGY<=%g",m_headerString,m_emax);
  }
  if(m_phimin){ // minimum phi
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sPHI>=%g",m_query,m_phimin);
    sprintf(m_headerString,"%sPHI>=%g",m_headerString,m_phimin);
  }
  if(m_phimax){ // maximum phi
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sPHI<=%g",m_query,m_phimax);
    sprintf(m_headerString,"%sPHI<=%g",m_headerString,m_phimax);
  }
  if(m_thetamin){ // minimum theta
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sTHETA>=%g",m_query,m_thetamin);
    sprintf(m_headerString,"%sTHETA>=%g",m_headerString,m_thetamin);
  }
  if(m_thetamax){ // maximum theta
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sTHETA<=%g",m_query,m_thetamax);
    sprintf(m_headerString,"%sTHETA<=%g",m_headerString,m_thetamax);
  }
  if(m_gammaProbMin){ // minimum IMGAMMAPROB
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sIMGAMMAPROB>= %f",m_query,m_gammaProbMin);
    sprintf(m_headerString,"%sIMGAMMAPROB>=%f",m_headerString,m_gammaProbMin);
  }
  if(m_gammaProbMax){ // maximum IMGAMMAPROB
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sIMGAMMAPROB<= %f",m_query,m_gammaProbMax);
    sprintf(m_headerString,"%sIMGAMMAPROB<=%f",m_headerString,m_gammaProbMax);
  }
  if(m_zmin){ // minimum ZENITH_ANGLE
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sZENITH_ANGLE>= %f",m_query,m_zmin);
    sprintf(m_headerString,"%sZENITH_ANGLE>=%f",m_headerString,m_zmin);
  }
  if(m_zmax){ // maximum ZENITH_ANGLE
    if(!first){
      strcat(m_query,"&&");
      strcat(m_headerString,"&&");
    } else {
      first=false;
    }
    sprintf(m_query,"%sZENITH_ANGLE<= %f",m_query,m_zmax);
    sprintf(m_headerString,"%sZENITH_ANGLE<=%f",m_headerString,m_zmax);
  }
  for (i=0;i<3;i++){ // loop over the three possilble calib_version entries
    if(m_calibVersion[i]){ // if !=0 use this entry in CALIB_VERSION to select (DC1 edition)
      if(!first){
        strcat(m_query,"&&");
        strcat(m_headerString,"&&");
      } else {
        first=false;
      }
      sprintf(m_query,"%sCALIB_VERSION[%d]== 1",m_query,i+1);
      sprintf(m_headerString,"%sCALIB_VERSION[%d]== 1",m_headerString,i+1);
    }
  }
  
//  sprintf(tmp,"(%s)",m_query);
//  strcpy(m_query,tmp);
  sprintf(tmp,"@%s@",m_headerString);  // add query delimiters
  strcpy(m_headerString,tmp);
  
  strcpy (expression,m_query);
  return expression;
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
  std::string type,unit,val;
  char value[80];
  char key1[10],key2[10],key3[10];
  int i,nKeys=0;
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
    sprintf(value,"circle(%g,%g,%g)",m_RA,m_Dec,m_radius);
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_tmin||m_tmax){  // time header keyword
    type=std::string("TIME");
    unit=std::string("s");
    if(m_tmin&&m_tmax){
      sprintf(value,"%.6f:%.6f",m_tmin,m_tmax);
    } else {
      if(m_tmin){
        sprintf(value,"%.6f:",m_tmin);
      } else {
        sprintf(value,":%.6f",m_tmax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_emin||m_emax){  // energy header keyword
    type=std::string("ENERGY");
    unit=std::string("MeV");
    if(m_emin&&m_emax){
      sprintf(value,"%f:%f",m_emin,m_emax);
    } else {
      if(m_emin){
        sprintf(value,"%f:",m_emin);
      } else {
        sprintf(value,":%f",m_emax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_phimin||m_phimax){  // PHI header keyword
    type=std::string("PHI");
    unit=std::string("deg");
    if(m_phimin&&m_phimax){
      sprintf(value,"%f:%f",m_phimin,m_phimax);
    } else {
      if(m_phimin){
        sprintf(value,"%f:",m_phimin);
      } else {
        sprintf(value,":%f",m_phimax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_thetamin||m_thetamax){  // theta header keyword
    type=std::string("THETA");
    unit=std::string("deg");
    if(m_thetamin&&m_thetamax){
      sprintf(value,"%f:%f",m_thetamin,m_thetamax);
    } else {
      if(m_thetamin){
        sprintf(value,"%f:",m_thetamin);
      } else {
        sprintf(value,":%f",m_thetamax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_gammaProbMin||m_gammaProbMax){  // IMGAMMAPROB header keyword
    type=std::string("IMGAMMAPROB");
    unit=std::string("dimensionless");
    if(m_gammaProbMin&&m_gammaProbMax){
      sprintf(value,"%f:%f",m_gammaProbMin,m_gammaProbMax);
    } else {
      if(m_gammaProbMin){
        sprintf(value,"%f:",m_gammaProbMin);
      } else {
        sprintf(value,":%f",m_gammaProbMax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
  }
  if(m_zmin||m_zmax){  // ZENTIH_ANGLE header keyword
    type=std::string("ZENTIH_ANGLE");
    unit=std::string("deg");
    if(m_zmin&&m_zmax){
      sprintf(value,"%f:%f",m_zmin,m_zmax);
    } else {
      if(m_zmin){
        sprintf(value,"%f:",m_zmin);
      } else {
        sprintf(value,":%f",m_zmax);
      }
    }
    val=std::string(value);
    nKeys++;
    sprintf(key1,"DSTYP%d",nKeys);
    sprintf(key2,"DSUNI%d",nKeys);
    sprintf(key3,"DSVAL%d",nKeys);
    if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
    ios->writeKey(key1,type);
    ios->writeKey(key2,unit);
    ios->writeKey(key3,val);
 } 
 for(i=0;i<3;i++){
   if(m_calibVersion[i]){  // CALIB_VERSION header keyword
     sprintf(value,"CALIB_VERSION[%d]",i+1);
     type=std::string(value);
     unit=std::string("dimensionless");
     val=std::string("1:1");
     nKeys++;
     sprintf(key1,"DSTYP%d",nKeys);
     sprintf(key2,"DSUNI%d",nKeys);
     sprintf(key3,"DSVAL%d",nKeys);
     if(DEBUG) fprintf(stdout,"******KEYS are: %s, %s, %s\n",key1,key2,key3);
     ios->writeKey(key1,type);
     ios->writeKey(key2,unit);
     ios->writeKey(key3,val);
   } 
 }
  
  // update the NDSKEYS keyword with the new value
  ios->writeKey("NDSKEYS",nKeys,"Number of data subspace keywords in header");
}

