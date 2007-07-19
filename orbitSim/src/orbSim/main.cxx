#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_facilities/Env.h"

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



class MyApp : public st_app::StApp {
  public:
  MyApp(): osf("MyApp", "", 0) {
      osf.setMethod("MyApp");
  }
  virtual void run();

  private:

  /// Stream to control output through verbosity level
  st_stream::StreamFormatter osf;
};


void MyApp::run() {
  using namespace tip;
  InitI initf;
  int stat;

  osf.setMethod("run");

  initf.occflag = 1;
  initf.chat = 0;
  initf.debug = 0;


  st_app::AppParGroup & pars(getParGroup("orbitSim/v0/pfiles/gttakosim"));


  pars.Prompt("typeinput");
  std::string Input = pars["typeinput"];
  osf.info() << "Input Type is: " << Input.c_str() << std::endl;
  if(match((const char*) Input.c_str(), "file") == 1){
    pars.Prompt("initFile");

    std::string initFile = pars["initFile"];
    const char *fname = initFile.c_str();
    stat = parseInit(fname, &initf);

    st_stream::SetMaximumChatter(initf.chat);
    st_stream::SetDebugMode(initf.debug);

  } else if (match((const char*) Input.c_str(), "console") == 1){
    pars.Prompt("start_MJD");
    initf.start_MJD = pars["start_MJD"];

    pars.Prompt("stop_MJD");
    initf.stop_MJD = pars["stop_MJD"];

    pars.Prompt("TLType");
    std::string Tlt = pars["TLType"];
    initf.TLtype = Tlt;



    if((match((const char*) Tlt.c_str(), "^TAKO$") == 1) || (match((const char*) Tlt.c_str(), "^ASFLOWN$") == 1)){
      pars.Prompt("Timeline");
      std::string Tml = pars["Timeline"];
      initf.TLname = Tml;

    } else if (match((const char*) Tlt.c_str(), "SINGLE") == 1){
      pars.Prompt("TimelnCmd");
      std::string Tml = pars["TimelnCmd"];
      initf.TLname = Tml;
 
    } else {
      throw std::runtime_error("\nERROR: UNKNOWN Timeline type {either TAKO, or ASFLOWN or SINGLE}\n\n");

    }
    pars.Prompt("EphemName");
    std::string EphN = pars["EphemName"];
    initf.EPHname = EphN;



    //  } else if(match((const char*) initf.EPHfunc,"^tlederive$") == 1){
    pars.Prompt("EphemFunc");
    std::string EpF = pars["EphemFunc"];
    if(!((match((const char*) EpF.c_str(), "^xyzll_eph$")==1) ||
	 (match((const char*) EpF.c_str(), "^yyyy_eph$")==1) ||
	 (match((const char*) EpF.c_str(), "^tlederive$")==1))){
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

    pars.Prompt("saafunc");
    std::string saaFu = pars["saafunc"];
    initf.saafunc = saaFu;

    

    stat = 1;

  } else {


    throw std::runtime_error("\nERROR: UNKNOWN Input type {either file or console}\n\n");
  }


  pars.Save();






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
    oBuf << "SAA function           " << initf.saafunc << ", (saa, latsaa)" << std::endl;
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

  osf.info().precision(15);
  osf.err().precision(15);
  osf.warn().precision(15);
  osf.out().precision(15);

  osf.info(3) <<"Initial start time=" << initf.start_MJD << ", corrected time=" << stmjd  << std::endl;
  initf.start_MJD = stmjd; 



  double enmjd = initf.stop_MJD;
  fday = (enmjd - (double)((int)enmjd))*1440.0;
  fday = (double)((int)(fday/initf.Resolution)+1)*initf.Resolution;
  enmjd = (double)((int)enmjd)+fday/1440.0;
  osf.info(3) <<"Initial stop time=" << initf.stop_MJD << ", corrected time=" << enmjd  << std::endl;


  initf.stop_MJD = enmjd;
  osf.info(1) <<"Optional File: ";
  
  if( initf.OptFile.empty()){
    osf.info(1) << "(NULL)";
  } else {
    osf.info(1) << initf.OptFile;
  }
  osf.info(1) << std::endl;



  FILE *ephF = NULL;

  osf.info(2) << "Opening Ephem file " << initf.EPHname << " for reading\n";
  if ( (ephF=fopen(initf.EPHname.c_str(),"r")) == NULL) {
    std::string fname( initf.EPHname);
    throw std::runtime_error("Cound not open Ephemeredis file:\n" + fname);
  }

  initf.Resolution = initf.Resolution/minInDay;
  EphemData * ephemeris = NULL;

  osf.info(2) << "Populating Ephemeris structure by calling " << initf.EPHfunc << "function.\n";

  if(match( initf.EPHfunc.c_str(),"^yyyy_eph$") == 1){
    ephemeris = yyyy_eph(ephF, initf.start_MJD, initf.stop_MJD, \
			 initf.Units, initf.Resolution);
  } else if(match( initf.EPHfunc.c_str(),"^xyzll_eph$") == 1){
    ephemeris = xyzll_eph(ephF, initf.start_MJD, initf.stop_MJD, \
			  initf.Units, initf.Resolution);
  } else if(match( initf.EPHfunc.c_str(),"^tlederive$") == 1){
    ephemeris = tlederive(ephF, initf.start_MJD, initf.stop_MJD, \
			  initf.Units, initf.Resolution);
  }


  fclose(ephF);
  if (ephemeris == NULL){
    throw std::runtime_error("\nPossibly something went wrong while reading/generating ephemeris.\nThe Ephemeris structure is still \"NULL\"\n\n");
  }

  osf.info(3) <<"From Ephem file, start mjd = " << ephemeris->MJD[0] <<  std::endl;




  Attitude *Oat = NULL;

  if(match( initf.TLtype.c_str(), "^TAKO$") == 1){
    Oat = makeAttTako(&initf, ephemeris);
  } else if (match( initf.TLtype.c_str(), "^ASFLOWN$") == 1){
    Oat = makeAttAsFl(&initf, ephemeris);
  } else if (match( initf.TLtype.c_str(), "^SINGLE$") == 1){
    Oat = doCmd(&initf, ephemeris);
  }

  if(Oat == NULL){
    throw std::runtime_error("\nPossibly something went wrong while calculating the spacecraft attitude.\nThe Attitude structure is still \"NULL\"\n\n");
  }



  std::string orbitsimroot = st_facilities::Env::getDataDir("orbitSim");

  std::string ifname("ft2.fits");

  std::string sfname = st_facilities::Env::appendFileName(orbitsimroot, ifname);

  osf.info(1) <<"OutPut File template is "<<sfname.c_str()<<"\n";

  IFileSvc::instance().createFile(initf.OutFile, sfname.c_str());

  time_t rawtime;
  struct tm * ptm;

  time ( &rawtime );
  ptm = gmtime ( &rawtime );

  ptm->tm_year = ptm->tm_year+1900;
  ptm->tm_mon = ptm->tm_mon+1;

  char ts[20];

  sprintf(ts, "%4d:%02d:%02dT%02d:%02d:%02d", ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);



  Table * table = IFileSvc::instance().editTable(initf.OutFile, "SC_DATA");

  table->setNumRecords(Oat->ent-1);
  int k = 0;
  Header & header = table->getHeader();
  header["DATE"].set(ts);

  std::vector<double> posit(3);
  osf.info(1) <<"\nUTC Current Time is " << ts << "\nTable should contain " << Oat->ent <<" elements\n\n";
  osf.info(2) <<"Starting loop to write output file\n";

  for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {



    posit[0] = Oat->X[k]*1000.0;
    posit[1] = Oat->Y[k]*1000.0;
    posit[2] = Oat->Z[k]*1000.0;

    posit[0] = Oat->X[k]*1.0;
    posit[1] = Oat->Y[k]*1.0;
    posit[2] = Oat->Z[k]*1.0;



/*
    printf("%4d) ok, mjd=%f\n", k, Oat->mjd[k]);
    printf("SC_POSITION= %f %f %f\n", posit[0], posit[1], posit[2]);
    printf("LAT_GEO=%f\n", Oat->Lat[k]);
    printf("LON_GEO=%f\n", Oat->Lon[k]);
    printf("RAD_GEO=%f\n", Oat->Hei[k]*1000.0);
    printf("RA_ZENITH=%f\n", Oat->SatRA[k]);
    printf("DEC_ZENITH=%f\n", Oat->SatDEC[k]);
    printf("RA_SCZ=%f\n", Oat->Zra[k]);
    printf("DEC_SCZ=%f\n", Oat->Zdec[k]);
    printf("RA_SCX=%f\n", Oat->Xra[k]);
    printf("DEC_SCX=%f\n", Oat->Xdec[k]);
    printf("setting LIVETIME\n\n");
*/

    (*itor)["SC_POSITION"].set(posit );
    (*itor)["LAT_GEO"].set(Oat->Lat[k]);
    (*itor)["LON_GEO"].set(Oat->Lon[k]);
    (*itor)["RAD_GEO"].set(Oat->Hei[k]*1000.0);
    (*itor)["RA_ZENITH"].set(Oat->SatRA[k]);
    (*itor)["DEC_ZENITH"].set(Oat->SatDEC[k]);
    (*itor)["RA_SCZ"].set(Oat->Zra[k]);
    (*itor)["DEC_SCZ"].set(Oat->Zdec[k]);
    (*itor)["RA_SCX"].set(Oat->Xra[k]);
    (*itor)["DEC_SCX"].set(Oat->Xdec[k]);



    if(Oat->in_saa[k] == 1){
      (*itor)["IN_SAA"].set(true);
      (*itor)["LIVETIME"].set(0.0);
      (*itor)["DEADTIME"].set(initf.Resolution*86400.0);
    } else {
      (*itor)["IN_SAA"].set(false);
      (*itor)["LIVETIME"].set(initf.Resolution*86400.0*0.93);
      (*itor)["DEADTIME"].set(initf.Resolution*86400.0*0.07);
    }

    long int Stop = (long int)((Oat->mjd[k]-MJDREF)*86400.0+0.5);
    long int Start = Stop - (long int)(initf.Resolution*86400.0);

    
    (*itor)["START"].set(Start);
    (*itor)["STOP"].set(Stop);

    k++;
  }
  delete table;
  
}

st_app::StAppFactory<MyApp> g_factory("gttakosim");

//make ; make install ; orbitSim.exe
///devtools/src/orbit_simulator/glast.init
