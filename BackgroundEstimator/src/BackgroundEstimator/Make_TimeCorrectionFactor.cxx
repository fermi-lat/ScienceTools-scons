//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"
#include <climits>

ClassImp(BackgroundEstimator)

void BackgroundEstimator::Make_TimeCorrectionFactor(string FitsAllSkyFile, const float DeltaT){

  sprintf(name,"%s/Plots_%s.root",DataDir.c_str(),DataClass.c_str());
  TFile * fPlots = TFile::Open(name);
  TH1F* hPtRazvsTime     = (TH1F*)fPlots->Get("hPtRazvsTime;1");
  TH1F* hPtDeczvsTime    = (TH1F*)fPlots->Get("hPtDeczvsTime;1");
  TH1F* hMcIlwainLvsTime = (TH1F*)fPlots->Get("hMcIlwainLvsTime;1");
  TH1F* hRockingAnglevsTime=(TH1F*)fPlots->Get("hRockingAnglevsTime;1");
  if (!hPtRazvsTime || !hPtDeczvsTime || !hMcIlwainLvsTime) {printf("%s: Can't read plots from file %s\n",__FUNCTION__,name); exit(1);}

  TH1F * hEvents_true[Energy_Bins_user+1],*hEvents_est[Energy_Bins_user+1],*hRatio_True_over_Est[Energy_Bins_user+1];
  const double StartTime=hMcIlwainLvsTime->GetXaxis()->GetXmin();
  const double EndTime=hMcIlwainLvsTime->GetXaxis()->GetXmax();
  const int TimeBins=int((EndTime-StartTime)/DeltaT);
 
  
  sprintf(name,"%s/RateFit_%s_%.1f.root",DataDir.c_str(),DataClass.c_str(),RateFit_version);
  TFile * fRate_Fits = TFile::Open(name);
  TF1 * RateFit[Energy_Bins_user+1];
  TH1F * hScaleFactor =(TH1F*)fRate_Fits->Get("hScaleFactor");
  TH1F* hPtB_Ave = new TH1F("hPtB_Ave","hPtB_Ave",TimeBins,StartTime,EndTime);
  TH1F* hEvents_all_true = new TH1F("hEvents_all_true","hEvents_all_true",TimeBins,StartTime,EndTime);
  TH1F* hEvents_all_est = new TH1F("hEvents_all_est","hEvents_all_est",TimeBins,StartTime,EndTime);

  TH2F * hMap_true = new TH2F("hMap_true","hMap_true",60,-180,180,60,-90,90);
  hMap_true->SetContour(256);
  TH2F * hMap_est = (TH2F*)hMap_true->Clone("hMap_est");
  TH2F * hMap_ratio_true_over_est = (TH2F*)hMap_true->Clone("hMap_ratio_true_over_est");
    
  for (int ie=1;ie<=Energy_Bins_user;ie++) {
     sprintf(name,"hEvents_true_%d",ie);
     hEvents_true[ie] = new TH1F(name,name,TimeBins,StartTime,EndTime);

     sprintf(name,"hEvents_est_%d",ie);
     hEvents_est[ie] = new TH1F(name,name,TimeBins,StartTime,EndTime);

     sprintf(name,"hEvents_ratio_true_over_est_%d",ie);
     hRatio_True_over_Est[ie] = new TH1F(name,name,TimeBins,StartTime,EndTime);
     
     sprintf(name,"RateFit_%d",ie);
     RateFit[ie]=(TF1*)fRate_Fits->Get(name);
     if (!RateFit[ie]) {printf("%s: Can't find fratefit!\n",__FUNCTION__); exit(1);}
  }
  
 
  const float B_Cut=70;
  const float TimeStep=30;
  const int iTimeStep=int(TimeStep/hMcIlwainLvsTime->GetBinWidth(1));
  const int i_1_max=hMcIlwainLvsTime->GetNbinsX();
  double TIME_0  = StartTime;
  int i_0=1;

  sprintf(name,"%s/aTimeCorrectionFactors_%s_%.1f.root",DataDir.c_str(),DataClass.c_str(),TimeCorrectionFactors_version);
  TFile * fResults = new TFile(name,"RECREATE");


  bool done=false;  
  while (!done) {
      //Check if TIME_0 is in GTI
      
      while (1) {
           if (i_0==i_1_max) {done=true; break;}
      
           if (hMcIlwainLvsTime->GetBinContent(i_0)==0) { //advance i_0 if out of GTI
              i_0++;
              //printf("advanced i_0 to %d becaue out of GTI\n",i_0);
              continue;
           }
      
           double PtL,PtB;          
           TOOLS::Galactic((double)hPtRazvsTime->GetBinContent(i_0),(double)hPtDeczvsTime->GetBinContent(i_0),&PtL,&PtB);
           if (fabs(PtB)<B_Cut || fabs(PtL)<40) {
              i_0++;
              //printf("advanced i_0 to %d because PtB=%f\n",i_0,PtB);
              continue;
           } //advance i_0 if at low GTIs
           break;
      }

      //supposedly i_0 is in a GTI and at a high PtB
      int i_1;
      for (i_1=i_0;i_1<i_0+iTimeStep;i_1++) {
          if (i_1==i_1_max) {done=true; break;}
          
          if (hMcIlwainLvsTime->GetBinContent(i_1)==0) {
        //      printf("break i_1 after %d steps because out of gti\n",i_1-i_0);
              break;
          }
          
          double PtL,PtB;          
          TOOLS::Galactic((double)hPtRazvsTime->GetBinContent(i_1),(double)hPtDeczvsTime->GetBinContent(i_1),&PtL,&PtB);
          if (fabs(PtB)<B_Cut  || fabs(PtL)<40) { //exited high PtB obs
        //      printf("break i_1 after %d steps because ptB=%f\n",i_1-i_0,PtB);
              break;
          }  
      }
      
      int imid=(i_0+i_1)/2;

      //CALCULATE RATE    
      float RockingAngle = hRockingAnglevsTime->GetBinContent(imid);
      if (RockingAngle<=0) {
             printf("%s: rocking angle weird rock=%f i_0=%d  time=%f\n",__FUNCTION__,RockingAngle, i_0,TIME_0);
             exit(1);
      }
      int RockingAngleBin=hScaleFactor->GetXaxis()->FindBin(RockingAngle);
        
      float McIlwainL=hMcIlwainLvsTime->GetBinContent(imid);
      if (!McIlwainL) {
              printf("%s: MCIlwainL=0? bin=%d\n",__FUNCTION__,i_0);
              exit(1);
      }
      
      double tmid=hMcIlwainLvsTime->GetBinCenter(imid);
      float dt=hMcIlwainLvsTime->GetXaxis()->GetBinLowEdge(i_1)-hMcIlwainLvsTime->GetXaxis()->GetBinLowEdge(i_0);
      double PtL,PtB;
      TOOLS::Galactic((double)hPtRazvsTime->GetBinContent(imid),(double)hPtDeczvsTime->GetBinContent(imid),&PtL,&PtB);
      if (PtL>180) PtL-=360;
      for (int ie=1;ie<=Energy_Bins_user;ie++) {
         float ScaleFactor = hScaleFactor->GetBinContent(RockingAngleBin,ie);
         if (ScaleFactor<=0) {printf("%s: Scalefactor=%f, rocking angle=%f \n",__FUNCTION__,ScaleFactor,RockingAngle); continue;}
          
         float AllSkyRate = RateFit[ie]->Eval(McIlwainL)*ScaleFactor;
         //if (ie==1) printf("i_=%d/%d dt=%f allrate=%f scalefactor=%f \n",i_0,i_1,dt,AllSkyRate,ScaleFactor);
      
         hEvents_est[ie]->Fill(tmid,dt*AllSkyRate);
         hMap_est->Fill(PtL,PtB,dt*AllSkyRate);
         
      }
      i_0=i_1;
  }

  fResults->cd();

  hMap_est->Write();

  ///////////////////////////////
  fitsfile *fptr;

  FILE* ftemp = fopen(FitsAllSkyFile.c_str(),"r");
  int ifile=0,ibin;
  while (fscanf(ftemp,"%s",name)==1) {
     printf("%d\r",ifile); fflush(0); ifile++;
     int status=0,hdutype,anynul;
     fits_open_file(&fptr, name, READONLY, &status);
     fits_movabs_hdu(fptr, 2, &hdutype, &status);
     long nrows=0;int ncols=0;
     fits_get_num_rows(fptr, &nrows, &status);
     fits_get_num_cols(fptr, &ncols, &status);
     int format;
     if      (DataClass.find("P7")!=string::npos) format=DATA_FORMAT_P7;
     else if (DataClass.find("P6")!=string::npos){
        if      (ncols==22) format=DATA_FORMAT_P6_NEW;
        else if (ncols==18) format=DATA_FORMAT_P6_OLD;
        else {printf("%s: unknown format file %s ncols=%d class=%s\n",__FUNCTION__,name,ncols,DataClass.c_str()); exit(1);}
     }
     else {printf("%s: Unknown fits file format file %s class=%s\n",__FUNCTION__,name,DataClass.c_str()); exit(1);}

     if (nrows<=0) {printf("%s: nrows=%ld\n", __FUNCTION__,nrows); exit(1);}
     for (long int i=1;i<=nrows;i++) {

         if (!PassesCuts(fptr,i,format)) continue;
         double PtTime;
         fits_read_col (fptr,TDOUBLE,10,i, 1, 1, NULL,&PtTime, &anynul, &status);

         int itimebin = hPtRazvsTime->FindBin(PtTime);
         if  (hPtRazvsTime->GetBinContent(itimebin)==0) {
            if      (hPtRazvsTime->GetBinContent(itimebin-1)!=0) ibin=itimebin-1;
            else if (hPtRazvsTime->GetBinContent(itimebin+1)!=0) ibin=itimebin+1;
            else if (hPtRazvsTime->GetBinContent(itimebin+2)!=0) ibin=itimebin+2;
            else if (hPtRazvsTime->GetBinContent(itimebin-2)!=0) ibin=itimebin-2;
            else    {printf("%s: there is a gap in the plots? %f %d\n",__FUNCTION__,PtTime,itimebin); continue;}
            itimebin=ibin;
         }

         double PtL,PtB;
         TOOLS::Galactic((double)hPtRazvsTime->GetBinContent(itimebin),(double)hPtDeczvsTime->GetBinContent(itimebin),&PtL,&PtB);
         //exclude observations near the galactic plane
         if (fabs(PtB)<B_Cut  || fabs(PtL)<40) continue;

         
         double FT1Energy;
         fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&FT1Energy, &anynul, &status);
         int iebin=Energy2Bin(FT1Energy);

         if (iebin>0 && iebin<=Energy_Bins_user) {
                hEvents_true[iebin]->Fill(PtTime);
                hPtB_Ave->Fill(PtTime,fabs(PtB));
                if (PtL>180) PtL-=360;
                hMap_true->Fill(PtL,PtB);
         }
     }
     fits_close_file(fptr, &status);
     if (ifile==12) break;
  }
  fclose(ftemp);



  for (int ie=1;ie<=Energy_Bins_user;ie++) {
      hEvents_est[ie]->Write();
      hEvents_true[ie]->Write();
      for (int ib=1;ib<=hRatio_True_over_Est[1]->GetNbinsX();ib++) {
          float real=hEvents_true[ie]->GetBinContent(ib);
          float est =hEvents_est[ie]->GetBinContent(ib);
          if (est>100) {
              float ratio=real/est;
              float ratio_err=ratio*sqrt(1/real+1/est);
              hRatio_True_over_Est[ie]->SetBinContent(ib,ratio);
              hRatio_True_over_Est[ie]->SetBinError(ib,ratio_err);
         }
      }
      hEvents_all_true->Add(hEvents_true[ie]);
      hEvents_all_est->Add(hEvents_est[ie]);
      hRatio_True_over_Est[ie]->Write();
  }
  
  hPtB_Ave->Divide(hEvents_all_true);
  hPtB_Ave->Write();
  hEvents_all_true->Write();

  hMap_true->Write();
  
  
  for (int i=1;i<=hMap_true->GetNbinsX();i++)  {
      for (int j=1;j<=hMap_true->GetNbinsY();j++)  {
         float est=hMap_est->GetBinContent(i,j);
         float real=hMap_true->GetBinContent(i,j);
         if (est>30) hMap_ratio_true_over_est->SetBinContent(i,j,real/est);
      }
  }
  hMap_ratio_true_over_est->Write();
  hEvents_all_est->Write();
  fResults->Close();
  fRate_Fits->Close();
  fPlots->Close();
}

