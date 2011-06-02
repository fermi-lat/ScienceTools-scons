//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"

#define DEBUG

ClassImp(BackgroundEstimator)

void BackgroundEstimator::CalcResiduals(string FitsAllSkyFile){

 TH2I * hSignal_True[Energy_Bins_datafiles+2];
 TH2F * hSimulatedSky = new TH2F("hSimulatedSky","hSimulatedSky",L_BINS,-180,180,B_BINS,-90,90);
 hSimulatedSky->GetXaxis()->SetTitle("Longitude (deg)");
 hSimulatedSky->GetYaxis()->SetTitle("Latitude (deg)");

 TH2F * hResidual = new TH2F("hResidual","hResidual",L_BINS,-180,180,B_BINS,-90,90);
 hResidual->GetXaxis()->SetTitle("Longitude (deg)");
 hResidual->GetYaxis()->SetTitle("Latitude (deg)");

 for (int ie=0;ie<Energy_Bins_datafiles+2;ie++) {
     sprintf(name,"hSignal_True_%d",ie);
     hSignal_True[ie] = new TH2I(name,"hSignal_True",L_BINS,-180,180,B_BINS,-90,90);
 }


 printf("%s: Filling Skymap\n",__FUNCTION__);
 unsigned int TrueEvents[Energy_Bins_datafiles+2];
 double SimEvents[Energy_Bins_datafiles+2];
 for (int ie=0;ie<Energy_Bins_datafiles+2;ie++) {
    TrueEvents[ie]=0;
    SimEvents[ie]=0;
 }
/////////////////////////////////////////////////////////////////




 fitsfile *fptr;
 vector <double> ALLSTARTGTI;
 vector <double> ALLENDGTI;

 int ifile=0;
 long GTIs;
 FILE* ftemp = fopen(FitsAllSkyFile.c_str(),"r");

 while (fscanf(ftemp,"%s",name)==1) {
    if (ifile%100==0) {printf("%d\r",ifile); fflush(0);}
    ifile++;
    long nrows;int ncols;
    int status=0,hdutype,anynul;
    fits_open_file(&fptr, name, READONLY, &status);
    if (status) {printf("error opening file %s\n",name); exit(1);}

    //Read GTIs 
    fits_movabs_hdu(fptr, 3, &hdutype, &status);
    fits_get_num_rows(fptr, &GTIs, &status);
    double agti;
    for (long int i=1;i<=GTIs;i++) {
       fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&agti, &anynul, &status);
       ALLSTARTGTI.push_back(agti);
       fits_read_col (fptr,TDOUBLE,2,i, 1, 1, NULL,&agti, &anynul, &status);
       ALLENDGTI.push_back(agti);
       //printf("%d %d %f %f\n",ifile,ALLGTIs,ALLSTARTGTI[ALLGTIs],ALLENDGTI[ALLGTIs]);
    }

    // Read event data
    fits_movabs_hdu(fptr, 2, &hdutype, &status);
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

    for (int i=1;i<=nrows;i++) {
 
         if (!PassesCuts(fptr,i,format)) continue;

         double FT1Energy;
         fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&FT1Energy, &anynul, &status);
         short int iebin=Energy2Bin(FT1Energy);

         double FT1L,FT1B;
         fits_read_col (fptr,TDOUBLE,4,i, 1, 1, NULL,&FT1L, &anynul, &status);
         fits_read_col (fptr,TDOUBLE,5,i, 1, 1, NULL,&FT1B, &anynul, &status);
         if (FT1L>180) FT1L-=360;

         hSignal_True[iebin]->Fill(FT1L,FT1B);
         TrueEvents[iebin]++;
    }

    fits_close_file(fptr, &status);
 }

 ALLSTARTGTI.push_back(0);
 ALLENDGTI.push_back(0);
 ////////////////////////////////////////////////////////////////

 printf("%s: Simulating Sky\n",__FUNCTION__);
 sprintf(name,"%s/Residual_%s.root",DataDir.c_str(),DataClass.c_str());
 TFile * fout = new TFile(name,"RECREATE");
 sprintf(name,"%s/Residual_Sim_%s.root",DataDir.c_str(),DataClass.c_str());
 TFile * fSim = new TFile(name,"RECREATE");
  
 for (int ie=1;ie<=Energy_Bins_datafiles;ie++) { 
     TOOLS::ProgressBar(ie,Energy_Bins_datafiles);

     sprintf(name,"%s/Plots_%s_%s.root",DataDir.c_str(),DataClassName_noConv.c_str(),ConversionName.c_str());
     SimulateSky(name, hSimulatedSky, ALLSTARTGTI, ALLENDGTI, 1e2*TimeBins,ie);
     SimEvents[ie] = hSimulatedSky->Integral();

     //SimEvents can be also nan
     if (!(SimEvents[ie]>0)) {printf("%s: Error simulating sky (result=%f)\n",__FUNCTION__,SimEvents[ie]); exit(1);}
     sprintf(name,"hResidual_%d",ie);
     hResidual->Reset();
     hResidual->SetNameTitle(name,name);
     hResidual->Add(hSignal_True[ie]);
     hResidual->Add(hSimulatedSky,-1); //-1  = subtract
     fout->cd();
     hResidual->Write(); 
     hSignal_True[ie]->Write();  hSignal_True[ie]->Delete();
     sprintf(name,"hSimulatedSky_%d",ie);  hSimulatedSky->Write(name);
     
     fSim->cd();
 }

 hResidual->Delete();
 hSimulatedSky->Delete();
 fSim->Close();

 sprintf(name,"%.2f",Residuals_version);
 TNamed Data = TNamed("version",name);
 fout->cd();Data.Write();

 fout->Close();
}

