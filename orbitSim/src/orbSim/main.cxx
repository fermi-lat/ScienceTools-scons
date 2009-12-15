/** \file main.cxx
    \brief gtorbsim exectuable

    \author Giuseppe Romeo (original), FSSC
            John Vernaleo (current), FSSC
*/
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "facilities/commonUtilities.h"
#include "facilities/Util.h"

#include "orbitSim/OrbSim.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <time.h>

#include "st_stream/Stream.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

// Identify version tag.
const std::string s_cvs_id("$Name$");

class orbitSimApp : public st_app::StApp {
  public:
  orbitSimApp(): osf("orbitSimApp", "", 1) {
      osf.setMethod("orbitSimApp");
      setName("gtorbsim");
      setVersion(s_cvs_id);
  }
  virtual void run();

  private:

  /// Stream to control output through verbosity level
  st_stream::StreamFormatter osf;
};


void orbitSimApp::run() {
  using namespace tip;
  InitI initf;
  int stat;

  osf.setMethod("run");

  initf.occflag = 1;
  initf.EAA = 20.0;
  initf.ELT_OFF_START = -99999.0;
  initf.ELT_OFF_STOP  = -99999.0;

  st_app::AppParGroup & pars(getParGroup("gtorbsim"));

  int chat = pars["chatter"];
  st_stream::SetMaximumChatter(chat);

  bool debug_mode = pars["debug"];
  st_stream::SetDebugMode(debug_mode);

  double tEAA = pars["EAA"];
  if(tEAA >= 0.0 && tEAA <=180.0){
    initf.EAA = pars["EAA"];
  }

  double elt_start =  pars["ELT_OFF_START"];
  if(elt_start > 0.0){
    initf.ELT_OFF_START = pars["ELT_OFF_START"];
  }
  
  osf.info().precision(15);
  osf.err().precision(15);
  osf.warn().precision(15);
  osf.out().precision(15);

  double elt_stop =  pars["ELT_OFF_STOP"];

  if(elt_stop > 0.0){
    initf.ELT_OFF_STOP = pars["ELT_OFF_STOP"];
  }


  if((initf.ELT_OFF_STOP != -99999.0 || initf.ELT_OFF_START != -99999.0)){
    if(initf.ELT_OFF_STOP <= initf.ELT_OFF_START){
      osf.warn()<<"Earth LIMB Tracing OFF stop time is smaller than start time\n";
      osf.warn()<<"These parameter are not acceptable, reverting to default:\nEarth LImb Tracing will be on at all times\n\n";
      initf.ELT_OFF_START = -99999.0;
      initf.ELT_OFF_STOP  = -99999.0;

    } else if(initf.ELT_OFF_STOP < 0.0 && initf.ELT_OFF_START > 0.0){
      osf.warn()<<"Earth LIMB Tracing OFF stop time is NOT defined\n";
      osf.warn()<<"These parameter are not acceptable, reverting to default:\nEarth LImb Tracing will be on at all times\n\n";

    } else if(initf.ELT_OFF_STOP > 0.0 && initf.ELT_OFF_START < 0.0){
      osf.warn()<<"Earth LIMB Tracing OFF start time is NOT defined\n";
      osf.warn()<<"These parameter are not acceptable, reverting to default:\nEarth LImb Tracing will be on at all times\n\n";

    }
  } 

  if((initf.ELT_OFF_STOP != -99999.0 && initf.ELT_OFF_START != -99999.0)){
    osf.info(1)<<"EARTH Limb Tracing is disabled from "<<initf.ELT_OFF_START<<" to "<<initf.ELT_OFF_STOP<<"\n";
  }

  //  exit(1);

  pars.Prompt("typeinput");
  std::string Input = pars["typeinput"];
  osf.info(1) << "Input Type is: " << Input.c_str() << std::endl;
  //  if(match((const char*) Input.c_str(), "file") == 1){
  if(match_str((const char*) Input.c_str(), "FILE") == 1){
    pars.Prompt("initFile");

    std::string initFile = pars["initFile"];
    const char *fname = initFile.c_str();
    stat = parseInit(fname, &initf);

    //  } else if (match((const char*) Input.c_str(), "console") == 1){
  } else if (match_str((const char*) Input.c_str(), "CONSOLE") == 1){
    pars.Prompt("start_MJD");
    initf.start_MJD = pars["start_MJD"];

    pars.Prompt("stop_MJD");
    initf.stop_MJD = pars["stop_MJD"];

    pars.Prompt("TLType");
    std::string Tlt = pars["TLType"];
    initf.TLtype = Tlt;


    //    if((match((const char*) Tlt.c_str(), "^TAKO$") == 1) || (match((const char*) Tlt.c_str(), "^ASFLOWN$") == 1)){
    if((match_str((const char*) Tlt.c_str(), "TAKO") == 1) || (match_str((const char*) Tlt.c_str(), "ASFLOWN") == 1)){
      pars.Prompt("Timeline");
      std::string Tml = pars["Timeline"];
      initf.TLname = Tml;

      //  } else if (match((const char*) Tlt.c_str(), "SINGLE") == 1){
    } else if (match_str((const char*) Tlt.c_str(), "SINGLE") == 1){
      pars.Prompt("TimelnCmd");
      std::string Tml = pars["TimelnCmd"];
      initf.TLname = Tml;
 
    } else {
      throw std::runtime_error("\nERROR: UNKNOWN Timeline type {either TAKO, or ASFLOWN or SINGLE}\n\n");

    }
    pars.Prompt("EphemName");
    std::string EphN = pars["EphemName"];
    initf.EPHname = EphN;

    pars.Prompt("EphemFunc");
    std::string EpF = pars["EphemFunc"];
//     if(!((match((const char*) EpF.c_str(), "^xyzll_eph$")==1) ||
// 	 (match((const char*) EpF.c_str(), "^yyyy_eph$")==1) ||
//    	 (match((const char*) EpF.c_str(), "^tlederive$")==1))){
    if(!((match_str((const char*) EpF.c_str(), "XYZLL_EPH")==1) ||
	 (match_str((const char*) EpF.c_str(), "YYYY_EPH")==1) ||
	 (match_str((const char*) EpF.c_str(), "TLEDERIVE")==1))){
      throw std::runtime_error("\nERROR: UNKNOWN Ephemeredis Function {either xyzll_eph, or yyyy_eph or tlederive}\n\n");
      
    } else {
      initf.EPHfunc = EpF;
    }
    pars.Prompt("Units");
    initf.Units = pars["Units"];

    pars.Prompt("Resolution");
    initf.Resolution = pars["Resolution"];

    pars.Prompt("Initial_RA");
    initf.Ira = pars["Initial_RA"];

    pars.Prompt("Initial_DEC");
    initf.Idec = pars["Initial_DEC"];

    pars.Prompt("OutPutFile");
    std::string OutF = pars["OutPutFile"];
    initf.OutFile = OutF;

    pars.Prompt("saafile");
    std::string saaF = pars["saafile"];
    initf.saafile = saaF;

    stat = 1;

  } else {
    
    throw std::runtime_error("\nERROR: UNKNOWN Input type {either file or console}\n\n");
  }


  pars.Save();
  // Add a little buffer to the end time so we calculate enough ephem
  // and attitude point to get positions up to the end of the desired time.
  double stop_buffer = 0.1;
  initf.stop_MJD=initf.stop_MJD+stop_buffer;
  osf.info(1)<<"Earth Avoidance Angle: "<<initf.EAA<<" degrees"<<std::endl;

  // stat = 0;

  if (stat == 0){
    std::ostringstream oBuf;
    oBuf << "\n############################################################\n\nSomething is wrong in the init file, please check:\nin parenthesis allowed values where applicable\n\n";

    oBuf << "start MJD              " << initf.start_MJD << std::endl;
    oBuf << "stop MJD               " << initf.stop_MJD << std::endl;
    oBuf << "Timeline file or cmd   " << initf.TLname << std::endl; 
    oBuf << "Timeline type          " << initf.TLtype << ",  (MUST be TAKO, ASFLOWN, SINGLE)" << std::endl;
    oBuf << "Ephemeredis file       " << initf.EPHname << std::endl;
    oBuf << "Ephemeredis func       " << initf.EPHfunc << ", (yyyy_eph, xyzll_eph, tlederive)" << std::endl;
    oBuf << "Units                  " << initf.Units   << std::endl; 
    oBuf << "Resolution             " << initf.Resolution << std::endl;
    oBuf << "Initial RA             " << initf.Ira << std::endl;
    oBuf << "Initial DEC            " << initf.Idec << std::endl;
    oBuf << "SAA file               " << initf.saafile << std::endl;
    oBuf << "Output file            " << initf.OutFile << std::endl;
    oBuf << "\nPlease, correct the problem\n\nExiting........\n\n############################################################\n\n";
    throw std::runtime_error(oBuf.str());


  }


////////////////////////////////////////////////////////////////////////////////
//
// Fixing the start time according to the resolution
  double stmjd = initf.start_MJD;
  double fday = (stmjd - (double)((int)stmjd))*1440.0;
  fday = (double)((int)(fday/initf.Resolution)-1)*initf.Resolution;
  stmjd = (double)((int)stmjd)+fday/1440.0;

  if((initf.start_MJD - stmjd)- initf.Resolution > 1.0E-6){
    osf.info(2) <<"Initial start time=" << initf.start_MJD << ", corrected time=" << stmjd  << std::endl;
    initf.start_MJD = stmjd; 
  }

  // Also fix the stop time based on resolution.
  double enmjd = initf.stop_MJD;
  fday = (enmjd - (double)((int)enmjd))*1440.0;
  fday = (double)((int)(fday/initf.Resolution)+1)*initf.Resolution;
  enmjd = (double)((int)enmjd)+fday/1440.0;

  if((initf.stop_MJD - enmjd)- initf.Resolution > 1.0E-6){
    osf.info(2) <<"Initial stop time=" << initf.stop_MJD << ", corrected time=" << enmjd  << std::endl;
    initf.stop_MJD = enmjd;
  }

  osf.info(2) <<"Optional File: ";
  
  if( initf.OptFile.empty()){
    osf.info(2) << "(NULL)";
  } else {
    osf.info(2) << initf.OptFile;
  }
  osf.info(2) << std::endl;



  FILE *ephF = NULL;

  osf.info(2) << "Opening Ephem file " << initf.EPHname << " for reading\n";
  if ( (ephF=fopen(initf.EPHname.c_str(),"r")) == NULL) {
    std::string fname( initf.EPHname);
    throw std::runtime_error("Cound not open Ephemeredis file:\n" + fname);
  }

  initf.Resolution = initf.Resolution/minInDay;
  EphemData * ephemeris = NULL;

  osf.info(2) << "Populating Ephemeris structure by calling " << initf.EPHfunc << " function.\n";

  if(match_str( initf.EPHfunc.c_str(),"YYYY_EPH") == 1){
    ephemeris = yyyy_eph(ephF, initf.start_MJD, initf.stop_MJD, \
			 initf.Units, initf.Resolution);
  } else if(match_str( initf.EPHfunc.c_str(),"XYZLL_EPH") == 1){
    ephemeris = xyzll_eph(ephF, initf.start_MJD, initf.stop_MJD, \
			  initf.Units, initf.Resolution);
  } else if(match_str( initf.EPHfunc.c_str(),"TLEDERIVE") == 1){
    ephemeris = tlederive(ephF, initf.start_MJD, initf.stop_MJD, \
			  initf.Units, initf.Resolution);
  }



  fclose(ephF);
  if (ephemeris == NULL){
    throw std::runtime_error("\nPossibly something went wrong while reading/generating ephemeris.\nThe Ephemeris structure is still \"NULL\"\n\n");
  }

  osf.info(3) <<"From Ephem file, start mjd = " << ephemeris->MJD[0] <<  std::endl;



  //Make an empty Attitude structure Oat.
  Attitude *Oat = NULL;

  //Basically all calculations are done by whatever this calls.
  if(match_str( initf.TLtype.c_str(), "TAKO") == 1){
    Oat = makeAttTako(&initf, ephemeris);
  } else if (match_str( initf.TLtype.c_str(), "ASFLOWN") == 1){
    Oat = makeAttAsFl(&initf, ephemeris);
  } else if (match_str( initf.TLtype.c_str(), "SINGLE") == 1){
    Oat = doCmd(&initf, ephemeris);
  }
  
  // Restore stop_MJD to its correct, but smaller value
  initf.stop_MJD=initf.stop_MJD-stop_buffer;

  if(Oat == NULL){
    throw std::runtime_error("\nPossibly something went wrong while calculating the spacecraft attitude.\nThe Attitude structure is still \"NULL\"\n\n");
  }

  // Get template file using facilities
  std::string orbitsimroot = facilities::commonUtilities::getDataPath("orbitSim");
  std::string ifname("ft2.tpl");
  std::string sfname = facilities::commonUtilities::joinPath(orbitsimroot, ifname);
  
  osf.info(2) <<"OutPut File template is "<<sfname.c_str()<<"\n";

  IFileSvc::instance().createFile(initf.OutFile, sfname.c_str());

  time_t rawtime;
  struct tm * ptm;

  time ( &rawtime );
  ptm = gmtime ( &rawtime );

  ptm->tm_year = ptm->tm_year+1900;
  ptm->tm_mon = ptm->tm_mon+1;

  char ts[20];
  char startTm[20];
  char endTm[20];

  int yy, MM, dd, hh, mm, ss;

  double metstart, metstop;

  sprintf(ts, "%4d-%02d-%02dT%02d:%02d:%02d", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);


  do_mjd2cal(initf.start_MJD, &yy, &MM, &dd, &hh, &mm, &ss);
  sprintf(startTm, "%4d-%02d-%02dT%02d:%02d:%02d", yy, MM, dd, hh, mm, ss);

  do_mjd2cal(initf.stop_MJD, &yy, &MM, &dd, &hh, &mm, &ss);
  sprintf(endTm, "%4d-%02d-%02dT%02d:%02d:%02d", yy, MM, dd, hh, mm, ss);

  osf.info(2) << __FILE__<<":"<<__LINE__<<" - start MJD="<< initf.start_MJD<<" startTm="<<startTm<<std::endl;
  osf.info(2) << __FILE__<<":"<<__LINE__<<" - stop MJD="<< initf.stop_MJD<<" endTm="<<endTm<<std::endl;

  metstart=do_mjd2met(initf.start_MJD);
  metstop=do_mjd2met(initf.stop_MJD);

  TypedImage<int> * tableP = IFileSvc::instance().editImageInt(initf.OutFile, "Primary");
  Header & headP = tableP->getHeader();
  headP["DATE"].set(ts);
  headP["DATE-OBS"].set(startTm);
  headP["DATE-END"].set(endTm);
  headP["TSTART"].set(metstart);
  headP["TSTOP"].set(metstop);
  headP["FILENAME"].set(facilities::Util::basename(initf.OutFile));
  delete tableP;




  Table * table = IFileSvc::instance().editTable(initf.OutFile, "SC_DATA");
  // Only make the table big enough for the points we want, not the 
  // extra buffer we have.
  Oat->ent=(Oat->ent)-int(stop_buffer/initf.Resolution)+2;
  table->setNumRecords(Oat->ent);
  int k = 0;
  Header & header = table->getHeader();
  header["DATE"].set(ts);
  header["DATE-OBS"].set(startTm);
  header["DATE-END"].set(endTm);
  header["TSTART"].set(metstart);
  header["TSTOP"].set(metstop);

  std::vector<double> posit(3);
  osf.info(2) <<"\nUTC Current Time is " << ts << "\nTable should contain " << Oat->ent <<" elements\n\n";
  osf.info(2) <<"Starting loop to write output file\n";

  // previous start and stop values
  long int pStart= 0;
  long int pStop = 0;
  // This is the loop that writes each line in the fits data table
  for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {



    posit[0] = Oat->X[k]*1000.0;
    posit[1] = Oat->Y[k]*1000.0;
    posit[2] = Oat->Z[k]*1000.0;

/*
    posit[0] = Oat->X[k]*1.0;
    posit[1] = Oat->Y[k]*1.0;
    posit[2] = Oat->Z[k]*1.0;

*/

    (*itor)["SC_POSITION"].set(posit );
    (*itor)["LAT_GEO"].set(Oat->Lat[k]);
    (*itor)["LON_GEO"].set(Oat->Lon[k]);
    (*itor)["RAD_GEO"].set(Oat->Hei[k]*1000.0);
    (*itor)["RA_ZENITH"].set(Oat->SatRA[k]);
    (*itor)["DEC_ZENITH"].set(Oat->SatDEC[k]);
    
    (*itor)["B_MCILWAIN"].set("NaN");
    (*itor)["L_MCILWAIN"].set("NaN");
    (*itor)["GEOMAG_LAT"].set("NaN");

    (*itor)["RA_SCZ"].set(Oat->Zra[k]);
    (*itor)["DEC_SCZ"].set(Oat->Zdec[k]);
    (*itor)["RA_SCX"].set(Oat->Xra[k]);
    (*itor)["DEC_SCX"].set(Oat->Xdec[k]);

    (*itor)["RA_NPOLE"].set("NaN");
    (*itor)["DEC_NPOLE"].set("NaN");

    // 5 is zenithpoint/survey so we'll stick with that.
    (*itor)["LAT_MODE"].set(5);
    // 1 means nomSciOps so that seems reasonable
    (*itor)["LAT_CONFIG"].set(1);
    // Simulated data so it is kind of good by definition
    (*itor)["DATA_QUAL"].set(1);

    if(Oat->in_saa[k] == 1){
      (*itor)["IN_SAA"].set(true);
      (*itor)["LIVETIME"].set(0.0);
    } else {
      (*itor)["IN_SAA"].set(false);
      (*itor)["LIVETIME"].set(initf.Resolution*86400.0*0.93);
    }
    
    (*itor)["QSJ_1"].set("NaN");
    (*itor)["QSJ_2"].set("NaN");
    (*itor)["QSJ_3"].set("NaN");
    (*itor)["QSJ_4"].set("NaN");

    do_mjd2cal(Oat->mjd[k], &yy, &MM, &dd, &hh, &mm, &ss);
    sprintf(endTm, "%4d:%02d:%02dT%02d:%02d:%02d", yy, MM, dd, hh, mm, ss);


    long int Stop = (long int)((Oat->mjd[k]-MJDREF)*86400.0+0.5);
    long int Start = Stop - (long int)(initf.Resolution*86400.0);
 
    if((Stop-Start) != (long int)(initf.Resolution*86400.0)){
      osf.err() << "WARNING ===> At k="<<k<<", Interval is "<< (Stop-Start)<<", while resolution is "<< (long int)(initf.Resolution*86400.0)<<"\n";
    }

    if(pStart != 0){
      if((Start - pStart) != (long int)(initf.Resolution*86400.0)){
	osf.err() << "WARNING ===> At k="<<k<<", Interval (from Start time "<<Start<<") is "<< (Start-pStart)<<", while resolution is "<< (long int)(initf.Resolution*86400.0)<<"\n";
      }
    }

    if(pStop != 0){
      if((Stop - pStop) != (long int)(initf.Resolution*86400.0)){
	osf.err() << "WARNING ===> At k="<<k<<", Interval (from Stop time " <<Stop<<") is "<< (Stop-pStop)<<", while resolution is "<< (long int)(initf.Resolution*86400.0)<<"\n";
      }
    }
    
    (*itor)["START"].set(Start);
    (*itor)["STOP"].set(Stop);

    k++;

    pStart = Start;
    pStop  = Stop;
  }
  delete table;
  
}

st_app::StAppFactory<orbitSimApp> g_factory("gtorbsim");
