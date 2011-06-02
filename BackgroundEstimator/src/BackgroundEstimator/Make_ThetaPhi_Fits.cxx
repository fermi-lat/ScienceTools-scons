//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"

ClassImp(BackgroundEstimator)

void BackgroundEstimator::Make_ThetaPhi_Fits(string FitsAllSkyFile){

  sprintf(name,"%s/Plots_%s.root",DataDir.c_str(),DataClassName_noConv.c_str());
  TFile * fPlots = TFile::Open(name);
  TH1F* hPtRazvsTime = (TH1F*)fPlots->Get("hPtRazvsTime");
  TH1F* hPtDeczvsTime = (TH1F*)fPlots->Get("hPtDeczvsTime");
  TH1F* hPtRaxvsTime = (TH1F*)fPlots->Get("hPtRaxvsTime");
  TH1F* hPtDecxvsTime = (TH1F*)fPlots->Get("hPtDecxvsTime");
  TH1F* hRAZenithvsTime = (TH1F*)fPlots->Get("hRAZenithvsTime");
  TH1F* hDecZenithvsTime = (TH1F*)fPlots->Get("hDecZenithvsTime");
  int thetabins=40,phibins=45;

  float MinB=70;
  if (DataClass.find("DIFFUSE")!=string::npos) MinB=65; //for diffuse class relax a bit the cut on MinB because we don't have enough bkg data

  TH1F * hTheta[Energy_Bins_user+1][7],*hPhi[Energy_Bins_user+1],*hTheta_away[Energy_Bins_user+1],*hTheta_towards[Energy_Bins_user+1];
  TF1 *PhiFit[Energy_Bins_user+1],*ThetaFit[Energy_Bins_user+1][7];

  TH2F* hThetavsPhi_restricted = new TH2F("hThetavsPhi_restricted","ThetavsPhi with a ztheta cut",360,0,360,90,0,90);
  TH2F* hThetavsPhi_unrestricted = new TH2F("hThetavsPhi_ur","ThetavsPhi with no zt cut and looking away from the earth",360,0,360,90,0,90);
  TH1F* hZTheta_away = new TH1F("hZTheta_away","ZTheta_looking away from the earth",120,0,120);
  TH1F* hZTheta_towards = new TH1F("hZTheta_towards","hZTheta looking towards the earth",120,0,120);


  TH1F* hPhi_away = new TH1F("hPhi_away","Phi looking away from the earth",360,0,360);
  TH1F* hPhi_towards = new TH1F("hPhi_towards","Phi looking towards the earth",360,0,360);

  for (int i=1;i<=Energy_Bins_user;i++) {
     for (int iphi=0;iphi<7;iphi++) {
        sprintf(name,"hTheta_%d_%d",i,iphi);
        if ((DataClass.find("DIFFUSE")!=string::npos) && i>10) thetabins=20;
        hTheta[i][iphi]  = new TH1F(name,name,thetabins,0,90);
        sprintf(name,"ThetaFit_%d_%d",i,iphi);

        if (DataClass.find("S3")!=string::npos) ThetaFit[i][iphi] = new TF1(name,"pol8");
        else ThetaFit[i][iphi] = new TF1(name,"pol5");
        ThetaFit[i][iphi]->FixParameter(0,0); //set the fit to go to 0 at theta->0
     }
     sprintf(name,"PhiFit_%d",i);
     PhiFit[i] = new TF1(name,"[0]+[1]*cos(TMath::Pi()*(x/45)) + [2]*cos(TMath::Pi()*(x/90))");
     //second fit is better but first fit is faster!
     //PhiFit[i] = new TF1(name,"[0] + [1]*exp(-pow((x-90)/53.,2)) + [2]*exp(-pow((x-180)/53.,2)) + [3]*exp(-pow((x-270)/53.,2)) + [4]*exp(-pow((x-360)/53.,2))+[4]*exp(-pow((x)/53.,2))");

     sprintf(name,"hPhi_%d",i);
     hPhi[i]= new TH1F(name,name,phibins,0,360);

     sprintf(name,"hTheta_away_%d",i);
     hTheta_away[i] = new TH1F(name,"Theta looking away from the earth",90,0,90);//don't make the number of bins variable -- it will mess up the rate correction-factor calculation

     sprintf(name,"hTheta_towards_%d",i);
     hTheta_towards[i] = new TH1F(name,"Theta looking towards the earth",90,0,90);

  }
  TH2F * hThetaCut = new TH2F("hThetaCut","Max theta",Energy_Bins_user,1,Energy_Bins_user,7,0,7);

  TH2F * hThetavsB = new TH2F("hThetavsB","hThetavsB",180,-90,90,90,0,90);
  hThetavsB->GetXaxis()->SetTitle("Galactic Latitude (deg)");
  hThetavsB->GetYaxis()->SetTitle("Theta angle (deg)");
  TH2F * hPhivsB = new TH2F("hPhivsB","hPhivsB",180,-90,90,90,0,360);
  hThetavsB->GetXaxis()->SetTitle("Galactic Latitude (deg)");
  hThetavsB->GetYaxis()->SetTitle("Azimuthal angle (deg)");


  short iphibin;
  int itimebin;

  fitsfile *fptr;
  double PtL,PtB;
  float PtRaz,PtDecz,RAZENITH,DECZENITH,PtRax,PtDecx;
 
  Hep3Vector localdir;
  const double maxtheta=80*DEG_TO_RAD;
  const double maxZtheta=FT1ZenithTheta_Cut*DEG_TO_RAD;
  HepRotation localToCelestial;
  localToCelestial.setTolerance(0.1);
  astro::SkyDir SCZenith,SCz,SCx,locGAL;    

  FILE* ftemp = fopen(FitsAllSkyFile.c_str(),"r");
  int ifile=0;
  int ibin;
  while (fscanf(ftemp,"%s",name)==1) {
    if (ifile%10==0) {printf("%d\r",ifile); fflush(0);} ifile++;
    long nrows;int ncols;
    int status=0,hdutype,anynul;
    fits_open_file(&fptr, name, READONLY, &status);
    if (status) {printf("%s: error opening file %s\n",__FUNCTION__,name); exit(1);}
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

    for (long i=1;i<=nrows;i++) {

        if (!PassesCuts(fptr,i,format)) continue;
        double FT1Energy;
        fits_read_col (fptr,TDOUBLE,1,i, 1, 1, NULL,&FT1Energy, &anynul, &status);
        short ebin=Energy2Bin(FT1Energy);

        double FT1ZenithTheta,FT1Theta,FT1Phi,PtTime;
        fits_read_col (fptr,TDOUBLE,8,i, 1, 1, NULL,&FT1ZenithTheta, &anynul, &status);
        fits_read_col (fptr,TDOUBLE,6,i, 1, 1, NULL,&FT1Theta, &anynul, &status);
        fits_read_col (fptr,TDOUBLE,7,i, 1, 1, NULL,&FT1Phi, &anynul, &status);
        fits_read_col (fptr,TDOUBLE,10,i, 1, 1, NULL,&PtTime, &anynul, &status);

        itimebin = hPtRazvsTime->FindBin(PtTime);

        if  (hPtRazvsTime->GetBinContent(itimebin)==0) {
           if      (hPtRazvsTime->GetBinContent(itimebin-1)!=0) ibin=itimebin-1;
           else if (hPtRazvsTime->GetBinContent(itimebin+1)!=0) ibin=itimebin+1;
           else if (hPtRazvsTime->GetBinContent(itimebin+2)!=0) ibin=itimebin+2;
           else if (hPtRazvsTime->GetBinContent(itimebin-2)!=0) ibin=itimebin-2;
           else    {printf("%s: there is a gap in the plots? %f %d\n",__FUNCTION__,PtTime,itimebin); continue;}
           itimebin=ibin;
        }
        PtRaz  = hPtRazvsTime->GetBinContent(itimebin);
        PtDecz = hPtDeczvsTime->GetBinContent(itimebin);
        TOOLS::Galactic((double)PtRaz,(double)PtDecz,&PtL,&PtB);
    
        if (fabs(PtB)>MinB) {
            RAZENITH  = hRAZenithvsTime->GetBinContent(itimebin);
            DECZENITH = hDecZenithvsTime->GetBinContent(itimebin);
            PtRax     = hPtRaxvsTime->GetBinContent(itimebin);
            PtDecx    = hPtDecxvsTime->GetBinContent(itimebin);
            SCZenith = astro::SkyDir(RAZENITH,DECZENITH,astro::SkyDir::EQUATORIAL);

            SCz = astro::SkyDir(PtRaz,PtDecz,astro::SkyDir::EQUATORIAL);
            SCx = astro::SkyDir(PtRax,PtDecx,astro::SkyDir::EQUATORIAL);
            localToCelestial = HepRotation(SCx.dir(),(SCz.dir()).cross(SCx.dir()),SCz.dir());

            localdir.setRThetaPhi(1,maxtheta,FT1Phi*DEG_TO_RAD);
            locGAL=astro::SkyDir(localToCelestial*localdir,astro::SkyDir::EQUATORIAL);

            //if an event with the same phi but with theta=maxtheta had a ztheta>maxZtheta then this event is towards the earth 
            if (SCZenith.difference(locGAL)>maxZtheta) {
                hThetavsPhi_restricted->Fill(FT1Phi,FT1Theta); 
                hZTheta_towards->Fill(FT1ZenithTheta);
                hTheta_towards[ebin]->Fill(FT1Theta);
                hPhi_towards->Fill(FT1Phi);
            }
            else {
               hThetavsPhi_unrestricted->Fill(FT1Phi,FT1Theta); 
               hZTheta_away->Fill(FT1ZenithTheta);
               hTheta_away[ebin]->Fill(FT1Theta);
               hPhi_away->Fill(FT1Phi);
               //Here I want to make a different Theta distribution for different Phi ranges.
               //If you plot Theta and Phi on a 2D histogram you see that hTheta is approximately constant at phi regions

               //the diffuse class does not have much data to do phi dependence, so we fill everything in the first bin
               if (DataClass.find("DIFFUSE")!=string::npos) {
                  iphibin=0;
               }
               else {
                  if   (FT1Phi>=350) iphibin=6;
                  else iphibin = int(floor(FT1Phi/50.));
               }
               hTheta[ebin][iphibin]->Fill(FT1Theta);
            } 
            //phi is filled for all directions (both towards and away from earth)
            //because the way we rotate the spacecraft to avoid the sun, some phis are preferrentially 
            //towards the earth
            hPhi[ebin]->Fill(FT1Phi);
        }
        hThetavsB->Fill(PtB,FT1Theta);
        hPhivsB->Fill(PtB,FT1Phi);
       //if (i%1000==0) { printf("%.0f \r",100*i/(float)imax);  fflush(stdout); }
   }
   fits_close_file(fptr, &status);
  }    
  fclose (ftemp);


  sprintf(name,"%s/ThetaPhi_Fits_%s_%s.root",DataDir.c_str(),DataClassName_noConv.c_str(),ConversionName.c_str());
  TFile * fout = new TFile(name,"RECREATE");
  hThetavsB->Write("hThetavsB_UnNormalized");
    
  for (int i=0;i<=hThetavsB->GetNbinsX();i++) {
     double sum=0;
     for (int j=0;j<=hThetavsB->GetNbinsY();j++) {
       sum+=hThetavsB->GetBinContent(i,j);
     }
     for (int j=0;j<=hThetavsB->GetNbinsY();j++) {
       if (sum) hThetavsB->SetBinContent(i,j,hThetavsB->GetBinContent(i,j)/sum);
     }

     sum=0;
     for (int j=0;j<=hPhivsB->GetNbinsY();j++) {
       sum+=hPhivsB->GetBinContent(i,j);
     }

     for (int j=0;j<=hPhivsB->GetNbinsY();j++) { 
       if (sum) hPhivsB->SetBinContent(i,j,hPhivsB->GetBinContent(i,j)/sum);
     }
  }

  int ncanvas = 1 + (Energy_Bins_user+1)/9;
  TCanvas * cc[ncanvas][7],*cc_Phi[ncanvas];
  for (int i=0;i<ncanvas;i++) {
        sprintf(name,"cPhi_%d",i);
        cc_Phi[i]= new TCanvas(name,name);
        cc_Phi[i]->Divide(3,3);

        for (int j=1;j<=9;j++) {
           cc_Phi[i]->cd(j);
           int ie = i*9+j;
           if (ie<=Energy_Bins_user) {
              hPhi[ie]->Scale(1./hPhi[ie]->GetMaximum());
              hPhi[ie]->GetXaxis()->SetTitle("Azimuthal angle (deg)");
              sprintf(name,"PhiFit_%d",ie);
              hPhi[ie]->Fit(name,"0Q","");
              PhiFit[ie]->SetLineColor(2);
    
              hPhi[ie]->Draw();
              PhiFit[ie]->Draw("SAME");
              PhiFit[ie]->Write();
           }
        }
        cc_Phi[i]->Update();
        cc_Phi[i]->Write();
  }

  int iphimax=7;
  if (DataClass.find("DIFFUSE")!=string::npos) iphimax=1;
  for (int iphi=0;iphi<iphimax;iphi++) {
     for (int i=0;i<ncanvas;i++) {
        sprintf(name,"cTheta_%d_%d",i,iphi);
        cc[i][iphi]= new TCanvas(name,name);
        cc[i][iphi]->Divide(3,3);
        
        for (int j=1;j<=9;j++) {
           cc[i][iphi]->cd(j);
           int ie = i*9+j;
           if (ie<=Energy_Bins_user) {
              hTheta[ie][iphi]->Scale(1./hTheta[ie][iphi]->GetMaximum());
              hTheta[ie][iphi]->GetXaxis()->SetTitle("Zenith angle (deg)");
    

              sprintf(name,"ThetaFit_%d_%d",ie,iphi);
              //search for max theta
              double thetaCut=0;
              for (ibin=hTheta[ie][iphi]->GetNbinsX();ibin>0;ibin--) {
                   if (hTheta[ie][iphi]->GetBinContent(ibin)) {
                      thetaCut=hTheta[ie][iphi]->GetXaxis()->GetBinUpEdge(ibin);
                      break;
                   }
              }
              hTheta[ie][iphi]->Fit(name,"QB","",0,thetaCut);
              hThetaCut->SetBinContent(ie,iphi,thetaCut);

              //Normalize ThetaFit so that it maxes at 1
              double max=0;
              for (float atheta=0;atheta<80;atheta++) {
                 double aval=ThetaFit[ie][iphi]->Eval(atheta);
                 if (max<aval) max=aval;
              }
              //printf("iphi=%d ie=%d %f\n",iphi,ie,max);
              sprintf(name,"ThetaFit_%d_%d_original",ie,iphi);
              ThetaFit[ie][iphi]->Write(name);
              for (int ipar=0;ipar<ThetaFit[ie][iphi]->GetNpar();ipar++) ThetaFit[ie][iphi]->SetParameter(ipar,ThetaFit[ie][iphi]->GetParameter(ipar)/max);
              ThetaFit[ie][iphi]->Write();
           }
        }
    
        cc[i][iphi]->Update();
        cc[i][iphi]->Write();
     }
  }

  hThetavsB->Write();
  hPhivsB->Write();

  for (int i=0;i<=hThetavsPhi_unrestricted->GetNbinsX();i++) {
     double sum1=0,sum2=0;
     for (int j=0;j<=hThetavsPhi_unrestricted->GetNbinsY();j++) {
       sum1+=hThetavsPhi_unrestricted->GetBinContent(i,j);
       sum2+=hThetavsPhi_restricted->GetBinContent(i,j);
     }

     for (int j=0;j<=hThetavsPhi_unrestricted->GetNbinsY();j++) {
         hThetavsPhi_unrestricted->SetBinContent(i,j,hThetavsPhi_unrestricted->GetBinContent(i,j)/sum1);
         hThetavsPhi_restricted->SetBinContent(i,j,hThetavsPhi_restricted->GetBinContent(i,j)/sum2);
     } 
  }

  hThetavsPhi_unrestricted->Write();
  hThetavsPhi_restricted->Write();
  hZTheta_away->Write();
  hZTheta_towards->Write();
  for (int i=1;i<=Energy_Bins_user;i++) {
     hTheta_away[i]->Write();
     hTheta_towards[i]->Write();
  }
  hPhi_away->Write();
  hPhi_towards->Write();
  hThetaCut->Write();
  fPlots->Close();

  sprintf(name,"%.2f",ThetaPhiFits_version);
  TNamed * Version_TNamed = new TNamed("version",name);
  Version_TNamed->Write();

  fout->Close();
}

