/**
 * @file OrbSim.cc
 * @brief This file contains functions to parse an init file and start the attitide calculation.
 * @author Giuseppe Romeo
 * @date Created:  Nov 15, 2005
 * 
 * $Header$
 */

// These two libraries are needed for the regression tests
#include <iostream>
#include <fstream>

#include "orbitSim/orbitSimStruct.h"
#include "orbitSim/OrbSim.h"
#include "orbitSim/atFunctions.h"
#include "orbitSim/functions.h"

#include <cstdlib>
#include <stdio.h>
#include <algorithm>
#include <stdexcept>
#include <string>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "st_stream/Stream.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"


  /// Stream to control output through verbosity level
st_stream::StreamFormatter losf("OrbSim", "", 2);

// Parse the initial parameters passed from the top level main function.
int parseInit( const char *fname, InitI *inA) {

  FILE *inf;
  char ln[bufsz];
  const int itm = 12;
  int it = 0;
  
  losf.setMethod("parseInit");


  if ( (inf=fopen(fname,"r")) == NULL) {
    std::string name(fname);
    throw std::runtime_error("Cound not open init file:\n" + name );

  } else {

    while(fgets(ln, bufsz,inf)) {
      //      printf("Found line: %s\n", ln);
      ln[strlen(ln) -1] = '\0';
      
      while (match_str((const char*)ln, "^ ") == 1) {
        char *tln = processline(ln, ' ');
        strcpy(ln, tln);

      }

      if(match_str((const char*)ln, "^#") == 1) {
        //      printf("Ignored line: %s\n", ln);
        continue;
      } else if(match_str((const char*)ln, "^start_MJD") == 1) {
        double t = -1.0;;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if(t>0.0){
            inA->start_MJD = t;
            it++;
          }
        }
      } else if(match_str((const char*)ln, "^stop_MJD") == 1) {
        double t = -1.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if(t>0.0){
            inA->stop_MJD = t;
            it++;
          }
        }

      } else if(match_str((const char*)ln, "^EAA") == 1) {
        double t = 99999.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if((t>=0.0 && t<=180.0)){
            inA->EAA = t;
          }
        }

      } else if(match_str((const char*)ln, "^Initial_RA") == 1) {
        double t = 99999.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if((t>=0.0 && t<=360.0) || (t>= -180.0 && t<= 180.0)){
            inA->Ira = t;
            it++;
          }
        }

      }else if(match_str((const char*)ln, "^Initial_DEC") == 1) {
        double t = 99999.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if((t>=-90.0 && t<= 90.0) || (t>= 0.0 && t<=180.0)){
            inA->Idec = t;
            it++;
          }
        }

      }else if(match_str((const char*)ln, "^Earth_Avoid") == 1) {
        int flgocc = 1;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%d", &flgocc);
          if(flgocc <= 0){
            inA->occflag = 0;
            losf.out() << "Earth avoidance is disabled\n\n";
          } else if ( flgocc >= 1){
            inA->occflag = 1;
          }
        }

      } else if(match_str((const char*)ln, "^Timeline") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }

          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL;
            if(jnk[0] == '|'){
              TL = strtok(jnk, "#");
            } else {
              TL = strtok(jnk, " #");
            }
            int len = strlen(TL);
            if(len > 0){
              inA->TLname.assign(TL);
              it++;
            } else {
              inA->TLname.assign("Not specified");

            }
          }
        }
      } else if(match_str((const char*)ln, "^TLType") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL = strtok(jnk, " #");
            inA->TLtype.assign(TL);
            int len = strlen(TL);
            if(len > 0){
              inA->TLtype.assign(TL);
              it++;
            }

          } else {
            inA->TLtype.assign("Not Specified");
          }
        }
      } else if(match_str((const char*)ln, "^EphemName") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL = strtok(jnk, " #");
            int len = strlen(TL);
            if(len > 0){
              inA->EPHname.assign(TL);
              it++;
            }
          } else {
            inA->EPHname.assign("Not Specified");
          }
        }
      }  else if(match_str((const char*)ln, "^EphemFunc") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0) {
            char *TL = strtok(jnk, " #");
            int len = strlen(TL);
            if(len > 0 && jnk[0] != '#'){
              inA->EPHfunc.assign(TL);
              it++;
            }
          } else {
              inA->EPHfunc.assign("Not Specified");
          }
        }
      } else if(match_str((const char*)ln, "^OutPutFile") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL = strtok(jnk, " #");
            int len = strlen(TL);
            if(len > 0){
              inA->OutFile.assign(TL);
              it++;
            }
          } else {
            inA->OutFile.assign("Not Specified");

          }
        }
      } else if(match_str((const char*)ln, "^OptFile") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL = strtok(jnk, " #");
            int len = strlen(TL);
            if(len > 0){
              inA->OptFile.assign(TL);
            }
          } 
        }
      } else if(match_str((const char*)ln, "^saafile") == 1) {
        char *jnk = processline(ln, '=');
        if(jnk != NULL) {

          // Removing initial space in any
          while(jnk[0] == ' '){
            ++jnk;
          }
          int lenj = strlen(jnk);
          if(lenj > 0 && jnk[0] != '#') {
            char *TL = strtok(jnk, " #");
            int len = strlen(TL);
            if(len > 0){
              inA->saafile.assign(TL);
              it++;
            }
          } else {
            inA->saafile.assign("Not Specified");

          }
        }
      } else if(match_str((const char*)ln, "^Units") == 1) {
        double t = -1.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if(t > 0.0){
            inA->Units = t;
            it++;
          }
        }

      } else if(match_str((const char*)ln, "^Resolution") == 1) {
        double t = -1.0;
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          sscanf(jnk, "%lf", &t);
          if(t > 0.0){
            inA->Resolution = t;
            it++;
          }
        }

      } else if(match_str((const char*)ln, "^Chatter") == 1) {
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          int t = -1;
          sscanf(jnk, "%d", &t);
          if(t > 0){
            inA->chat = t;
          }
        }
      } else if(match_str((const char*)ln, "^Debug") == 1) {
        char *jnk = processline(ln, '=');
        if (jnk != NULL) {
          int t = -1;
          sscanf(jnk, "%d", &t);
          if(t > 0){
            inA->debug = t;
          }
        }
      }

    }
  }

  int rv = 1;

  // Some checks


  if(!((match_str( inA->TLtype.c_str(), "^TAKO$") == 1) ||
       (match_str( inA->TLtype.c_str(), "^ASFLOWN$") == 1) ||
       (match_str( inA->TLtype.c_str(), "^SINGLE$") == 1))){
    it--;
  }

  if(!((match_str( inA->EPHfunc.c_str(), "^yyyy_eph$") == 1) ||
       (match_str( inA->EPHfunc.c_str(), "^xyzll_eph$") == 1) || 
       (match_str( inA->EPHfunc.c_str(), "^tlederive$") == 1))){
    it--;
  }


  if(it != itm){
    rv = 0;
  }

  return rv;

}


char *processline(char *ln, char find) {

  char *first_ptr;
  char *last_ptr ;

  int nl = strlen(ln);
  int i = 0;

  last_ptr = ln;
  first_ptr = ln;

  if(first_ptr[0] == '#'){
    return NULL;
  }
  
  while(first_ptr[0] != find && i<nl){

    //    printf("%s:%d, in processline, first=%c\n", __FILE__,__LINE__, first_ptr[0]);
    if(*first_ptr == '\0')
      break;

    ++first_ptr;
    i++;
  }


  if(*first_ptr == '\0'){

    throw std::runtime_error("\nCannot parse line\n\n");
  }
 
  *first_ptr = '\0';
  ++first_ptr;

  //  printf("First=%s, Last=%s\n", first_ptr, last_ptr);
  return first_ptr;
}



// makeAttTako does all of the heavy lifting for the TAKO timeline attitude calculation
Attitude * makeAttTako(InitI *ini, EphemData *ephem) {

  // Create and Open regression test file object first (Todo: Handle this better later.) ~JA 20140908
  std::ofstream	takoTestFile;
  takoTestFile.open ("TakoAttitude.out");

  FILE *ITL;
  FILE *OutF = NULL;
  double Timespan, res;
  int inum, oinum, i;

  double org_stT = ini->start_MJD;
  double org_enT = ini->stop_MJD;

  losf.setMethod("makeAttTako");
  losf.err().precision(15);
  losf.info().precision(15);

  Timespan = (ini->stop_MJD-ini->start_MJD);
  res = ini->Resolution;
  inum = (int)((Timespan+res/2.0)/res);
  inum++; // Include the end point
  Attitude *OAtt = allocateAttitude(inum);

  if ( OAtt == (Attitude *)NULL) {
    throw std::runtime_error("\nERROR: In makeAttTako, cannot allocate attitude data structure\n\n");
  }

  oinum = inum;


  char ln[bufsz];

  if ( (ITL=fopen(ini->TLname.c_str(),"r")) == NULL) {
    std::string fname(ini->TLname);
    throw std::runtime_error("\nCound not open Timeline file "+fname);
  } else {  
    double pra = ini->Ira; // Initial spacecraft ra
    double pdec = ini->Idec; // Initial spacecraft dec
    double tl_start = 0.0; //Timeline Start MJD
    double tl_end = 0.0;   //Timeline End MJD
    double res = ini->Resolution;  //convert resolution in days for mjd
    int flg = 0; // Multipurpose flag (mostly used for errors).  Todo: Should split this flag variable into several single-task oriented flags. ~JA 20141008
    double mjdt = 0.0; //Profile Start MJD
    int mode = -1; // Mode 1 = Survey, Mode 2 = Obs, Mode 3 = Profile
    double offset = -999.0; //Rocking angle offset
    double ra = -999.0;
    double dec = -999.0;
    double mjds = 0.0; //Slew MJD
    double mjde = 0.0; //Profile End MJD
    SurProf profile;

    double lastend = 0.0;

   int yyy, doy, hh, mm, ss;
   char lineBuf[bufsz];


   // Loop to find the beginning of the first command in the timeline.  Use the timestamp for this command as the timeline start time.

   while(fgets(ln,bufsz, ITL)) {
     strcpy(lineBuf,ln);
	   if (match_str((const char*) ln, "Begin") == 1) {
		   break;
	   }
   }

   char *LB = strtok(lineBuf, " ");
   LB = strtok(NULL, " ");
   sscanf(LB, "%d/%d:%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
   tl_start = do_utcj2mjd (yyy, doy, hh, mm, ss);

   // Continue looping until the last line of the timeline is found.  Use the last line's timestamp as the timeline end time.
   while(fgets(ln, bufsz, ITL)) {
     strcpy(lineBuf,ln);
   }

   LB = strtok(lineBuf, " ");
   LB = strtok(NULL, " ");
   sscanf(LB, "%d/%d:%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
   tl_end = do_utcj2mjd (yyy, doy, hh, mm, ss);

   if (ini->stop_MJD < tl_start) throw std::runtime_error("\nERROR: Invalid Time Range. stop_MJD occurs before ATS Timeline begins!");
   if (ini->start_MJD > tl_end) throw std::runtime_error("\nERROR: Invalid Time Range. start_MJD occurs after ATS Timeline ends!");
   if (ini->start_MJD < tl_start) throw std::runtime_error("\nERROR: Invalid Time Range. start_MJD occurs before ATS Timeline begins!");
   if (ini->stop_MJD > tl_end) { //throw std::runtime_error("\nERROR: Invalid Time Range. stop_MJD occurs after ATS Timeline ends!");
     losf.warn(1) << "WARNING: stop_mjd=" << ini->stop_MJD << " exceeds ATS Timeline End=" << tl_end << ". Orbitsim will only run up to mjd=" << tl_end << "\n";
     ini->stop_MJD = tl_end;
   }

   //Reset the file input buffer so that looping restarts from the beginning.
   memset(ln,'0',bufsz);
   ITL=fopen(ini->TLname.c_str(),"r");

   // Loop until a command keyword is identified and act accordingly.
    while(fgets(ln, bufsz, ITL)) {

      if(strlen(ln) == 1){
        continue;
      }
      if((match_str((const char*) ln, " Survey ") == 1) &&
         (match_str((const char*) ln, "Begin") == 1)){

        mode = 1;
        mjdt = getMJD(ln);
        //      printf("SURVEY from %s ==> %f\n", ln, mjdt);

        while(fgets(ln, bufsz, ITL)) {
          if ((match_str((const char*) ln, " offset ") == 1)){
            char *jnk = processline(ln, '=');
            if (jnk != NULL) {
              sscanf(jnk, "%lf", &offset);
            }
          } else if((match_str((const char*) ln, " Slew ") == 1) && 
                    (match_str((const char*) ln, "End") == 1)) {
            mjds =getMJD(ln);

          } else if ((match_str((const char*) ln, " Survey ") == 1) &&
                     (match_str((const char*) ln, "End") == 1)) {
            mjde =getMJD(ln);
            break;
          }
        }

        // There could be cases where there is no slewing,
        // in such cases the end slew is the same as the 
        // starting of the observation.

        if(mjds == 0.0){
          mjds = mjdt;
        }
        
        //  Some sanity checks

        if(offset <-180.0 || offset > 180.0){
          losf.warn(1) << "\t\t\tERROR:\nFixed Survey Observation starting at MJD " << mjdt << "\nDOES NOT have a proper offset (" << offset << ")\n\n";

          flg = 3;
        }
        if(mjde < mjdt){
          losf.warn(1)  << "\t\t\tERROR:\nFixed Survey Observation starting at MJD " << mjdt << "\nwill end at an earlier time " << mjde << "\n\n";
          flg = 3;
        }
        if(mjde < mjds){
          losf.warn(1)  << "\t\t\tERROR:\nFixed Survey Observation starting at MJD " << mjdt << "\nends at " << mjde << ", but the end slew is at " << mjds << "\n\n";

          flg = 3;
        }

      } else if((match_str((const char*) ln, " Obs ") == 1) &&
               (match_str((const char*) ln, "Begin") == 1)){
        mode = 2;
        mjdt = getMJD(ln);
        //      printf("POINTED from %s ==> %f\n", ln, mjdt);
        while(fgets(ln, bufsz, ITL)) {
          if ((match_str((const char*) ln, " RA ") == 1) &&
                    (match_str((const char*) ln, "^//") == 1)){
            char *jnk = processline(ln, '=');
            if (jnk != NULL) {
              sscanf(jnk, "%lf", &ra);
            }
          }else if ((match_str((const char*) ln, " dec ") == 1) &&
                    (match_str((const char*) ln, "^//") == 1)){
            char *jnk = processline(ln, '=');
            if (jnk != NULL) {
              sscanf(jnk, "%lf", &dec);
            }
          } else if((match_str((const char*) ln, " Slew ") == 1) && 
                    (match_str((const char*) ln, "End") == 1) &&
                    (mode != -1)) {
            mjds =getMJD(ln);

          } else if ((match_str((const char*) ln, " Obs ") == 1) &&
                     (match_str((const char*) ln, "End") == 1)&&
                    (mode != -1)) {
            mjde =getMJD(ln);
            break;
          }
        }


        // There could be cases where there is no slewing,
        // in such cases the end slew is the same as the 
        // starting of the observation.

        if(mjds == 0.0){
          mjds = mjdt;
        }
        

        //  Some sanity checks

        if(ra <0.0 || ra > 360.0){
          losf.warn(1) << "\t\t\tERROR:\nPointed Observation starting at MJD " << mjdt << "\nDOES NOT have a proper RA (" << ra << ")\n\n";
          flg = 3;
        }
        if(dec <-90.0 || dec > 90.0){
          losf.warn(1) << "\t\t\tERROR:\nPointed Observation starting at MJD " << mjdt << "\nDOES NOT have a proper DEC (" << dec << ")\n\n";
          flg = 3;
        }
        if(mjde < mjdt){
          losf.warn(1) << "\t\t\tERROR:\nPointed Observation starting at MJD " << mjdt << "\nwill end at an earlier time " << mjde << "\n\n";
          flg = 3;
        }
        if(mjde < mjds){
          losf.warn(1) << "\t\t\tERROR:\nPointed Observation starting at MJD " << mjdt << "\nends at " << mjde << ", but the end slew is at " << mjds << "\n\n";
          flg = 3;
        }

      } else if((match_str((const char*) ln, " Profile ") == 1) &&
               (match_str((const char*) ln, "Begin") == 1)){

        mode = 3;
        mjdt = getMJD(ln);

        while(fgets(ln, bufsz, ITL)) {
          if((match_str((const char*) ln, " Slew ") == 1) &&
                    (match_str((const char*) ln, "End") == 1)) {
            mjds =getMJD(ln);

          } else if ((match_str((const char*) ln, " Profile ") == 1) &&
                     (match_str((const char*) ln, "End") == 1)) {
            mjde =getMJD(ln);
            break;

          }else if((match_str((const char*) ln, " Rocking ") == 1) && 
                    (match_str((const char*) ln, " Profile:") == 1)) {
            if((fgets(ln, bufsz, ITL)) != NULL){
              if(match_str((const char*) ln, " ROCKSTART ") == 1){
                char *jnk = processline(ln, '=');
                if (jnk != NULL) {
                  char date[17];
                  double ep;
                  sscanf(jnk, "%s (%lf)", date, &ep);
                  profile.epoch = do_met2mjd(ep);
                }
              }
            } else {
              throw std::runtime_error("ERROR: Could read TAKO Timeline any further\n");
            }

            if((fgets(ln, bufsz, ITL)) != NULL){
              if(match_str((const char*) ln, " ROCKDEFAULT ") == 1){
                char *jnk = processline(ln, '=');
                double to;
                if (jnk != NULL) {
                  sscanf(jnk, "%lf", &to);
                  profile.defofst = to;
                }
              }
            } else {
              throw std::runtime_error("ERROR: Could read TAKO Timeline any further\n");
            }

            if((fgets(ln, bufsz, ITL)) != NULL){
              if((match_str((const char*) ln, " ROCKTIME ") == 1) &&
                 (match_str((const char*) ln, " ROCKANGLE") == 1)){
                int i = 0;
                for(i=0; i<17; i++){
                  if((fgets(ln, bufsz, ITL)) != NULL){
                    int idx;
                    double tm, an;
                    char jnk[24];
                    sscanf(ln, "%s%02d %lf %lf", jnk, &idx, &tm, &an);
                    losf.info(6) << "While Reading line: "<<ln<<"\n"<<i<<") Got Angle: "<<an<<"Time: "<<tm<<"\n";
                    profile.ofsts[i] = an;
                    profile.times[i] = tm;
                  } else {
                    throw std::runtime_error("ERROR: Could read TAKO Timeline any further\n");
                  }
                }
              }
            } else {
              throw std::runtime_error("ERROR: Could read TAKO Timeline any further\n");
            }
          } // End of else if Rocking Profile
        }   /* end of while(fgets(ln, bufsz, ITL)) */
      } /* end of Profile Begin */
      
      losf.info(3) << "mode="<<mode<<", mjdt="<<mjdt<<", mjds="<<mjds<<", mjde="<<mjde<<"\n";

      if(flg > 0){
        continue;
      }

      losf.info(4) << "About to check reallocation\n";

      if(mjde <= ini->start_MJD){
        flg = 100;
      }else if (mjdt < ini->start_MJD && mjde > ini->start_MJD){

        losf.info(4) << "reallocating attitude structure\n";

        ini->start_MJD = mjds;
        ephem = deallocateEphem(ephem);
        FILE *ephF = fopen(ini->EPHname.c_str(),"r");

        if(match_str( ini->EPHfunc.c_str(), "^yyyy_eph$") == 1){
          ephem = yyyy_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                           ini->Units, ini->Resolution);
        }else if(match_str( ini->EPHfunc.c_str(), "^xyzll_eph$") == 1){
          ephem = xyzll_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                            ini->Units, ini->Resolution);
        }else if(match_str( ini->EPHfunc.c_str(), "^tlederive$") == 1){
          ephem = tlederive(ephF, ini->start_MJD, ini->stop_MJD, \
                            ini->Units, ini->Resolution);
        }

        fclose(ephF);

        Timespan = (ini->stop_MJD-ini->start_MJD);
        res = ini->Resolution;
        inum = (int)((Timespan+res/2.0)/res);
        inum++;
        OAtt = reallocateAttitude(inum, OAtt);

        if ( OAtt == (Attitude *)NULL) {
          throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n");
        }

      } else if(mjdt < ini->stop_MJD &&  mjde > ini->stop_MJD){
        losf.info(4) << "Reallocating items: stop_MJD=" << ini->stop_MJD << ", inum=" << inum << "\n";

        ini->stop_MJD = mjde;

        ephem = deallocateEphem(ephem);
        FILE *ephF = fopen(ini->EPHname.c_str(),"r");


        if(match_str( ini->EPHfunc.c_str(), "^yyyy_eph$") == 1){
          ephem = yyyy_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                           ini->Units, ini->Resolution);
        }else if(match_str( ini->EPHfunc.c_str(), "^xyzll_eph$") == 1){
          ephem = xyzll_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                            ini->Units, ini->Resolution);
        }else if(match_str( ini->EPHfunc.c_str(), "^tlederive$") == 1){
          ephem = tlederive(ephF, ini->start_MJD, ini->stop_MJD, \
                            ini->Units, ini->Resolution);
        }


        fclose(ephF);


        Timespan = (ini->stop_MJD-ini->start_MJD);
        res = ini->Resolution;
        inum = (int)((Timespan+res/2.0)/res);
        inum++;
        OAtt = reallocateAttitude(inum, OAtt);

        if ( OAtt == (Attitude *)NULL) {
          throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n");
        }


      } else if (mjdt >= ini->stop_MJD){
        losf.info(4) << "Reached end of loop\n";
        break;
      }
// Todo: Find out why the next 7 lines are necessary.  ~JA 20141008
      if(flg == 0){

        if(lastend > 0.0){
          if(lastend < mjdt){
            mjdt = lastend;
          }
        }

        lastend = mjde;

        //Based on mode set by command keyword, call the appropriate attitude calculation function.
        if(mode == 1 || mode == 2){
          double lpos[2];
          //Todo: Rewrite mode 1 (Survey mode) to use MakeProfiled instead of MakeAtt (JA: 20141008)
          losf.info(3) << "Calling MakeAtt with: mjdt="<<mjdt<<", mjde="<<mjde<<", mjds="<<mjds<<", pra="<<pra<<", pdec="<<pdec<<", offset="<<offset<<", ra="<<ra<<", dec="<<dec<<", mode="<<mode<<"\n";
          MakeAtt(mjdt, mjde, mjds, pra, pdec, offset, ra, dec, mode, ini->Resolution, ephem, lpos, OAtt, ini->start_MJD);
          pra = lpos[0];
          pdec = lpos[1];

        } else if (mode == 3){

          losf.info(3) << "Calling MakeProfiled with: mjdt="<<mjdt<<", mjde="<<mjde<<", mjds="<<mjds<<", pra="<<pra<<", pdec="<<pdec<<"\n";

          MakeProfiled(mjdt, mjde, ini->Resolution, pra, pdec, profile.epoch, profile.times, profile.ofsts, ephem, OAtt, ini->start_MJD);

          int idx = (int)(((mjde-ini->start_MJD))/ini->Resolution);
          idx--;
          pra = OAtt->Zra[idx];
          pdec = OAtt->Zdec[idx];
          //printf("Called MakeProfiled, mjdt=%f mjde=%18.10f startMJD=%18.10f Resol=%15.10f pra=%f pdec=%f idx=%d\n\n\n",  mjdt, mjde, ini->start_MJD, ini->Resolution, pra, pdec, idx);
        }

        // flg == 100 IIF mjde <= ini->start_MJD
      }else if(flg == 100){
        if(mode == 2){
          pra = ra;
          pdec = dec;
        }else if (mode == 1){
          double RaDec[2];
          MakeSurvey(mjde, mjde, res, offset, ephem, OAtt, RaDec, 0, ini->start_MJD);
          pra = RaDec[0];
          pdec = RaDec[1];
        }
      }



      losf.info(3) << "Looping with start mjd = "<<mjdt<<"\n";
      flg = 0;
      mode = -1;
      mjdt = 0.0;
      mjds = 0.0;
      mjde = 0.0;
      offset = -999.0;
      ra = -999.0;
      dec = -999.0;


    }
  }

  // Calculate target occultation and limb trace
  if(ini->occflag == 1){
    // Getting the occultation

    occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
             OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);

    doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
  
    int rechk = 0;

    if(rechk){
      occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
               OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);

      doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
    }
  }

  //Calculate SAA instances
  saa( ephem, ini->saafile.c_str(), ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
  
  OAtt->ent = inum;

  if(!ini->OptFile.empty() ){
    if ( (OutF=fopen(ini->OptFile.c_str(),"w")) == NULL) {
      losf.warn(1) << "Cound not open OutPut file " << ini->OptFile << "\n";
    }  
    fprintf(OutF, "     MJD          UTC            SAT_RA       SAT_DEC       X_RA       X_DEC       Y_RA       Y_DEC       Z_RA       Z_DEC       IN_SAA\n");
  }


  //Loop over attitude outputs and check for time discontinuities.  Complain if some are found.
  for(i=1; i<inum-1; i++){
    if(OAtt->mjd[i] >= org_stT && OAtt->mjd[i] <= org_enT) {
      int yyy, doy, hh, mm, ss;
      do_mjd2utc(OAtt->mjd[i], &yyy, &doy, &hh, &mm, &ss);
    

      if(!ini->OptFile.empty() && OutF != NULL){
        fprintf(OutF, " %15.8f   %d/%03d:%02d:%02d:%02d  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f     %d\n", 
                OAtt->mjd[i], yyy, doy, hh, mm, ss,  OAtt->SatRA[i], OAtt->SatDEC[i], OAtt->Xra[i], OAtt->Xdec[i],
                OAtt->Yra[i], OAtt->Ydec[i], OAtt->Zra[i], OAtt->Zdec[i], OAtt->in_saa[i] );
      }

      /*
        printf( " %15.8f   %d/%03d:%02d:%02d:%02d  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f\n", 
        OAtt->mjd[i], yyy, doy, hh, mm, ss,  OAtt->SatRA[i], OAtt->SatDEC[i], OAtt->Xra[i], OAtt->Xdec[i],
        OAtt->Yra[i], OAtt->Ydec[i], OAtt->Zra[i], OAtt->Zdec[i]);

      */


      if(i>1){
        // Complain if there is a discontinuity in subsequent time entries
        if(fabs((OAtt->mjd[i] - OAtt->mjd[i-1]) - ini->Resolution) > 0.00000000001){

          losf.warn(1) << "Something is wrong in makeAttTako:\n   i=" << (i-1) << " mjd=" << OAtt->mjd[i-1]
                     << "\n  i=" << i << " mjd=" << OAtt->mjd[i] 
                     << " ===> " << (fabs(OAtt->mjd[i]-OAtt->mjd[i-1])*1440.0) << " minutes difference\n";

        }
      }
    }
    
  }

  // Allocate an attitude object.  Loop over each entry in the outputted OAtt object and copy them into the new RAtt object to pass back to main().
  Attitude *RAtt = allocateAttitude(oinum);
  if ( RAtt == (Attitude *)NULL) {
    throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n");
  }  

  RAtt->ent = oinum;

  int k = 0;
  for(i=0; i<inum; i++){
    if(OAtt->mjd[i] >= org_stT && OAtt->mjd[i] <= org_enT) {
      int yyy, doy, hh, mm, ss;
      do_mjd2utc(OAtt->mjd[i], &yyy, &doy, &hh, &mm, &ss);
      if(OAtt->mjd[i] == ephem->MJD[i]){
        if(k > oinum){

          losf.warn(1) << "Expected " << oinum << " entries in the table, and obtained " << k << "\n";
          throw std::runtime_error("ERROR: Something is wrong in OrbSim.cxx::makeAttTako. It tried to access array element beyond limits\n\n");
        }

  	  // For testing, all of this needs to be output to a file for compairison.
  	  // Begin Regression Testing portion:
  		  // Output values below:
  		  takoTestFile << "MJD = "<<OAtt->mjd[k]<<" , ";
  		  takoTestFile << "SatRA = "<<OAtt->SatRA[k]<< " , ";
  		  takoTestFile << "SatDEC = "<<OAtt->SatDEC[k]<< " , ";
  		  takoTestFile << "Xra = "<<OAtt->Xra[k]<< " , ";
  		  takoTestFile << "Xdec = "<<OAtt->Xdec[k]<< " , ";
  		  takoTestFile << "Yra = "<<OAtt->Yra[k]<< " , ";
  		  takoTestFile << "Ydec = "<<OAtt->Ydec[k]<< " , ";
  		  takoTestFile << "Zra = "<<OAtt->Zra[k]<< " , ";
  		  takoTestFile << "Zdec = "<<OAtt->Zdec[k]<< " , ";
  		  takoTestFile << "SatRA = "<<OAtt->SatRA[k]<< " , ";
  		  takoTestFile << "RockAngle = "<<OAtt->rockAngle[k]<<"\n";
  	  // End Regression Testing portion.

        RAtt->mjd[k]    = OAtt->mjd[i];
        RAtt->X[k]      = ephem->X[i];
        RAtt->Y[k]      = ephem->Y[i];
        RAtt->Z[k]      = ephem->Z[i];
        RAtt->Lat[k]    = ephem->Lat[i];
        RAtt->Lon[k]    = ephem->Long[i];
        RAtt->Hei[k]    = ephem->Alt[i];
        RAtt->SatRA[k]  = OAtt->SatRA[i];
        RAtt->SatDEC[k] = OAtt->SatDEC[i];
        RAtt->Xra[k]    = OAtt->Xra[i];
        RAtt->Xdec[k]   = OAtt->Xdec[i];
        RAtt->Yra[k]    = OAtt->Yra[i];
        RAtt->Ydec[k]   = OAtt->Ydec[i];
        RAtt->Zra[k]    = OAtt->Zra[i];
        RAtt->Zdec[k]   = OAtt->Zdec[i];
        RAtt->in_saa[k] = OAtt->in_saa[i];
        RAtt->rockAngle[k] = OAtt->rockAngle[i];
        k++;
      }

    }

  }

  takoTestFile.close();

  // Only Reallocate if there's a reason to reallocate
  if (k < oinum)  RAtt = reallocateAttitude( (oinum - (oinum-k) ) , RAtt );
  //  OAtt = deallocateAttitude(OAtt);


  if(!ini->OptFile.empty() && OutF != NULL){
    fclose(OutF);
  }


  return RAtt;

}




// makeAttAsFl does all of the heavy lifting for the AsFlown timeline attitude calculation
Attitude * makeAttAsFl(InitI *ini, EphemData *ephem) {

  std::ofstream asflTestFile;
  asflTestFile.open ("ASFLAttitude.out");

  FILE *ITL;
  FILE *OutF = NULL;
  double Timespan, res;
  int inum, oinum, i;


  double org_stT = ini->start_MJD;
  double org_enT = ini->stop_MJD;

  losf.setMethod("makeAttAsFl");
  losf.err().precision(12);
  losf.info().precision(12);

  Timespan = (ini->stop_MJD-ini->start_MJD);
  res = ini->Resolution;
  inum = (int)((Timespan+(res/2.0))/res); // inum is used to denote the total number of entries in the attitude vectors
  inum++; // Plus one to include the end point
  Attitude *OAtt = allocateAttitude(inum); 

  if ( OAtt == (Attitude *)NULL) {
    throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n\n");
  }

  oinum = inum;

/*
  This Ephem is needed to calculate pointing position during survey mode
  for time previous the given one
*/

  EphemData * Oephem;

  FILE *OephF = fopen(ini->EPHname.c_str(),"r");

  if(match_str( ini->EPHfunc.c_str(), "^yyyy_eph$") == 1){
    Oephem = yyyy_eph(OephF, ini->start_MJD, ini->stop_MJD, \
                     ini->Units, ini->Resolution);
  }else if(match_str( ini->EPHfunc.c_str(), "^xyzll_eph$") == 1){
    Oephem = xyzll_eph(OephF, ini->start_MJD, ini->stop_MJD, \
                      ini->Units, ini->Resolution);
  }else if(match_str( ini->EPHfunc.c_str(), "^tlederive$") == 1){
    Oephem = tlederive(OephF, ini->start_MJD, ini->stop_MJD, \
                      ini->Units, ini->Resolution);
  }


  fclose(OephF);

  char ln[bufsz];

  if ( (ITL=fopen(ini->TLname.c_str(),"r")) == NULL) {
    std::string fname(ini->TLname);
    throw std::runtime_error("\nCound not open Timeline file "+fname);
  } else {  
    double pra = ini->Ira; // Initial spacecraft ra
    double pdec = ini->Idec; // Initial spacecraft dec
    double res = ini->Resolution;  //Resolution already converted to days.
    int mode = -1;    // Used to distinguish between Survey (1) and Pointed (2)
    int type = -1;    // Used to distinguish between simple Survey (1) and Profiled survey (2)
    double offset = -999.0;  // Rocking angle offset
    double ra = -999.0;
    double dec = -999.0;
    double mjds = 0.0;  // Used (in this function) to mark the start of a maneuver (i.e. inertial point/zenith point)
    double mjde = 0.0;  // Used (in this function) to mark the current line being parsed
    double val1, val2; // Used to hold ra and dec values for passage to functions (not sure why these extra variables are needed.)  Todo: Find out why they are needed.  ~JA 20141009
    double lastTime = 0.0;  // Used to store the timestamp of the previous line
    bool zenithOpen = 0;  // Used to indicate whether a zenith point maneuver is sill 'open' or not at the current parsed line
    bool inertialOpen = 0;  // Used to indicate whether an inertial point maneuver is sill 'open' or not at the current parsed line


    // Adding Time range checking of AFS schedule by parsing default Filename
    std::string token = ini->TLname.substr(0,ini->TLname.find("_")); //Find Filename Start. should be AFST
    if (!strcmp(token.c_str(),"AFST")) std::runtime_error("\nERROR: ASFLOWN mode with non-AFST timeline!\n");
    std::string start_time = ini->TLname.substr(token.length()+1,11); //Find the start time string
    std::string stop_time = ini->TLname.substr(token.length() + 1 + start_time.length() + 1 ,11); //Find the end time string
    int start_yr, start_day , start_hr , start_min;
    sscanf(start_time.substr(0,4).c_str(), "%d" , &start_yr);
    sscanf(start_time.substr(4,3).c_str(), "%d" , &start_day);
    sscanf(start_time.substr(7,2).c_str(), "%d" , &start_hr);
    sscanf(start_time.substr(9,2).c_str(), "%d" , &start_min);
    double tl_start = do_utcj2mjd (start_yr, start_day, start_hr, start_min, 0);
    int stop_yr , stop_day , stop_hr , stop_min;
    sscanf(stop_time.substr(0,4).c_str(), "%d" , &stop_yr);
    sscanf(stop_time.substr(4,3).c_str(), "%d" , &stop_day);
    sscanf(stop_time.substr(7,2).c_str(), "%d" , &stop_hr);
    sscanf(stop_time.substr(9,2).c_str(), "%d" , &stop_min);
    double tl_end = do_utcj2mjd (stop_yr, stop_day, stop_hr, stop_min, 0);
    if (ini->stop_MJD < tl_start) throw std::runtime_error("\nERROR: Invalid Time Range. stop_MJD occurs before AFS Timeline begins!");
    if (ini->start_MJD > tl_end) throw std::runtime_error("\nERROR: Invalid Time Range. start_MJD occurs after AFS Timeline ends!");
    if (ini->start_MJD < tl_start) throw std::runtime_error("\nERROR: Invalid Time Range. start_MJD occurs before AFS Timeline begins!");
    if (ini->stop_MJD > tl_end) { //throw std::runtime_error("\nERROR: Invalid Time Range. stop_MJD occurs after AFS Timeline ends!");
        losf.warn(1) << "WARNING: stop_mjd=" << ini->stop_MJD << " exceeds AFS Timeline End=" << tl_end << ". Orbitsim will only run up to mjd=" << tl_end << "\n";
        ini->stop_MJD = tl_end;
    }


    SurProf profile;

    std::string initTime; // Initial time of last command read in from the previous satellite state information.  (In header of ASFLOWN timeline.)
    std::string initDEC; // Initial dec of the satellite read in from the previous satellite state information.
    std::string initRA; // Initial ra of the satellite read in from the previous satellite state information.
    std::string initMode; // Initial mode of the satellite read in from the previous satellite state information.
    std::string lastCommand;  // Used to hold the previous command read in from the profile.  This is needed in several maneuver transition cases.
    

    int flgprof = 0; // Flag used to denote whether or not a rocking profile has been read into the program.
    bool bufovrflg = 0; // Need to provide a flag to indicate when the string buffer overflows, so that the check knows to skip the following line iteration.  Note: This only works for 1 overflow.

    while(fgets(ln, bufsz, ITL)){

      //  If this is not a header line, set mjde to current time stamp
      if(match_str((const char*) ln,"//") != 1) {
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "|");
    	  int yyy, doy, hh, mm, ss;
    	  sscanf(TL, "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
    	  mjde = do_utcj2mjd (yyy, doy, hh, mm, ss);
      	 }

      int flgT = 1;

      // Parse the initial values provided by the header to set the initial satellite state
      if(match_str((const char*) ln, "//DEC") == 1) {
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "=\n");
    	  TL = strtok(NULL, "=");
    	  initDEC = TL;
    	  initDEC.erase(std::remove(initDEC.begin(), initDEC.end(), '\n'), initDEC.end());
      }

      if(match_str((const char*) ln, "//RA") == 1) {
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "=\n");
    	  TL = strtok(NULL, "=");
    	  initRA = TL;
	  initRA.erase(std::remove(initRA.begin(), initRA.end(), '\n'), initRA.end());
      }

      if(match_str((const char*) ln, "//SAC_MODE=") == 1) {
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "=\n");
    	  TL = strtok(NULL, "=");
    	  initMode = TL;
    	  initMode.erase(std::remove(initMode.begin(), initMode.end(), '\n'), initMode.end());
      }

      if(match_str((const char*) ln, "//SS_Param") == 1) {
    	  parseInitParams(ln, &profile);
    	  flgprof = 1;
      }

      // Load the last header input and concatenate it into a line describing the state
      if(match_str((const char*) ln, "//TIME") == 1) {
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "=\n");
    	  TL = strtok(NULL, "=");
    	  initTime = TL;
    	  initTime.erase(std::remove(initTime.begin(), initTime.end(), '\n'), initTime.end());
    	  memset(ln,'0',650);

    	  std::string lineManeuver = " | Maneuver | ";
    	  std::string linePipe = " | ";
    	  std::string lineEndPipe = " |\n";

    	  std::string initSchedule  = initTime.c_str()+lineManeuver+initMode.c_str()+linePipe+initRA.c_str()+linePipe+initDEC.c_str()+lineEndPipe;
    	  strcpy(ln, initSchedule.c_str());
    	  int yyy, doy, hh, mm, ss;
    	  sscanf(initTime.c_str(), "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
    	  mjds = do_utcj2mjd (yyy, doy, hh, mm, ss);
      	 }

      // Check to see if the satellite is goint INTO a maneuver.  This triggers a pointing calculation.
      // Otherwise, check to see if the parsed line is providing a rocking profile.
      if(checkManeuver((const char*) ln) == 1) {
        mjde = parseAsFline(ln, &mode, &val1, &val2);
        printf("%s ===> Starting obs with mjde=%f, RA=%f, DEC=%f\n", ln, mjde, val1, val2);
      } else if((match_str((const char*) ln, "SS_Param") == 1) && (match_str((const char*) ln,"//") != 1)) {
        parseProfile(ln, &profile);
        flgprof = 1;
      } 

      // Check to see if the satellite is going into a zenith or inertial point.
      if((checkManZenith((const char*) ln) == 1) || (checkManInertial((const char*) ln) == 1)){
    	  char jnk[bufsz];
    	  strcpy (jnk, ln);
    	  char *TL = strtok(jnk, "|");
    	  int yyy, doy, hh, mm, ss;
    	  sscanf(TL, "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
    	  mjds = do_utcj2mjd (yyy, doy, hh, mm, ss);
    	  if((checkManZenith((const char*) ln) == 1)) {
    	      zenithOpen = 1;
	  } else if(checkManInertial((const char*) ln) == 1) {
	    char jnk[bufsz];
	    strcpy (jnk, lastCommand.c_str());
	    char *TL = strtok(jnk, "|");
	    int yyy, doy, hh, mm, ss;
	    sscanf(TL, "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);
	    lastTime = do_utcj2mjd (yyy, doy, hh, mm, ss);
	    TL = strtok(NULL, "|");
	    if(match_str((const char*) TL, "SS_Param") == 1) {
	      mjds = lastTime;
	    }
	    inertialOpen = 1;
	  }
      }

      // Check to see if the satellite is going into a zenith point without a survey profile defined.
      if((checkManZenith((const char*) ln) == 1) && (flgprof == 0)){
        losf.warn() << "\n##########################################################\n\n";
        losf.warn() << "\t\tWARNING\n\tNO SURVEY PROFILE DEFINED!\n   Could not calculate attitude between:\n       " << mjds << " and " << mjde << "\n" << "\n##########################################################\n\n";
          flgT = -1;
        }else if((match_str((const char*) ln, "//") != 1) && bufovrflg != 1){

    // Define survey type
	if((profile.epoch <= mjds) && (zenithOpen == 1)){
            type = 2;
          }else {
            type = 1;
            offset = profile.defofst;
          }

        }

      //  Begin reallocations of the attitude structure to ensure it is sized to the time-of-interest window
      //  Heavy lifting calculations are in this if block as well (namely makeatt2 and makeprofiled)
      if((match_str((const char*) ln,"//") != 1) && bufovrflg != 1) {
      if(mjds > 0 && mjds < mjde){

        if(org_stT >= mjds && org_enT > mjde){

          ini->start_MJD = mjds;
          ephem = deallocateEphem(ephem);
          FILE *ephF = fopen(ini->EPHname.c_str(),"r");


          if(match_str( ini->EPHfunc.c_str(), "^yyyy_eph$") == 1){
            ephem = yyyy_eph(ephF, ini->start_MJD, ini->stop_MJD,	\
                              ini->Units, ini->Resolution);
	    }else if(match_str( ini->EPHfunc.c_str(), "^xyzll_eph$") == 1){
            ephem = xyzll_eph(ephF, ini->start_MJD, ini->stop_MJD,	\
                               ini->Units, ini->Resolution);
	    }else if(match_str( ini->EPHfunc.c_str(), "^tlederive$") == 1){
            ephem = tlederive(ephF, ini->start_MJD, ini->stop_MJD,	\
	                       ini->Units, ini->Resolution);
	    }



          fclose(ephF);


          Timespan = (ini->stop_MJD-ini->start_MJD);
          res = ini->Resolution;
          inum = (int)((Timespan+res/2.0)/res);
          inum++;
          OAtt = reallocateAttitude(inum, OAtt);

          if ( OAtt == (Attitude *)NULL) {
            throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n\n");
          }
	} else if (mjds < org_enT && mjde >= org_enT ){

          ini->stop_MJD = mjde;
          Timespan = (ini->stop_MJD-ini->start_MJD);
          res = ini->Resolution;
          inum = (int)((Timespan+res/2.0)/res);
          inum++;

          ephem = deallocateEphem(ephem);
          FILE *ephF = fopen(ini->EPHname.c_str(),"r");


          if(match_str( ini->EPHfunc.c_str(), "^yyyy_eph$") == 1){
            ephem = yyyy_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                              ini->Units, ini->Resolution);
          }else if(match_str( ini->EPHfunc.c_str(), "^xyzll_eph$") == 1){
            ephem = xyzll_eph(ephF, ini->start_MJD, ini->stop_MJD, \
                               ini->Units, ini->Resolution);
          }else if(match_str( ini->EPHfunc.c_str(), "^tlederive$") == 1){
            ephem = tlederive(ephF, ini->start_MJD, ini->stop_MJD, \
                               ini->Units, ini->Resolution);
          }


          fclose(ephF);


          OAtt = reallocateAttitude(inum, OAtt);

          if ( OAtt == (Attitude *)NULL) {
            throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n\n");
          }
        } else if (mjds >= org_enT){
          break;
        } else if (mjde < org_stT){
          flgT = -1;
        }



        if(flgT == 1){
          if((mode == 2) ||
             (mode == 1 && type == 1)){
            double lpos[2];
            if(mode == 2){
              ra = val1;
              dec = val2;
              offset = profile.defofst;
            } else if (mode == 1){
              ra = 0.0;
              dec =0.0;
              offset = profile.defofst;
            }

            MakeAtt2(mjds, mjde, pra, pdec, offset, ra, dec, 
                     mode, ini->Resolution, ephem, lpos, OAtt, ini->start_MJD);

            pra = lpos[0];
            pdec = lpos[1];
	    mode = 0;
	    type = 0;
            inertialOpen = 0;

          } else if((mode == 1) && (type == 2)){

            MakeProfiled(mjds, mjde, ini->Resolution, pra, pdec, profile.epoch, 
                         profile.times, profile.ofsts, ephem, OAtt, ini->start_MJD);

            //    printf("Called MakeProfiled\n");
            int idx = (int)(((mjde-ini->start_MJD)+ini->Resolution/0.5)/ini->Resolution);
            pra = OAtt->Zra[idx];
            pdec = OAtt->Zdec[idx];
	    mode = 0;
	    type = 0;
	    zenithOpen = 0;
          }
        }

        if (mjds >= org_enT){
          losf.warn(1) << "Observation at " << mjde << " is outside the limits\n";
          break;
        }

      }
     } // End of "if((match_str((const char*) ln,"//") != 1) && bufovrflg != 1)" Block
      if(match_str((const char*) ln,"\n") != 1) {
    	  bufovrflg = 1;
      } else if(org_enT <= mjde) {
    	// In the event we have passed the end of the time-of-interest, check to see if a profile
    	// is open, and if it is, finish the profile calculation and break.
        mjde = parseAsFline(ln, &mode, &val1, &val2);
        printf("%s ===> Starting obs with mjde=%f, RA=%f, DEC=%f\n", ln, mjde, val1, val2);
	lastCommand = ln;
	bufovrflg = 0;
	memset(ln,'0',bufsz);
	losf.warn(4) << "Clearing parsed line buffer" << ini->OptFile << "\n";
      } else {
	lastCommand = ln;
    	  bufovrflg = 0;
    	  memset(ln,'0',bufsz);
    	  losf.warn(4) << "Clearing parsed line buffer" << ini->OptFile << "\n";
      }
      if(org_enT <= mjde) {
	if((zenithOpen == 1) || (inertialOpen == 1)) {
	  offset = profile.defofst;
	  if(zenithOpen == 1) {
	    mode = 1;
	    ra = 0;
	    dec = 0;
	  MakeProfiled(mjds, mjde, ini->Resolution, pra, pdec, profile.epoch,
		       profile.times, profile.ofsts, ephem, OAtt, ini->start_MJD);
	  zenithOpen = 0;
	  } else if(inertialOpen == 1) {
	    mode = 2;
	    ra = val1;
	    dec = val2;
	  double lpos[2];
	  MakeAtt2(mjds, mjde, pra, pdec, offset, ra, dec,
	         mode, ini->Resolution, ephem, lpos, OAtt, ini->start_MJD);
	  inertialOpen = 0;
	    }
	}
	break;
      }
    }


  }

  // Calculate saa polygon
  saa( ephem, ini->saafile.c_str(), ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);

  // Calculate target occultations
  if(ini->occflag == 1){
    // Getting the occultation

    occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
             OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);
    doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
  
    int rechk = 0;

    if(rechk){
      occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
               OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);
      doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
    }
  }

  OAtt->ent= inum;

  if(!ini->OptFile.empty() ){
    if ( (OutF=fopen(ini->OptFile.c_str(),"w")) == NULL) {
      losf.warn(1) << "Cound not open OutPut file " << ini->OptFile << "\n";
    }  
    fprintf(OutF, "     MJD          UTC            SAT_RA       SAT_DEC       X_RA       X_DEC       Y_RA       Y_DEC       Z_RA       Z_DEC       IN_SAA\n");
  }

  for(i=1; i<inum-1; i++){
    if(OAtt->mjd[i] >= org_stT && OAtt->mjd[i] <= org_enT) {
      int yyy, doy, hh, mm, ss;
      do_mjd2utc(OAtt->mjd[i], &yyy, &doy, &hh, &mm, &ss);
    

      if(!ini->OptFile.empty() && OutF != NULL){
        fprintf(OutF, " %15.8f   %d/%03d:%02d:%02d:%02d  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f     %d\n", 
                OAtt->mjd[i], yyy, doy, hh, mm, ss,  OAtt->SatRA[i], OAtt->SatDEC[i], OAtt->Xra[i], OAtt->Xdec[i],
                OAtt->Yra[i], OAtt->Ydec[i], OAtt->Zra[i], OAtt->Zdec[i], OAtt->in_saa[i] );
      }


      if(i>1){
        
        if(fabs((OAtt->mjd[i]-OAtt->mjd[i-1])-ini->Resolution) > 0.00000000001){

          losf.warn(1) << "Something is wrong in OrbSim::makeAttAsFl:\n   i=" << (i-1) << " mjd=" << OAtt->mjd[i-1] << "\n  i=" << i << " mjd=" << OAtt->mjd[i] << " ===> " << (fabs(OAtt->mjd[i]-OAtt->mjd[i-1])*1440.0) << " minutes difference\n";

        }
      }
    }
    
  }

  if(!ini->OptFile.empty() && OutF != NULL){
    fclose(OutF);
  }


  oinum += 2;
  Attitude *RAtt = allocateAttitude(oinum);
  if ( RAtt == (Attitude *)NULL) {
    throw std::runtime_error("ERROR: Cannot Allocate attitude data structure\nExiting..............\n\n");
  }  


  RAtt->ent = oinum;
  losf.info(3) << "Expected " << oinum << " entries in the table\n";
  int k = 0;
  for(i=0; i<inum; i++){
    if(((OAtt->mjd[i] >= org_stT) || (res >= fabs(OAtt->mjd[i]-org_stT))) && ((OAtt->mjd[i] <= org_enT) || (res >= fabs(OAtt->mjd[i]-org_enT)))) {
      int yyy, doy, hh, mm, ss;
      do_mjd2utc(OAtt->mjd[i], &yyy, &doy, &hh, &mm, &ss);
      if(OAtt->mjd[i] == ephem->MJD[i]){
        if(k > oinum){
          losf.warn(1) << "Expected " << oinum << " entries in the table, and obtained " << k << "\n";
          throw std::runtime_error("ERROR: Something is wrong in OrbSim::makeAttAsFl. since tried to access array element beyond limits\n\n");
        }
    	  // For testing, all of this needs to be output to a file for compairison.
    	  // Begin Regression Testing portion:
    		  // Output values below:
    		  asflTestFile << "MJD = "<<OAtt->mjd[k]<<" , ";
    		  asflTestFile << "SatRA = "<<OAtt->SatRA[k]<< " , ";
    		  asflTestFile << "SatDEC = "<<OAtt->SatDEC[k]<< " , ";
    		  asflTestFile << "Xra = "<<OAtt->Xra[k]<< " , ";
    		  asflTestFile << "Xdec = "<<OAtt->Xdec[k]<< " , ";
    		  asflTestFile << "Yra = "<<OAtt->Yra[k]<< " , ";
    		  asflTestFile << "Ydec = "<<OAtt->Ydec[k]<< " , ";
    		  asflTestFile << "Zra = "<<OAtt->Zra[k]<< " , ";
    		  asflTestFile << "Zdec = "<<OAtt->Zdec[k]<< " , ";
    		  asflTestFile << "SatRA = "<<OAtt->SatRA[k]<< " , ";
    		  asflTestFile << "RockAngle = "<<OAtt->rockAngle[k]<<"\n";
    	  // End Regression Testing portion.

        RAtt->mjd[k]    = OAtt->mjd[i];
        RAtt->X[k]      = ephem->X[i];
        RAtt->Y[k]      = ephem->Y[i];
        RAtt->Z[k]      = ephem->Z[i];
        RAtt->Lat[k]    = ephem->Lat[i];
        RAtt->Lon[k]    = ephem->Long[i];
        RAtt->Hei[k]    = ephem->Alt[i];
        RAtt->SatRA[k]  = OAtt->SatRA[i];
        RAtt->SatDEC[k] = OAtt->SatDEC[i];
        RAtt->Xra[k]    = OAtt->Xra[i];
        RAtt->Xdec[k]   = OAtt->Xdec[i];
        RAtt->Yra[k]    = OAtt->Yra[i];
        RAtt->Ydec[k]   = OAtt->Ydec[i];
        RAtt->Zra[k]    = OAtt->Zra[i];
        RAtt->Zdec[k]   = OAtt->Zdec[i];
        RAtt->in_saa[k] = OAtt->in_saa[i];
        RAtt->rockAngle[k] = OAtt->rockAngle[i];

        //      printf("%5d), mjd=%f\n", k, RAtt->mjd[k]);
        k++;
      } 

    } 

  }

  asflTestFile.close();

  if(RAtt->mjd[(RAtt->mjd.size()-1)] == 0) {
    if(RAtt->mjd[(RAtt->mjd.size()-2)] == 0) {
      oinum--;
    }
    oinum--;
    RAtt = reallocateAttitude(oinum, RAtt);
  }

  OAtt = deallocateAttitude(OAtt);
  return RAtt;

}

double parseAsFline(char *ln, int *mode, double *val1, double *val2){

  double mjd;

  char jnk[bufsz];
  strcpy (jnk, ln);
  char *TL = strtok(jnk, "|");

  int yyy, doy, hh, mm, ss;

  sscanf(TL, "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);

  mjd = do_utcj2mjd (yyy, doy, hh, mm, ss);
  TL = strtok(NULL, "|");
  
  if((match_str((const char*) TL, "AutoRepoint") == 1) ||
     (match_str((const char*) TL, "InertialPoint") == 1)) {
     *mode = 2;
  } else if (match_str((const char*) TL, "ZenithPoint") == 1) {
     *mode = 1;
  }

  TL = strtok(NULL, "|");
  TL = strtok(NULL, "|");
  *val1 = atof(TL);

  TL = strtok(NULL, "|");
  *val2 = atof(TL);


  return mjd;

}






Attitude * doCmd(InitI *ini, EphemData *ephem) {

  FILE *OutF = NULL;

  double Timespan = (ini->stop_MJD-ini->start_MJD);
  double res = ini->Resolution;
  int inum = (int)((Timespan+res/2.0)/res);
  inum++;


  losf.setMethod("doCmd");
  losf.err().precision(12);
  losf.info().precision(12);


  losf.info(3) << "About to allocate attitude to " << inum << " elements\n";


  Attitude *OAtt = allocateAttitude(inum);

  if ( OAtt == (Attitude *)NULL) {
    std::ostringstream oBuf;
    oBuf << __FILE__ <<":" << __LINE__ << " ERROR: Cannot Allocate attitude data structure\nExiting..............\n\n" <<std::ends;

    throw std::runtime_error(oBuf.str());
  }


// Identify which command mode (SURVEY, PROFILE, or OBS) is being issued and act appropriately.
  if(match_str((const char*)  ini->TLname.c_str(), "SURVEY") == 1){
    std::string jnk = ini->TLname;
    char *TL = strtok((char *)jnk.c_str(), "|");
    TL = strtok(NULL, "|");
    double offset;

    //Set Epoch as an arbitrary value, since in this case the profile propagation does not matter.
    double epoch = 9999;

    if(chkStr(TL) == 0){
      std::ostringstream oBuf;
      oBuf << __FILE__ <<":" << __LINE__ << " in doCmd: while I should be doing a SURVEY mode\nobservation, the offset indicated (" << TL << ") should be a number only!\nExiting now............\n\n" << std::ends;
      

      throw std::runtime_error(oBuf.str());
    }

    // Read in rocking offset (Single rocking angle)
    sscanf(TL, "%lf", &offset);

    // Advance Token stream and read in orbit duration.  If no duration is provided, assume an orbit of 5722 seconds.  (5722 was provided by Elizabeth Ferrera as a standard orbit duration)

    TL = strtok(NULL, "|");
    double duration;

    if (TL == NULL) {
    	duration = 5722;
    } else {
    	duration = std::atof(TL);
    }

    if(offset > 90.0 || offset < -90.0){

      std::ostringstream oBuf;
      
      oBuf << __FILE__ <<":" << __LINE__ << " in doCmd: while I should be doing a SURVEY mode\nobservation, the offset indicated ( " << offset << ") is outside the limits\nEXITING NOW....................\n\n" <<std::ends;

      throw std::runtime_error(oBuf.str());
      
    } else {
      losf.info(3) << "OrbSim should be doing a single cmd about survey with offset=" << offset<< "\n";

      // Create Arrays to hold tms and offset

      const int sz = 17;

      int i;

      double ofst[sz];
      double tms[sz];

      int slice = duration/sz;

      for (i=0;i<17;i++){
    	  if(i<16){
    	  tms[i] = i*slice;
    	  }else if(i==16){
    	  tms[i] = duration;
    	  }
    	  ofst[i] = offset;
      }

      MakeProfiled(ini->start_MJD, ini->stop_MJD, ini->Resolution,
                   ini->Ira, ini->Idec, epoch, tms, ofst, ephem, OAtt, ini->start_MJD);

      //doSurvey(ini->start_MJD, ini->stop_MJD, ini->Resolution,
       //        ini->Ira, ini->Idec, offset, ephem, OAtt);
    }
  }else if(match_str( ini->TLname.c_str(), "PROFILED") == 1){
    std::string jnk = ini->TLname;
    char *TL = strtok((char *)jnk.c_str(), "|");
    TL = strtok(NULL, "|");
    double epoch;
    
    const int sz = 17;

    int i;

    double ofst[sz];
    double tms[sz];

    if(chkStr(TL) == 0){

      std::ostringstream oBuf;
      
      oBuf << __FILE__ <<":" << __LINE__ << ",  in doCmd: while I should be doing a PROFILED SURVEY mode\nobservation, the epoch indicated (" << TL << ") should be a number only!\nExiting now............\n\n" << std::ends;

      throw std::runtime_error(oBuf.str());

    }
    sscanf(TL, "%lf", &epoch);

    if(epoch > ini->start_MJD ){
      
      std::ostringstream oBuf;
      oBuf << "\n" << __FILE__ << ":" << __LINE__ << "\nERROR: doCmd, the profile epoch is not within the windows of interest\nExiting now........................\n\n" << std::ends;
      throw std::runtime_error(oBuf.str());
      
    }

    TL = strtok(NULL, "|");
    while (*TL == ' ')
      ++TL;

    int iret = sscanf(TL, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &tms[0], &ofst[0], &tms[1], &ofst[1], &tms[2], &ofst[2], &tms[3], &ofst[3], &tms[4], &ofst[4], &tms[5], &ofst[5], &tms[6], &ofst[6], &tms[7], &ofst[7], &tms[8], &ofst[8], &tms[9], &ofst[9], &tms[10], &ofst[10], &tms[11], &ofst[11], &tms[12], &ofst[12], &tms[13], &ofst[13], &tms[14], &ofst[14], &tms[15], &ofst[15], &tms[16], &ofst[16]);

    if(iret != sz*2){

      std::ostringstream oBufT;
      
      oBufT << __FILE__ <<":" << __LINE__ << ", ERROR: doCmd, the profile for the survey MUST contain " << (sz*2) << " elements, but is has " << iret <<". Exiting now........................\n\n" << std::ends;
      

      throw std::runtime_error(oBufT.str());

    }

    for(i=0; i<sz-1; i++){
      if(tms[i+1] <= tms[i]){
        std::ostringstream oBufT;
      
        oBufT << __FILE__ <<":" << __LINE__ << ", doCmd, survey profile times must be increasing\n" << i << " ==> " << tms[i] << "\n" << (i+1) << " ==> " << tms[i+1] << "\nExiting now........................\n\n" << std::ends;

        throw std::runtime_error(oBufT.str());
      }
    }


    for(i=0; i<sz; i++){
      if(ofst[i] <= -90.0 || ofst[i] >= 90.0){
        std::ostringstream oBufT;
      
        oBufT << __FILE__ <<":" << __LINE__ << ", doCmd, survey profile offsets must be between -90.0 and 90.0\nExiting now........................\n\n" << std::ends;

        throw std::runtime_error(oBufT.str());
      }
    }


    MakeProfiled(ini->start_MJD, ini->stop_MJD, ini->Resolution, 
                 ini->Ira, ini->Idec, epoch, tms, ofst, ephem, OAtt, ini->start_MJD);

  } else if (match_str( ini->TLname.c_str(), "POINTED") == 1){

    std::string jnk = ini->TLname;
    char *TL = strtok((char *)jnk.c_str(), "|");
    TL = strtok(NULL, "|");
    double ra, dec, offset;

    offset = -9999.0;
    int flgE = 0;
    int mode = 2;
    double lpos[2];


    std::ostringstream eBufT;
    if(chkStr(TL) == 0){
      eBufT << "\n\nin doCmd: while I should be doing a POINTED mode\nobservation, the ra indicated (" << TL << ") should be a number only!\n\n";
      flgE++;
    }
    sscanf(TL, "%lf", &ra);
    if(ra < 0.0 || ra > 360.0){
      eBufT << "in doCmd: while I should be doing a POINTED mode\nobservation, the ra indicated (" << ra << ") is outside the limits (0; 360)\n\n";
      flgE++;
    }

    TL = strtok(NULL, "|");
    if(chkStr(TL) == 0){
      eBufT << "in doCmd: while I should be doing a POINTED mode\nobservation, the dec indicated (" << TL << ") should be a number only!\n\n";
      flgE++;
    }
    sscanf(TL, "%lf", &dec);
    if(dec < -90.0 || dec > 90.0){
      eBufT << "in doCmd: while I should be doing a POINTED mode\nobservation, the dec indicated (" << dec << ") is outside the limits (-90; 90)\n\n";
      flgE++;
    }

    if(flgE > 0){
      eBufT << "\nExiting now......................\n\n" << std::ends;

      throw std::runtime_error(eBufT.str());
    }



    MakeAtt2(ini->start_MJD, ini->stop_MJD, ini->Ira, ini->Idec, offset, ra, dec, 
             mode, ini->Resolution, ephem, lpos, OAtt, ini->start_MJD);


  }else {
    
    std::ostringstream eBuf;
    eBuf << "\n"<<__FILE__ << ":" << __LINE__ << " ERROR: doCmd SINGLE command " << ini->TLname << " is unknown!\nExiting now........................\n\n" << std::ends;

    throw std::runtime_error(eBuf.str());

  }

  saa( ephem, ini->saafile.c_str(), ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);

  if(ini->occflag == 1){
    // Getting the occultation

    occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
             OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);
    doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
  
    int rechk = 0;

    if(rechk){
      occult ( ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution,
               OAtt, ini->EAA, ini->ELT_OFF_START, ini->ELT_OFF_STOP);
      doLimbTrace(ephem, ini->start_MJD, ini->stop_MJD, ini->Resolution, OAtt);
    }
  }

  if(!ini->OptFile.empty() ){
    if ( (OutF=fopen(ini->OptFile.c_str(),"w")) == NULL) {
      losf.warn(1) << "Cound not open OutPut file " << ini->OptFile << "\n";
    }  
    fprintf(OutF, "     MJD          UTC            SAT_RA       SAT_DEC       X_RA       X_DEC       Y_RA       Y_DEC       Z_RA       Z_DEC       IN_SAA\n");
  }


  OAtt->ent= inum;
  int i;
  for(i=1; i<inum; i++){
    int yyy, doy, hh, mm, ss;
    do_mjd2utc(OAtt->mjd[i], &yyy, &doy, &hh, &mm, &ss);
    

    if(!ini->OptFile.empty() && OutF != NULL){
      fprintf(OutF, " %15.8f   %d/%03d:%02d:%02d:%02d  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f  %10.6f     %d\n", 
              OAtt->mjd[i], yyy, doy, hh, mm, ss,  OAtt->SatRA[i], OAtt->SatDEC[i], OAtt->Xra[i], OAtt->Xdec[i],
              OAtt->Yra[i], OAtt->Ydec[i], OAtt->Zra[i], OAtt->Zdec[i], OAtt->in_saa[i] );
    }

    if(i>1){
      if(fabs((OAtt->mjd[i]-OAtt->mjd[i-1])-ini->Resolution) > 0.000000001){ // This previously caused rounding error problems.  Changed 0.0000000001 to 0.000000001 to fix it. ~JA

          losf.warn(1) << "Something is wrong in OrbSim::doCmd:\n  i=" << (i-1) << " mjd=" << OAtt->mjd[i-1] << "\n  i=" << i << " mjd=" << OAtt->mjd[i] << "\n ===> " << (fabs(OAtt->mjd[i]-OAtt->mjd[i-1])*1440.0) << " minutes difference\n";

      }
    }

  }


  if(!ini->OptFile.empty() && OutF != NULL){
    fclose(OutF);
  }

  int k = 0;
  for(i=0; i<inum; i++){

    if(OAtt->mjd[i] == ephem->MJD[i]){
      OAtt->X[i]      = ephem->X[i];
      OAtt->Y[i]      = ephem->Y[i];
      OAtt->Z[i]      = ephem->Z[i];
      OAtt->Lat[i]    = ephem->Lat[i];
      OAtt->Lon[i]    = ephem->Long[i];
      OAtt->Hei[i]    = ephem->Alt[i];
      k++;
    } else {
      losf.out() << __FILE__ <<":" << __LINE__ << ", " << i << ") OAtt->mjd[" << i << "]=" << OAtt->mjd[i] << ", ephem->MJD[" << i << "]=" << ephem->MJD[i] << "\n";

    }


    if(k > inum){
      std::ostringstream eBuf;
      eBuf << "\n"<<__FILE__ << ":" << __LINE__ << " ERROR: Something is wrong in OrbSim::doCmd since tried to access array element beyond limits of the attitude structure\n\n" << std::ends;

      throw std::runtime_error(eBuf.str());
    }

  }

  if(k != inum){
    std::ostringstream eBuf;
    eBuf << "\n"<<__FILE__ << ":" << __LINE__ << " ERROR: Expected " << inum << " elements, but found " << k << " in the attitude structure\n\n" << std::ends;

    throw std::runtime_error(eBuf.str());
  }

  return OAtt;
}


void parseProfile(char *ln, SurProf *profile){

  double mjd;

  char jnk[bufsz];
  strcpy (jnk, ln);
  char *TL = strtok(jnk, "|");

  int yyy, doy, hh, mm, ss;

  sscanf(TL, "%d-%d-%d:%d:%d", &yyy, &doy, &hh, &mm, &ss);

  mjd = do_utcj2mjd (yyy, doy, hh, mm, ss);
  TL = strtok(NULL, "|");
  TL = strtok(NULL, "|");
  const int len = strlen(TL);
  char *jnk2 = NULL;
  jnk2 = (char *) malloc((len) * sizeof(char));

  strcpy(jnk2, TL);
  char *Pf = strtok(jnk2, ",");

  int i = 0;
  int ia = 0;
  int it = 0;
  for(i=0; i<36; i++){
    double val;
    sscanf(Pf, "%lf", &val);
    if(i<17){
      profile->ofsts[ia] = val;
      ia++;
    } else if(i>=17 && i<34){
      profile->times[it] = val;
      it++;
    } else if (i == 34){
      profile->defofst = val;
    } else if (i == 35){
      profile->epoch = do_met2mjd(val);
    }

    Pf = strtok(NULL, ",");
  }

  free (jnk2);

  return;
}

// Get the RA and declination of the north orbit pole
//   This is needed for the FT2 file

void getNPole(AtVect P1, AtVect P2, double *polCoor){

  AtVect vSat, vNSat, NP1, NP2;
  AtPolarVect gSatP;

  double nra, ndec;

  atNormVect(P1, NP1);
  atNormVect(P2, NP2);
  

  atVectProd(NP1, NP2, vSat);
  atNormVect(vSat, vNSat);
  atVectToPol(vNSat,&gSatP);
  

  ndec = gSatP.lat*RAD2DEG;
  nra  = gSatP.lon*RAD2DEG-0.2;
  if(nra  < 0.0){
    nra = nra + 360.0;
  }

  polCoor[0] = nra;
  polCoor[1] = ndec;
  return;

}

void parseInitParams(char *ln, SurProf *profile){

  char jnk[bufsz];
  strcpy (jnk, ln);
  char *TL = strtok(jnk, "//,=");

  int i = 0;
  int ia = 0;
  int it = 0;
  for(i=0; i<36; i++){
    double val;
    if((match_str((const char*) TL,  "SS_PARAM") == 1 ) || (match_str((const char*) TL, "ANG") == 1) || (match_str((const char*) TL, "TIM") == 1) || (match_str((const char*) TL, "RockDefault") == 1) || (match_str((const char*) TL, "RockStart") == 1)){
    	TL = strtok(NULL,",=");
	if((match_str((const char*) TL, "SS_PARAM") == 1 ) || (match_str((const char*) TL, "ANG") == 1) || (match_str((const char*) TL, "TIM") == 1) || (match_str((const char*) TL, "RockDefault") == 1) || (match_str((const char*) TL, "RockStart") == 1)){
	  TL = strtok(NULL,",=");
	}
    }
    sscanf(TL, "%lf", &val);
    if(i<17){
      profile->ofsts[ia] = val;
      ia++;
    } else if(i>=17 && i<34){
      profile->times[it] = val;
      it++;
    } else if (i == 34){
      profile->defofst = val;
    } else if (i == 35){
      profile->epoch = do_met2mjd(val);
    }

    TL = strtok(NULL, "=,");
  }
  return;
}


