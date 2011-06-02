//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"

ClassImp(BackgroundEstimator)

int BackgroundEstimator::Make_Background_Map(string FT1_FILE, string FT2_FILE, string GRB_DIR, double Burst_t0, double Burst_Dur, const double Iterations, int verbosity, bool Calc_Residual){

 //Return codes 
 //1 NO GTI
 //2 NO EXPOSURE (i.e. burst out of FOV)
 //0 ALL OK or file exists OK 
  string ResultsFile = GRB_DIR+"/"+DataClass+"_BackgroundMaps.root";
  FILE * ftemp = fopen(ResultsFile.c_str(),"r");
  bool ZThetaChanged=false;
  if (ftemp) {
    fclose(ftemp); 
    TFile * fres = TFile::Open(ResultsFile.c_str());
    if  (fres->TestBit(TFile::kRecovered) || fres->GetNkeys()<=0) {
	printf("%s: File %s was not closed ok. Will overwrite\n",__FUNCTION__,ResultsFile.c_str()); 
    }
    else if (fres->Get("ERROR")) {
	printf("%s: ERROR code cound in file %s, skipping\n",__FUNCTION__,ResultsFile.c_str()); 
	fres->Close();
	return 1;
    }
    else if (fabs(atof(fres->Get("FT1ZenithTheta_Cut")->GetTitle())-FT1ZenithTheta_Cut)>.1) {
	printf("%s: Zenith Theta cuts different old=%s new=%.0f. Will resimulate sky\n",__FUNCTION__,fres->Get("FT1ZenithTheta_Cut")->GetTitle(),FT1ZenithTheta_Cut);
	ZThetaChanged=true;
    }
    else if (fres->GetNkeys()>1) {
	if (verbosity>1) {
	    printf("%s: File %s exists.. skipping\n",__FUNCTION__,ResultsFile.c_str());
	}
	fres->Close();
	return 0;
    }
    fres->Close();
  }

  TFile * fResults;
  int tries=0;
  ///Sometimes root does not manage to create the output file.. I keep looping until it manages?
  while (1) {
     string cmd="echo >"+ResultsFile;
     system(cmd.c_str());
     fResults = new TFile(ResultsFile.c_str(),"RECREATE");
     ftemp = fopen(ResultsFile.c_str(),"r");
     if (ftemp) {
        fclose(ftemp);
        break;
     }
     printf("%s: Could not create file %s. Retrying.. \n",__FUNCTION__,ResultsFile.c_str());
     tries++;
     if (tries>100){ printf("%s: Won't retry more.. exiting\n",__FUNCTION__); exit(1);}
  }
  TH2D* hSolidAngle = (TH2D*)fResidualOverExposure->Get("hSolidAngle");
  if (verbosity>1) printf("%s: Creating Background Map\n",__FUNCTION__);
  ///////////////////////////////////////////////////////

  //Decide on time range to be analyzed
  //Because it is hard to find events in intervals smaller than 1sec if the user
  //requests the background for <1sec durations, we make the calculations for an 1sec duration
  //and then we scale down to the requested duration
  double Burst_Dur_Used=-1;
  //Calculate all-sky background
  int TimeBinsB=-1;
  if (Burst_Dur<1) {
    TimeBinsB=1;
    Burst_Dur_Used=1;
    if (verbosity>1) {
        printf("%s: Calculating background for an 1sec interval.\n",__FUNCTION__);
        printf("%s: Results will be scaled down to the requested %f sec interval\n",__FUNCTION__,Burst_Dur);
    }
  }
  else {
    TimeBinsB = (int)ceil(Burst_Dur);
    Burst_Dur_Used = TimeBinsB*1; 
  }
  double Burst_t1 = Burst_t0 + Burst_Dur_Used;
  if (verbosity>2) {
     printf("%s: Making calculations using %d %.2f sec bins\n",__FUNCTION__,TimeBinsB,Burst_Dur_Used/TimeBinsB);
  }
  //////////////////////////////////////////
  vector <string> FT1FILES;
  if (FT1_FILE[0] == '@') {
      ftemp = fopen((FT1_FILE.substr(1,FT1_FILE.length())).c_str(),"r");
      if (!ftemp) {printf("%s: Can't open list file %s\n",__FUNCTION__,(FT1_FILE.substr(1,FT1_FILE.length())).c_str()); exit(1);}
      while (fscanf(ftemp,"%s",name)==1) FT1FILES.push_back(string(name));
      fclose (ftemp);
  }
  /* //capability not used any more after joining GRBAnalysis
  else if (FT1_FILE[0]=='^') {
     char FITSDIR[2000];
     sscanf(FT1_FILE.c_str(),"^%s",FITSDIR);
     FT1FILES = TOOLS::MakeFitsList(Burst_t0,Burst_Dur,FITSDIR);
     string astring=GRB_DIR+"/fitslist.txt";
     ftemp = fopen(astring.c_str(),"w");
     for (unsigned int i=0;i<FT1FILES.size();i++) fprintf(ftemp,"%s\n",FT1FILES[i].c_str());
     fclose (ftemp);
     FT1_FILE="@"+astring;
  }
  */
  else FT1FILES.push_back(FT1_FILE);

  //Check if data file provided has required data
  fitsfile *fptr;
  int status = 0;
  int  anynul;
  long nrows;
  int hdutype;
  double File_t0,File_t1;
  //Open first file
  char COMMENT[2000];
  fits_open_file(&fptr, FT1FILES[0].c_str(), READONLY, &status);
  if (status) {
	printf("%s: Can't open fits file %s\n",__FUNCTION__,FT1FILES[0].c_str());
        fits_report_error(stderr, status);
	exit(1);
  }
  fits_movabs_hdu(fptr, 2, &hdutype, &status);
  fits_get_num_rows(fptr, &nrows, &status);      
  //fits_read_col (fptr,TDOUBLE,10,1, 1, 1, NULL,&File_t0, &anynul, &status);
  status=0;
  fits_read_keyword(fptr, (char*)"TSTART",  name, COMMENT, &status);
  File_t0=atof(name);
  if (File_t0<1) sscanf(name,"'%lf'",&File_t0); //parsing failed so let's try with '' around the number

  if (FT1FILES.size()==1) { //one file
     status=0;
     fits_read_keyword(fptr, (char*)"TSTOP",  name, COMMENT, &status); 
     File_t1=atof(name);
     if (File_t1<1)   sscanf(name,"'%lf'",&File_t1); //parsing failed so let's try with '' around the number
     //printf("%d %s %f\n",status,name,File_t1);
     //fits_read_col (fptr,TDOUBLE,10,nrows, 1, 1, NULL,&File_t1, &anynul, &status);
     fits_close_file(fptr, &status);
  }
  else { //many files
     fits_close_file(fptr, &status);
     status=0;
     fits_open_file(&fptr, FT1FILES[FT1FILES.size()-1].c_str(), READONLY, &status);
     fits_movabs_hdu(fptr, 2, &hdutype, &status);
     //fits_get_num_rows(fptr, &nrows, &status);    
     fits_read_keyword(fptr, (char*)"TSTOP",  name, COMMENT, &status); File_t1=atof(name);  
     if (File_t1<1)  sscanf(name,"'%lf'",&File_t1); //parsing failed so let's try with '' around the number
     //fits_read_col (fptr,TDOUBLE,10,nrows, 1, 1, NULL,&File_t1, &anynul, &status);
     fits_close_file(fptr, &status);
  }
  if (Burst_t0<File_t0) {printf("%s: data files start at time %f and you requested an estimation for an earlier time (%f).\n",__FUNCTION__,File_t0,Burst_t0); exit(1);}
  if (Burst_t1>File_t1) {
      if (Burst_Dur!=Burst_Dur_Used) {
         printf("%s: data files end at time %f and this analysis needs data up to MET %f.\n",__FUNCTION__,File_t1,Burst_t1); 
         exit(1);
     }
     else {
        printf("%s: data files end at time %f and you requested an estimation for a later time (%f).\n",__FUNCTION__,File_t1,Burst_t1);
        exit(1);
     }
  }
  //////////////////////////////////////////////

  //Making plots
  string astring=GRB_DIR+"/Plots.root";
  ftemp = fopen(astring.c_str(),"r");
  if (ftemp && !ZThetaChanged) fclose (ftemp);
  else TOOLS::Make_Plots(0,Burst_Dur,Burst_t0,astring,FT2_FILE, verbosity);

  //Live-time cube
  if (verbosity>1) printf("%s: Creating live-time cube\n",__FUNCTION__);

  if (Calc_Residual) {
     sprintf(name,"%s/burst_ltCube.fits",GRB_DIR.c_str());
     ftemp = fopen(name,"r");
     if (ftemp && !ZThetaChanged) {
        printf("%s: File %s already exists.. skipping creation of it\n",__FUNCTION__,name);
        fclose(ftemp);
     }
     else TOOLS::Run_gtltcube(GRB_DIR, Burst_t0, Burst_t1, FT2_FILE, FT1ZenithTheta_Cut, verbosity);
  }
  //////////////////////////////////////////////////////

  //READ GTIs
  vector <double> ALLSTARTGTI;
  vector <double> ALLENDGTI;

  long GTIs;
  for (unsigned int ifile=0;ifile<FT1FILES.size();ifile++) {
      status=0;
      fits_open_file(&fptr, FT1FILES[ifile].c_str(), READONLY, &status);
      if (status) {printf("%s: Error opening file %s\n",__FUNCTION__,FT1FILES[ifile].c_str()); exit(1);}

      //READ GTIs 
      fits_movabs_hdu(fptr, 3, &hdutype, &status);
      fits_get_num_rows(fptr, &GTIs, &status);

      for (int i=1;i<=GTIs;i++) {
           double agti_start,agti_end;
           fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&agti_start, &anynul, &status);
           fits_read_col (fptr,TDOUBLE,2,i, 1, 1, NULL,&agti_end, &anynul, &status);
           if (agti_end<Burst_t0 || agti_start>Burst_t1) continue; //skip external GTIs
           ALLSTARTGTI.push_back(agti_start);
           ALLENDGTI.push_back(agti_end);
           //int ALLGTIs=ALLSTARTGTI.size();
           //printf("%d %d %f %f\n",ifile,ALLGTIs,ALLSTARTGTI[ALLGTIs],ALLENDGTI[ALLGTIs]);
      }
      fits_close_file(fptr, &status);
  }
  if (ALLSTARTGTI.size()==0) { 
       fResults->cd();
       printf("%s: No GTIs found\n",__FUNCTION__);
       TNamed Data = TNamed("ERROR","NO GTI");
       Data.Write();
       fResults->Close();
       return 1;
  }
  ALLSTARTGTI.push_back(0);
  ALLENDGTI.push_back(0);
  if (verbosity>1) printf("%s: Read %d GTIs\n",__FUNCTION__,(int)ALLSTARTGTI.size()-1);
  /////////////////////////////////////////////////

  double SimEvents;
  TH2F * hSimulatedSky = new TH2F("hSimulatedSky","hSimulatedSky",L_BINS,-180,180,B_BINS,-90,90);
  hSimulatedSky->SetContour(200);

  TH2F* hExposureBurst=NULL,*hExposureBurst_1deg=NULL,*hFinalBackground=NULL,*hFinalResidual=NULL,*hFinalSimulatedSky=NULL;
  if (Calc_Residual) {
     hExposureBurst = new TH2F("hExposureBurst","hExposureBurst",L_BINS,-180,180,B_BINS,-90,90);
     hExposureBurst_1deg = new TH2F("hExposureBurst_1deg","hExposureBurst_1deg",360,-180,180,180,-90,90);
     hFinalBackground = new TH2F("hFinalBackground","hFinalBackground",L_BINS,-180,180,B_BINS,-90,90);
     hFinalBackground->SetContour(200);
     hFinalResidual = new TH2F("hFinalResidual","hFinalResidual",L_BINS,-180,180,B_BINS,-90,90);
     hFinalResidual->SetContour(200);
     hFinalSimulatedSky = new TH2F("hFinalSimulatedSky","hFinalSimulatedSky",L_BINS,-180,180,B_BINS,-90,90);
     hFinalSimulatedSky->SetContour(200);
  }

  //EXPOSURE
  if (Calc_Residual) {
     sprintf(name,"%s/%s_burst_exposure.fits",GRB_DIR.c_str(),DataClass.c_str());
     ftemp = fopen(name,"r");
     if (ftemp && !ZThetaChanged) fclose(ftemp);
     else TOOLS::Run_gtexpcube(GRB_DIR, Burst_t0, Burst_t1, FT2_FILE, DataClass, FT1ZenithTheta_Cut, name, Energy_Min_datafiles, Energy_Max_datafiles, Energy_Bins_datafiles,verbosity);
  }

  if (verbosity>1) printf("%s: Simulating CR background\n",__FUNCTION__);
  for (unsigned short int ie=1;ie<=Energy_Bins_datafiles;ie++) { 
     if (verbosity>0) TOOLS::ProgressBar(ie-1,Energy_Bins_datafiles);

     //EXPOSURE
     if (Calc_Residual) {
        sprintf(name,"%s/%s_burst_exposure.fits",GRB_DIR.c_str(),DataClass.c_str());
        TOOLS::ReadExposureMap(name,hExposureBurst,ie,verbosity);
        TOOLS::ReadExposureMap(name,hExposureBurst_1deg,ie,verbosity);
        if (hExposureBurst->Integral()==0) {
    	    printf("%s: Integral of exposure is zero? bailing out.. Check or delete file %s\n",__FUNCTION__,name);
    	    return 2;
    	}
     }

     //SIMULATION
     sprintf(name,"%s/Plots.root",GRB_DIR.c_str());

     //Because usually for higher energies we use a very small ROI, the statistical error of the bkg estimate gets worse.
     //for that case, we simulate a big more for E>1GeV to have good statistical error even at HE.
     int Extra_Iterations_Factor;
     if (Bin2Energy(ie)>1e3) Extra_Iterations_Factor=5;
     else                    Extra_Iterations_Factor=1;
     if (TOOLS::GetS("_DataClassName_noConv")=="DIFFUSE") {
	Extra_Iterations_Factor*=3;
	if (Bin2Energy(ie)>1e4) Extra_Iterations_Factor*=2;
     }

     SimulateSky( name, hSimulatedSky, ALLSTARTGTI,ALLENDGTI,Extra_Iterations_Factor*Iterations, ie);
     
     fResults->cd();
     //sprintf(name,"hSimulatedSky_nosolid_%d",ie);
     //hSimulatedSky->Write(name);

     SimEvents = hSimulatedSky->GetEntries();
     if (SimEvents<=0) {printf("%s: Problem simulating sky (return %f)\n",__FUNCTION__,hSimulatedSky->Integral()); exit(1);}
 
     /////////////////////////////////////////////////////
     hSimulatedSky->Divide(hSolidAngle);
     fResults->cd();
     //RESIDUAL
     if (Calc_Residual) {
        sprintf(name,"hResidual_Over_Exposure_%d;1",ie);
        TH2F* htemp = (TH2F*)fResidualOverExposure->Get(name);
        sprintf(name,"hResidualBurst_%d",ie);
        TH2F* hResidualBurst = (TH2F*)htemp->Clone(name);
        htemp->Delete();
        hResidualBurst->SetTitle(name);
        hResidualBurst->Multiply(hExposureBurst);
        //Scale maps 
        if (Burst_Dur_Used!=Burst_Dur) {
           double scalefactor = Burst_Dur/Burst_Dur_Used;
           //printf("%s: Scaling maps by a factor %.2f/%.2f\n",__FUNCTION__,Burst_Dur,Burst_Dur_Used);
           hResidualBurst->Scale(scalefactor);
           hExposureBurst->Scale(scalefactor);
        }
        if (ie!=0 && ie!=(Energy_Bins_datafiles+1)) {
	   hFinalResidual->Add(hResidualBurst);
    	   hFinalSimulatedSky->Add(hSimulatedSky);
        }
	sprintf(name,"hExposure_Burst_%d",ie);
	hExposureBurst->Write(name);
	sprintf(name,"hExposure_Burst_1deg_%d",ie);
        hExposureBurst_1deg->Write(name);
        sprintf(name,"hResidualBurst_%d",ie);
        hResidualBurst->Write(name);
        hResidualBurst->Delete();
    }

    //Scale maps 
    if (Burst_Dur_Used!=Burst_Dur) {
       double scalefactor = Burst_Dur/Burst_Dur_Used;
       //printf("%s: Scaling maps by a factor %.2f/%.2f\n",__FUNCTION__,Burst_Dur,Burst_Dur_Used);
        hSimulatedSky->Scale(scalefactor);
    }
    sprintf(name,"hSimulatedSky_%d",ie);
    hSimulatedSky->SetTitle(name);
    hSimulatedSky->Write(name);

  }
  if (Calc_Residual) {
     hSolidAngle->Write();
     hFinalBackground->Add(hFinalSimulatedSky);
     hFinalBackground->Add(hFinalResidual);
     hFinalBackground->Write();   hFinalBackground->Delete();
     hFinalResidual->Write();     hFinalResidual->Delete();
     hFinalSimulatedSky->Write(); hFinalSimulatedSky->Delete();
  }
  printf("\n");
  //////////////////////

  sprintf(name,"%e-%e-%d",Energy_Min_datafiles,Energy_Max_datafiles,Energy_Bins_datafiles);
  TNamed Data = TNamed("Energy_Data",name);
  Data.Write();

  sprintf(name,"%.2f-%.2f",Burst_t0,Burst_Dur);
  Data = TNamed("Time_Data",name);
  Data.Write();

  sprintf(name,"%.1f",FT1ZenithTheta_Cut);
  Data = TNamed("FT1ZenithTheta_Cut",name);
  Data.Write();

  sprintf(name,"%.2f",EstimatorVersion);
  Data = TNamed("Estimator_Version",name);
  Data.Write();

  sprintf(name,"%.1f %.1f %.1f",Residuals_version,RateFit_version,ThetaPhiFits_version);
  Data = TNamed("DataFiles_Version",name);
  Data.Write();

  Data = TNamed("GRB_NAME",(TOOLS::GetS("GRB_NAME")).c_str());  Data.Write();
  Data = TNamed("FT1_FILE",FT1_FILE.c_str());  Data.Write();
  Data = TNamed("FT2_FILE",FT2_FILE.c_str());  Data.Write();
  Data = TNamed("DataClass",DataClass.c_str());  Data.Write();

  if (Calc_Residual) {
    hExposureBurst->Delete();
    hExposureBurst_1deg->Delete();
  }
  hSimulatedSky->Delete();

  fResults->Close();
  return 0;
}

