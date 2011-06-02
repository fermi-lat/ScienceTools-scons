//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/GANGSTER.h"
#include <vector>
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "TH1F.h"

int GANGSTER::MakeCRBackground_LC(string FT1_FILE, string FT2_FILE, string DATACLASS, string TIMEDATA_FILE, bool OverwritePlots, int verbosity, float gtlike_ROI){
/*
 bool WasBatch=gROOT->IsBatch();
 if (!WasBatch) gROOT->SetBatch(kTRUE);

 static bool first=false;

 const double RA=TOOLS::Get("GRB_RA");
 const double DEC=TOOLS::Get("GRB_DEC");
 const float NewFT1ZenithTheta_Cut = TOOLS::Get("FT1ZENITH_THETA_CUT");
 const string GRB_NAME=TOOLS::GetS("GRB_NAME");

 char name[1000];
 double TIME_BEFORE,TIME_AFTER,DURATION,MET;
 char astring[2000],FT1[2000];
 BackgroundEstimator * Est = new BackgroundEstimator(DATACLASS,true,first);
 ///////////////////////////////////////////////////

 FILE * list = fopen(TIMEDATA_FILE.c_str(),"r");
 if (!list) {printf("%s:Can't open file %s\n",__FUNCTION__,TIMEDATA_FILE.c_str());exit(1);}

 if (NewFT1ZenithTheta_Cut>0 || fabs(Est->FT1ZenithTheta_Cut-NewFT1ZenithTheta_Cut)>1) {
   if (first) printf("WARNING: Your are overriding the default FT1ZenithTheta_Cut (%.1fdeg). New value is %.0fdeg\n ",Est->FT1ZenithTheta_Cut,NewFT1ZenithTheta_Cut);
   Est->FT1ZenithTheta_Cut=NewFT1ZenithTheta_Cut;
 }

 FILE * ftemp;
 char GRB_DIR[1000],Interval_name[1000];

 TH1F hROI = TH1F("hROI","hROI",Est->Energy_Bins,log10(Est->Energy_Min),log10(Est->Energy_Max));
 for (int i=1;i<=hROI.GetNbinsX();i++) hROI.SetBinContent(i,gtlike_ROI);

 while (fgets(astring,sizeof(astring),list)) { //main loop
    if (astring[0]=='#') continue;
    if (sscanf(astring,"%s %lf %lf %lf %s",Interval_name,&MET,&TIME_BEFORE,&TIME_AFTER,FT1)!=5) break;

    sprintf(GRB_DIR,"%s/Bkg_Estimates/%s",(TOOLS::GetS("OUTPUT_DIR")).c_str(),Interval_name);
    DURATION=TIME_BEFORE+TIME_AFTER;
    MET-=TIME_BEFORE;

    //check if the estimation finished correctly first
    sprintf(name,"%s/%s_BackgroundMaps.root",GRB_DIR,Est->DataClass.c_str());
    TFile * fBkgMaps = TFile::Open(name);
    if (!fBkgMaps) {printf("%s: Problem with the bkg file.. skipping\n",__FUNCTION__); fBkgMaps->Close(); continue;}      
    TNamed * err = (TNamed*)fBkgMaps->Get("ERROR");
    if (err) {printf("%s: Problem with the bkg file. Error code was '%s'. Skipping\n",__FUNCTION__,err->GetTitle()); fBkgMaps->Close(); continue;}
    fBkgMaps->Close();

    //////////////////////////////////////////////////////
    //Background calculation
    bool BkgOK=true;
    sprintf(name,"%s/%s_bkg_%.0f_%.0f.root",GRB_DIR, Est->DataClass.c_str(),Est->Energy_Min,Est->Energy_Max);
    ftemp = fopen(name,"r");
    bool ProcessFile=false;
 
    if (!ftemp || OverwritePlots) ProcessFile=true;
    else { 
       TFile * fcheck = TFile::Open(name);      
       //check ztheta cut
       if (!ProcessFile && BkgOK==true) {
          float ztheta_cut_file = atof(((TNamed*)fcheck->Get("FT1ZenithTheta_Cut"))->GetTitle());
          if (fabs(ztheta_cut_file-Est->FT1ZenithTheta_Cut)>0.1) {
            printf("%s: Different ztheta cuts detected %.0f vs %.0f, will recalculate (bkg)..\n",__FUNCTION__,ztheta_cut_file,Est->FT1ZenithTheta_Cut);
	    ProcessFile=true;
          }
       }
       fcheck->Close();
    }
    if (ftemp) fclose(ftemp);

     if (Est->FillBackgroundHist(GRB_DIR, &hROI,RA,DEC,2,1,verbosity)) 
	    printf("%s: Problem with the bkg file.. skipping\n",__FUNCTION__);
 }
 //Cleanup
 if (Est) delete Est;
 if (!WasBatch) gROOT->SetBatch(kFALSE);
 fclose(list);
 first=false;
 return 0;
 */
}

