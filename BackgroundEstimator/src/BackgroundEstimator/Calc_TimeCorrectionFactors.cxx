//Author: Vlasios Vasileiou <vlasisva@gmail.com>
//$Header$

#include "BackgroundEstimator/BackgroundEstimator.h"
#include <algorithm>
#include "TProfile.h"
#include "TLine.h"

void BKGE_NS::Calc_TimeCorrectionFactors(vector<string> GRB_folders, vector <double> METs, vector <double> GRB_L, vector <double> GRB_B, string Dataclass, double MinE, double MaxE, int NBins, const int Max_iE_For_Correction){


 char name[1000],name2[1000];

 const double MinMET=*min_element(METs.begin(),METs.end());
 const double MaxMET=*max_element(METs.begin(),METs.end());

 const double lMinE = log10(MinE);
 const double lMaxE = log10(MaxE);
 const double dlMinE = (lMaxE-lMinE)/NBins;

 sprintf(name,"Ratio distribution %.0fMeV<E<%.0fGeV",MinE,MaxE/1000.);
 TH1F * hRatioDist = new TH1F("hRatioDist",name,NBins,0.8,1.2);
 hRatioDist->GetXaxis()->SetTitle("Ratio");

 TH1F * hSigError = new TH1F("hSigError","Statistical error on the number of real events",NBins,lMinE,lMaxE);
 hSigError->GetYaxis()->SetTitle("Error");
 hSigError->GetXaxis()->SetTitle("log_{10}(Energy/MeV)");
 
 TH2F * hRatio2vsE[NBins],*hRatiovsSig[NBins],*hRatio2vsE_Norm[NBins],*hSigvsBack[NBins];
 TH1F *hRatioDistvsE[NBins];
 TProfile * hRatiovsTime[NBins];
 TH2F * hRatiovsTimeMap[NBins];
 for (int i=0;i<NBins;i++) {
    sprintf(name,"RatioDist_%d",i);
    sprintf(name2,"Ratio distribution, %.0f<E<%.0f MeV",pow(10,lMinE+(i-1)*dlMinE),pow(10,lMinE+i*dlMinE));
    hRatioDistvsE[i] = new TH1F(name,name2,100,0,3);
    hRatioDistvsE[i]->GetXaxis()->SetTitle("Ratio Est/Measured");

    sprintf(name,"Ratio2vsE_%d",i);
    sprintf(name2,"Average Ratio vs L/B, %.0f<E<%.0f MeV",pow(10,lMinE+(i-1)*dlMinE),pow(10,lMinE+(i)*dlMinE));
    hRatio2vsE[i]= new TH2F(name,name2,20,-180,180,20,-90,90);
    hRatio2vsE[i]->GetXaxis()->SetTitle("L (deg)");
    hRatio2vsE[i]->GetYaxis()->SetTitle("B (deg)");
    hRatio2vsE[i]->SetContour(200);

    sprintf(name,"Ratio2vsE_Norm__%d",i);
    hRatio2vsE_Norm[i]= (TH2F*)hRatio2vsE[i]->Clone(name);

    sprintf(name,"RatiovsSig_%d",i);
    sprintf(name2,"Ratio vs Measured Signal, %.0f<E<%.0f MeV",pow(10,lMinE+(i-1)*dlMinE),pow(10,lMinE+(i)*dlMinE));
    hRatiovsSig[i]= new TH2F(name,name2,50,0,log10(5000),30,1/2.,2);
    hRatiovsSig[i]->GetXaxis()->SetTitle("log_{10}(Signal)");
    hRatiovsSig[i]->GetYaxis()->SetTitle("Ratio Est/Real");
    hRatiovsSig[i]->SetContour(200);

    sprintf(name,"SigvsBack_%d",i);
    sprintf(name2,"Measured Signal vs Estimated Background, %.0f<E<%.0f MeV",pow(10,lMinE+(i-1)*dlMinE),pow(10,lMinE+(i)*dlMinE));
    hSigvsBack[i]= new TH2F(name,name2,30,0,log10(5000),30,0,log10(5000));
    hSigvsBack[i]->GetXaxis()->SetTitle("log_{10}(Measured Signal)");
    hSigvsBack[i]->GetYaxis()->SetTitle("log_{10}(Estimated Background)");
    hSigvsBack[i]->SetContour(200);

    sprintf(name2,"Ratio vs Time, %.0f<E<%.0f MeV",pow(10,lMinE+(i-1)*dlMinE),pow(10,lMinE+(i)*dlMinE));
    
    hRatiovsTime[i]= NULL;
    sprintf(name,"RatiovsTimeMap_%d",i);
    hRatiovsTimeMap[i]= new TH2F(name,name2,25,MinMET,MaxMET,50,0.7,1.4);
 }

 
 double Sig[NBins][METs.size()],AveR[NBins],Back[NBins][METs.size()];
 float ratio[NBins][METs.size()];

 //#DEFINE DEBUG
 #ifdef DEBUG
 TCanvas * ccheck = new TCanvas("ccheck","ccheck"); 
 #endif

 for (unsigned int iSig=0;iSig<METs.size();iSig++) {
 
      sprintf(name,"%s/%s_bkg_%.0f_%.0f.root",GRB_folders[iSig].c_str(),Dataclass.c_str(),MinE,MaxE);
      TFile * fEst = TFile::Open(name);
      if (!fEst) { printf("no %s\n",name);  continue;    }
      TH1F * hEst = (TH1F*)fEst->Get("hCtsvsEnergy_Est");
      if (!hEst){printf("no hBurst in %s\n",name);  fEst->Close(); continue;}

      sprintf(name,"%s/%s_sig_%.0f_%.0f.root",GRB_folders[iSig].c_str(),Dataclass.c_str(),MinE,MaxE);
      TFile * fBurst = TFile::Open(name);
      if (!fBurst) { printf("no %s\n",name);     continue;    }
      TH1F * hBurst = (TH1F*)fBurst->Get("hCtsvsEnergy_Burst");
      if (!hBurst){printf("no hBurst in %s\n",name);  fBurst->Close(); continue;}
      if (GRB_L[iSig]>180) GRB_L[iSig]-=360;
      
      for (int iE=0;iE<NBins;iE++) {
          Back[iE][iSig]=hEst->GetBinContent(iE+1);
          Sig[iE][iSig] =hBurst->GetBinContent(iE+1);
          ratio[iE][iSig]=0;
          if (Sig[iE][iSig]) ratio[iE][iSig] = Back[iE][iSig]/Sig[iE][iSig];
          if (Sig[iE][iSig]<20 || ratio[iE][iSig]<0) continue;

          hRatio2vsE[iE]->Fill(GRB_L[iSig],GRB_B[iSig],ratio[iE][iSig]);
          hRatio2vsE_Norm[iE]->Fill(GRB_L[iSig],GRB_B[iSig]);
          //printf("%d\t %d\t %lf\t %d\t %d\t %f\n",iSig,iE,MET,hEst->GetBinContent(iE),Sig[iE][iSig],ratio[iE][iSig]);
          #ifdef DEBUG
          if (ratio>1.5 && iE<8 && Sig[iE][iSig]>20) { 
               ccheck->cd();
               hBurst->SetMaximum(1.5*hEst->GetMaximum());
                hBurst->Draw();
               hEst->SetLineColor(2);
               hEst->Draw("SAME");
               ccheck->Update();
                 getchar();    
          }
          #endif

          hRatioDistvsE[iE]->Fill(ratio[iE][iSig]);
          //printf("%f %f %f\n",ratio[iE][iSig],Back[iE][iSig],Sig[iE][iSig]);
          hRatiovsSig[iE]->Fill(log10(Sig[iE][iSig]),ratio[iE][iSig]);
          hSigvsBack[iE]->Fill(log10(Sig[iE][iSig]),log10(Back[iE][iSig]));
//          hSigvsBack[iE]->Fill(Sig[iE][iSig],Back[iE][iSig]);
         // hRatiovsTime[iE]->Fill(METs[iSig],ratio[iE][iSig]);
          hRatiovsTimeMap[iE]->Fill(METs[iSig],ratio[iE][iSig]);
      }
      fBurst->Close();
      fEst->Close();
 }
 
 double ERROR[NBins];

 for (int iE=0;iE<NBins;iE++) {
    ERROR[iE-1]=0;
    AveR[iE]=hRatioDistvsE[iE]->GetMean();
//    ERROR[iE]=DecomposeError(iSig,Back[iE],Sig[iE],false);
    printf("%d %f\n",iE,ERROR[iE]);
 }

  TCanvas * cFinal = new TCanvas("cFinal","cFinal");
  cFinal->Divide(2,2);
  cFinal->cd(4); cFinal->GetPad(4)->SetGridx();hRatioDist->Draw();

  TH1F * hErrorvsE = new TH1F("hErrorvsE","Systematic error vs energy",NBins,lMinE,lMaxE);
  hErrorvsE->GetYaxis()->SetTitle("Width");
  hErrorvsE->GetXaxis()->SetTitle("log_{10}(Energy/MeV)");
  TH1F * hAvevsE = new TH1F("hAvevsE","Average ratio vs Energy",NBins,lMinE,lMaxE);
  hAvevsE->GetYaxis()->SetTitle("Error");
  hAvevsE->GetXaxis()->SetTitle("log_{10}(Energy/MeV)");

  hErrorvsE ->GetYaxis()->SetTitle("Error");
  hAvevsE ->GetYaxis()->SetTitle("Ratio Est/Measured");

  TCanvas * cEnergyRatio = new TCanvas("cEnergyRatio","cEnergyRatio");
  cEnergyRatio->Divide(5,4);

  TCanvas * cEnergyRatio2 = new TCanvas("cEnergyRatio2","cEnergyRatio2");
  cEnergyRatio2->Divide(5,4);

  TCanvas * cErrorvsSig = new TCanvas("cErrorvsSig","cErrorvsSig");
  cErrorvsSig->Divide(5,4);

  TCanvas * cSigvsBack = new TCanvas("cSigvsBack","Signal vs Background");
  cSigvsBack->Divide(5,4);

  TCanvas * cRatiovsTime = new TCanvas("cRatiovsTime","Ratio vs Time");
  cRatiovsTime->Divide(5,4);
  
  TCanvas * cRatiovsTimeMap = new TCanvas("cRatiovsTimeMap","Ratio vs Time");
  cRatiovsTimeMap->Divide(5,4);

  for (int iE=0;iE<NBins;iE++) {
  
     //Make hRatiovsTime Profiles
     int bins=50;
     sprintf(name,"RatiovsTime_%d",iE);
     sprintf(name2,"Ratio vs Time, %.0f<E<%.0f MeV",pow(10,lMinE+(iE)*dlMinE),pow(10,lMinE+(iE+1)*dlMinE));

     while (bins>=10) {
        if (hRatiovsTime[iE]) hRatiovsTime[iE]->Delete();
        hRatiovsTime[iE]= new TProfile(name,name2,bins,MinMET,MaxMET);
        for (unsigned int iSig=0;iSig<METs.size();iSig++) {
             if (ratio[iE][iSig]>0) hRatiovsTime[iE]->Fill(METs[iSig],ratio[iE][iSig]);
        }         
        bool hist_ok=true;
        for (int ibin=1;ibin<=bins;ibin++) {
            if (hRatiovsTime[iE]->GetBinError(ibin)>0.03) {
               hist_ok=false;
               break;
            }
        }
        if (hist_ok==true){
            break;
        }
        bins--;
     }
  
  
     cEnergyRatio->cd(iE+1);
     hRatioDistvsE[iE]->Draw();
     TLine * l1 = new TLine(1,hRatioDistvsE[iE]->GetMinimum(),1,hRatioDistvsE[iE]->GetMaximum()*1.05);  
     l1->SetLineColor(2);
     l1->Draw();

     cEnergyRatio2->cd(iE+1);
     hRatio2vsE[iE]->Divide(hRatio2vsE_Norm[iE]);
     hRatio2vsE[iE]->GetZaxis()->SetRangeUser(0.5,1.5);
     hRatio2vsE[iE]->Draw("COLZ");

     
     cSigvsBack->cd(iE+1);
     l1 = new TLine(hSigvsBack[iE]->GetXaxis()->GetXmin(),hSigvsBack[iE]->GetYaxis()->GetXmin(),
                           hSigvsBack[iE]->GetXaxis()->GetXmax(),hSigvsBack[iE]->GetYaxis()->GetXmax());
     hSigvsBack[iE]->Draw("COLZ");
     l1->Draw("SAME");

     cErrorvsSig->cd(iE+1);

     l1 = new TLine(hRatiovsSig[iE]->GetXaxis()->GetXmin(),1,hRatiovsSig[iE]->GetXaxis()->GetXmax(),1);     
     hRatiovsSig[iE]->Draw("COLZ");
     l1->Draw("SAME");

     hErrorvsE->SetBinContent(iE,ERROR[iE]);
     hAvevsE->SetBinContent(iE,AveR[iE]);
     hAvevsE->SetBinError(iE,hRatioDistvsE[iE]->GetRMS());

     cRatiovsTime->cd(iE+1);
     l1 = new TLine(MinMET,1,MaxMET,1);
     hRatiovsTime[iE]->GetYaxis()->SetRangeUser(0.7,1.3);
     hRatiovsTime[iE]->Draw();
     l1->Draw("SAME");

     cRatiovsTimeMap->cd(iE+1);
     hRatiovsTimeMap[iE]->SetContour(64);
     hRatiovsTimeMap[iE]->Draw("COLZ");

  }

 // hErrorvsE->SetBinContent(NBins,-1);

  cEnergyRatio->cd(NBins+1); hSigError->Draw();
  cFinal->cd(1); 
    cFinal->GetPad(1)->SetGridy();
    hErrorvsE->SetMinimum(0);
    hErrorvsE->SetMaximum(1);
    hErrorvsE->Draw("P");
  cFinal->cd(2); 
    hAvevsE->SetMinimum(0.5);
    hAvevsE->SetMaximum(1.5);
    TLine * l1 = new TLine(hAvevsE->GetXaxis()->GetXmin(),1,hAvevsE->GetXaxis()->GetXmax(),1);     
    hAvevsE->Draw("");
    l1->SetLineWidth(2);
    l1->SetLineColor(2);
    l1->Draw("SAME"); 
    hAvevsE->Draw("SAME");
    //Save Correction Factors
    sprintf(name,"CorrectionFactors_%s.root",Dataclass.c_str());
    TFile *fout = new TFile(name,"RECREATE");
    for (int iE=0;iE<=Max_iE_For_Correction;iE++) {
        hRatiovsTime[iE]->Write();
    }
    fout->Close();
    printf("Correction factors saved in file %s\n",name);
}


