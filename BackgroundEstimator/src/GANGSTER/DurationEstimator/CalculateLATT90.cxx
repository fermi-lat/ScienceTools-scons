//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/DurationEstimator.h"
#include "Math/QuantFuncMathCore.h"
#include "Math/PdfFuncMathCore.h"


//returns >=0 if ok, <0 if crap

DurationEstimator::DurationEstimator(string aFT1_FILE, string aFT2_FILE, string aDATACLASS, double aGRB_TRIGGER_TIME, string aGRB_NAME, double Emin, double Emax, int averbosity, bool aWeighByExposure, bool aJumpGTIs, bool aOverwrite):
FailedFraction(-1),verbosity(averbosity),WeighByExposure(aWeighByExposure),JumpGTIs(aJumpGTIs),Overwrite(aOverwrite), FT1ZenithTheta_Cut(95),
FT1_FILE(aFT1_FILE),FT2_FILE(aFT2_FILE),DATACLASS(aDATACLASS),GRB_NAME(aGRB_NAME),
MIN_ENERGY(Emin), MAX_ENERGY(Emax), GRB_TRIGGER_TIME(aGRB_TRIGGER_TIME) {

 if (MIN_ENERGY<=0) {printf("%s: MIN_ENERGY not set. Using 50MeV as MIN_ENERGY.\n",__FUNCTION__); MIN_ENERGY=50;}
 if (MAX_ENERGY<=0) {printf("%s: MAX_ENERGY not set. Using 50MeV as MIN_ENERGY.\n",__FUNCTION__); MIN_ENERGY=300000;}
 Iterations=50000;
 
 //Recycle canvases from previous run of the code
 cCoarse    = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cDuration_Coarse");
 if (cCoarse) delete cCoarse;
 cExtras    = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cExtras");
 if (cExtras) delete cExtras;
 cDuration1 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("cDurations");
 if (cDuration1) delete cDuration1;

 cCoarse = new TCanvas("cDuration_Coarse",GRB_NAME.c_str(),1024,768);
 cCoarse->Divide(2,2);
 cExtras = new TCanvas("cExtras",GRB_NAME.c_str(),1024,768);
 cExtras->Divide(2,2);
 cDuration1 = new TCanvas("cDurations",GRB_NAME.c_str(),1024,768);
 cDuration1->Divide(2,3);
 
 sprintf(ResultsRootFilename,"%s/%s_%s_%.0f_%.0f_%d_T90.root",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
 
}
  

int DurationEstimator::CalculateLATT90(){

 bool Combine=false;
 const string DATACLASSName=DATACLASS.substr(0,DATACLASS.find("::"));
 if (DATACLASS.find("FRONT+BACK")!=string::npos)  Combine=true;
 if (Combine==true) {printf("%s: This method doesn't work with FRONT+BACK yet\n",__FUNCTION__); return -1;}

 char name[1000];
 
 if (!Overwrite) {
   FILE * ftemp = fopen(ResultsRootFilename,"r");
   if (ftemp) {
     fclose(ftemp); 
     printf("%s: Duration already estimated.. see results in file %s. To recalculate the duration (will not rerun the bkge) pass an overwrite=True parameter to the function.\n",__FUNCTION__,ResultsRootFilename);
     return 0;
   }
 }

 vector <TH1F*> hROI;
 vector <double> TSTART,TSTOP;
 vector <TFile*> fBkg;
 float GTI_Offset_0;
 int status = EvaluateBins_Coarse(hROI,TSTART,TSTOP,fBkg,GTI_Offset_0);
 
 printf("%s: Results will be saved in file %s\n",__FUNCTION__,ResultsRootFilename);
 sprintf(name,"mkdir %s",TOOLS::GetS("OUTPUT_DIR").c_str());
 system(name);
 
 TFile * fResults = new TFile(ResultsRootFilename,"RECREATE");
 if (status>=0) {
    EvaluateBins_Fine(hROI,TSTART,TSTOP); //calculate fine lightcurve
 
 
   cCoarse->cd(1);
   gIntDet_fine->Draw("PSAME");
   gIntBkg_fine->Draw("PSAME");

   cCoarse->cd(4);
   gIntDiff_fine->Draw("PSAME");

   cExtras->cd(4);
   gFluence_fine->Draw("AP");

   cCoarse->Update();
   cExtras->Update();

    //2. Find durations on counted space with errors
    double q_T95[3]={-1,-1,-1},q_T05[3]={-1,-1,-1}, q_T90[3]={-1,-1,-1},q_Plateau[3]={-1,-1,-1}, q_DetTotal[3]={-1,-1,-1};
     if (GTI_Offset_0==0) {
       if (PerformPerturbedEstimation(q_T95,q_T05, q_T90,q_Plateau, q_DetTotal, Iterations)<0) {
          status=-6;
       }
    
       if (status>=0) { //all is well
          //Plot plateaus and T95s over the detected curve for verification reasons
           
           TPaveText * ptext = new TPaveText(0.1,0.1,0.9,0.9);
           ptext->SetFillColor(0);
           ptext->SetTextAlign(12);
           ptext->AddText(GRB_NAME.c_str());
           sprintf(name,"Energy Range       : %.1f--%.1f MeV",MIN_ENERGY,MAX_ENERGY);  ptext->AddText(name);
           sprintf(name,"WeighByExposure    : %d",WeighByExposure);   ptext->AddText(name);
           sprintf(name,"---Results (-1#sigma/median/+1#sigma)--- ");   ptext->AddText(name);
           sprintf(name,"T05 :%.2f/%.2f/%.2fs",q_T05[0],q_T05[1],q_T05[2]);   ptext->AddText(name);
           sprintf(name,"T95 :%.2f/%.2f/%.2fs",q_T95[0],q_T95[1],q_T95[2]);   ptext->AddText(name);
           sprintf(name,"T90 :%.2f/%.2f/%.2fs",q_T90[0],q_T90[1],q_T90[2]);   ptext->AddText(name);
           sprintf(name,"TotalDetected   :%.2f/%.2f/%.2f",q_DetTotal[0],q_DetTotal[1],q_DetTotal[2]);   ptext->AddText(name);
           sprintf(name,"Fraction Failed :%.2f",FailedFraction);   ptext->AddText(name);
           if (FailedFraction>0.2) { sprintf(name,"Fraction Failed high. Emission likely continues to next GTI.");   ptext->AddText(name);}
           cCoarse->cd(3); ptext->Draw();

           TLine *l;
           //for (int i=1;i<=6;i++) cDuration1->GetPad(i)->SetLogy();
           cDuration1->cd(1); hT95->Draw(); for (int i=0;i<3;i++) { l = new TLine(q_T95[i],0,q_T95[i],hT95->GetMaximum());Plot(l,2);}
           cDuration1->cd(2); hT05->Draw(); for (int i=0;i<3;i++) {l = new TLine(q_T05[i],0,q_T05[i],hT05->GetMaximum()); Plot(l,2);}
           cDuration1->cd(3); hT90->Draw(); for (int i=0;i<3;i++) {l = new TLine(q_T90[i],0,q_T90[i],hT90->GetMaximum()); Plot(l,2);}
           cDuration1->cd(5); hPlateau->Draw(); for (int i=0;i<3;i++) {l = new TLine(q_Plateau[i],0,q_Plateau[i],hPlateau->GetMaximum()); Plot(l,2);}
           cDuration1->cd(6); hDetTotal->Draw(); for (int i=0;i<3;i++) {l = new TLine(q_DetTotal[i],0,q_DetTotal[i],hDetTotal->GetMaximum()); Plot(l,2);}
           /////////////////////////////////       SAVE RESULTS     //////////////////////////////////////////////////////////////////
        }
    } //if crossed GTI
    else { //simple calculation
	double Plateau_stop,Plateau_start=-2.5; //here ask for an increased significance since the code makes many trials
        bool Found_Plateau=FindPlateau(gIntDiff_fine,gIntDet_fine,gIntBkg_fine,Plateau_start,Plateau_stop,300.,GTI_Offset_0,false);
        if (!Found_Plateau) {
           status=-7;
        }
    	else {
	    q_Plateau[1]=gIntDiff_fine->Eval(Plateau_start);    	    
    	    if (FindT90(gIntDiff_fine, q_T05[1], q_T95[1], q_Plateau[1])<0) status=-8;
    	    q_T90[1]=q_T95[1]-q_T05[1];
    	    q_T90[0]=q_T05[0]=q_T95[0]=0;
    	    q_T90[2]=q_T05[2]=q_T95[2]=0;
    	    q_Plateau[0]=q_Plateau[2]=0;
    	}
    }    
    if (status>=0) {
       if (WeighByExposure) cExtras->cd(4);
       else                 cCoarse->cd(4);
       TLine *l;
       for (int ii=0;ii<3;ii++) {
           l = new TLine(0,q_Plateau[ii],TSTOP.back(),q_Plateau[ii]);  Plot(l,2);
	   l = new TLine(q_T90[ii],0,q_T90[ii],q_Plateau[ii]);          Plot(l,2);
       }
       
       cDuration1->Update(); 
       for (unsigned int i=0;i<fBkg.size();i++) {fBkg[i]->Close();} //close files
       sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90.png",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
       cDuration1->SaveAs(name);
       sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90.eps",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
       cDuration1->SaveAs(name);
            
       fResults->cd();
       cDuration1->Write();
       sprintf(name,"%e_%e_%e",q_T05[0],q_T95[0],q_T90[0]);TNamed Data = TNamed("f_LL_T05_T95_T90",name); Data.Write();
       sprintf(name,"%e_%e_%e",q_T05[1],q_T95[1],q_T90[1]);Data = TNamed("f_MED_T05_T95_T90",name); Data.Write();
       sprintf(name,"%e_%e_%e",q_T05[2],q_T95[2],q_T90[2]);Data = TNamed("f_UL_T05_T95_T90",name); Data.Write(); 
       
       if   (WeighByExposure) printf("%s: Results using exposure weighting.\n",__FUNCTION__);
       else                   printf("%s: Results without using exposure weighting.\n",__FUNCTION__);
       printf("%s: T05: %f - %f - %f\n",__FUNCTION__,q_T05[0],q_T05[1],q_T05[2]);
       printf("%s: T95: %f - %f - %f\n",__FUNCTION__,q_T95[0],q_T95[1],q_T95[2]);
       printf("%s: T90: %f - %f - %f\n",__FUNCTION__,q_T90[0],q_T90[1],q_T90[2]);
       
    }
  }
  
  if (status<0) {
        fResults->cd();  
        TNamed Data = TNamed("Failed",""); Data.Write();

        if      (status==-2) {Data = TNamed("ErrorCode","not_enough_events"); Data.Write();}
        else if (status==-3) {Data = TNamed("ErrorCode","burst_not_significant"); Data.Write();}
        else if (status==-4) {Data = TNamed("ErrorCode","burst_out_of_LAT_FOV_at_t_trig"); Data.Write();}
        else if (status==-1) {Data = TNamed("ErrorCode","general_bkg_estimation_error"); Data.Write();}
        else if (status==-5) {Data = TNamed("ErrorCode","burst stopped being observable and JumpGTI=false"); Data.Write();}
        else if (status==-6) {Data = TNamed("ErrorCode","too_many_failed_sims"); Data.Write();}
        else if (status==-7) {Data = TNamed("ErrorCode","Could not find Plateau"); Data.Write();}
        else if (status==-8) {Data = TNamed("ErrorCode","Could not estimate duration"); Data.Write();}
       
        TPaveText * ptext = new TPaveText(0.1,0.1,0.9,0.9);
        ptext->SetFillColor(0);
        ptext->SetTextAlign(12);
        ptext->AddText(GRB_NAME.c_str());
        sprintf(name,"Energy Range       : %.1f--%.1f MeV",MIN_ENERGY,MAX_ENERGY);  ptext->AddText(name);
        sprintf(name,"WeighByExposure    : %d",WeighByExposure);   ptext->AddText(name);
        sprintf(name,"%s",Data.GetTitle());   ptext->AddText(name);
        if (status==-6) { sprintf(name,"Emission likely continues to next GTI"); ptext->AddText(name);}
        cCoarse->cd(3); ptext->Draw();  
  }
  
  cCoarse->Update();  cCoarse->Write();
  cExtras->Update();  cExtras->Write();
 
  sprintf(name,"%.3f",FailedFraction); TNamed Data = TNamed("FailedFraction",name); Data.Write();
  
  if (FailedFraction>0) {
     //save latest processed time
     sprintf(name,"%f",gIntBkg_fine->GetX()[gIntBkg_fine->GetN()-1]);
     Data = TNamed("ProcessedUntilTime",name); Data.Write();
  }
    
  sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90_2.png",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
  cCoarse->SaveAs(name);
  sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90_2.eps",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
  cCoarse->SaveAs(name);
  sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90_Extras.png",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
  cExtras->SaveAs(name);
  sprintf(name,"%s/%s_%s_%.0f_%.0f_%d_T90_Extras.eps",(TOOLS::GetS("OUTPUT_DIR")).c_str(),DATACLASS.c_str(),GRB_NAME.c_str(),MIN_ENERGY,MAX_ENERGY,WeighByExposure);
  cExtras->SaveAs(name);
  
  fResults->Close();
  return status;
}

//Ok this function receives more data than it uses, but I keep it like that for possible future changes in the Plateau-finding algorithm (VV)
bool DurationEstimator::FindPlateau(TGraph* gIntDiff, TGraph* gIntDet, TGraphErrors *gIntBkg, double &Plateau_start, double & Plateau_stop, const float min_plateau_duration, float GTI_Offset_0, bool quick) {
 double SigmaLim=2.0;
 if (Plateau_start<0) SigmaLim=-Plateau_start;

 #ifdef DRAW
 TLine l;
 cCoarse->cd(2);
 gIntDet->Draw("AL*");
 gIntBkg->SetMarkerColor(2);
 gIntBkg->SetLineColor(2);
 gIntBkg->Draw("LSAME*");
   
 cCoarse->cd(1);
 gIntDiff->Draw("AL*");
 #endif 
 
 int points = gIntDiff->GetN();
 double* x = gIntDiff->GetX();
 bool found_plateau=false;
 const double graph_stop=x[points-1];

 if (min_plateau_duration>(graph_stop-GTI_Offset_0)){
    //printf("%s:min plateau duration greater than graph_stop %f %f\n",__FUNCTION__,min_plateau_duration,graph_stop); 
    return false;
 }
 

 const float time_step=std::max((graph_stop-GTI_Offset_0)/20.,20.);
 for (Plateau_start=GTI_Offset_0; Plateau_start<graph_stop-min_plateau_duration && !found_plateau;Plateau_start+=time_step) {

    double level_start=gIntDiff->Eval(Plateau_start);
    #ifdef DRAW
    l = TLine(Plateau_start,level_start,graph_stop,level_start);l.SetLineWidth(2);l.SetLineColor(2);l.Draw("SAME");
    cCoarse->Update();
    #endif

    double Det_start=gIntDet->Eval(Plateau_start);
    double Bkg_start=gIntBkg->Eval(Plateau_start);
    bool PlateauOK=true;
    for (float apoint=Plateau_start;apoint<graph_stop && PlateauOK;apoint+=std::min(graph_stop/50.,30.)) { //find the highest point of the diff plateau in the plateau
        if (gIntDiff->Eval(apoint)<level_start || apoint==Plateau_start) continue;
        double Det_stop =gIntDet->Eval(apoint);
        double Bkg_stop =gIntBkg->Eval(apoint);
        float Significance,aBkg;
	/*Here I calculate some "Significance" of the fluctuations throught the analysed interval.
	 We are in the Poisson regime but the calls to the ROOT::Math::poisson_cdf_c functions are slow and if I used them to 
	 estimate the true significances the code would take a long time. I instead calculate a quick Gaussian-regime significance as an approximation
	 of the true significance.
	*/
        int aSignal=int(Det_stop-Det_start+0.5);
	aBkg        =Bkg_stop-Bkg_start;
	if (quick) {
            Significance=(aSignal-aBkg)/sqrt(aBkg);
        }
	else {
	    Significance=ROOT::Math::gaussian_quantile_c(ROOT::Math::poisson_cdf_c(aSignal,aBkg)+ROOT::Math::poisson_pdf(aSignal,aBkg),1);
        }
        #ifdef DRAW
        printf("Check time %f (p.s.=%f) signal=%d bkg=%.1f (%.1f-%.1f) (%.1f-%.1f) sig=%.2f min_plateau_dur=%f\n",apoint,Plateau_start,aSignal,aBkg,Det_stop,Det_start,Bkg_stop,Bkg_start,Significance,min_plateau_duration);
        #endif

        if (Significance>SigmaLim) PlateauOK=false;
     }
     if (!PlateauOK) continue; 
     found_plateau=true;
     #ifdef DRAW
     printf("FOUND!\n");
     #endif
 }

 #ifdef DRAW
 if (found_plateau) {
    l = TLine(Plateau_start,gIntDiff->Eval(Plateau_start),graph_stop,gIntDiff->Eval(graph_stop));l.SetLineWidth(2);l.SetLineColor(2);l.Draw("SAME");
 }
 else printf("NOT FOUND\n");
 cCoarse->Update();
 printf("press\n"); 
 getchar();
 #endif 
 
 return found_plateau;
}

//////////////////////////////////////////////////////////////////////////

int DurationEstimator::FindT90(TGraph * gIntDiff, double &T05, double &T95, double Plateau) {

 float max_Time=gIntDiff->GetXaxis()->GetXmax();
 T05=0;
 while (gIntDiff->Eval(T05)/Plateau<0.05) {
    T05+=1;
    if (T05>max_Time) {printf("%s: Problem finding T05, reached max time! \n",__FUNCTION__); return -1;}
 }
 while (gIntDiff->Eval(T05)/Plateau>0.05) {
    T05-=0.1;
    if (T05<=0) {T05=0.1;break;}
 }

 T95=T05; 
 float min_Time=gIntDiff->GetXaxis()->GetXmin();
 while (gIntDiff->Eval(T95)/Plateau<0.95)  {
    if (T95<min_Time) {printf("%s: Problem finding T95, reached min time \n",__FUNCTION__); return -1;}
    T95+=1;
 }

 while (gIntDiff->Eval(T95)/Plateau>0.95) {
    if (T95<=0) { printf("T95 underflow\n");return -1;}
    T95-=0.1;
 }

 return 0;
}

void DurationEstimator::Plot(TLine * l, int color) {
  l->SetLineStyle(2);
  l->SetLineColor(color);
  l->SetLineWidth(2);
  l->Draw();
}

    

////////////////////////////////////////////////////////////////////////////////////////////


