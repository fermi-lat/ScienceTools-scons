/*
double WeightedP(int nev,double bkg,double dbkg);
double ProbToSigma(double Prob);

double DecomposeError(int ent0, double Back0[], double MeasSig0[], bool Plot){
Plot=false;
 gSystem->Load("libMathCore");
 const int bins=100;
 const int binss=100;

 double MeasSig[4000],Back[4000];
 int ent=0;

 for (int i=0;i<ent0;i++) {
    if (Back0[i]>20){
       MeasSig[ent]=MeasSig0[i];
       Back[ent]=Back0[i];
       ent++;       
    }
 }

 if (ent<10) {printf ("return\n"); return 1;}

 int max=-1, min=999;
 for (int i=0;i<ent;i++) {
   if (MeasSig[i]>max) {max=MeasSig[i];}
   if (MeasSig[i]<min) {min=MeasSig[i];}
 }
 printf("ent=%d min/max= %d/%d ,",ent,min,max);


 TH1F * hBack, * hSig, * hSigma,* hProbFlat, * hProbC,
      * hProbCG,*hProb;

 TCanvas * cc_cc = NULL;
 if (Plot) {
    cc_cc= new TCanvas("cD123","cD123",1024,768);
    cc_cc->Divide(2,3);
 }

 hProb = new TH1F("hProb","Probability distribution",binss,0,1);
 hProb->GetXaxis()->SetTitle("Probability");
 hProbFlat = new TH1F("hProbFlat","hProbFlat",binss,0,1);
 if (Plot) {
   hBack = new TH1F("hBack","Background Estimates",bins,min,max);
   hBack->GetXaxis()->SetTitle("Estimated number of bkg events");
   hSig = new TH1F("hSig","Signal distributions",bins,min,max);
   hSig->GetXaxis()->SetTitle("Number of signal events");
   hSigma = new TH1F("hSigma","Significance distribution",100,-10,10);
   hSigma->GetXaxis()->SetTitle("Significance (sigma)");
   hSigma->GetYaxis()->SetTitle("dN/dP");
   hProbC = new TH1F("hProbC","hProbC",binss,0,1);
   hProbC->GetXaxis()->SetTitle("Probability P'");
   hProbC->GetXaxis()->SetTitle("Number of events with P<P'");
   hProbCG = new TH1F("hProbCG","hProbCG",binss,0,1);
  }

 //Simulate
 for (int i=0;i<ent;i++) {
     if (MeasSig[i] && Plot) {
       hSig->Fill(MeasSig[i]);
       hBack->Fill(Back[i]);
     }
 }

 TGraph * gQ;
 double ERROR[100],Q[100];
 int nerrors=0;
 float BestQ=0,BestProb=0;
 for (double anerror=1e-5;anerror<0.2;anerror+=0.01) {
    hProb->Reset();
    if (Plot) hSigma->Reset();
    double Sigma;
    for (int i=0;i<ent;i++) {
        Sigma=anerror*Back[i];
        double CorrectedIntP=0;
      
        int iback;
        iback=int(Back[i]+0.5);
        if (MeasSig[i]<=iback)for (int isig=MeasSig[i];isig<iback;isig++)       CorrectedIntP+=WeightedP(isig,Back[i],Sigma);
        else                  for (int isig=iback+1;isig<=MeasSig[i];isig++)    CorrectedIntP+=WeightedP(isig,Back[i],Sigma);
        CorrectedIntP*=2;
        CorrectedIntP+=WeightedP(iback,iback,Sigma);
   
        if (CorrectedIntP>1) CorrectedIntP=0.99;
        if (CorrectedIntP<=0) {
           printf("corp=%e MeasSig[i]=%.1f  back=%f iback=%d\n",CorrectedIntP,MeasSig[i],Back[i],iback); //check to see if denominator is ~1 as it should be
           CorrectedIntP=1e-1;     
        }

        CorrectedIntP=1-CorrectedIntP;
        hProb->Fill(CorrectedIntP);  
        if (Plot) {
           if (MeasSig[i]<Back[i]) CorrectedIntP*=-1;
           hSigma->Fill(ProbToSigma(CorrectedIntP));
        }
     }
     for (int ib=1;ib<=hProbFlat->GetNbinsX();ib++) hProbFlat->SetBinContent(ib,(float)ent/hProbFlat->GetNbinsX());
     
     if (Plot) {
        cc_cc->cd(1);
        cc_cc->GetPad(1)->SetLogy();
        hSig->Draw();
    
        hBack->SetLineColor(3);
        hBack->Draw("SAME"); 

        TLegend * l = new TLegend(0.7,0.8,0.9,0.9);
        l->AddEntry(hSig,"Measured signal","l");
        l->AddEntry(hBack,"Background","l");
        l->Draw();

        cc_cc->cd(2); 
        hProb->SetMinimum(0);
        hProb->Draw();
        hProbFlat->Draw("SAME");
    
        cc_cc->cd(3);
     }
     
     double X[binss],Y[binss],YGOOD[binss],XGOOD[binss];
     int nintbins=0;
     for (int ii=0;ii<binss;ii++) {
         if (!hProb->GetBinContent(ii) && ii!=0 ) continue;
         XGOOD[nintbins]=hProb->GetBinCenter(ii);
         YGOOD[nintbins]=ent*(hProb->GetXaxis()->GetBinWidth(1))*(ii);
         X[nintbins]=hProb->GetBinCenter(ii);
         Y[nintbins]=hProb->Integral(0,ii);
         nintbins++;
      }
  
      TGraph * gCum = new TGraph(nintbins,X,Y);
      gCum->SetMarkerStyle(4);
      gCum->SetMarkerSize(0.2);
      gCum->SetMinimum(YGOOD[0]);
      gCum->GetXaxis()->SetTitle("Probability P'");
      gCum->GetYaxis()->SetTitle("Number of events with P<P'");


      TGraph * gCumG = new TGraph(nintbins,XGOOD,YGOOD);
      gCumG->SetMarkerStyle(2);
      gCumG->SetMarkerSize(0.2);
      gCumG->SetMarkerColor(2);
      
      if (Plot) {
        gCum->Draw("AP"); 
        gCumG->Draw("PSAME"); 
     
        cc_cc->cd(4);
        //cc_cc->GetPad(4)->SetLogy();
        TF1 * fitgaus= new TF1("fit","gaus");
        fitgaus->SetParameters(1,0,1);
        fitgaus->FixParameter(1,0);
        fitgaus->FixParameter(2,1);
        hSigma->Fit("fit","BQ");
      }
      
      ERROR[nerrors]=anerror;
      Q[nerrors]=TMath::KolmogorovTest(nintbins,YGOOD,nintbins,Y,"");
      if (Q[nerrors]>BestQ) {BestQ=Q[nerrors];BestProb=anerror;}
      gQ = new TGraph(nerrors+1,ERROR,Q);
      gQ->SetTitle("Kolmogorov-test result");
      gQ->GetXaxis()->SetTitle("Test systematic error on the bkg estimate");
      gQ->GetYaxis()->SetTitle("Kolmogorov-test result");
      
      if (Plot){ cc_cc->cd(5); gQ->Draw("A*"); }
      //printf("%f\n",anerror,Q[nerrors]);
      if (Plot){ cc_cc->Update();  cc_cc->SaveAs("error.gif+");}
      nerrors++;
   }

   TF1 * fit = new TF1("fit","gaus");
   gQ->Fit("fit","Q0","",BestProb/2.,BestProb*2);
   if (Plot) cc_cc->Update(); 
  // printf("error=%.2f bestQ=%.1f\n",fit->GetParameter(1),BestQ);
 //  getchar();
 if (Plot) {
   delete  hBack;
   delete hSig;
   delete hSigma;

   delete hProbC;
   delete  hProbCG;
   
 }
   delete hProb;
   delete  hProbFlat;
   return fit->GetParameter(1);

}   

double ProbToSigma(double Prob) {

  bool neg=false;
  short int fac=1;
  if (Prob<0) {fac=-1; Prob*=-1;}
  Prob=1-Prob;
  double sigma=0,aprob=0;
  double dsigma=0.01;
 
  while (1){
     aprob = 2*ROOT::Math::gaussian_cdf_c(sigma);
     if (aprob<=Prob) break;
     //printf("sigma=%f Prob=%f aprob=%f\n",sigma,Prob,aprob);
     sigma+=dsigma;         
  };
  return sigma*fac;

}



double WeightedP(int nev,double bkg,double dbkg) {

    double PSum=0,WSum=0;
    double Start=bkg-3*dbkg;
    if (Start<0) Start=0;
    double daback=dbkg/10.;
    double weight,ap;
    for (double aback=Start;aback<=bkg+3*dbkg;aback+=daback) {
       weight = ROOT::Math::gaussian_pdf(aback,dbkg,bkg);
       ap = ROOT::Math::poisson_pdf(nev,aback);
       PSum+=weight*ap*daback;
       WSum+=weight*daback;
    }
    if (WSum<0.8 || WSum>1) {printf("wsum=%e %d %e %e \n",WSum,nev,bkg,dbkg); exit(1);}
    return PSum/WSum;

}
*/