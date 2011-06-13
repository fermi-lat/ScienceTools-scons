//Author: Vlasios Vasileiou <vlasisva@gmail.com>
//$Header$
#include "BackgroundEstimator/BKGE_Tools.h"
#include "TGraphAsymmErrors.h"

int TOOLS::Make_Burst_Plots(string DataClass, string FT1_FILE, string GRB_DIR, float FT1ZenithTheta_Cut, double RA_BURST, double DEC_BURST, double GRB_t0, double Burst_Dur, TH1F* hROI_Max, short int verbosity, TH1F* hROI_Min, TH1F * hCtsvsEnergy_copy) {
  const double ENERGY_MIN=hROI_Max->GetXaxis()->GetXmin();
  const double ENERGY_MAX=hROI_Max->GetXaxis()->GetXmax();

  char name[1000];
  double Burst_t1 = GRB_t0 + Burst_Dur;
  int MinCTBClassLevel  =GetCTBClassLevel(DataClass);
  int ConversionType    =GetConversionType(DataClass);
  string ConversionName = GetConversionName(DataClass);

  vector <string> FT1FILES;
  if (FT1_FILE[0] == '@') {
      FILE* ftemp = fopen((FT1_FILE.substr(1,FT1_FILE.length())).c_str(),"r");
      if (!ftemp) {printf("%s: Can't open file %s\n",__FUNCTION__,(FT1_FILE.substr(1,FT1_FILE.length())).c_str()); exit(1);}
      while (fscanf(ftemp,"%s",name)==1) FT1FILES.push_back(string(name));
      fclose (ftemp);
  }
  else FT1FILES.push_back(FT1_FILE);

  gROOT->cd("/");
  int iebin;
  const int Energy_Bins_NEW = hROI_Max->GetNbinsX();
  const double Energy_Min_NEW = hROI_Max->GetXaxis()->GetXmin();
  const double Energy_Max_NEW = hROI_Max->GetXaxis()->GetXmax();

  int TrueEvents[Energy_Bins_NEW+2];
  memset(TrueEvents,0,sizeof(int)*(Energy_Bins_NEW+2));
  astro::SkyDir SCBurst = astro::SkyDir(RA_BURST, DEC_BURST ,astro::SkyDir::EQUATORIAL);
  astro::SkyDir SCEvent;

  float radius_max[Energy_Bins_NEW+2],radius_min[Energy_Bins_NEW+2];
  for (int i=1;i<=Energy_Bins_NEW;i++) {
       radius_max[i] = hROI_Max->GetBinContent(i)* DEG_TO_RAD;
       if (hROI_Min) radius_min[i] = hROI_Min->GetBinContent(i)* DEG_TO_RAD;
       else radius_min[i]=0;
       //printf("%d %f-%f\n",i,radius_min[i],radius_max[i]);
  }

  TH1F * hCtsvsEnergy = (TH1F*)gROOT->Get("hCtsvsEnergy_Burst");
  if (hCtsvsEnergy) delete hCtsvsEnergy;
  hCtsvsEnergy = new TH1F("hCtsvsEnergy_Burst","hCtsvsEnergy_Burst",Energy_Bins_NEW,Energy_Min_NEW,Energy_Max_NEW);
  hCtsvsEnergy->GetXaxis()->SetTitle("log_{10}(Energy/MeV)");
  hCtsvsEnergy->GetYaxis()->SetTitle("Counts/bin");

  fitsfile *fptr; 


  for (unsigned int ifile=0;ifile<FT1FILES.size();ifile++) {
      int status = 0,anynul,hdutype;
      fits_open_file(&fptr, FT1FILES[ifile].c_str(), READONLY, &status);
      if (status)  {printf("%s: Can't open file %s\n",__FUNCTION__,FT1FILES[ifile].c_str()); exit(1);}
      fits_movabs_hdu(fptr, 2, &hdutype, &status);
      long nrows;int ncols;
      fits_get_num_rows(fptr, &nrows, &status);
      fits_get_num_cols(fptr, &ncols, &status);
      int format;
      if (ncols==22) format=1;
      else if (ncols==18) format=0;
      else {printf("%s: Unknown fits file format file %s ncols=%d\n",__FUNCTION__,name,ncols); exit(1);}

      for (long i=1;i<=nrows;i++) {
         double PtTime;
         fits_read_col (fptr,TDOUBLE,10,i, 1, 1, NULL,&PtTime, &anynul, &status);
         if (PtTime<GRB_t0) continue;
         if (PtTime>Burst_t1) break;

         //CHECK CUTS
         int CTBClassLevel;
         if      (format==0) fits_read_col (fptr,TINT,18,i, 1, 1, NULL,&CTBClassLevel, &anynul, &status);
         else if (format==1) fits_read_col (fptr,TINT,15,i, 1, 1, NULL,&CTBClassLevel, &anynul, &status);
         if (CTBClassLevel<MinCTBClassLevel) continue;
         if (ConversionType!=-1) { //if !BOTH
             int aConversionType=0;
             fits_read_col (fptr,TINT,16,i, 1, 1, NULL,&aConversionType, &anynul, &status);
             if (aConversionType!=ConversionType) continue; //apply Conversion Type front/back
         }
  
         double FT1Energy;
         fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&FT1Energy, &anynul, &status);
         FT1Energy=log10(FT1Energy);
         if (FT1Energy<ENERGY_MIN || FT1Energy>ENERGY_MAX) continue; //energy cut
  
         double FT1ZenithTheta;
         fits_read_col (fptr,TDOUBLE,8,i, 1, 1, NULL,&FT1ZenithTheta, &anynul, &status);

         if (FT1ZenithTheta>FT1ZenithTheta_Cut) continue; //ZTheta cut
         /////////////////////////////////////////////////////////////////////////

         double FT1RA,FT1DEC;
         fits_read_col (fptr,TDOUBLE,2,i, 1, 1, NULL,&FT1RA, &anynul, &status);
         fits_read_col (fptr,TDOUBLE,3,i, 1, 1, NULL,&FT1DEC, &anynul, &status);

         SCEvent = astro::SkyDir(FT1RA,FT1DEC,astro::SkyDir::EQUATORIAL);
         double EvAngDistance = SCBurst.difference(SCEvent);
         if (EvAngDistance>TMath::Pi()) EvAngDistance=TMath::Pi()-EvAngDistance;

         iebin=hROI_Max->FindBin(FT1Energy);

         //printf("%f %d %f\n",FT1Energy,iebin,EvAngDistance);
         //printf("%f \n",radius_max[iebin]);
         //printf("time=%.1lf E=%.2f coo=%4.1f/%4.1f \tdistance=%f \tradius=%f\n",PtTime,FT1Energy,FT1L,FT1B,EvAngDistance/DEG_TO_RAD,radius_max[iebin]/DEG_TO_RAD);
         //printf("%f %f %f\n",EvAngDistance,radius_max[iebin],radius_min[iebin]);
         if (EvAngDistance>radius_max[iebin] || EvAngDistance<radius_min[iebin]) continue; 

         TrueEvents[iebin]++;
         //printf("%f (RA/DEC)=(%3.1f,%3.1f) %f %f ztheta=%f dist=%f rad=%f\n",PtTime,FT1RA,FT1DEC,RA_BURST,DEC_BURST,FT1ZenithTheta,EvAngDistance/DEG_TO_RAD,radius_max[iebin]/DEG_TO_RAD);
     }
     fits_close_file(fptr, &status);
  } 

  for (int ie=0;ie<=Energy_Bins_NEW+1;ie++) {
      hCtsvsEnergy->SetBinContent(ie,TrueEvents[ie]);
      if (hCtsvsEnergy_copy) hCtsvsEnergy_copy->SetBinContent(ie,TrueEvents[ie]);
  }

  if (GRB_DIR!="") { //Save stuff 
     float SigError[2][Energy_Bins_NEW];
     memset(SigError,0,sizeof(SigError));
     float EnergyData[3][Energy_Bins_NEW];
     float TrueEvents_middlebins[Energy_Bins_NEW];
     for (int ie=0;ie<Energy_Bins_NEW+2;ie++) {
        if (ie>0 && ie<=Energy_Bins_NEW) { //no under/overflows
              TrueEvents_middlebins[ie-1]=TrueEvents[ie];
              EnergyData[0][ie-1]=hCtsvsEnergy->GetXaxis()->GetBinCenter(ie)-hCtsvsEnergy->GetXaxis()->GetBinLowEdge(ie);
              EnergyData[1][ie-1]=hCtsvsEnergy->GetXaxis()->GetBinCenter(ie);
              EnergyData[2][ie-1]=hCtsvsEnergy->GetXaxis()->GetBinUpEdge(ie)-hCtsvsEnergy->GetXaxis()->GetBinCenter(ie);
              SigError[1][ie-1]=PoissonErrorBar(1,TrueEvents[ie]);
              SigError[0][ie-1]=PoissonErrorBar(0,TrueEvents[ie]);
          }
     }

     TGraphAsymmErrors * gSignal = new TGraphAsymmErrors(Energy_Bins_NEW,EnergyData[1],TrueEvents_middlebins,
                      EnergyData[0],EnergyData[2],SigError[0],SigError[1]);
     gSignal->SetTitle("gSignal");

     char ResultsFilename[1000];
     sprintf(ResultsFilename,"%s/%s_sig_%.0f_%.0f.root",GRB_DIR.c_str(),DataClass.c_str(),pow(10,Energy_Min_NEW),pow(10,Energy_Max_NEW));
     //string ResultsFilename = GRB_DIR + "/sig_"+ConversionName+".root";
     TFile * fResults = new TFile(ResultsFilename,"RECREATE");

     gSignal->Write("gSignal");
     hROI_Max->Write("hROI");
     if (hROI_Min) hROI_Min->Write("hROI_Min");
     hCtsvsEnergy->Write();

     sprintf(name,"RA/DEC %.3f %.3f",RA_BURST,DEC_BURST);
     TNamed Data = TNamed("Localization_Data",name);
     Data.Write();

     sprintf(name,"%.1f",FT1ZenithTheta_Cut);
     Data = TNamed("FT1ZenithTheta_Cut",name);
     Data.Write();

     fResults->Close();
  }
  int SigEvents=0;
  for (int i=1;i<=Energy_Bins_NEW;i++) SigEvents+=TrueEvents[i];
  return SigEvents;
}

