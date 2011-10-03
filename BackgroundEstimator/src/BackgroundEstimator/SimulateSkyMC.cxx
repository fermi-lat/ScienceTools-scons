//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/BackgroundEstimator.h"

#include "TRandom2.h"

void BackgroundEstimator::SimulateSkyMC(Plots_Struct myPlots_Struct, TH2F * hSimulatedSky, vector <double> STARTGTI, vector <double> ENDGTI, const long int IterationsPerSec, const int iEnergy, TH2F* hMAP_ZTheta_EAzimuth) {

  hSimulatedSky->Reset();
  if (hMAP_ZTheta_EAzimuth)  hMAP_ZTheta_EAzimuth->Reset();
  
  //Make a temp buffer skymap
  TH2F htemp = TH2F("htemp","htemp",L_BINS,-180,180,B_BINS,-90,90);

  char name[2000];

  sprintf(name,"%s/EastWest_Correction_%s_%.1f.root",DataDir.c_str(),DataClass.c_str(),EastWest_version);
  static bool HaveEastWest=false;
  TH2F * hEastWest=NULL;
  TFile * fEastWest=NULL;
  fEastWest = TFile::Open(name);
  if (fEastWest) {
     sprintf(name,"hZTheta_EarthAzimuth_Ratio_TrueOverEst_%d",iEnergy);
     hEastWest = (TH2F*)fEastWest->Get(name);
     if  (!hEastWest) printf("%s: Can't find %s. Will not apply East West corrections\n",__FUNCTION__,name); 
     else HaveEastWest=true;
  }
  else  printf("%s: Can't find file %s. Will not apply East West corrections\n",__FUNCTION__,name);

  sprintf(name,"%s/RateFit_%s_%.1f.root",DataDir.c_str(),DataClass.c_str(),RateFit_version);
  TFile * fRates = TFile::Open(name);
  sprintf(name,"RateFit_%d;1",iEnergy);
  TF1 * RateFit = (TF1*)fRates->Get(name);
  TH1F * hScaleFactor =(TH1F*)fRates->Get("hScaleFactor");

  sprintf(name,"%s/ThetaPhi_Fits_%s_%.1f.root",DataDir.c_str(),DataClass.c_str(),ThetaPhiFits_version);
  TFile * fThetaPhi_Fits = TFile::Open(name);


  bool DIFFUSE_CLASS=false; //for the diffuse class we don't do phi dependence
  TF1* ThetaFit[7];
  for (int iphi=0;iphi<7;iphi++) {
     sprintf(name,"ThetaFit_%d_%d",iEnergy,iphi);
     ThetaFit[iphi] = (TF1*)fThetaPhi_Fits->Get(name);
     if (iphi==0 && !ThetaFit[iphi]) {printf("%s: Can't find fit %s\n",__FUNCTION__,name); exit(1);}
     else if (!ThetaFit[iphi]) {DIFFUSE_CLASS=true; break;}
  }
  TH2F * hThetaCut = (TH2F*)fThetaPhi_Fits->Get("hThetaCut");
  float ThetaCut[7]; 
  int iphimax=7;
  if (DIFFUSE_CLASS) iphimax=1;
  for (int i=0;i<iphimax;i++) ThetaCut[i]=hThetaCut->GetBinContent(iEnergy,i);
  
  sprintf(name,"PhiFit_%d",iEnergy);
  TF1* PhiFit = (TF1*)fThetaPhi_Fits->Get(name);
  double PhiFitMax = PhiFit->GetMaximum(0,360);
  float FT1L;

  const double TimeStep = myPlots_Struct.hMcIlwainLvsTime->GetXaxis()->GetBinWidth(1);
  double AllSkyRate;
  float McIlwainL;
  TRandom2 tRand;
  tRand.SetSeed(0);

  double rand;
  float FT1Phi,FT1Theta;
  astro::SkyDir SCz,SCx,SCZenith;
  CLHEP::HepRotation inverse;
  CLHEP::Hep3Vector LocalDir;
  astro::SkyDir local;
  float PtRaz,PtRax,PtDecz,PtDecx,RAZenith,DecZenith,FT1ZenithTheta;
  //float PtL,PtB;
  const double FT1ZenithTheta_Cut_Rad=FT1ZenithTheta_Cut*DEG_TO_RAD;
  unsigned short int nev;
  const long int TimeBins=myPlots_Struct.hMcIlwainLvsTime->GetNbinsX();
  HepRotation localToCelestial;
  localToCelestial.setTolerance(0.1);
  double TIME;
  unsigned int igti=0;
  short int iphibin;
  float RockingAngle,ScaleFactor;
  int RockingAngleBin;
  const int Sec_per_flush=100;
  bool BailOut=false;
  for (long long int i=1;i<=TimeBins && BailOut==false;i++) { //make sure you go up to i==TimeBins so that the East-West effect is applied for the last chunk
      //1.CHECK IF WE ARE IN A GTI
      TIME=myPlots_Struct.hMcIlwainLvsTime->GetBinCenter(i);
      //if (i%1000000==0) {printf("%.3f   %f - %f   \r",i/float(TimeBins),TIME,ENDGTI[ENDGTI.size()-2]);fflush(0);}

      if (TIME>ENDGTI.back()) BailOut=true;
      if (TIME>=STARTGTI[igti+1] && TIME<=ENDGTI[igti+1]) igti++; //moved to next gti

      if (TIME>=STARTGTI[igti] && TIME<=ENDGTI[igti]) {//don't use this with continue because it might miss some flushing
         //printf("accept i=%ld gti=%d time=%f %f %f\n",i,igti,TIME,STARTGTI[igti],ENDGTI[igti]);
         if (igti>=STARTGTI.size()) {printf("heyyyy %d %d\n",igti,(int)STARTGTI.size()); exit(1);}

          //2.CALCULATE RATE    
          RockingAngle = myPlots_Struct.hRockingAnglevsTime->GetBinContent(i);
          if (RockingAngle<=0) {
             printf("%s: rocking angle weird rock=%f i=%lld  iEnergy=%d time=%f\n",__FUNCTION__,RockingAngle, i,iEnergy,TIME); 
             printf("%s: GTI data start\end: %f %f\n",__FUNCTION__,STARTGTI[igti],ENDGTI[igti]);
             exit(1);
          }
          RockingAngleBin=hScaleFactor->GetXaxis()->FindBin(RockingAngle);
          ScaleFactor = hScaleFactor->GetBinContent(RockingAngleBin,iEnergy);
          if (ScaleFactor<=0) {printf("%s: Scalefactor=%f, rocking angle=%f TIME=%f i=%lld gti=%d %f/%f\n",__FUNCTION__,ScaleFactor,RockingAngle,TIME,i,igti,STARTGTI[igti],ENDGTI[igti]); ScaleFactor=0;}
          McIlwainL=myPlots_Struct.hMcIlwainLvsTime->GetBinContent(i);
          if (!McIlwainL) {
              printf("%s: MCIlwainL=0? bin=%lld time=%f\n",__FUNCTION__,i,TIME);
              exit(1);
          }
    
          AllSkyRate = RateFit->Eval(McIlwainL)*ScaleFactor;
          //printf("%d %d %f %f\n",i,TimeBins,ScaleFactor,AllSkyRate);
          //3.PREPARE VECTORS
          PtRaz =  myPlots_Struct.hPtRazvsTime->GetBinContent(i);
          PtRax =  myPlots_Struct.hPtRaxvsTime->GetBinContent(i);
          PtDecz = myPlots_Struct.hPtDeczvsTime->GetBinContent(i);
          PtDecx = myPlots_Struct.hPtDecxvsTime->GetBinContent(i);
          //if (PtRaz==0 || PtRax==0 || PtDecz==0 || PtDecx==0){ printf("%s: there is a gap? %d %f %f %f %f \n",__FUNCTION__,i,PtRaz,PtRax,PtDecz,PtDecx); exit(1);}
    
          SCz = astro::SkyDir(PtRaz,PtDecz,astro::SkyDir::EQUATORIAL);
          SCx = astro::SkyDir(PtRax,PtDecx,astro::SkyDir::EQUATORIAL);
          //printf("mc=%f allskyrate=%f\n",McIlwainL,AllSkyRate);
          localToCelestial = HepRotation(SCx.dir(),(SCz.dir()).cross(SCx.dir()),SCz.dir());
    
          RAZenith = myPlots_Struct.hRAZenithvsTime->GetBinContent(i);
          DecZenith= myPlots_Struct.hDecZenithvsTime->GetBinContent(i);
          SCZenith = astro::SkyDir(RAZenith,DecZenith,astro::SkyDir::EQUATORIAL);
    
          //float RockingAngle=SCZenith.difference(SCz)/DEG_TO_RAD;

          if (RAZenith==0 || DecZenith==0) {printf("%s: FT2 has a gap! %lld time=%f %f %f\n",__FUNCTION__,i,myPlots_Struct.hRAZenithvsTime->GetBinCenter(i),RAZenith,DecZenith); exit(1);}
          for (long int iter=0;iter<IterationsPerSec;iter++) {
             nev = tRand.Poisson(AllSkyRate*TimeStep);
             for (int iev=0;iev<nev;iev++) {
                do {
                     //Sample a random phi (this depends on just the phi)
                     //Using acception-rejection method with PhiFit as the PDF
                     do {
                         FT1Phi = tRand.Uniform()*360;
                         rand = tRand.Uniform();
                     } while (rand>(PhiFit->Eval(FT1Phi)/PhiFitMax));
                     if (!DIFFUSE_CLASS) {//for !DIFFUSE class
                     if   (FT1Phi>=350) iphibin=6;
                        else iphibin = (int)(floor(FT1Phi/50.));
                     }
                     else iphibin=0; //TRANSIENT class -- no phi dependence
                     //Sample a theta (this depends on both the Energy and Phi)
                     do {
                         FT1Theta = tRand.Uniform()*ThetaCut[iphibin]; //Only allow theta to go up to thetaCut
                         rand = tRand.Uniform();
                         //printf("%f theta=%f bin=%d %d %f\n",rand,FT1Theta,hThetaHist[0]->FindBin(FT1Theta),iphibin,hThetaHist[iphibin]->GetBinContent(hThetaHist[0]->FindBin(FT1Theta)));
                     } while (rand>(ThetaFit[iphibin]->Eval(FT1Theta)));
                     LocalDir.setRThetaPhi(1,FT1Theta*DEG_TO_RAD,FT1Phi*DEG_TO_RAD);
                     //Hep3Vector dir(LocalDir);
                     local = astro::SkyDir(localToCelestial*LocalDir,astro::SkyDir::EQUATORIAL);
    
                  } while ((FT1ZenithTheta=SCZenith.difference(local))>FT1ZenithTheta_Cut_Rad);
    
                  FT1L=local.l();
                  if (FT1L>180) FT1L-=360;
                  htemp.Fill(FT1L,local.b());

                  if (hMAP_ZTheta_EAzimuth && fabs(local.b())>20) {  //East-West Studies stuff
                       Hep3Vector north_pole(0,0,1);
                       Hep3Vector east_dir(north_pole.cross(SCZenith()).unit() ); // east is perp to north_pole and SCZenith
                       Hep3Vector north_dir(SCZenith().cross(east_dir));
                       float azimuth=atan2( local().dot(east_dir), local().dot(north_dir) ); // z is north, heading.
                       if( azimuth <0) azimuth += 2*M_PI; // to 0-360 deg.
                       azimuth/=DEG_TO_RAD;
                       double x=sin(azimuth*TMath::Pi()/180.)*FT1ZenithTheta/DEG_TO_RAD;
                       double y=cos(azimuth*TMath::Pi()/180.)*FT1ZenithTheta/DEG_TO_RAD;
                       hMAP_ZTheta_EAzimuth->Fill(x,y);
                  } 
                  
            } //for iev
         }//for iter
          //This part of the code applies the East-West effect corrections to the simulated segment and moves the simulated
          //segment to the final simulated skymap
      }
      if (HaveEastWest && i && (i%Sec_per_flush==0 || i==TimeBins || BailOut)) { //correct and add htemp to simulatedsky
           //printf("flush htemp i=%ld timebins=%ld\n",i,TimeBins);
           int imid;
           if (i!=TimeBins) imid=i-Sec_per_flush/2;
           else         imid=i;
           RAZenith = myPlots_Struct.hRAZenithvsTime->GetBinContent(imid);
           DecZenith= myPlots_Struct.hDecZenithvsTime->GetBinContent(imid);
           SCZenith = astro::SkyDir(RAZenith,DecZenith,astro::SkyDir::EQUATORIAL);
           for (int ix=1;ix<=L_BINS;ix++) {
                float L=htemp.GetXaxis()->GetBinCenter(ix);
                for (int iy=1;iy<=B_BINS;iy++) {
                    float bc=htemp.GetBinContent(ix,iy);
                    if (!bc) continue;
                    float B=htemp.GetYaxis()->GetBinCenter(iy);
                    astro::SkyDir dirBin = astro::SkyDir(L,B,astro::SkyDir::GALACTIC);
                    float ztheta=SCZenith.difference(dirBin);
                    if (ztheta>FT1ZenithTheta_Cut_Rad) continue;
                    float rabin,decbin;
                    TOOLS::unGalactic(L,B, &rabin, &decbin);
                    float EarthAzimuth=DEG_TO_RAD*TOOLS::GimmeEarthAzimuth(rabin,decbin,RAZenith,DecZenith, astro::SkyDir::EQUATORIAL);
                    ztheta*=RAD_TO_DEG;
                    double x=sin(EarthAzimuth)*ztheta;
                    double y=cos(EarthAzimuth)*ztheta;
                    int EWBin=hEastWest->FindBin(x,y);
                    float EWCorrection = hEastWest->GetBinContent(EWBin);
                    if (EWCorrection==0) EWCorrection=1;
                    htemp.SetBinContent(ix,iy,bc*EWCorrection);
                }
           }
           hSimulatedSky->Add(&htemp);
           htemp.Reset();
           //printf("done\n");
    }
  } //for timebins
  
  fRates->Close();
  fThetaPhi_Fits->Close();
  if (HaveEastWest) fEastWest->Close();
  else hSimulatedSky->Add(&htemp);
  hSimulatedSky->Scale(1./IterationsPerSec);
  if (hMAP_ZTheta_EAzimuth) hMAP_ZTheta_EAzimuth->Scale(1./IterationsPerSec);
}

