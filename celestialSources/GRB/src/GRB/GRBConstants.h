/*! 
 * \class GRBConstants
 * \brief Class instantiated to access general parameters and constants.
 *  
 * The namespace cst contains all the constant needed to the simulation.
 * All the constant relative to physical model are included in the GRBParam.txt
 *
 * \author Nicola Omodei nicola.omodei@pi.infn.it
 * \author Johann Cohen-Tanugi johann.cohen@pi.infn.it
 */

#ifndef GRBCONSTANTS_HH
#define GRBCONSTANTS_HH 1
#include <string>
#include <cmath>

namespace cst
{
  /// Universal constants :
  const double mpc2      = 938.2;
  const double erg2MeV   = 624151.0;
  const double mpc2cm    = 3.0857e+24;

  const double G2MeV     = 815.78;
  const double st        = 6.65225e-25;
  const double mec2      = 0.510999;
  const double mu0       = 4.0*M_PI*1.0e-7; //N A^-2
  //! light speed in cm/sec.
  const double c         = 2.98e+10;
  const double c2        = c*c;
  //! Planck constant in eV*sec.
  const double hplanck   = 4.13567e-15;
  const double pi        = 3.1415926535897932385;
  const double Hubble    = 6.5e+1; 
  const double wzel      = 0.0;

  const double csi       = 1.0;
  const double alphae    = .33;
  const double alphab    = .33;
  const double p         = 2.5;
  const double viscosity = 0.0;
  /// Internal Parameters
  const double enmax     = 3.0e+12;
  //! Min. photon energy detectable by GLAST: set to 1 MeV
  //  double enph= 25.0e+3;
//const double enph      = 1.0e+7; 
//    const double enph      = 25.0e+3; // = 25 KeV BGO band
  //! Minimal Temporal separation between 2 photons
  const double DeadTime  = 1.0e-5; //sec
  const double enmin     = 1.0e+3;
  const int nstep        = 200;
  const int enstep       = 100;
  //! flag =[0,1], if ==0, No inverse compton;
  const float flagIC     = 1.;
  //! If some of the constants in the GRBParam.txt file is ==0, it will be choosen randomly, accoding the to kind of burst selected.
  //!  The possibilities are:
  //! 'Short' or 'Long' to select the kind of burst will be generated.
  //!  Everything else to select short or long burst whith different prop=bability.
  const bool savef=false;
  const std::string paramFile= "GRBdata.txt";  
}

class GRBConstants 
{ 
  
 public:
  //! Constructor calls method readParam wich gets an external file
  GRBConstants();

  //! Destructor
  ~GRBConstants() { }

  //! Initialize the Random Number Generator. Every run is differrent.
  void InitializeRandom();
  
  //! Parameters are read from a file using facilities::Util::expandEnvVar ethod
  void ReadParam();
  
  //! Printout Utility
  void Print();
  
  //! Save the parameters in a file. It could be usefull to mantain trace of the models runned.
  void Save(bool flag=false);

  //! Returns a random number between \param min and \param max with a \em flat distributin 
  static double SelectFlatRandom(double min=0.0, double max=1.0);

  //! Returns a random number between \param min and \param max with a \em gaussian distributin 
  static double SelectGaussRandom(double min=0.0, double max=2.0);

  //! Number of shells generated from the source
  inline int Nshell() {return m_nshell;}
  
  // inline void SeveFile(bool value){savef=value;}
  //! Set the number of shells. If the arguments is 0 it returns a random number.
  void setNshell(int value=10);
  
  //! Redshift of the source.
  inline double Redshift() {return m_redshift;}

  //! Set the redshift of the source. If the arguments is 0 it returns a random number.
  void setRedshift(double value=1.0);
  
  //! Total Energy available (ergs)
  inline double Etot(){return m_etot;}

  //! Set the total Energy available.
  void setEtot(double value);
  
  //! Initial separation between shells (cm)
  inline double R0(){return m_r0;}

  //! Set the initial separation between the shells.
  void setR0(double value);
  
  //! Initial thickness of the shells (cm)
  inline double T0(){return m_t0;}
  //! Set the initial thickness
  inline void setT0(double value);
  
  //! Minimum Lorentz factor of the shells
  //  inline double Gamma0(){return g0;}
  inline double GammaMin(){return m_g0;}

  //! Set the minimum Lorentz factor of the shells
  void setGammaMin(double value=100.0);
  
  //! Maximum Lorentz of the shells
  inline double GammaMax(){return m_g1;}

  //! Set the maximum Lorenz of the shells
  void setGammaMax(double value=1000.0);
  //////////////////////////////////////////////////
  //! Duration of the burst
  inline double Duration(){return m_duration;}

  //! Set the maximum Lorenz of the shells
  void setDuration(double value=0.4);
  //! Set the maximum Lorenz of the shells
  inline void setEnergyPh(double value=25.0e+3){m_enph = value;}
  inline double EnergyPh(){return m_enph;}

  
  inline std::string GetParamFile() {return  m_paramFile;}
  double MakeGRB();
  //  bool CompareResults(double duration, double ftot);
  
 private:
  char* m_burst_type;
  int m_nshell;
  double m_redshift;
  double m_etot;
  double m_r0;
  double m_t0;
  double m_g0,m_g1;
  double m_duration;
  double m_enph;
  bool m_GenerateAgain;
  std::string m_paramFile;
 
};

#endif


