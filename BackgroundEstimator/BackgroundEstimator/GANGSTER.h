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
    string PlotBackground(string Interval_name, double MET, double DURATION, string FT1_FILE, string FT2_FILE, string DATACLASS, double Energy_Min_user, double Energy_Max_user, int Energy_Bins_user, float FT1ZenithTheta_Cut,  bool OverwritePlots=true, int verbosity=1, double MET_FOR_THETA=-1);
    int MakeGtLikeTemplate(float gtlike_ROI, string GRB_DIR, string DATACLASS, float ZTheta_Cut, double &GALGAMMAS_BKG, double &CR_EGAL_BKG, int verbosity=1);
    int MakeCRBackground_LC(string FT1_FILE, string FT2_FILE, string DATACLASS, string TIMEDATA_FILE,bool OverwritePlots, int verbosity, float gtlike_ROI);
    ///Plot a bkg-subtracted light curve et al.
    void PlotAfterglow(double GRB_TRIGGER_TIME, string TIMEDATA_FILE, string DATACLASS, int MinBin=-1, int MaxBin=-1, float a=-2.0);
    
};

#endif

