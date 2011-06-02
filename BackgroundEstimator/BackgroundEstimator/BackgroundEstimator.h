//Author: Vlasios Vasileiou <vlasisva@gmail.com>
// $Header$
#ifndef _BackgroundEstimator_H
#define _BackgroundEstimator_H

#include "BKGE_Tools.h"

//this enum helps with monitoring the different data formats
typedef enum {DATA_FORMAT_P6_OLD,DATA_FORMAT_P6_NEW,DATA_FORMAT_P7};

/// LAT Background estimation for transient events
class BackgroundEstimator{

  public:
    BackgroundEstimator(string aClass, double EMin=-1, double EMax=-1, int EBins=-1, float ZTheta_Cut=-1, bool initialize=true, bool ShowLogo=true);
    ~BackgroundEstimator();
    ///Create the data files used for the background estimation. Normal users don't need to run that
    void CreateDataFiles(string FitsAllSkyFilesList, string FT2_FILE); 

    ///Calculate a background skymap. This is the first part of the bkg estimation
    int Make_Background_Map(string FT1_FILE, string FT2File, string GRB_DIR, double Burst_t0, double Burst_Dur,const double Iterations, int verbosity=1, bool Calc_Residual=true); 

    ///Integrate a background map over the ROI to produce the final bkg estimate
    int FillBackgroundHist(string GRB_DIR, TH1F * hROI, double par1, double par2, int CoordType, short int type, int verbosity=0);

    ///Min and max energy in MeV of the datafiles
    double Energy_Min_datafiles, Energy_Max_datafiles;
    ///Number of logarithmically-spaced bins for the bkg estimate in the datafiles
    int Energy_Bins_datafiles;
    ///Min and max energy in MeV of the user
    double Energy_Min_user, Energy_Max_user;
    ///Number of logarithmically-spaced bins for the bkg estimate of the user
    int Energy_Bins_user;
    ///Zenith Theta cut -- can be used to override the default cut
    float FT1ZenithTheta_Cut;
    ///Flag that shows if the user is using the energy binning of the data files (set by the bkge)
    bool UsingDefaultBinning;

    ///Dataclass (e.g. P6_V3_TRANSIENT::FRONT)
    string DataClass;
    string DataClassName_noConv;
    string ConversionName;
    string DataClassVersion;
    int ConversionType;

    unsigned short int Energy2Bin(float Energy);
    float Bin2Energy(unsigned short int bin);


 private:
    void SimulateSky(string PlotsFile, TH2F*,  vector <double> STARTGTI, vector <double> ENDGTI, const double Iterations, const short int iEnergy); ///Simulate the background to make a background map
    double GimmeCorrectionFactor(short int ie, double MET);
    ///Data Files
    void CalcResiduals(string FitsAllSkyFile);
    void Make_McIlwainL_Fits(string FitsAllSkyFile);
    void Make_ThetaPhi_Fits(string FitsAllSkyFile);
    bool PassesCuts(fitsfile * fptr, long int i, int format);

    ///Correction factors
    vector <TH1F*> RatiovsTime;

    float EstimatorVersion,Residuals_version,RateFit_version,ThetaPhiFits_version;
    double StartTime, EndTime, StopTime;
    long int TimeBins;
    
    int MinCTBClassLevel; //for P6
    int EventClassMask;   //for P7
    float BinSize;
    double TimeStep;
    int L_BINS, B_BINS; ///Number of longitude and latitude map bins 
    TFile * fResidualOverExposure,*fRateFit,*fThetaPhiFits,*fCorrectionFactors;

    char name[2000];
    string DataDir;
    ClassDef(BackgroundEstimator,1)

};

#endif

