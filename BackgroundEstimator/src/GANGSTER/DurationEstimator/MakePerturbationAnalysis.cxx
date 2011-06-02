//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#include "BackgroundEstimator/DurationEstimator.h"

//returns -1 if bad, 0 if good!
int DurationEstimator::PerformPerturbedEstimation( double * q_T95, double *q_T05, double *q_T90, double *q_Plateau, double *q_DetTotal, int iterations) {
 const float Min_Plateau_Duration=40;

 double * Effective_Area = NULL;
 if (WeighByExposure) Effective_Area = gEffective_Area_fine->GetY();
 double * IntBkg_fine  = gIntBkg_fine->GetY();
 double * IntDet_fine  = gIntDet_fine->GetY();
 double * TMIDDLE_fine = gIntBkg_fine->GetX();

 const int nIntervals_fine  = gIntBkg_fine->GetN();

 double * T95_Fluctuated = new double[iterations];
 double * T05_Fluctuated = new double[iterations];
 double * T90_Fluctuated = new double[iterations];
 double * Plateau_Fluctuated= new double[iterations];
 double * DetTotal_Fluctuated= new double[iterations];


 vector <double> Det,Bkg;
 Det.reserve(nIntervals_fine);
 Bkg.reserve(nIntervals_fine);
 for (int i=0;i<nIntervals_fine;i++) {
    if (i==0) {
        Det.push_back(IntDet_fine[i]);
	Bkg.push_back(IntBkg_fine[i]);
    }
    else {
        Det.push_back(IntDet_fine[i]-IntDet_fine[i-1]);
	Bkg.push_back(IntBkg_fine[i]-IntBkg_fine[i-1]);    
        //printf("%d det:%e %e %e exp:%e %e %e\n",i, IntDet_fine[i]-IntDet_fine[i-1],IntDet_fine[i],IntDet_fine[i-1], IntBkg_fine[i]-IntBkg_fine[i-1],IntBkg_fine[i],IntBkg_fine[i-1]);
    }
 }

 TRandom2 t;
 t.SetSeed(0);
 
 unsigned int aborts=0;

 for (int i=0;i<iterations;i++) {

    FailedFraction=float(aborts)/(aborts+i);
    if (FailedFraction>0.5 && aborts>1000){ 
         printf("%s: too many failed simulations... Will not estimate the duration...\n",__FUNCTION__); 
         delete [] T95_Fluctuated;
         delete [] T05_Fluctuated ;
         delete [] T90_Fluctuated ;
         delete [] Plateau_Fluctuated;
         delete [] DetTotal_Fluctuated;
         return -1;
   }
  
   double Diff_Fluctuated[nIntervals_fine];
   if (i%200==0) {printf("Done:%.1f Failed:%.2lf (%d)  \r",100*(float)i/iterations,FailedFraction,aborts); fflush(0);}
 
   for (unsigned int ii=0;ii<Det.size();ii++) {
       Diff_Fluctuated[ii]=t.Poisson(Det[ii])-Bkg[ii]; 
   }

   //create a cumulative lightcurve that does not have any possible exposure weighting to calculate the plateau
   double IntDiff_Fluctuated[nIntervals_fine],IntDet_Fluctuated[nIntervals_fine];
   for (int ii=0;ii<nIntervals_fine;ii++) {
       if (ii==0) IntDiff_Fluctuated[ii]=Diff_Fluctuated[ii]; 
       else       IntDiff_Fluctuated[ii]=IntDiff_Fluctuated[ii-1]+Diff_Fluctuated[ii];
       IntDet_Fluctuated[ii]=IntDiff_Fluctuated[ii]+IntBkg_fine[ii];
   }
   TGraph * gDiff_Fluctuated = new TGraph(nIntervals_fine,TMIDDLE_fine,IntDiff_Fluctuated); 
   TGraph * gDet_Fluctuated  = new TGraph(nIntervals_fine,TMIDDLE_fine,IntDet_Fluctuated); 
   
   //find plateau
   double Plateau_start,Plateau_stop;
   if (!FindPlateau(gDiff_Fluctuated, gDet_Fluctuated, gIntBkg_fine, Plateau_start, Plateau_stop,(const float)Min_Plateau_Duration)) {
       //printf("%s: Can't find plateau\n",__FUNCTION__);
       delete gDiff_Fluctuated;
       delete gDet_Fluctuated;  
       i--; aborts++; continue;
   }
   Plateau_Fluctuated[i]=gDiff_Fluctuated->Eval(Plateau_start);//calculate the plateau value.
   if (Plateau_Fluctuated[i]<5) { //plateau too low!
      //printf("%s: Can't find plateau\n",__FUNCTION__);
       delete gDiff_Fluctuated;
       delete gDet_Fluctuated;  
       i--; aborts++; continue;
   } 
   
   //now we have a plateau. Will apply exposure weighting if needed and recreate the cumulative graph
   if (WeighByExposure) {
       for (int ii=0;ii<nIntervals_fine;ii++) {
           if (Effective_Area[ii]==0) Diff_Fluctuated[ii]=0; 
           else                       Diff_Fluctuated[ii]/=Effective_Area[ii];
           if (ii==0) IntDiff_Fluctuated[ii]=Diff_Fluctuated[ii];
           else       IntDiff_Fluctuated[ii]=IntDiff_Fluctuated[ii-1]+Diff_Fluctuated[ii];
       }
       delete gDiff_Fluctuated;
       gDiff_Fluctuated = new TGraph(nIntervals_fine,TMIDDLE_fine,IntDiff_Fluctuated); 
       Plateau_Fluctuated[i]=gDiff_Fluctuated->Eval(Plateau_start);//calculate the plateau value.
   }

   
   if (FindT90(gDiff_Fluctuated, T05_Fluctuated[i], T95_Fluctuated[i], Plateau_Fluctuated[i])) { 
       /*
       cCoarse->cd(2);
       cCoarse->GetPad(2)->SetLogy(0);
       gDiff_Fluctuated.Draw("AC");
       printf("failed\n");
       cCoarse->Update();
       getchar();
       */
       delete gDiff_Fluctuated;
       delete gDet_Fluctuated;  
       i--;  aborts++; continue; //retry..
   }

   T90_Fluctuated[i] = T95_Fluctuated[i]-T05_Fluctuated[i];
   //printf("%d %f %f %f\n",i,T95_Fluctuated[i],T05_Fluctuated[i],T90_Fluctuated[i]);
   DetTotal_Fluctuated[i] = IntDiff_Fluctuated[nIntervals_fine-1];
 
   /*
   printf("%d %f %f %f\n",i,T05_Fluctuated[i],T95_Fluctuated[i],Plateau_Fluctuated[i]);
   cCoarse->cd(2);
   cCoarse->GetPad(2)->SetLogy(0);
   gDiff_Fluctuated.Draw("AC");
   TLine l = TLine(T05_Fluctuated[i],0,T05_Fluctuated[i],1000); l.Draw("SAME");
         l = TLine(T95_Fluctuated[i],0,T95_Fluctuated[i],1000); l.Draw("SAME");
	 l = TLine(Plateau_start,Plateau_Fluctuated[i],Plateau_stop,Plateau_Fluctuated[i]);l.SetLineColor(2);l.SetLineWidth(1.5);l.Draw("SAME");
   cCoarse->GetPad(2)->Modified();
   cCoarse->Update();
   getchar();
   */
   delete gDiff_Fluctuated;
   delete gDet_Fluctuated;  


 }
 
 double p[3]={0.15866,0.5,0.84134};
 TMath::Quantiles(iterations,3,T95_Fluctuated,q_T95,p,false);
 TMath::Quantiles(iterations,3,T05_Fluctuated,q_T05,p,false);
 TMath::Quantiles(iterations,3,T90_Fluctuated,q_T90,p,false);
 TMath::Quantiles(iterations,3,Plateau_Fluctuated,q_Plateau,p,false);
 TMath::Quantiles(iterations,3,DetTotal_Fluctuated,q_DetTotal,p,false);

 char name[100];
 if (WeighByExposure)   sprintf(name,"%s Durations using exposure weighting",TOOLS::GetS("GRB_NAME").c_str());
 else                   sprintf(name,"%s Durations without performing exposure weighting",TOOLS::GetS("GRB_NAME").c_str());
 
 hT95 = new TH1F("hT95","hT95",100,TMath::MinElement(iterations,T95_Fluctuated)*0.9,TMath::MaxElement(iterations,T95_Fluctuated));
 hT05 = new TH1F("hT05","hT05",100,TMath::MinElement(iterations,T05_Fluctuated)*0.9,TMath::MaxElement(iterations,T05_Fluctuated));
 hT90 = new TH1F("hT90","hT90",100,TMath::MinElement(iterations,T90_Fluctuated)*0.9,TMath::MaxElement(iterations,T90_Fluctuated));
 hPlateau = new TH1F("hPlateau","hPlateau",80,TMath::MinElement(iterations,Plateau_Fluctuated),TMath::MaxElement(iterations,Plateau_Fluctuated));
 hDetTotal = new TH1F("hDetectedTotal","hDetectedTotal",80,TMath::MinElement(iterations,DetTotal_Fluctuated),TMath::MaxElement(iterations,DetTotal_Fluctuated));
 for (int i=0;i<iterations;i++) {
     hT95->Fill(T95_Fluctuated[i]);
     hT05->Fill(T05_Fluctuated[i]);
     hT90->Fill(T90_Fluctuated[i]);
     hPlateau->Fill(Plateau_Fluctuated[i]);
     hDetTotal->Fill(DetTotal_Fluctuated[i]);
 }

 delete [] T95_Fluctuated;
 delete [] T05_Fluctuated ;
 delete [] T90_Fluctuated ;
 delete [] Plateau_Fluctuated;
 delete [] DetTotal_Fluctuated;
 /*
 cDuration1->Update();
 printf("next\n");
 getchar();
 */
 return 0;
}

/*
int GimmeASignal(double B,double N) {
    static TRandom2 t;
    int min=0;
    int max=std::max(4*N,(N-B)*3);
    int aS=0;
    while (1){
        aS=min+t.Integer(max+1);
        double C=0;
        for (int an=0;an<=N;an++) C+=exp(-B)*pow(B,an)/TMath::Factorial(an);
        double aProb=exp(-(aS+B))*pow(aS+B,N)/TMath::Factorial(N)*C;
    	if (t.Uniform()<aProb) break;
    }
    return aS;
}
*/
