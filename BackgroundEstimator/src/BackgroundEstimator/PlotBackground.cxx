//Author: Vlasios Vasileiou <vlasisva@gmail.com>
//$Header$
#include "BackgroundEstimator/BackgroundEstimator.h"
#include "TGraph.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TMath.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "Math/PdfFuncMathCore.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"
#include "TH1F.h"
#include <vector>

//if Energy_Min_user,Energy_Max_user,Energy_Bins_user<=0 then the default values will be used
string BKGE_NS::PlotBackground(string Interval_name, double MET, double DURATION, string FT1_FILE, string FT2_FILE, string DATACLASS, double Energy_Min_user, double Energy_Max_user, int Energy_Bins_user,  float FT1ZenithTheta_Cut, bool OverwritePlots, int verbosity, double MET_FOR_THETA, bool Save_Earth_Coo_Map){
 if (MET_FOR_THETA<=0) MET_FOR_THETA=MET;
 
 bool OverwriteResults=false;

 bool WasBatch=gROOT->IsBatch();
 if (!WasBatch) gROOT->SetBatch(kTRUE);

 bool MakeAnimation=false;
 static bool first=false;

 const double RA=TOOLS::Get("GRB_RA");
 const double DEC=TOOLS::Get("GRB_DEC");
 const string GRB_NAME=TOOLS::GetS("GRB_NAME");

 const int CALCULATE_ROI=(int)TOOLS::Get("CALCULATE_ROI");
 const double ROI_LOCALIZATIONERROR=TOOLS::Get("ROI_LOCALIZATION_ERROR");
 const float ROI_MAX_RADIUS=TOOLS::Get("ROI_MAX_RADIUS");
 const float error = TOOLS::Get("BKG_ESTIMATE_ERROR");

 char name[1000];

 //Initialize Estimators
 BackgroundEstimator * Est[2];
 bool Combine=false;
 string DataClassName=DATACLASS.substr(0,DATACLASS.find("::"));
 if (DATACLASS.find("FRONT+BACK")!=string::npos) {
     Est[0] = new BackgroundEstimator(DataClassName+"::FRONT",Energy_Min_user,Energy_Max_user,Energy_Bins_user, FT1ZenithTheta_Cut, true,first);
     Est[1] = new BackgroundEstimator(DataClassName+"::BACK",Energy_Min_user,Energy_Max_user,Energy_Bins_user, FT1ZenithTheta_Cut, true,false);
     Combine=true;
 }
 else Est[0] = new BackgroundEstimator(DATACLASS ,Energy_Min_user,Energy_Max_user,Energy_Bins_user, FT1ZenithTheta_Cut ,true,first);
 
 Energy_Min_user=Est[0]->Energy_Min_user;
 Energy_Max_user=Est[0]->Energy_Max_user;
 Energy_Bins_user=Est[0]->Energy_Bins_user;

 TCanvas *c= (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cBkgEstimate");
 if (c) delete c;
 c = new TCanvas("cBkgEstimate","cBkgEstimate",1000,500); 
 
 TPad * pad[2];
 pad[0] = new TPad("p","p",0.025,0,0.5,1);
 pad[1] = new TPad("p1","p1",0.5,0,0.975,0.6);
 for (int i=0; i<2;i++) {
   pad[i]->SetLogy();
   pad[i]->SetGridy();
   pad[i]->SetFillColor(0);
   pad[i]->Draw();
 }
 
 TH1F * hEst[2],*histBurst[2];
 TGraphAsymmErrors * gBurst=NULL;

 short int EstMax=1;
 if (Combine) EstMax=2;

 //INITIALIZE ROI(s)
 TH1F * hROI[2];
 for (int iEst=0;iEst<EstMax;iEst++) {
    sprintf(name,"hROI_%d",iEst);
    TH1F * h = (TH1F*)gROOT->Get(name); if (h) delete h;
    hROI[iEst] = new TH1F(name,"ROI Radius",Energy_Bins_user,log10(Energy_Min_user),log10(Energy_Max_user));
    hROI[iEst]->GetXaxis()->SetTitle("log_{10}(Energy/MeV)");
    hROI[iEst]->GetYaxis()->SetTitle("Radius (deg)");
    if (CALCULATE_ROI==2) {
       if (TOOLS::ReadROI_File(hROI[iEst], TOOLS::GetS("ROI_RADIUSFILE"))) return "";
    }
    else if (CALCULATE_ROI==0) {
        for (int i=1;i<=Energy_Bins_user;i++) hROI[iEst]->SetBinContent(i,ROI_MAX_RADIUS);
    }
 }

 FILE * ftemp;
 TH1F * hROIEff;
 TH1F * h = (TH1F*)gROOT->Get("hROIEff"); if (h) delete h;
 hROIEff = new TH1F("hROIEff","hROIEff",Energy_Bins_user,log10(Energy_Min_user),log10(Energy_Max_user));
 TFile * fEst[2]={0,0},*fSig[2]={0,0};
 char GRB_DIR[1000];
 sprintf(GRB_DIR,"%s/Bkg_Estimates/%s",(TOOLS::GetS("OUTPUT_DIR")).c_str(),Interval_name.c_str());

 char ResultsFilename[1000];
 if (Combine) sprintf(ResultsFilename,"%s/%s-%s_FRONT+BACK_Results_%1f_%.1f.root",GRB_DIR,GRB_NAME.c_str(),Est[0]->DataClassName_noConv.c_str(),MET,MET+DURATION);
 else         sprintf(ResultsFilename,"%s/%s-%s_Results_%.1f_%.1f.root",GRB_DIR,GRB_NAME.c_str(),Est[0]->DataClass.c_str(),MET,MET+DURATION);

    //check if the estimation finished correctly first
    sprintf(name,"%s/%s_BackgroundMaps.root",GRB_DIR,Est[0]->DataClass.c_str());
    TFile * fBkgMaps = TFile::Open(name);
    if (!fBkgMaps) {printf("%s: Problem with bkg file %s.. skipping\n",__FUNCTION__,name); fBkgMaps->Close(); return "";}      
    TNamed * err = (TNamed*)fBkgMaps->Get("ERROR");
    if (err) {printf("%s: Problem with the bkg file. Error code was '%s'. Skipping\n",__FUNCTION__,err->GetTitle()); fBkgMaps->Close(); return "";}
    fBkgMaps->Close();

    //////////////////////////////////////////////////////
    //Background calculation
    bool BkgOK=true;
    for (int iEst=0;iEst<EstMax;iEst++) {
       sprintf(name,"%s/%s_bkg_%.0f_%.0f.root",GRB_DIR, Est[0]->DataClass.c_str(),Energy_Min_user,Energy_Max_user);
       ftemp = fopen(name,"r");
       
       bool ProcessFile=false;
       if (!ftemp || OverwritePlots) ProcessFile=true;
       else { //check if energy range is the same  
          TFile * fcheck = TFile::Open(name);      
          //check ztheta cut
   
          float FT1ZenithTheta_Cut_file = atof(((TNamed*)fcheck->Get("FT1ZenithTheta_Cut"))->GetTitle());
          if (fabs(FT1ZenithTheta_Cut_file-FT1ZenithTheta_Cut)>0.1) {
       	      printf("%s: Different ztheta cuts detected bkg_file's=%.0f vs configured=%.0f. Will have to recalculate the background!\n",__FUNCTION__,FT1ZenithTheta_Cut_file,FT1ZenithTheta_Cut);
	      ProcessFile=true;
	      
          }
    
          TH1F * hexp = (TH1F*)fcheck->Get("hROI");
          if (!hexp) ProcessFile=true;
          else {
              if      (fabs(Energy_Min_user-pow(10,hexp->GetXaxis()->GetXmin()))>0.1) ProcessFile=true;
              else if (fabs(Energy_Max_user-pow(10,hexp->GetXaxis()->GetXmax()))>0.1) ProcessFile=true;
              else if (Energy_Bins_user!=hexp->GetNbinsX()) ProcessFile=true;
              if (ProcessFile==true && verbosity>1) printf("%s: Recalculating bkg -- energy limits different\n",__FUNCTION__);
          }
	
          if (ProcessFile==false) {
              if (CALCULATE_ROI==1) {
                 char PlotsFile[1000];
                 sprintf(PlotsFile,"%s/Burst_Plots.root",GRB_DIR);
                 TOOLS::CalculatePSF(hROI[iEst],MET_FOR_THETA,FT2_FILE,DATACLASS);
                 if (hROI[iEst]->GetBinContent(1)<0) { BkgOK=false; break;} 
              }
	      for (int ib=1;ib<=Energy_Bins_user;ib++) {
		    if (fabs(hROI[iEst]->GetBinContent(ib)-hexp->GetBinContent(ib))>0.0001) ProcessFile=true;
	      }
	      if (ProcessFile==true && verbosity>1) printf("%s: Recalculating bkg -- ROI different\n",__FUNCTION__);
          }
	 
          fcheck->Close();
       }

       if (ProcessFile) {
	  OverwriteResults=true;
          //the next is true when we don't have any data to calculate the pointing (e.g. SAA)
          //in such a case hROI is filled with -1 and the process skips that interval
          if (CALCULATE_ROI==1) {
              char PlotsFile[1000];
              sprintf(PlotsFile,"%s/Burst_Plots.root",GRB_DIR);
              TOOLS::CalculatePSF(hROI[iEst],MET_FOR_THETA,FT2_FILE,DATACLASS);
          }
          if (hROI[iEst]->GetBinContent(1)<0) { BkgOK=false; break;}
          if (Est[iEst]->FillBackgroundHist(GRB_DIR, hROI[iEst],RA,DEC,3,verbosity)) {BkgOK=false; break; }//both
       }

       if (ftemp) fclose(ftemp);
    }
    if (!BkgOK) {printf("%s: Problem with the bkg file.. skipping\n",__FUNCTION__); return "";}

    //Exposure && ROI read
    TH1F * hExp[2],*hROIEst[2];
    for (int iEst=0;iEst<EstMax;iEst++) {
          sprintf(name,"%s/%s_bkg_%.0f_%.0f.root",GRB_DIR,Est[iEst]->DataClass.c_str(),Energy_Min_user,Energy_Max_user);
          if (fEst[iEst]) fEst[iEst]->Close();
          fEst[iEst] = TFile::Open(name);
          if (fEst[iEst]->TestBit(TFile::kRecovered)) {
              printf("%s: file %s has not been closed ok. Probably job failed. Delete its folder and run again.\n",__FUNCTION__,name);
              exit(1);
          }
          hExp[iEst] = (TH1F*)fEst[iEst]->Get("hExposure");

          hROIEst[iEst] = (TH1F*)fEst[iEst]->Get("hROI");
          hEst[iEst] = (TH1F*)fEst[iEst]->Get("hCtsvsEnergy_Est");
          if (!hEst[iEst]) {
             printf("no hCtsvsEnergy histogram in %s\n Delete that file and run again.",name); 
             fEst[iEst]->ls();
             exit(1);
          }
          
          hEst[iEst]->SetLineColor(2);
          hEst[iEst]->SetTitle(GRB_DIR);
     }
     hROIEff->Reset();

     if (Combine) {
          hExp[0]->Add(hExp[1]);
          hEst[0]->Add(hEst[1]);

          hROIEff->Add(hROIEst[0]);
          hROIEff->Add(hROIEst[1]);
          hROIEff->Scale(0.5);
 
          //Create a sig file with the combined results
          sprintf(name,"%s/%s_FRONT_BACK_%.0f_%.0f.root",GRB_DIR,Est[0]->DataClassName_noConv.c_str(),Energy_Min_user,Energy_Max_user);
          TFile * fBkgNew = new TFile(name,"RECREATE");
          hEst[0]->Write();
          hExp[0]->Write();
          hROIEff->Write();
          fBkgNew->Close();
     }
     else hROIEff->Add(hROIEst[0]);
     //////////////////////////////////////////
     
    ////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////
    //REAL-SIGNAL COUNT
    TH1F *hROISig[2];
    for (int iEst=0;iEst<EstMax;iEst++) {
       sprintf(name,"%s/%s_sig_%.0f_%.0f.root",GRB_DIR,Est[iEst]->DataClass.c_str(),Energy_Min_user,Energy_Max_user);
       ftemp = fopen(name,"r");

       bool ProcessFile=false;
       if (!ftemp || OverwritePlots) ProcessFile=true;
       else { //check if energy levels are the same
          TFile * fcheck = TFile::Open(name);        
          TH1F * hexp = (TH1F*)fcheck->Get("hROI");
          if (!hexp) ProcessFile=true;
          else {
              if      (fabs(Energy_Min_user-pow(10,hexp->GetXaxis()->GetXmin()))>0.1) ProcessFile=true;
              else if (fabs(Energy_Max_user-pow(10,hexp->GetXaxis()->GetXmax()))>0.1) ProcessFile=true;
              if (ProcessFile==true && verbosity>1) printf("%s: Recounting events -- energy limits different\n",__FUNCTION__);
          }

          if (!ProcessFile) {
              if (CALCULATE_ROI==1) {
                 char PlotsFile[1000];
                 sprintf(PlotsFile,"%s/Burst_Plots.root",GRB_DIR);
                 TOOLS::CalculatePSF(hROI[iEst],MET_FOR_THETA,FT2_FILE,DATACLASS);
              }
	      for (int ib=1;ib<=Energy_Bins_user;ib++) {
		    if (fabs(hROI[iEst]->GetBinContent(ib)-hexp->GetBinContent(ib))>0.0001) ProcessFile=true;
	      }
	      if (ProcessFile==true && verbosity>1) printf("%s: Recounting signal -- ROIs different\n",__FUNCTION__);
          }
          
	  if (!ProcessFile) {
	     float FT1ZenithTheta_Cut_file = atof(((TNamed*)fcheck->Get("FT1ZenithTheta_Cut"))->GetTitle());
	     if (fabs(FT1ZenithTheta_Cut_file-FT1ZenithTheta_Cut)>0.1) {
	 	if (verbosity>1) printf("%s: Different ztheta cuts detected %.0f vs %.0f, will recalculate signal..\n",__FUNCTION__,FT1ZenithTheta_Cut_file,FT1ZenithTheta_Cut);
	 	ProcessFile=true;
             }
    	  }
          
          fcheck->Close();
       }

       if (ProcessFile) {
	    OverwriteResults=true;
            //Check if we have to make a listfile
            if (FT1_FILE[0]=='^') FT1_FILE="@"+string(GRB_DIR)+"/fitslist.txt";
            TOOLS::Make_Burst_Plots(DATACLASS, FT1_FILE, GRB_DIR,FT1ZenithTheta_Cut,RA,DEC,MET,DURATION,hROIEst[iEst],0,0,0,Save_Earth_Coo_Map);
       }
       if (ftemp) fclose (ftemp);

       sprintf(name,"%s/%s_sig_%.0f_%.0f.root",GRB_DIR,Est[iEst]->DataClass.c_str(),Energy_Min_user,Energy_Max_user);
       if (fSig[iEst]) fSig[iEst]->Close();
       fSig[iEst] = TFile::Open(name);
       if (fSig[iEst]->TestBit(TFile::kRecovered)) {
              printf("%s: file %s has not been closed ok. Probably job failed. Delete its folder and run again.\n",__FUNCTION__,name);
              exit(1);
       }

       if (!Combine) {
          gBurst = (TGraphAsymmErrors*)fSig[iEst]->Get("gSignal;1");
          if (!gBurst) {
                 printf("no gSignal in %s. Delete the file and try again.\n",name); 
                 fSig[iEst]->ls();
                 exit(1);
          }
       }

       histBurst[iEst] =(TH1F*)fSig[iEst]->Get("hCtsvsEnergy_Burst");
       hROISig[iEst] = (TH1F*)fSig[iEst]->Get("hROI");
       if (!hROISig[iEst]) {printf("no hROI in %s\n",name); fSig[iEst]->ls(); exit(1);}
       //compare ROI Radii and make sure they are the same
       if (hROIEst[iEst]->GetNbinsX()!=hROISig[iEst]->GetNbinsX()) {printf("ROI Radii have different number of bins! %d %d\n",hROIEst[iEst]->GetNbinsX(),hROISig[iEst]->GetNbinsX()); exit(1);}
       for (int i=1;i<=hROIEst[iEst]->GetNbinsX();i++) {
           if (fabs(hROIEst[iEst]->GetBinContent(i)-hROISig[iEst]->GetBinContent(i))>0.0001) {
                 printf("Different ROI RADII Est:%f SIG:%f bin:%d\n",hROIEst[iEst]->GetBinContent(i),hROISig[iEst]->GetBinContent(i),i);
                 exit(1);
           }
       }
       
       //Compare ZenithTheta cuts
       if (strcmp(((TNamed*)fSig[iEst]->Get("FT1ZenithTheta_Cut"))->GetTitle(),((TNamed*)fEst[iEst]->Get("FT1ZenithTheta_Cut"))->GetTitle())) {
            printf("ZenithTheta cuts are different! (sig) %s %s\n",((TNamed*)fSig[iEst]->Get("FT1ZenithTheta_Cut"))->GetTitle(),((TNamed*)fEst[iEst]->Get("FT1ZenithTheta_Cut"))->GetTitle());
            exit(1);
       }
    }


    if (Combine) { //if we are combining create the signal graph from scratch
         if (gBurst) delete gBurst;
         double Sig[200],SigError[2][200];
         double EnergyData[3][200];
         for (int ie=1;ie<=histBurst[0]->GetNbinsX();ie++) {
             Sig[ie-1]=histBurst[0]->GetBinContent(ie)+histBurst[1]->GetBinContent(ie);
             EnergyData[0][ie-1]=histBurst[0]->GetXaxis()->GetBinCenter(ie)-histBurst[0]->GetXaxis()->GetBinLowEdge(ie);
             EnergyData[1][ie-1]=histBurst[0]->GetXaxis()->GetBinCenter(ie);
             EnergyData[2][ie-1]=histBurst[0]->GetXaxis()->GetBinUpEdge(ie)-histBurst[0]->GetXaxis()->GetBinCenter(ie);
             SigError[1][ie-1]=TOOLS::PoissonErrorBar(1,(int)Sig[ie-1]);
             SigError[0][ie-1]=TOOLS::PoissonErrorBar(0,(int)Sig[ie-1]);
         }
         gBurst = new TGraphAsymmErrors(Energy_Bins_user,EnergyData[1],Sig, EnergyData[0],EnergyData[2],SigError[0],SigError[1]);
         histBurst[0]->Add(histBurst[1]);

         sprintf(name,"%s/%s_FRONT_BACK_sig_%.0f_%.0f.root",GRB_DIR,Est[0]->DataClassName_noConv.c_str(),Energy_Min_user,Energy_Max_user);
         TFile *fSignew = new TFile(name,"RECREATE");
         histBurst[0]->Write();
         hROIEff->Write();
         fSignew->Close();

    }

    //TEXT OUTPUT
    if (Combine) sprintf(name,"%s/%s_FRONT_BACK_Results_%s.txt",GRB_DIR,Est[0]->DataClassName_noConv.c_str(),GRB_NAME.c_str());
    else         sprintf(name,"%s/%s_Results_%s.txt",GRB_DIR,Est[0]->DataClass.c_str(),GRB_NAME.c_str());
    FILE * fdata = fopen(name,"w");
    fprintf(fdata,"\n%3s %7s %7s %7s %9s %7s %7s %9s %20s\n","Bin","Emin","Emax","ROI","bkg/bin","Sig/bin","Int.Bkg.","Int.Sig.","Exposure (cm2*sec)");
    double sumback=0,sumsig=0;

    for (int ie=1;ie<=Energy_Bins_user;ie++) {
       float emin=pow(10,hEst[0]->GetXaxis()->GetBinLowEdge(ie));
       float emax=pow(10,hEst[0]->GetXaxis()->GetBinUpEdge(ie));
       sumback+=hEst[0]->GetBinContent(ie);
       sumsig+=histBurst[0]->GetBinContent(ie);
       fprintf(fdata,"%3d %7.0f %7.0f %7.1f %9.5e %7.0f %7.5e %9.0f %20.1f\n",ie,emin,emax,hROIEff->GetBinContent(ie),hEst[0]->GetBinContent(ie), histBurst[0]->GetBinContent(ie),sumback,sumsig,hExp[0]->GetBinContent(ie));
    }

    fprintf(fdata,"\n%5s %20s %22s %9s %35s\n","a","Exposure (cm2*sec)", "Ave. energy in MeV","in ergs","AveEnergy/AveExposure (erg/cm2/sec)");
    for (float a=-2.4;a<=-1.79;a+=0.2) {
        double AveE=TOOLS::CalcMeanEnergy(Energy_Min_user,Energy_Max_user,a);
        double AveExp=TOOLS::CalcSpectrallyWeightedExposure(hExp[0],a);
        if (AveExp<0) {
           printf("%s: Results are wrong! Wrong exposure.\n",__FUNCTION__);
           fprintf(fdata,"WRONG!!! ");
        }
        const double MeV2erg=1.60217646e-6;
        fprintf(fdata,"%5.1f %20.2e %22.2f %9.2e %35.2e\n",a,AveExp,AveE,AveE*MeV2erg,AveE*MeV2erg/AveExp);
    }
    fprintf(fdata,"Spectral index a is for dN/dE \\propto E^(a)\n");
    fclose(fdata);


    gBurst->SetMarkerStyle(4);
    //////////////////////////////////////////////////////////////

    float max=histBurst[0]->GetMaximum();
    if (hEst[0]->GetMaximum()>max) max=hEst[0]->GetMaximum();

    float min=histBurst[0]->GetMinimum(0);
    if (hEst[0]->GetMinimum()<min) min=hEst[0]->GetMinimum();
    hEst[0]->GetYaxis()->SetRangeUser(min/5,max*5);

    for (int i=0;i<=hEst[0]->GetNbinsX();i++) {
       float est=hEst[0]->GetBinContent(i);
       hEst[0]->SetBinError(i,error*est);
    }

    pad[0]->cd();
    hEst[0]->Draw("E");
    gBurst->Draw("PSAME");

    TLegend * l = new TLegend(0.6,0.87,0.94,0.93);
    l->AddEntry(gBurst,"Burst events","p");
    l->AddEntry(hEst[0],"Estimated background","l");
    l->SetFillColor(0);
    l->Draw();

    pad[1]->cd(); hROIEst[0]->Draw();
    if (Combine) {
        TLegend lROI = TLegend(0.75,0.88,0.89,0.99);
        hROIEst[1]->SetLineColor(2);
        hROIEst[1]->Draw("SAME");
        lROI.AddEntry(hROIEst[0],"Front","l");
        lROI.AddEntry(hROIEst[1],"Back","l");
        lROI.Draw();
    }

    c->cd();

     TPaveText * ptext = new TPaveText(0.5,0.65,0.975,1);
     ptext->SetFillColor(0);

     ptext->AddText(GRB_NAME.c_str());
     sprintf(name,"MET : %.2fs, Duration: %.2fs",MET,DURATION);
     ptext->AddText(name);

     sprintf(name,"RA/DEC    : (%.3f,%.3f)deg, error: %.1edeg",RA,DEC,ROI_LOCALIZATIONERROR);
     ptext->AddText(name);


    /////////////////////////////////////////////////////////////////////////
    //Calculate probs and print out some data
     double TotalEstimate = hEst[0]->Integral();
     double TotalSignal   = histBurst[0]->Integral();
     double P = ROOT::Math::poisson_cdf_c((int)TotalSignal,TotalEstimate) + ROOT::Math::poisson_pdf((int)TotalSignal,TotalEstimate);
     double PwithBkgUncertainty = TOOLS::WeightedP((int)TotalSignal,TotalEstimate,error*TotalEstimate);
     sprintf(name,"%.1f>E>%.0eMeV. Signal: %.0f ev., Bkg:%.2e ev. Prob:%.2e (%.1f#sigma)",Energy_Min_user,Energy_Max_user,TotalSignal,TotalEstimate,P,ROOT::Math::gaussian_quantile_c(P,1));
     ptext->AddText(name);
     sprintf(name,"Probability with %.0f %% bkg uncertainty:%.1e (%.1f#sigma)",100*error,PwithBkgUncertainty,ROOT::Math::gaussian_quantile_c(PwithBkgUncertainty,1));
     ptext->AddText(name);
    /////////////////////////////////////////////////////////////////////////

     ptext->AddText("Bkg estimate includes both CR and gammas");
     sprintf(name,"Estimator v.%s, data files v.%s",
             ((TNamed*)fEst[0]->Get("Estimator_Version"))->GetTitle(),((TNamed*)fEst[0]->Get("DataFiles_Version"))->GetTitle());
     ptext->AddText(name);

     ptext->SetTextSize(0.03);
     ptext->SetTextAlign(12);
     ptext->Draw();

     sprintf(name,"%s %s",GRB_NAME.c_str(),Interval_name.c_str());
     c->SetTitle(name);

    //animated gif
    if (MakeAnimation) {
       c->Print("%animation.gif+");
    }

    //Delete bulky files

    int result;
//    sprintf(name,"rm %s/burst_ltCube.fits 2>/dev/null",GRB_DIR);    result=system(name);
//    sprintf(name,"rm %s/Burst_Exposure_user.fits 2>/dev/null",GRB_DIR);    result=system(name);
//    sprintf(name,"rm %s/*_burst_exposure.fits 2>/dev/null",GRB_DIR);    result=system(name);
    c->Update();

    if (Combine)  sprintf(name,"%s/%s-%s_FRONT+BACK_Results_%.1f_%.1f.png",GRB_DIR,GRB_NAME.c_str(),Est[0]->DataClass.c_str(),MET,DURATION);
    else          sprintf(name,"%s/%s-%s_Results_%.1f_%.1f.png",GRB_DIR,GRB_NAME.c_str(),Est[0]->DataClass.c_str(),MET,DURATION);
    FILE * ff = fopen(name,"r");

    if (ff && !OverwritePlots && !OverwriteResults) {fclose(ff); return ResultsFilename;}
    c->SaveAs(name);

    
    TFile * fout = new TFile(ResultsFilename,"RECREATE");
    c->Write();
    
    //Save some info for gtgrb 
    sprintf(name,"%.0f",TotalSignal);
    TNamed Data = TNamed("BKGE_NDET",name);Data.Write();
    sprintf(name,"%.3e",TotalEstimate);
    Data = TNamed("BKGE_NEXP",name);Data.Write();
    sprintf(name,"%.2f",ROOT::Math::gaussian_quantile_c(P,1));
    Data = TNamed("BKGE_SIGNIF",name);Data.Write();
    sprintf(name,"%.2f",ROOT::Math::gaussian_quantile_c(PwithBkgUncertainty,1));
    Data = TNamed("BKGE_SIGNIF_WITH_UNCERTAINTY",name);Data.Write();
    
    
    fout->Close();

 //Cleanup
 for (int iEst=0;iEst<EstMax;iEst++) {
     if (fEst[iEst]) fEst[iEst]->Close();
     if (fSig[iEst]) fSig[iEst]->Close();
     if (Est[iEst]) delete Est[iEst];
 }
 if (!WasBatch) gROOT->SetBatch(kFALSE);
 
 first=false;
 return ResultsFilename;
}

