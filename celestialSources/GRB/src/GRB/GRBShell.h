/*!
 * \class GRBShell 
 * \brief Describes a spherical shell produced by the blast of the GRB inner engine.
 *
 * The shells will be stacked in a vector.
 * \author Nicola Omodei       nicola.omodei@pi.infn.it 
 * \author Johann Cohen-Tanugi johann.cohen@pi.infn.it
 *
 */

#include "GRBConstants.h"

#ifndef GRBSHELL_H
#define GRBSHELL_H 1

class GRBShell 
{  
 public:

  /*!
   * \brief Creation of a new shell.
   *
   * \arg The Lorentz factor \c m_gamma is randomly drawn (see \c generateGamma). 
   * \arg The mass of the shell is computed as: \f$\displaystyle{E\over \Gamma c^2}\f$.
   * .
   * \param E energy of the shell (in erg). In practice, all created shells share
   * the same fraction of the total energy released by the inner engine. 
   */
  //  GRBShell(double /*energy*/);
  GRBShell(double gamma, double mass, 
	   double thickness, double radius);

  //! Destructor
  ~GRBShell() { }
  
  // Accessors to data member values:
  //! Returns the Mass
  inline double getMass()      {return m_mass;}
  //! Returns the Lorentz Factor
  inline double getGamma()     {return m_gamma;}
  //! Returns the Thickness
  inline double getThickness() {return m_thickness;}
  //! Returns the Radius of the shell
  inline double getRadius()    {return m_radius;} 
  //! Returns the beta of the shell
  inline double getBeta()      {return beta(m_gamma);}
  /*! \brief computes and returns the comoving volume (in \f$cm^3\f$).
   *
   * The comoving volume is defined as 
   *\f$\Large{4\pi\times{thickness}\times{radius}^2\times\Gamma}\f$
   */
  inline double getVolCom()    {return 4.0*cst::pi*m_thickness*m_radius*m_radius*m_gamma;}
  
  //Set functions: Should be more protected!!
  //! Set the mass
  inline void setMass(double value)      {m_mass = value;}
  //! Set the Lorentz factor
  inline void setGamma(double value)     {m_gamma = value;}
  //! Set the Thickness
  inline void setThickness(double value) {m_thickness = value;}
  //! Set the Radius of the shell
  inline void setRadius(double value)    {m_radius = value;}
 
  //Higher  level functions:


  /*! 
   * \retval  beta = \f$\sqrt{1-{1\over\Gamma^2}}\f$
   * \param gamma (Lorentz factor of the shell) \f$\Gamma\f$
   */
  double    beta(const double gamma);

  /*!
   * \brief Time evolution of the shell.
   *
   * This method is used in GRBSim to evolve the shells prior to checking
   * for new shocks.
   * Radius of the shell is moved forward by \f$\beta\times c\times time\f$
   * \param time timestep of evolution
   */
  void      evolve(double time);
  
  
 private:
  //Data Members:
  double m_mass;
  double m_gamma;
  double m_thickness;
  double m_radius;
};

#endif

