/*!
 * \class GRBSim
 * \brief Simulator engine of a GRB source.
 *
 * This class initializes the simulation generating shells with random Lorentz factor and stacks the shells in a vector.
 * The evolution with the time is evaluated for all the shells and with two shells have the same radius they collide, and a GRBShock is created. 
 * All the shocks are staked in a vector. For each shock is evaluated the observed time at which it occurs.
 * This time is used to sort the vector. GRBsim compute the spectrum at a given time and returns it in a vector. 
 * This class returns the energy of a photon (chosen from the spectrum), the flux and the rate.

 * \author Nicola Omodei       nicola.omodei@pi.infn.it 
 * \author Johann Cohen-Tanugi johann.cohen@pi.infn.it
 *
 */

#include "GRBShell.h"
#include "GRBShock.h"
#include "GRBConstants.h"
#include <vector>

#ifndef GRBSIM_H
#define GRBSIM_H 1

class GRBSim 
{
 public:

  //! Gathers all relevant constants for the simulation 
  GRBConstants *myParam;


  GRBSim();
  //! destructor
  ~GRBSim();

  /*!
   * \brief generate a random Lorentz factor.
   *
   * For a uniform random number \c u, the method returns \f$\Gamma_0+ud\Gamma\f$.
   *\param gamma0 \f$\Gamma_0\f$
   *\param dgamma \f$d\Gamma\f$
   */
  double    generateGamma(double gamma0, double dgamma);
  
  bool CompareResults(double duration, double ftot);
  double GRBSim::GRBDuration(char* burst_type="Both");
  /*!
  * \brief Starts the GRB simulation
  *
  * \arg Step 1: Creation of the shells
  * \arg Step 2: Calculation of the evolution of the shells
  * \arg Step 3: Sorting the Shocks with respect to \c tobs. 
  */
  void MakeGRB(double time_offset = 0.);
  /*! \arg Step 4:Compute the Flux, at given time.
   * \param time is the time in which the spectrum is calculated.
   */
  std::vector<double> ComputeFlux(double time);
  //! Set the private data member m_spectrum to the \param value.
  void setSpectrum(std::vector<double> value) {m_spectrum=value;}
  //! The direction of the GRB is chosen randomly in the sky.
  std::pair<float,float> GRBdir()       {return m_grbdir;}
  //! The Spectrum() vector contains the flux \f$\phi\f$ in \f$ph/s/MeV/m^2\f$
  std::vector<double>    Spectrum()     {return m_spectrum;}
  //! Is the vectror that contains the energy bin,in \f$eV\f$, in wich the flux is evaluated.
  std::vector<double>    Energy()       {return m_energy;}
  //! Is the energy bin size in \f$eV\f$ (the energy is in log scale!)
  std::vector<double>    DeltaE()       {return m_de;}

  //! It conresponds to the time (in the GLAST frame) in which the burst ends
  double                 Tmax()         {return m_tmax;}

  /*! Is the Area of sphere having as radius 
   * the distance of the source (in \f$cm^2\f$)
   */ 
  double                 Area()         {return m_Area;}

  //!  Return the value of the flux (\f$ph/s/MeV/m^2\f$)
  double                 Flux(int en)   {return m_spectrum[en];}

  //! Return the \c en energy bin.
  double                 Energy(int en) {return m_energy[en];}

  /*! \brief Return the integrated flux (\f$eV/(m^2 s)\f$ for energy greather than \en enmin.
   *
   * It calculates the following integral:
   * \f[ \int_{enmin}^{enmax} \phi(E) E dE\f]
   * \param emin minimal energy below which no photon energy is drawn. This 
   * is to avoid generation of low energy photons without interest to GLAST. 
   */
  double              IFlux( std::vector<double>,
			     double enmin=cst::enmin,
			     double enmax=cst::enmax);
  
  /*! \brief Return the integrated photon rate (\f$ ph/(m^2 s)\f$) for energy greather than enmin.
   *
   * It calculates the following integral:
   * \f[ \int_{enmin}^{enmax} \phi(E) dE\f]
   * \param emin minimal energy below which no photon energy is drawn. This 
   * is to avoid generation of low energy photons without interest to GLAST. 
   */
  double              IRate(std::vector<double>,
			    double enmin=cst::enmin,
			    double enmax=cst::enmax);
  /*!
   * \brief returns a photon energy sampled from the current spectrum vector.
   *
   * This is essentially a copycat from the ROOT TH1::GetRandom() method.
   * \param spctrmVec the current spectrum vector \c m_spectrum.
   * \param u uniform random number.
   * \param emin minimal energy below which no photon energy is drawn. This 
   * is to avoid generation of low energy photons without interest to GLAST.
   */
  double 		DrawPhotonFromSpectrum(std::vector<double>, 
  			float u=0.0, 
  			double enmin=cst::enmin);

 inline double EnergyPh(){return m_enph;}
 private:
  //data member
  std::vector<GRBShell> theShells;
  std::vector<GRBShock> theShocks;
  std::vector<double>    m_energy, m_de, m_spectrum;
  std::vector<double>    m_intervals;
  std::pair<float,float> m_grbdir;
  std::vector<std::vector<double> > m_Fvt;
  
  double m_tmax;
  double m_ftot;
  double m_phtot;
  double m_Area;
  double m_DeadTime;
  double m_enph;
  };

#endif


