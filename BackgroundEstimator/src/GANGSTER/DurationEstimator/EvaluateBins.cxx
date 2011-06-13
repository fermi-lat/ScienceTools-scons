//Author: Vlasios Vasileiou <vlasisva@gmail.com>
//$Header$
#include "BackgroundEstimator/DurationEstimator.h"

//returns 0 if ok, -1 if fail, -2/-3 if non-significant signal,-4 if started out of FOV, +5 if stopped because out of FOV
int DurationEstimator::EvaluateBins_Coarse(vector <TH1F*> &hROI, vector <double>& TSTART, vector <double> &TSTOP, vector <TFile*> fBkg, float & GTI_Offset_0) {

 GTI_Offset_0=0; //starting offset of current GTI
 //Setup
 gStyle->SetOptTitle(1);
 const float BKG_ESTIMATOR_ERROR=TOOLS::Get("BKG_ESTIMATE_ERROR");

 TRandom t;
 t.SetSeed(0);
 /////////////////////////////////////////////////////

 double offset_start=0,dt;
 vector <double> DT_HALF,TMIDDLE; 
 vector <double> IntDet,IntBkg,IntDiff,Flux,Effective_Area,Dets,Bkgs,Bkgs_Err,Flux_Err_Up,Flux_Err_Down;

 //TF1 * fPlateau = new TF1("fPlateau","pol1");
 bool Found_Plateau=false;
 bool IsSignificant=false;
 
 TFeldmanCousins fc = TFeldmanCousins(0.68);
 int status=0;
 double Plateau_level=0;
 
 while (!Found_Plateau) {
 
    //Decide dt
    ///for the beginning of the burst sample a bit more finer to have an accurate T05
    dt=std::max(3.,offset_start*0.5);
    dt=std::min(dt,120.);
    
    if (verbosity>3) printf("%s: offset_start=%f, dt=%f\n",__FUNCTION__,offset_start,dt);
    if (verbosity>1) printf("MET=%lf tstart=%f tstop=%f\n",GRB_TRIGGER_TIME+offset_start,offset_start,offset_start+dt);
 
    //Check if burst observable ok at the beginning of the interval
    double theta0,ztheta0,phi;
    TOOLS::GetThetaPhi(theta0,phi,ztheta0,GRB_TRIGGER_TIME+offset_start, FT2_FILE, -999,-999);
    if (theta0>75 || ztheta0>90 || theta0<0) {
        if (verbosity>0) printf ("%s: Burst is out of FOV (theta=%f) or occulted by the earth (ztheta=%f) or in SAA at time offset %f.",__FUNCTION__,theta0,ztheta0,offset_start);
	if (!JumpGTIs) {
             if (verbosity>0) printf(" Will stop the calculation at tstop=%f s. \n",offset_start);
             if (TSTART.size()==0) status=-4;
             else status=+5;
             break; //this breaks the main loop
	} 
	else {
             if (verbosity>0) printf (" Will fast forward the starting time..\n");        
             double offset_start_0=offset_start;
             
    	     do {
    	        offset_start+=100;
    	        TOOLS::GetThetaPhi(theta0, phi, ztheta0, GRB_TRIGGER_TIME+offset_start, FT2_FILE, -999,-999);
    	    	if (verbosity>2) {printf ("%5.2f %5.2f %5.2f    \n",offset_start,theta0,ztheta0); fflush(0);}
             } while (theta0>75 || ztheta0>90 || theta0<0); //fast forward until we get in GTI

    	     while (theta0<75 && ztheta0<90 && theta0>0) { //rewind until we exit GTI
    	        offset_start-=50;
    	        TOOLS::GetThetaPhi(theta0, phi, ztheta0, GRB_TRIGGER_TIME+offset_start, FT2_FILE, -999,-999);
    	    	if (verbosity>2) {printf ("%5.2f %5.2f %5.2f    \n",offset_start,theta0,ztheta0); fflush(0);}
	     }

             while (theta0>75 || ztheta0>90 || theta0<0) { //forward until we enter GTI
    	        offset_start+=2;
    	        TOOLS::GetThetaPhi(theta0,phi,ztheta0,GRB_TRIGGER_TIME+offset_start,FT2_FILE, -999,-999);
    	    	if (verbosity>2) {printf ("%5.2f %5.2f %5.2f    \n",offset_start,theta0,ztheta0); fflush(0);}
	     }
	     GTI_Offset_0=offset_start;
	     //fill out a blank interval
	     double Bad_dt=offset_start-offset_start_0;
	     TSTART.push_back(offset_start_0);
	     TSTOP.push_back(offset_start_0+Bad_dt);
	     TMIDDLE.push_back(offset_start_0+Bad_dt/2);
	     DT_HALF.push_back(0);
	     hROI.push_back(0);
	     Effective_Area.push_back(0);
    	     fBkg.push_back(0);
	     Dets.push_back(0);
	     Bkgs.push_back(0);
	     Bkgs_Err.push_back(0);
	     if (IntDet.size()==0) {
                 IntDet.push_back(0);
                 IntBkg.push_back(0);
                 IntDiff.push_back(0);
             }
             else {
                 IntDet.push_back(IntDet.back());
                 IntBkg.push_back(IntBkg.back());
                 IntDiff.push_back(IntDiff.back()); 
             }
	     Flux.push_back(0);
	     Flux_Err_Up.push_back(0);
	     Flux_Err_Down.push_back(0);
        }
    }
    
    //now check that the burst is in FOV for the whole time interval
    //if not then set dt to the point that it just crossed the boundaries. Proceed with the calculation. 
    //If JumpGTIs==false then at the next iteration, the starting-time-check code above should trigger and stop the calculation.
    for (float adt=1;adt<=dt;adt+=2) {
	double theta1,ztheta1,phi;	
        TOOLS::GetThetaPhi(theta1,phi,ztheta1,GRB_TRIGGER_TIME+offset_start+adt,FT2_FILE, -999,-999);
        if (theta1>75 || ztheta1>90 || theta1<0) {
            if (verbosity>1) printf("%s: Burst is out of FOV (theta_1=%f) or occulted by the earth (ztheta_1=%.0f) or LAT in SAA at offset=%f (offset_planned=%f).\n ",__FUNCTION__,theta1,ztheta1,offset_start+adt,offset_start+dt);
            dt=adt;
            break;
        }
    }
    ////////////////////////////////////////
    
//   if (CalculateBackground(verbosity)!=0){ offset_start+=dt; continue;}
    char Interval_name[1000];
    sprintf(Interval_name,"%.2f_%.2f",offset_start,offset_start+dt);
    int result=GANGSTER::CalculateBackground(Interval_name, GRB_TRIGGER_TIME+offset_start,dt,FT1_FILE,FT2_FILE, DATACLASS, MIN_ENERGY, MAX_ENERGY, 0, FT1ZenithTheta_Cut, verbosity);
    /*
    if (result==1){
        printf("%s: We reached the end of the GTI. Will stop the calculation here (offset=%f). It is possible that the emission continues well after this point.\n",__FUNCTION__,offset_start);
        if (TSTART.size()==0) status=-4;
        break;
    }
    */
    if (result==2) {
        printf("%s: The background estimator failed.. bailing out\n",__FUNCTION__);
        status=-1;
        break;
    }

    if (GANGSTER::PlotBackground(Interval_name, GRB_TRIGGER_TIME+offset_start,dt,FT1_FILE,FT2_FILE, DATACLASS, MIN_ENERGY, MAX_ENERGY, -1, FT1ZenithTheta_Cut, 0,verbosity)=="")   { break;}

    char GRB_DIR[1000];
    sprintf(GRB_DIR,"%s/Bkg_Estimates/%.2f_%.2f",(TOOLS::GetS("OUTPUT_DIR")).c_str(),offset_start,offset_start+dt);

    char name[1000];
    sprintf(name,"%s/%s_sig_%.0f_%.0f.root",GRB_DIR,DATACLASS.c_str(),MIN_ENERGY,MAX_ENERGY);
    TFile *fs = TFile::Open(name);
    if (!fs) {printf("%s: Can't open file %s\n",__FUNCTION__,name); offset_start+=dt;continue;}

    TH1F * hDet = (TH1F*)fs->Get("hCtsvsEnergy_Burst");

    if (!hDet) {printf("%s: File %s is empty\n",__FUNCTION__,name);  fs->Close(); offset_start+=dt;continue;}

    sprintf(name,"%s/%s_bkg_%.0f_%.0f.root",GRB_DIR,DATACLASS.c_str(),MIN_ENERGY,MAX_ENERGY);
    TFile *fb = TFile::Open(name);
    if (!fb) {printf("%s: Can't open file %s\n",__FUNCTION__,name); offset_start+=dt;continue;}

    TH1F *hBkg = (TH1F*)fb->Get("hCtsvsEnergy_Est");
    if (!hBkg) {printf("%s: Can't find hBkg\n",__FUNCTION__); fb->Close(); offset_start+=dt;continue;}

    TH1F * hExposure = (TH1F*)fb->Get("hExposure");
    if (!hExposure) {printf("%s: Can't find hExposure\n",__FUNCTION__);  fb->Close(); offset_start+=dt;continue;}
    if (hExposure->Integral()==0) {
          printf("%s: Burst is out of FOV. Will stop the calculation here (offset=%f). It is possible that the emission continues well after this point.\n",__FUNCTION__,offset_start);
          break;
    }

    
    TH1F * hr = (TH1F*)fb->Get("hROI");
    if (!hr) {printf("%s: Can't find hROI\n",__FUNCTION__);  fb->Close(); offset_start+=dt;continue;}
   
    //All went well with this interval, let's fill our data structures
    TSTART.push_back(offset_start);
    TSTOP.push_back(offset_start+dt);
    TMIDDLE.push_back(offset_start+dt/2);
    DT_HALF.push_back(dt/2);
    hROI.push_back(hr);
    Effective_Area.push_back(TOOLS::CalcSpectrallyWeightedExposure(hExposure,-2)/dt);

    /*    
    //debugging stuff
    printf("%d %f %f %f\n",(int)Flux.size(),dt*Effective_Area.back(),dt,Effective_Area.back());
    static bool first=true;
    cCoarse->cd(1); hExposure.Scale(1./dt);
    if (first) {hExposure->Draw(); first=false;}
    else hExposure->Draw("SAME");
    cCoarse->Update();printf("next\n");getchar();
    ////////////////////////////////////////////////////////////////
    */
    fBkg.push_back(fb);
    //decide if we have enough signal to keep going
    double sig=hDet->Integral();
    double bkg=hBkg->Integral();
    if (bkg<0){printf("%s: Bkg negative for %f-%f. Setting it to zero\n",__FUNCTION__,TSTART.back(),TSTOP.back()); bkg=0;}

    //printf("%f %f %f %f\n",TSTART.back(),TSTOP.back(),sig,bkg);
    double diff=sig-bkg;
    Dets.push_back(sig);
    Bkgs.push_back(bkg);
    Bkgs_Err.push_back(bkg*BKG_ESTIMATOR_ERROR);
    if (IntDet.size()==0) {
       IntDet.push_back(sig);
       IntBkg.push_back(bkg);
       IntDiff.push_back(diff);
    } else {
       IntDet.push_back(sig+IntDet.back());
       IntBkg.push_back(bkg+IntBkg.back());
       IntDiff.push_back(diff+IntDiff.back());
    }
    //Check if accumulated signal and background is significant
    if (!IsSignificant &&  ROOT::Math::poisson_cdf_c(int(IntDet.back()),IntBkg.back())<1e-2 &&  IntDiff.back()>5) IsSignificant=true;
    
    Flux.push_back(diff/(Effective_Area.back()*dt));
    if (sig>fc.GetMuMax()*0.5) fc.SetMuMax(sig*1.5);
    Flux_Err_Up.push_back((fc.CalculateUpperLimit(sig,bkg)-diff)/(Effective_Area.back()*dt));
    Flux_Err_Down.push_back((diff-fc.CalculateLowerLimit(sig,bkg))/(Effective_Area.back()*dt));
    if (Flux.back()==0 || (Flux.back()-Flux_Err_Down.back()<=0))  Flux.back()=-1; //zero points will be removed

    //printf(" %e %e %e %e\n",Flux.back(),Flux.back()-Flux_Err_Down.back(),Flux_Err_Down.back(),Flux_Err_Up.back());
    //check if we reached a plateau
    float Min_Plateau_Duration=std::min(1000.,std::max(400.,0.15*TSTOP.back()));
    if (verbosity>3) printf("%s:TSTOP.back()=%f GTI_Offset_0=%f diff=%f min_plateau_duration=%f\n",__FUNCTION__,
        	    TSTOP.back(),GTI_Offset_0,TSTOP.back()-GTI_Offset_0,Min_Plateau_Duration);
    if ((TSTOP.back()-GTI_Offset_0)>Min_Plateau_Duration) { //start checking for a plateau 350s after the start of the current GTI

       TGraph gIntDiff_temp = TGraph(IntDet.size(),&TSTOP.front(),&IntDiff.front());
       TGraph gIntDet_temp = TGraph(IntDet.size(),&TSTOP.front(),&IntDet.front());
       TGraphErrors gIntBkg_temp = TGraphErrors(IntDet.size(),&TSTOP.front(),&IntBkg.front());
       cCoarse->cd(1);gIntDiff_temp.Draw("A*");
       double Plateau_stop,Plateau_start=-1.5;  
       /*here we ask for a well defined plateau which when simulated will still have a high chance of giving good plateaus
         if we stop evaluating the bins after only a short not very well defined plateau has been found,
         then sometimes the simulated lightcurves do not show a good plateau and their duration estimation fails.
       */
       
       Found_Plateau=FindPlateau(&gIntDiff_temp,&gIntDet_temp,&gIntBkg_temp,Plateau_start,Plateau_stop,Min_Plateau_Duration,GTI_Offset_0);
       if (Found_Plateau) Plateau_level=gIntDiff_temp.Eval(Plateau_start);
    }
    cCoarse->Update();
    fs->Close();
    offset_start+=dt;

 } ;

///////////////////////////////          PLOTTING TIME        /////////////////////////////////////////////////////////////////////// 
 gIntDet = new TGraphErrors(IntDet.size(),&TMIDDLE.front(),&IntDet.front(),&DT_HALF.front()); //integral signal
 gIntBkg = new TGraphErrors(IntDet.size(),&TMIDDLE.front(),&IntBkg.front(),&DT_HALF.front()); //integral bkg
 gDiff =   new TGraphErrors(IntDet.size(),&TMIDDLE.front(),&IntDiff.front(),&DT_HALF.front()); //integral signal-bkg

 gFlux =  new TGraphAsymmErrors(Effective_Area.size(),&TMIDDLE.front(),&Flux.front(),&DT_HALF.front(),&DT_HALF.front(),&Flux_Err_Down.front(),&Flux_Err_Up.front()); //differential flux
 gDets =  new TGraphErrors(Dets.size(),&TMIDDLE.front(),&Dets.front(),&DT_HALF.front()); //differential signal
 gBkgs =  new TGraphErrors(Bkgs.size(),&TMIDDLE.front(),&Bkgs.front(),&DT_HALF.front(),&Bkgs_Err.front()); //differential bkg
 gEffective_Area =  new TGraphErrors(Effective_Area.size(),&TMIDDLE.front(),&Effective_Area.front(),&DT_HALF.front()); 


 //Remove negative fluxes from the graph
 double aflu,atime;
 for (int i=0;i<gFlux->GetN();i++) {
    gFlux->GetPoint(i,atime,aflu);
    if (aflu<=0) {gFlux->RemovePoint(i);i--;}
 }

 

 cCoarse->cd(1); 
 gIntBkg->SetLineColor(2);
 gIntDet->SetName("gIntDet");
 gIntBkg->SetName("gIntBkg");
 gIntDet->SetMinimum(0);
 gIntDet->SetTitle("Integrated number of detected and expected events");
 gIntBkg->SetMinimum(0);
 gIntBkg->GetXaxis()->SetTitle("Time after trigger (sec)");
 gIntDet->GetXaxis()->SetTitle("Time after trigger (sec)");
 gIntDet->SetMarkerStyle(21);
 gIntDet->SetMarkerSize(0.4);
 gIntBkg->SetMarkerSize(0.4);
 gIntBkg->SetMarkerStyle(21);
 gIntBkg->SetMarkerColor(2);
 gIntDet->GetYaxis()->SetTitle("Integrated Events");
 gIntDet->Draw("PA"); 
 gIntBkg->Draw("PSAME"); //don't change the name or location of gbkg
 
 cCoarse->cd(2); 
 //cCoarse->GetPad(2)->SetLogy();
 gDets->SetTitle("Number of detected and expected number of events per bin");
 gDets->SetMinimum(std::min(gBkgs->GetHistogram()->GetMinimum(),gDets->GetHistogram()->GetMinimum()));
 gBkgs->SetMarkerColor(2);
 gBkgs->SetName("gBkg");
 gBkgs->SetLineColor(2);
 gDets->GetXaxis()->SetTitle("Time after trigger (sec)");
 gDets->GetYaxis()->SetTitle("Events/bin");
 gDets->SetMaximum(std::max(gDets->GetHistogram()->GetMaximum(),gBkgs->GetHistogram()->GetMaximum()));
 gDets->Draw("AP"); 
 gBkgs->Draw("PSAME");

 cCoarse->cd(4); 
 gDiff->SetName("gDiff");
 gDiff->SetTitle("Accumulated background-subtracted counts");
 gDiff->GetXaxis()->SetTitle("Time after trigger (s)");
 gDiff->GetYaxis()->SetTitle("Accumulated events");
 gDiff->SetMarkerSize(0.4);
 gDiff->SetMarkerStyle(21);
 gDiff->SetMarkerColor(2);
 gDiff->SetMaximum(1.5*gDiff->GetHistogram()->GetMaximum());
 gDiff->SetMinimum(0);
 gDiff->Draw("PA");
 
 cExtras->cd(1); 
 cExtras->GetPad(1)->SetLogy();
 cExtras->GetPad(1)->SetLogx();
 cExtras->GetPad(1)->SetGridy();
 gFlux->GetYaxis()->SetRangeUser(1e-6,1e-1);
 gFlux->SetTitle("Flux (for an assumed spectral index a=-2)");
 gFlux->GetXaxis()->SetTitle("Time after trigger (sec)");
 gFlux->GetYaxis()->SetTitle("Flux (ev/(cm^{2} sec))");
 gFlux->SetName("gFlux");
 gFlux->Draw("PA"); 
 TF1 * fitTimeDecay = new TF1("fitTimeDecay","[0]*pow(x,[1])");
 //find time of maximum flux so that we start the fit from there
 double maxflux=0,fit_start=0,fit_stop=-1;
 for (int i=0;i<gFlux->GetN();i++) {
    gFlux->GetPoint(i,atime,aflu);
    if (maxflux<aflu) {maxflux=aflu;fit_start=atime;}
    if ((aflu-gFlux->GetErrorYlow(i))<8e-6 && fit_stop<0) fit_stop=atime;
    //printf("%d %e %e %f %e %e\n",i,fit_start,fit_stop,atime,aflu,gFlux->GetErrorYlow(i));
 }
 //if (fit_stop<0) fit_stop=T95;
 gFlux->Fit("fitTimeDecay","Q","",fit_start,fit_stop);
 /////////////////////////////////////////////////////////////////////
 
 cExtras->cd(2);
 gEffective_Area->SetTitle("Spectrally-weighted effective area (a=-2)");
 gEffective_Area->GetXaxis()->SetTitle("Time after trigger (sec)");
 gEffective_Area->GetYaxis()->SetTitle("Effective area (cm^{2})");
 gEffective_Area->SetName("gEffective_Area");
 gEffective_Area->SetMaximum(gEffective_Area->GetHistogram()->GetMaximum()*1.3);
 gEffective_Area->SetMinimum(0);
 gEffective_Area->Draw("PA");
 
 cCoarse->Update();
 cExtras->Update();
 
 if (status==0) {
   if (Plateau_level<10) {
     printf("%s: Warning: Not enough events to be able to estimate the duration (plateau=%.2f). bailing out\n",__FUNCTION__,Plateau_level);
     status=-2;
   }
   else if (!IsSignificant) {
      printf("%s: It seems that the burst is not detected significantly for your class and energy ranges.. Bailing out\n",__FUNCTION__);
      status=-3;
   }
 }
 else if (status==+5 && (*max_element( IntDiff.begin(), IntDiff.end()))<12) {
    printf("%s: Warning: Not enough events to be able to estimate the duration (%.2f). bailing out\n",__FUNCTION__,Plateau_level);
    status=-2;
 }
 if (verbosity>3) printf("%s: Status=%d\n",__FUNCTION__,status);
 return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//0 is good, -1 is error
int DurationEstimator::EvaluateBins_Fine(vector <TH1F*> &hROI, vector <double>& TSTART, vector <double>& TSTOP) {

 //Make Background fit
 TF1 * fitBkg = new TF1("fitBkg","pol5");
 gIntBkg->Fit("fitBkg","WQ0");

 //Recalculate Signals and redo the plot
 double atstart=0;
 vector <double> IntDet_fine,IntBkg_fine,TSTOP_fine,IntDiff_fine,TMIDDLE_fine,DT_HALF_fine,Fluence_fine, Diff_fine,Effective_Area_fine;
 IntDet_fine.push_back(0);
 Effective_Area_fine.push_back(0);
 IntBkg_fine.push_back(0);
 TSTOP_fine.push_back(0);
 IntDiff_fine.push_back(0);
 Diff_fine.push_back(0);
 TMIDDLE_fine.push_back(0);
 DT_HALF_fine.push_back(0);
 Fluence_fine.push_back(0);
 if (verbosity>1) printf("%s: Recounting detected signal\n",__FUNCTION__);

 const double RA=TOOLS::Get("GRB_RA");
 const double DEC=TOOLS::Get("GRB_DEC");
 float dt;

 while (atstart<=TSTOP.back()) {
     if (atstart<10) dt=0.2;
     else dt=2;
     //printf("%f\r",atstart); fflush(0);
     //find appropriate ROI 
     unsigned int interval=0;
     for (;interval<TSTOP.size();interval++) {
        //printf("%f %f %f %d\n",atstart,TSTART[interval],TSTOP[interval],interval);
        if (atstart>=TSTART[interval] && atstart<=TSTOP[interval]) break;
     }
     if (hROI[interval]==0) { //out of GTI        
        atstart+=dt; 
        continue;
     }


     //double bkg_rate = gBkgs->Eval(atstart+dt/2,0,"S")/(TSTOP[interval]-TSTART[interval]);//background rate for a coarse interval
     double bkg_rate = gBkgs->Eval(atstart+dt/2)/(TSTOP[interval]-TSTART[interval]);//background rate for a coarse interval
     if (isnan(bkg_rate))  break;
     if (bkg_rate<0 || bkg_rate>0.3 || interval==(TSTART.size()-1)) {
          //if the bkg looks dorked or if we are at the last coarse interval stop being fancy (i.e. use eval) and just use the bin value
	  //This because the interpolation breaks at the end of the historam and the results are crap..
           bkg_rate = (gBkgs->GetY())[interval]/(TSTOP[interval]-TSTART[interval]); 
     }
     //printf("eval=%f rate=%f dt=%f interval %d TSTART.size() %d\n",gBkgs->Eval(atstart+dt/2,0,"S"),bkg_rate,TSTOP[interval]-TSTART[interval],interval,TSTART.size());
     double bkg_fine = bkg_rate*dt; //amount of background for our fine interval
     if (bkg_fine<=0) {printf("%s: bkg negative at time %f-%f..Probably a high-theta observation. Interval skipped.\n",__FUNCTION__,atstart,atstart+dt);atstart+=dt; continue;} 
    
     double sig_fine = (double) TOOLS::Make_Burst_Plots(DATACLASS, FT1_FILE,"",FT1ZenithTheta_Cut, RA,DEC,GRB_TRIGGER_TIME+atstart,dt,hROI[interval],0);
     //printf("%f %e %e %e %e %e\n",atstart,sig_fine,bkg_fine,bkg_rate,gBkgs->Eval(atstart+dt/2,0,"S"),gBkgs->Eval(atstart+dt/2));
     TSTOP_fine.push_back(atstart+dt);
     TMIDDLE_fine.push_back(atstart+dt/2);
     DT_HALF_fine.push_back(dt/2);

     IntDet_fine.push_back(IntDet_fine.back()+sig_fine);
     IntBkg_fine.push_back(IntBkg_fine.back()+bkg_fine);
     Diff_fine.push_back(sig_fine-bkg_fine);
     IntDiff_fine.push_back(IntDet_fine.back()-IntBkg_fine.back());
     Effective_Area_fine.push_back(gEffective_Area->GetY()[interval]);
     //printf("%d persec %f tot %f atstart=%f %f %f\n",interval,(gEffective_Area->GetX())[interval],Effective_Area.back(),atstart,TSTART[interval],TSTOP[interval]);
     Fluence_fine.push_back(Fluence_fine.back()+(sig_fine-bkg_fine)/Effective_Area_fine.back()); 
     //printf("bin=%d atstart=%lf sig_fine=%lf int_bkg_fine=%lf interval=%d\n",IntDet_fine.size(),atstart,sig_fine,int_bkg_fine,interval);
     atstart+=dt;
 }
 ////////////////////////////////////////////

 gIntDet_fine  = new TGraphErrors(IntDet_fine.size(),&TMIDDLE_fine.front(),&IntDet_fine.front(),&DT_HALF_fine.front());
 gIntBkg_fine  = new TGraphErrors(IntDet_fine.size(),&TMIDDLE_fine.front(),&IntBkg_fine.front(),&DT_HALF_fine.front());
 gIntDiff_fine = new TGraphErrors(IntDet_fine.size(),&TMIDDLE_fine.front(),&IntDiff_fine.front(),&DT_HALF_fine.front());
 gFluence_fine = new TGraphErrors(IntDet_fine.size(),&TMIDDLE_fine.front(),&Fluence_fine.front(),&DT_HALF_fine.front());
 gEffective_Area_fine = new TGraphErrors(IntDet_fine.size(),&TMIDDLE_fine.front(),&Effective_Area_fine.front(),&DT_HALF_fine.front());

 gIntDet_fine->SetName("gIntDet_fine");
 gIntBkg_fine->SetName("gIntBkg_fine");
 // gDiff->SetMarkerStyle(21); gDiff->SetMarkerSize(0.4);
 // gDiff->SetMinimum(0);
 gIntBkg_fine->SetLineColor(2);
 
 gIntDiff_fine->GetXaxis()->SetTitle("Time after trigger (sec)");
 gIntDiff_fine->GetYaxis()->SetTitle("Integrated Events");
 gIntDiff_fine->SetTitle("Accumulated background-subtracted signal");
 gIntDiff_fine->SetName("gIntDiff_fine");

 gFluence_fine->SetMarkerStyle(21);
 gFluence_fine->SetMarkerColor(2);
 gFluence_fine->SetMarkerSize(0.2);
 gFluence_fine->SetName("gFluence_fine");
 gFluence_fine->GetXaxis()->SetTitle("Time after trigger (sec)");
 gFluence_fine->GetYaxis()->SetTitle("Fluence (photons/cm^{2})");
 gFluence_fine->SetTitle("Fluence");

 return 0;
}

