///
///   GRBSpectrum: Spectrum class for the GRB source
///    Authors: Nicola Omodei & Johann Cohen Tanugi 
///
#include "GRBSpectrum.h"
#include "CLHEP/Random/RandFlat.h"
#include <iostream>
#include <math.h>

GRBSpectrum::GRBSpectrum(const std::string& params) 
{
  m_grbsim = new GRBSim();
  int flag=0;
  while(flag==0)
    {
      try 
	{ 
	  m_grbsim->Start();
	  flag=1;
	}
      catch (char * s)
	{
	  std::cout<<"Restarting GRBSim...\n";
	  flag=0;
	}
    }
}

GRBSpectrum::~GRBSpectrum() 
{
  delete m_grbsim;
}

// Must be set to 1.0 for a point source.
double GRBSpectrum::solidAngle() const
{
  return 1.0;
}

///return flux, given a time
double GRBSpectrum::flux(double time) const
{
  //  cout<<"FLUX"<<endl;
  //m_grbsim->ComputeFlux(time);
  /// test to implement the right rate...
  return m_grbsim->IRate(m_spectrum); // in ph/(m^2 s) 
}

double GRBSpectrum::rate(double time) const
{
  //cout<<"RATE"<<endl;
  //m_grbsim->ComputeFlux(time);
  /// test to implement the right rate...
  return m_grbsim->IRate(m_spectrum); // in ph/(m^2 s) 
}

double GRBSpectrum::interval(double time)// const
{
  //  cout<<" INTERVAL @ TIME ="<<time<<endl;
  /// test to implement the right rate...
  double sum=0.0;         // Initialize the integral=0
  double temp=rate(time); // The m_spectrum is already computed from energySrc()
  
  double t1=time;         // t1 is the integration variable...
  double dt=1.0e-2;       // .. and dt is its integration step.

  if(time>m_grbsim->Tmax()) return 1.0e+6; //this stops the execution of GRBtestAlg...(See GRBTest.cxx)
  while (sum<1.0){ // It compute the integral of the rate...
    // The idea is to compute t1 for which the 
    // integral between time and t1 of the rate(t) dt =1.
    
    if (temp<=1.0) temp=1.0; // Minumum rate...
    
    if (temp>=1/cst::DeadTime) // If the rate is greater then 1/DeadTime...
      { 
	// ... then the minimum time separation between two photons is the dead time...
	if(t1==time) return cst::DeadTime; 
	return t1-time; // it returns  t1-time;
      }
    else if (temp>=1.0/dt)  // the sum will be > 1, and I can stop the while loop...
      {
    	return t1-time+1.0/temp;
      }
    else
      {
	sum+=temp*dt; //...integrate...
	t1+=dt;       // ... and  incrase the time...
	temp=m_grbsim->IRate(m_grbsim->ComputeFlux(t1)); // is the rate at t1...
      }
  }//end of while
  return t1-time; // in Seconds 
}

double GRBSpectrum::energySrc(HepRandomEngine* engine, double time)
{
  // cout<<"energySrc @ time = "<<time<<endl;

  //return the flux in photons/(m^2 sec)
  /// Use time to update the spectrum
  m_spectrum.clear();
  //  m_grbsim->ComputeFlux(time);
  m_spectrum = m_grbsim->ComputeFlux(time);//;m_grbsim->Spectrum();
  m_grbsim->setSpectrum( m_spectrum);
    /// Then returns a uniform random value, to be used by operator()
  return (*this)(engine->flat());
}

float GRBSpectrum::operator() (float u) const
{
 
  float energy = m_grbsim->DrawPhotonFromSpectrum(m_spectrum, u);
  return (float) energy;
}

std::pair<float,float> GRBSpectrum::dir(float energy) const
{  
  return m_grbsim->GRBdir();
}



