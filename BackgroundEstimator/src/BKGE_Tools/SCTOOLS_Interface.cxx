#include "BackgroundEstimator/BKGE_Tools.h"
// $Header$

void TOOLS::Run_gtltcube(string GRB_DIR, double TMin, double TMax, string FT2_FILE, float FT1ZenithTheta_Cut, int verbosity) {

  //gtltcube takes parameter binsize for SCTOOLS v<=r14 and binsz for v>=15
  //Have to choose which parameter to use -- I don't want to rely on parsing the version number of science tools
  //so I just check if there exists a parameter binsz
  char buffer[1000];char name[1000];
  buffer[0]='\0'; //empty char array
  sprintf(name,"plist gtltcube |grep binsize");
  FILE * pipe = popen(name,"r");
  char *cha;
  cha =fgets(buffer,sizeof(buffer),pipe);
  pclose(pipe);
  //printf("--%s--\n",buffer);
  if (strcmp(buffer,"")) sprintf(buffer,"gtltcube scfile=%s evfile="" outfile=%s/burst_ltCube.fits dcostheta=0.025 binsize=1 zmax=%f chatter=4 phibins=1 tmin=%f tmax=%f",
     FT2_FILE.c_str(),GRB_DIR.c_str(),FT1ZenithTheta_Cut,TMin,TMax);
  else                  sprintf(buffer,"gtltcube scfile=%s evfile="" outfile=%s/burst_ltCube.fits dcostheta=0.025 binsz=1 zmax=%f chatter=4 phibins=1 tmin=%f tmax=%f",
     FT2_FILE.c_str(),GRB_DIR.c_str(),FT1ZenithTheta_Cut,TMin,TMax);
 
 if (verbosity>3 ) { 
     pipe = popen(buffer, "r");
     printf("%s: Executing command: %s\n",__FUNCTION__,buffer);
     while (fgets(buffer,sizeof(buffer),pipe)) printf("|%s",buffer);
     pclose(pipe);
 }
  else {
     sprintf(buffer,"%s >/dev/null 2>&1",buffer);
     system(buffer);
 }

}

void TOOLS::Run_gtexpcube(string GRB_DIR,  double TMin, double TMax, string FT2_FILE, string DATACLASS, float FT1ZenithTheta_Cut, char * Outfile, float Energy_Min, float Energy_Max, int Energy_Bins, int verbosity) {
 //first check if the ltcube exists
  char name[1000];
  sprintf(name,"%s/burst_ltCube.fits",GRB_DIR.c_str());
  FILE * ftemp  = fopen(name,"r");
  if (ftemp) fclose(ftemp);
  else Run_gtltcube(GRB_DIR, TMin, TMax, FT2_FILE, FT1ZenithTheta_Cut, verbosity);

  char buffer[1000];
  buffer[0]='\0'; //empty char array
  sprintf(name,"plist gtexpcube |grep deltaphi");
  FILE * pipe = popen(name,"r");
  fgets(buffer,sizeof(buffer),pipe);
  pclose(pipe);
  
  if (!strcmp(buffer,"")) sprintf(buffer,"gtexpcube infile=%s/burst_ltCube.fits evfile="" cmfile=NONE outfile=%s irfs=%s nxpix=1 nypix=1 pixscale=1 coordsys=GAL xref=0 yref=0 axisrot=0 proj=CAR emin=%f emax=%f enumbins=%d bincalc=CENTER chatter=4  ",
    	GRB_DIR.c_str(),Outfile,DATACLASS.c_str(),Energy_Min,Energy_Max,Energy_Bins);
  else                    sprintf(buffer,"gtexpcube infile=%s/burst_ltCube.fits evfile="" cmfile=NONE outfile=%s irfs=%s nxpix=1 nypix=1 pixscale=1 coordsys=GAL xref=0 yref=0 axisrot=0 proj=CAR emin=%f emax=%f enumbins=%d bincalc=CENTER chatter=4 deltaphi=45",
	GRB_DIR.c_str(),Outfile,DATACLASS.c_str(),Energy_Min,Energy_Max,Energy_Bins);

  if (verbosity>3) {
        FILE * pipe = popen(buffer, "r");
        printf("%s: Executing command: %s\n",__FUNCTION__,buffer);
        while (fgets(buffer,sizeof(buffer),pipe))   printf("|%s",buffer);
        pclose(pipe);
  }
  else  {
        sprintf(buffer,"%s >/dev/null 2>&1",buffer);
        system(buffer);
  }
}
