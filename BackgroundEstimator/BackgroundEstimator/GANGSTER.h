//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#ifndef GANGSTER_h
#define GANGSTER_h

#include "BackgroundEstimator.h"


//!
namespace GANGSTER{
    ///Calculate the background map
    int CalculateBackground(string Interval_name, double MET, double DURATION, string FT1_FILE, string FT2_FILE, string DATACLASS, double Energy_Min_user, double Energy_Max_user, int Energy_Bins_user, float FT1ZenithTheta_Cut, int verbosity=1,bool Calc_Residual=true);
    ///Integrate the background map over the ROI
    string PlotBackground(string Interval_name, double MET, double DURATION, string FT1_FILE, string FT2_FILE, string DATACLASS, double Energy_Min_user, double Energy_Max_user, int Energy_Bins_user, float FT1ZenithTheta_Cut,  bool OverwritePlots=true, int verbosity=1, double MET_FOR_THETA=-1, bool Save_Earth_Coo_Map=false);
    int MakeGtLikeTemplate(float gtlike_ROI, string GRB_DIR, string DATACLASS, float ZTheta_Cut, double &GALGAMMAS_BKG, double &CR_EGAL_BKG, int verbosity=1);
    ///Plot a bkg-subtracted light curve et al.
    void PlotAfterglow(double GRB_TRIGGER_TIME, string TIMEDATA_FILE, string DATACLASS, int MinBin=-1, int MaxBin=-1, float a=-2.0);
    void Calc_TimeCorrectionFactors(vector<string> GRB_folders, vector  <double> METs, vector <double> GRB_L, vector <double> GRB_B,  string Dataclass, double MinE, double MaxE, int NBins, const int Max_iE_For_Correction=25);

    
};

#endif

