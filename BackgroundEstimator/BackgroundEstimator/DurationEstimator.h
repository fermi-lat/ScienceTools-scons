//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#ifndef DURATION_ESTIMATOR_h
#define DURATION_ESTIMATOR_h

#include "GANGSTER.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TRandom2.h"
#include "TLine.h"
#include "TFeldmanCousins.h"
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include "TLegend.h"
#include "TMath.h"
#include "TPaveText.h"
#include "Math/ProbFuncMathCore.h"
#include <algorithm>

class DurationEstimator {
    public:
	DurationEstimator(string aFT1_FILE, string aFT2_FILE, string aDATACLASS, double aGRB_TRIGGER_TIME, string aGRB_NAME, double Emin=-1, double Emax=-1, int verbosity=1, bool WeighByExposure=false, bool JumpGTIs=false, bool overwrite=false);
	~DurationEstimator(){};
        int CalculateLATT90();
        int PerformPerturbedEstimation( double * q_T95, double *q_T05, double *q_T90, double *q_Plateau, double *q_DetTotal, int iterations);
	int FindT90(TGraph * gDiff, double &T05, double &T95, double Plateau);
	bool FindPlateau(TGraph* gIntDiff, TGraph* gIntDet, TGraphErrors *gIntBkg, double &Plateau_start, double &Plateau_stop, float min_plateau_duration, float GTI_Offset_0=0, bool quick=true);
	double FailedFraction;
	int Iterations;
	TCanvas *cCoarse, *cExtras, *cDuration1;
	TGraphErrors *gIntDiff_fine, *gIntDet_fine, *gIntBkg_fine, *gFluence_fine;
	TGraphErrors *gIntDet, *gIntBkg, *gDiff, *gDets, *gBkgs, *gEffective_Area, *gEffective_Area_fine;
	TGraphAsymmErrors *gFlux;
	char ResultsRootFilename[500];
	TH1F * hT95,*hT05, *hT90, *hPlateau, *hDetTotal;
	float FT1ZenithTheta_Cut;

    private:
        int EvaluateBins_Fine(vector <TH1F*> &hROI, vector <double>& TSTART, vector <double>& TSTOP);
        int EvaluateBins_Coarse(vector <TH1F*> &hROI, vector <double>& TSTART, vector <double> &TSTOP, vector <TFile*> fBkg, float & GTI_Offset_0);
   	void Plot(TLine * l, int color);
   	int verbosity;
   	bool WeighByExposure,JumpGTIs,Overwrite;
	string FT1_FILE, FT2_FILE, DATACLASS, GRB_NAME;
   	double MIN_ENERGY,MAX_ENERGY,GRB_TRIGGER_TIME;
    
};



#endif


