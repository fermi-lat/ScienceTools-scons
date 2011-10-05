//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"
#include "TRolke.h"
#include "TGraphErrors.h"
#include "TLine.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/PdfFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"
#include "TDirectory.h"
#include "TArc.h"

void BKGE_NS::PlotAfterglow(double GRB_TRIGGER_TIME, string TIMEDATA_FILE, string DATACLASS, int MinBin, int MaxBin, float a){

 const string GRB_NAME  = TOOLS::GetS("GRB_NAME");
 const float MIN_ENERGY = TOOLS::Get("MIN_ENERGY");
 const float MAX_ENERGY = TOOLS::Get("MAX_ENERGY");
 const float ErrorBar_HalfWidth = 0.68;
 /////////////////////////////////////////////////////////////////////////////////////////

 TRolke trol;
 trol.SetCL(ErrorBar_HalfWidth);

 //Figure out what classes we are using
 string DataClassName = TOOLS::GetDataClassName_noConv(DATACLASS);
 string ConversionName= TOOLS::GetConversionName(DATACLASS);

 int Limits[2]={MinBin,MaxBin};

 char name[1000];

 double MET;
 double MinE=0,MaxE=0;
 TH1F * hEst,*hBurst;
 TFile * fEst,*fSig;

 vector <double> NDetected[3] ///Keeps number of detected events 0=ll,1=ul,2=number
 ,Time,
 NExpected[2] ///keeps number of expected bkg events 0=error, 1=number
 ,HALF_DURATION,AEFF_SpectWgt;
 int ifile=0; ///number of intervals
 int ifile_first=0; ///this keeps the index of the first post-burst interval

 char Interval_name[1000],astring[2000];
 FILE * list = fopen(TIMEDATA_FILE.c_str(),"r");
 if (!list) {
   printf("%s: Can't open file %s\n",__FUNCTION__,TIMEDATA_FILE.c_str());
   return;
 }


 while (fgets(astring,sizeof(astring),list)) {
    //if (ifile>120) break;
    if (astring[0]=='#') continue;
    float bef,aft;
    if (sscanf(astring,"%s %lf %f %f %*s",Interval_name,&MET,&bef,&aft)!=4) break;
    printf("%15s  ",Interval_name);
    float DURATION=bef+aft;

    ///////////////////////////////////////////////////////////////////
    //REAL-SIGNAL COUNT
    sprintf(name,"%s/Bkg_Estimates/%s/sig_%.0f_%.0f_%s.root",(TOOLS::GetS("OUTPUT_DIR")).c_str(),Interval_name,MIN_ENERGY,MAX_ENERGY,ConversionName.c_str());
    fSig = TFile::Open(name);
    if (!fSig) {
        printf ("no %s\n",name);
        continue;
    }

    hBurst = (TH1F*)fSig->Get("hCtsvsEnergy_Burst");
    if (!hBurst) {
        printf("%s: no hBurst in signal file %s. Broken?\n",__FUNCTION__,name); 
        fSig->ls();
        fSig->Close();
        continue;
    }

    //////////////////////////////////////////////////////
    //Background
    sprintf(name,"%s/Bkg_Estimates/%s/bkg_%.0f_%.0f_%s.root",(TOOLS::GetS("OUTPUT_DIR")).c_str(),Interval_name,MIN_ENERGY,MAX_ENERGY,ConversionName.c_str());
    fEst = TFile::Open(name);
    if (!fEst){
        printf ("no %s \n",name);
        fSig->Close();
        continue;
    }

    hEst = (TH1F*)fEst->Get("hCtsvsEnergy_Est");
    if (!hEst) {
        printf("%s: no hCtsvsEnergy_Est in bkg file %s. Broken?\n",__FUNCTION__,name); 
        fEst->ls();
        fSig->Close();
        continue;
    }


    if (MinE==0) {
           char BkgMapsFile[1000];
           sprintf(BkgMapsFile,"%s/Bkg_Estimates/%s/BackgroundMaps_%s.root",(TOOLS::GetS("OUTPUT_DIR")).c_str(),Interval_name,ConversionName.c_str());
           TFile * fBkgMaps = TFile::Open(BkgMapsFile);
           TNamed * tn = (TNamed*)fBkgMaps->Get("Energy_Data");
           if (!tn) {
              printf ("%s: Error: File %s not completed..\n",__FUNCTION__,BkgMapsFile); 
              return;
           }
           //sscanf(tn->GetTitle(),"%lf-%lf-%*d",&MinE,&MaxE);
           if (Limits[0]<0) Limits[0]=1;
           if (Limits[1]<0) sscanf(tn->GetTitle(),"%*f-%*f-%d",&Limits[1]);
           MinE=pow(10,hEst->GetXaxis()->GetBinLowEdge(Limits[0]));
           MaxE=pow(10,hEst->GetXaxis()->GetBinUpEdge(Limits[1]));
           fBkgMaps->Close();
    }


    //////////////////////////////////////////////////////////////////

    double ndet=hBurst->Integral(Limits[0],Limits[1]);
    double nexp=hEst->Integral(Limits[0],Limits[1]);
    if (isnan(nexp)) {
         printf("%s: bkg integral is nan (?)\n",__FUNCTION__);
         fEst->Close(); fSig->Close(); continue;
    }
    if (isnan(ndet)) {
         printf("%s: signal integral is nan (?)\n",__FUNCTION__);
         fEst->Close(); fSig->Close(); continue;
    }

    if (nexp) {
      NDetected[2].push_back(ndet);
      NExpected[1].push_back(nexp);
      HALF_DURATION.push_back(DURATION/2.);
      Time.push_back(MET-GRB_TRIGGER_TIME+DURATION/2);
      if (fabs(Time[ifile]-DURATION/2.)<0.1) ifile_first=ifile;
      AEFF_SpectWgt.push_back(TOOLS::CalcSpectrallyWeightedExposure((TH1F*)fEst->Get("hExposure"),a));
      ifile++;
    }
    fEst->Close(); fSig->Close();
    printf("-- read ok\n");
 }

 if (ifile==0) {
   printf("%s: Couldn't find any data!? \n", __FUNCTION__);
   return;
 }
 else printf("%s: Read %d intervals \n",__FUNCTION__,ifile);

 double Sum_BkgSubtractedSignal[ifile],Sig_Limits[3][ifile],Flux[3][ifile];
 double ProbC[ifile];
 vector <double> SigSum,BackSum;
// const float error=TOOLS::Get("BKG_ESTIMATE_ERROR");
 const float error=0.2;

 for (int i=0;i<ifile;i++) {
     //Calculate range for the background-subtracted signal
     //1 is efficiency, 5 is method (this is TRolke jargon)
     NExpected[0].push_back(error*NExpected[1][i]);
     if (NExpected[1][i]) 
       trol.CalculateInterval((int)NDetected[2][i],0,0,NExpected[1][i],0,1,5,0,NExpected[0][i],0,0,0);
     else NExpected[0][i]=0;
     Sig_Limits[2][i]=NDetected[2][i]-NExpected[1][i];
     if (Sig_Limits[2][i]<0) Sig_Limits[2][i]=0;

     Sig_Limits[0][i]=Sig_Limits[2][i]-trol.GetLowerLimit();
     Sig_Limits[1][i]=trol.GetUpperLimit()-Sig_Limits[2][i];

     for (int itype=0;itype<3;itype++) {
         Sig_Limits[itype][i]/=2*HALF_DURATION[i];
         Flux[itype][i]=Sig_Limits[itype][i]/AEFF_SpectWgt[i];
     }

     for (int itype=0;itype<2;itype++) {
         NDetected[itype].push_back(TOOLS::PoissonErrorBar(itype,(int)NDetected[2][i]));
     }

     if (i==0) {
       SigSum.push_back(NDetected[2][i]);
       BackSum.push_back(NExpected[1][i]);
    }
    else {
       SigSum.push_back(SigSum[i-1]+NDetected[2][i]);
       BackSum.push_back(BackSum[i-1]+NExpected[1][i]);
     }
 }


 //Calculate error range
 //0 is low 1 is up
 vector <double> AccSig_Range1[2], //this is error (15%) times accumulated background
                 AccSig_Range2[2];    //this is assuming each step has 20% error
 double AccSig_Error[2]={0,0};
 for (int i=0;i<ifile;i++) {
   const float short_error=0.2;
   
   double SigMinusBack_Error[2];
   for (int itype=0;itype<2;itype++) {
      SigMinusBack_Error[itype] = sqrt(pow(short_error*NExpected[1][i],2) + pow(NDetected[itype][i],2));
      AccSig_Error[itype]       = sqrt(pow(AccSig_Error[itype],2) + pow(SigMinusBack_Error[itype],2));
      printf("%d nexperr=%f ndeterror=%f sigminuserr=%f accsig=%f\n",i,error*NExpected[1][i],NDetected[itype][i],SigMinusBack_Error[itype],AccSig_Error[itype]);
   }
   AccSig_Range2[0].push_back(-AccSig_Error[0]);
   AccSig_Range2[1].push_back(AccSig_Error[1]);
   
   AccSig_Range1[0].push_back(-error*BackSum[i]);
   AccSig_Range1[1].push_back(error*BackSum[i]);

 }

 ///Offset data so that we start at zero for ifile==ifile_first
 double Offset_Sig=0,Offset_Back=0;
 if (ifile_first>0) {
   Offset_Sig =SigSum[ifile_first-1];
   Offset_Back=BackSum[ifile_first-1];
 }
 for (int i=0;i<ifile;i++) {
    if (ifile_first>0) {
      SigSum[i] -=Offset_Sig;
      BackSum[i]-=Offset_Back;
    }

    ProbC[i] = ROOT::Math::poisson_cdf_c((int)SigSum[i],BackSum[i])+ROOT::Math::poisson_pdf((int)SigSum[i],BackSum[i]);
    if (ProbC[i]==1 || i<ifile_first) ProbC[i]=-99;
    else  ProbC[i] = ROOT::Math::gaussian_quantile_c(ProbC[i],1);
 }

 //Calculate accumulated background-subtracted signal
 for (int i=0;i<ifile;i++) {
     if (i==0) Sum_BkgSubtractedSignal[i]=NDetected[2][i]-NExpected[1][i];
     else      Sum_BkgSubtractedSignal[i]=Sum_BkgSubtractedSignal[i-1]+NDetected[2][i]-NExpected[1][i];
 }


 ///Plotting time!!
 TGraphAsymmErrors* gDiffSig = new TGraphAsymmErrors(ifile,&Time.front(),&NDetected[2].front(),&HALF_DURATION.front(),&HALF_DURATION.front(),&NDetected[0].front(),&NDetected[1].front());
 gDiffSig->SetName("gSignal");
 TGraphErrors* gDiffBack= new TGraphErrors(ifile,&Time.front(),&NExpected[1].front(),&HALF_DURATION.front(),&NExpected[0].front());
 gDiffBack->SetName("gBack");
 TGraphErrors* gAccSignal= new TGraphErrors(ifile,&Time.front(),&SigSum.front(),&HALF_DURATION.front());
 TGraphErrors* gAccBack= new TGraphErrors(ifile,&Time.front(),&BackSum.front(),&HALF_DURATION.front());
 TGraphErrors* gSum_BkgSubtractedSignal = new TGraphErrors(ifile,&Time.front(),Sum_BkgSubtractedSignal,&HALF_DURATION.front());
 TGraphAsymmErrors* gDiffSigMBack = new TGraphAsymmErrors(ifile,&Time.front(),Sig_Limits[2],&HALF_DURATION.front(),&HALF_DURATION.front(),Sig_Limits[0],Sig_Limits[1]);
 TGraphAsymmErrors* gFlux = new TGraphAsymmErrors(ifile,&Time.front(),Flux[2],&HALF_DURATION.front(),&HALF_DURATION.front(),Flux[0],Flux[1]);
 TGraph *gAccSig_Range1[2],*gAccSig_Range2[2];
 for (int i=0;i<2;i++) {
    gAccSig_Range1[i] = new TGraph(ifile,&Time.front(),&AccSig_Range1[i].front());
    gAccSig_Range2[i] = new TGraph(ifile,&Time.front(),&AccSig_Range2[i].front());
 }

 char name2[1000];
 sprintf(name,"c_%s_%.0f_%.0f",DATACLASS.c_str(),MinE,MaxE);
 sprintf(name2,"Differential Plots %.1f<E<%.1fMeV",MinE,MaxE);
 TCanvas *c = new TCanvas(name,name2,500,800);
 sprintf(name,"c1_%s_%.0f_%.0f",DATACLASS.c_str(),MinE,MaxE);
 sprintf(name2,"Cumulative Plots %.1f<E<%.1fMeV",MinE,MaxE);
 TCanvas *c1 = new TCanvas(name,name2,500,800);

 c->Divide(1,3);
 c1->Divide(1,3);

 for (int ip=0;ip<3;ip++) { 
   c->GetPad(1+ip)->SetGridx();
   c->GetPad(1+ip)->SetGridy();
   c1->GetPad(1+ip)->SetGridx();
   c1->GetPad(1+ip)->SetGridy();
 }


 c->cd(1);
    //c->GetPad(1)->SetLogy();
    sprintf(name,"Detected and Expected Background Events");
    gDiffSig->GetXaxis()->SetTitle("Time after the burst [sec]");
    gDiffSig->GetYaxis()->SetTitle("Events/bin");
    gDiffSig->GetXaxis()->SetDecimals(false);
    gDiffSig->SetTitle(name);
    gDiffSig->SetMarkerColor(2);
    gDiffSig->SetMarkerStyle(3);
    gDiffSig->SetMarkerSize(0.7);
    gDiffSig->SetLineColor(2);
    gDiffSig->Draw("AP");
    gDiffBack->GetXaxis()->SetDecimals(false);
    gDiffBack->SetTitle("Background");
    gDiffBack->SetMarkerStyle(7);
    gDiffBack->SetMarkerSize(0.7);
    gDiffBack->Draw("EPSAME");

    TLegend * leg = new TLegend(0.67,0.86,0.96,0.99);
    leg->SetFillColor(0);
    leg->AddEntry(gDiffSig,"Detected Events","p");
    leg->AddEntry(gDiffBack,"Expected Background","p");
    leg->Draw();
     c->cd(2);
    sprintf(name,"Background-subtracted Signal");
    gDiffSigMBack->GetXaxis()->SetTitle("Time after the burst [sec]");
    gDiffSigMBack->GetYaxis()->SetTitle("Event rate [Hz]");
    gDiffSigMBack->SetTitle(name);
    gDiffSigMBack->SetMarkerStyle(8);
    //gDiffSigMBack->SetMarkerColor(2);
    //gDiffSigMBack->SetLineColor(2);
    gDiffSigMBack->SetMarkerSize(0.6);
    gDiffSigMBack->Draw("AEP");
     c->cd(3);
    //c->GetPad(3)->SetLogy();
    //gFlux->SetMinimum(-1e-5);
    gFlux->SetMarkerColor(2);
    gFlux->SetTitle("Flux");
    gFlux->GetYaxis()->SetTitle("Flux [ph/(cm^{2} sec)]");
    gFlux->GetXaxis()->SetTitle("Time after burst [sec]");
    gFlux->Draw("AEP");

 c1->cd(1);
    sprintf(name,"Accumulated signal and background");
    //gAccSignal->GetXaxis()->SetRangeUser(0,Time[ifile-1]+2*HALF_DURATION[ifile-1]);
    gAccSignal->SetMarkerSize(1);
    gAccSignal->SetMarkerColor(1);
    gAccSignal->SetMarkerStyle(7);
    gAccSignal->SetTitle(name);
    gAccSignal->GetXaxis()->SetTitle("Time after the burst [sec]");
    gAccSignal->GetYaxis()->SetTitle("Events");
    gAccSignal->Draw("PEA");
    sprintf(name,"Accumulated background over time");
    //gAccBack->GetXaxis()->SetRangeUser(0-HALF_DURATION[ifile_first],Time[ifile-1]+2*HALF_DURATION[ifile-1]);
    gAccBack->SetMarkerColor(2);
    gAccBack->SetMarkerSize(2);
    gAccBack->SetMarkerStyle(7);
    gAccBack->SetTitle(name);
    gAccBack->GetXaxis()->SetTitle("Time after the burst [sec]");
    gAccBack->GetYaxis()->SetTitle("Events");
    gAccBack->Draw("PESAME");

    TLegend * leg2 = new TLegend(0.18,0.78,0.40,0.87);
    leg2->SetFillColor(0);
    leg2->AddEntry(gAccSignal,"Detected events","p");
    leg2->AddEntry(gAccBack,"Expected background","p");
    leg2->Draw();

 c1->cd(2);
    sprintf(name,"Probability of accumulated signal");
    TGraphErrors * gProbC = new TGraphErrors(ifile,&Time.front(),ProbC,&HALF_DURATION.front());
    gProbC->GetXaxis()->SetRangeUser(0-HALF_DURATION[ifile_first],Time[ifile-1]+2*HALF_DURATION[ifile-1]);
    gProbC->SetMinimum(0);
    gProbC->SetMaximum(gProbC->GetYaxis()->GetXmax()*0.6);
    gProbC->SetMarkerColor(2);
    gProbC->SetMarkerSize(1);
    gProbC->SetMarkerStyle(7);
    gProbC->SetTitle(name);
    gProbC->GetXaxis()->SetTitle("Time after the burst [sec]");
    gProbC->GetYaxis()->SetTitle("Significance [#sigma]");
    gProbC->Draw("PEA");

 c1->cd(3);
    gSum_BkgSubtractedSignal->GetYaxis()->SetRangeUser(AccSig_Range1[0][ifile-1]*1.1,AccSig_Range1[1][ifile-1]*0.9);
    gSum_BkgSubtractedSignal->GetXaxis()->SetTitle("Time after the burst [sec]");
    gSum_BkgSubtractedSignal->GetYaxis()->SetTitle("Events");
    gSum_BkgSubtractedSignal->SetMarkerColor(2);
    gSum_BkgSubtractedSignal->SetMarkerStyle(4);
    gSum_BkgSubtractedSignal->SetMarkerSize(0.4);
    gSum_BkgSubtractedSignal->SetMarkerColor(2);
    sprintf(name,"Accumulated background-subtracted signal, %.1f<E<%.1fMeV",MinE,MaxE);
    gSum_BkgSubtractedSignal->SetTitle(name);
    gSum_BkgSubtractedSignal->Draw("AP");

    for (int i=0;i<2;i++) {
       gAccSig_Range1[i]->SetLineStyle(2);
       gAccSig_Range1[i]->Draw("CSAME");
       gAccSig_Range2[i]->SetLineColor(2);
       gAccSig_Range2[i]->SetLineStyle(2);
       gAccSig_Range2[i]->Draw("CSAME");


    }
/*
   ///SAVE results to files
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Differential.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   //c->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Cumulative.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c1->SaveAs(name);

   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Differential.eps",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   //c->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Cumulative.eps",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c1->SaveAs(name);
   gStyle->SetOptTitle(0);

   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Duration.eps",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c1->GetPad(3)->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Cumulative.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c1->GetPad(3)->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_AccumulatedSigAndBkg.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c1->GetPad(1)->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Lightcurves.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c->GetPad(1)->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Signal.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   //c->GetPad(2)->SaveAs(name);
   sprintf(name,"Results/%s/Bkg_Estimates/%s_%s_%.0f_%.0f_Flux.png",GRB_NAME.c_str(),GRB_NAME.c_str(),DATACLASS.c_str(),MinE,MaxE);
   c->GetPad(3)->SaveAs(name);
   gStyle->SetOptTitle(1);
*/
   sprintf(name,"Spectrally-weighted exposure, a=%.2f",a);
   TCanvas * cAEFF = new TCanvas("cAEFF",name);
   TGraphErrors * gAEFF= new TGraphErrors(ifile,&Time.front(),&AEFF_SpectWgt.front(),&HALF_DURATION.front());
   gAEFF->GetXaxis()->SetTitle("Time after the burst [s]");
   gAEFF->GetYaxis()->SetTitle("Exposure [cm^{2} s]");
   sprintf(name,"%.1f<E<%.1fMeV",MinE,MaxE);
   gAEFF->SetTitle(name);
   gAEFF->Draw("AP");
   sprintf(name,"%s/Bkg_Estimates/%s_%.0f_%.0f_AEFF.png",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),MinE,MaxE);
   cAEFF->SaveAs(name);
  //done!

  TDirectory * gRootDir = gROOT->GetDirectory("/");
  gRootDir->Add(c);
  gRootDir->Add(c1);

}
