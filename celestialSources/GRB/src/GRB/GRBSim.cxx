#include <iterator>
#include <iostream>
//#include <stdio.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include <string>
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGeneral.h"
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RanluxEngine.h"
#include "GRBSim.h"

//for windows to know about exit()
//#include <cstdlib>

/*------------------------------------------------------*/
using namespace cst;
using namespace std;
/*------------------------------------------------------*/

/*!
 *  Utility class for the sort() algorithm of Shock vector: sort in 
 *  decreasing order with respect to the observer time (in the GLAST reference frame).
 */
class ShockCmp{
public:
  bool operator()(const GRBShock& Sho1,const GRBShock& Sho2)
  {
    //    return const_cast<GRBShock&>(Sho1).tobs() < const_cast<GRBShock&>(Sho2).tobs();    
    return Sho1.tobs() < Sho2.tobs();    
  }
};
/*------------------------------------------------------*/


GRBSim::GRBSim()
{
  cout<<"******Staring The GRB Simulation******"<<endl;
}

/*------------------------------------------------------*/
GRBSim::~GRBSim()
{
  delete myParam;
  
  cout<<"*******Exiting The GRB Simulation ******"<<endl;
}
/*------------------------------------------------------*/


double GRBSim::generateGamma(double gamma0,double dgamma) 
{
  HepRandom::setTheEngine(new RanluxEngine);
  double gamma = gamma0 + (double (RandFlat::shoot(1.0)))*dgamma;
  return gamma;
}

void GRBSim::Start() 
{
  int nshock=0;
  while (nshock==0)
    {
      theShells.clear();
      try
	{
	  myParam=new GRBConstants();
	}
      catch (char * s)
	{
	  std::cout<< "Failure initializing the GRB constants \n";
	  //TODO LIST: still need to remove this, without getting a core dump!
	  exit(1);
	}  
      
      /*
	imax=myParam->Nshell();
	for (i=0;i<=imax;i++)
	{
	RandFlat::shoot(1.0);
	}
      */
      //! Step 1: Creation of the shells
      double ei = myParam->Etot()/myParam->Nshell(); //erg
      int i;
      for(i=myParam->Nshell();i>0;i--) {
	
	double gi = generateGamma(myParam->Gamma0(),myParam->DGamma()); 
	double m = ei/(gi*cst::c2); // Shell mass
	double r = i*(myParam->R0())+myParam->T0(); //radius
	GRBShell iShell(gi,m,myParam->T0(),r);
	theShells.push_back(iShell);
	
	/*
	cout <<" Shell n: "<<myParam->Nshell()-(i-1)
	     <<" Gamma= "<<iShell.Gamma()
	     <<" Initiaml Radius "<<iShell.Radius()
	     <<" Initial Thickness= " <<iShell.Thickness()<< endl;
	*/
      }
      
      double tmax = dt1*nstep;
      /*
	double g1=myParam->Gamma0()+myParam->DGamma();
	double v1=c*sqrt(1. - 1./(g1*g1));
	double v2=c*sqrt(1. - 1./((g1)*(g1+1.)));
	double tmax=(myParam->Nshell()*(myParam->R0()+myParam->T0())/(v2-v1));
	double dt1=tmax/nstep;
      */
      
      double time = 0.0;
      //! Step 2: Calculation of the evolution
      while(time<tmax)
	{
	  for(i=1;i<=myParam->Nshell()-nshock;i++)
	    {
	      theShells[i].evolve(dt1);      
	    }
	  for(i=2;i<=myParam->Nshell()-nshock;i++) 
	    { 
	      GRBShell Sh1 = theShells[i-1];
	      GRBShell Sh2 = theShells[i];

	      if(Sh1.getRadius()
		 <= (Sh2.getRadius()+Sh2.getThickness())) 
		{
		  GRBShock iShock(Sh1, Sh2, time);
		  theShocks.push_back(iShock);	
		  theShells.erase(&theShells[i]);
		  nshock++;
		}
	    }
	  time+=dt1;
	}
    }
  // All is ok
      double temp1=(enmax/enmin);
      double temp2=(1.0/enstep);
      double denergy = pow(temp1,temp2);
      int en;
      for(en=0;en<=enstep;en++)
	{
	  m_energy.push_back(enmin*pow(denergy,en)); 
	}
      for(en=0;en<enstep;en++)
	{
	  m_de.push_back(m_energy[en+1]-m_energy[en]);
	}
      
      
  myParam->Print();
  myParam->Save(cst::savef);

  m_grbdir=std::make_pair(((RandFlat::shoot(1.0))*1.4)
			  -0.4,(RandFlat::shoot(1.0))*2*M_PI);
  
  double qo=(1.0+3.0*cst::wzel)/2.0;
  double Dist=(cst::c/(Hubble*1.0e+5)/pow(qo,2.0))*
    (myParam->Redshift()*qo+(qo-1.0)*
     (-1.0+sqrt(2.0*qo*myParam->Redshift()+1.0)))*cst::mpc2cm;
  m_Area=(4.*cst::pi)*pow(Dist,2); // [cm^2]
  
  cout<<"Dist  of the source = "<<Dist<<endl;
  cout<< "Number of Shocks = " <<theShocks.size()<< endl;
  
  
  /*------------------------------------------------------*/
  /// Step 3: Sorting the shocks and setting t min=0
  std::sort(theShocks.begin(), theShocks.end(), ShockCmp());

  double t0 = (theShocks.front()).tobs();
  std::vector<GRBShock>::iterator itr;
  for(itr=theShocks.begin();itr != theShocks.end();++itr)
    {
      //shift of the shock observeed times, so that first 
      //is at tobs=0.
      (*itr).setTobs( (*itr).tobs() - t0);
    }
  
  // Warning: tmax redeclared here!!
  GRBShock last = theShocks.back();

  //Now compute the Flux sum produced in each Shock
  m_tmax=1.2*last.tobs()+0.1;
  if (m_tmax<2.)
    {cout<<"The burst is Short "<<endl;}
  else
    {cout<<"The burst is Long "<<endl;}
  double dt=m_tmax/nstep;
  for(itr=theShocks.begin();itr != theShocks.end();++itr)
    {
      (*itr).FluxSum(m_de,m_energy,dt,false);
    }

  m_DeadTime=1e-4;
}

void GRBSim::TotalFlux()
{
  cout<<"Compute the Total Flux !!"<<endl;
  double ti=0;
  int ii=0;
  m_Fvt.clear();
  
  while (ti<=m_tmax)
    {
      if (ii%100==0) cout<<"Time Max / Time = "
			 <<m_tmax<<" - "<<ti<<endl;
      ComputeFlux2(ti);
      m_Fvt.push_back(m_spectrum);
      ti += m_DeadTime;
      ii++;
    } 
}

void GRBSim::ComputeFlux2(double time)
{
  double ti=0.0;
  int i=0;
  if (time<=0) time=m_DeadTime;
  if (time>=m_tmax) exit(1);//time=m_tmax-m_DeadTime;
  while (ti<time) 
    {
      ti+=m_DeadTime;
      i++;
    }
  m_spectrum=m_Fvt[i];
}

/*------------------------------------------------------*/
void GRBSim::ComputeFlux(double time)
{
  double norma;
  double temp;
  m_spectrum.clear();
  m_spectrum.resize(enstep,0.);
  //    for (int en=0;en<enstep;en++)
  //{
  // m_spectrum.push_back(0.0);
  //}
  
  // ATTENZIONE!!
  if (time<=0.0) 
    {
      time=1.0e-6;
      //      cout<<" Time can not be 0.0 !! Set time = "<<time<<" s"<<endl;
    }
  //
  std::vector<GRBShock>::iterator itr;
  for(itr=theShocks.begin();itr != theShocks.end();++itr)
    {
      norma = ((*itr).Eint())/(m_Area); //erg/cm^2
      std::vector<double>::iterator it;
      int en=0;
      for (it = m_spectrum.begin();it!=m_spectrum.end();++it)
	{
	  // temp is in erg/s/eV
	  temp = (*itr).FluxAtT(m_energy[en],time,true);
	  *it += (erg2MeV*1.0e+16)/m_energy[en]
	    *norma*temp;
	  // (eV/erg) (1/eV) (erg/cm^2) (1/erg) (erg/s/eV) = 1/cm^2/s/eV
	  // converted in ->photons/s/MeV/m^2
	  en++;
	}
    }
}

/*------------------------------------------------------*/
double GRBSim::IFlux(double enmin)
{
  // gives the integrated flux of energy (eV/s/m^2) for energy > enmin 
  double flux=0.0;
  for (int en=0;en<enstep;en++)
    {
      if(m_energy[en]>=enmin)
	{
	  flux +=m_energy[en]*m_spectrum[en]*(m_de[en])*(1.0e-6);
	  //eV/s/m^2
	}
    }
  return flux;
}
/*------------------------------------------------------*/
double GRBSim::IRate(double enmin)
{
  // rate of particle arrivals for energy > enmin
  double rate=0.0;
  for (int en=0;en<enstep;en++)
    {
      if(m_energy[en]>=enmin)
	{  
	  rate += m_spectrum[en]*(m_de[en])*(1.0e-6);
	  //ph/s/m^2
	}
    }
  // ATTENZIONE!!
  if (rate<=0.01) rate = 0.01;
  return rate;  
}
/*------------------------------------------------------*/
double GRBSim::IEnergy(double enmin,double enmax)
{
  // Integrated flux of energy for energy > enmin
  // that flows in a time step dt .
  double flux=0.0;
  double dt=m_tmax/nstep;
  for (int en=0;en<enstep;en++)
    {
      if(m_energy[en]>=enmin && m_energy[en]<=enmax)
	{  
	  flux +=m_energy[en]*m_spectrum[en]*(m_de[en])*(1.0e-6)*dt;
	  //eV/m^2
	}
    }
  return flux;  
}

/*------------------------------------------------------*/

float GRBSim::DrawPhotonFromSpectrum(std::vector<double> spctrmVec, float u, double emin)
{
  //  cout<<" Energy Min ="<<emin<<endl;
  int nbins = spctrmVec.size();
  if(nbins==0) return 0.0;
    
  //STEP 1: we need to remove low energy part of the spectrum,
  // in order to avoid drawing photons of no interest to GLAST.
  // minbin: ebergy bin # after which the energy of a photon 
  // will be above emin.
  int minbin = 0;
  for (int ebin=0; ebin<m_energy.size(); ebin++)
    {
      if(m_energy[ebin] > emin)
        {
          minbin = ebin; 
          break;
        }
    }
  
  //STEP 2: copy the relevant part of the spectrum to Integral vector,
  //and then compute cumulative sum.
  std::vector<double> Integral(nbins-minbin,0.0);
  
  std::copy(spctrmVec.begin()+minbin, spctrmVec.end(), Integral.begin());

  int i;
  for(i=1;i<nbins-minbin;i++) 
    {
      Integral[i] += Integral[i-1]; //Computing cumulative sum
    }
  for(i=0;i<nbins-minbin;i++)
    {
      Integral[i] /= Integral.back(); //Normalizing to 1
    }
  if(Integral.back()!=1.0) return m_energy[0]*1.0e-9;
  
  //STEP 3: Find in the cumulative sum vector, the bin for which
  //the flat random variable u is closest to the value of Integral
  int nabove, nbelow, middle;
  nabove = nbins+1;
  nbelow = 0;
  int ibin =0;
  while(nabove-nbelow > 1) 
    {
      middle = (nabove+nbelow)/2;
      if (u == Integral[middle-1]) 
        {
          ibin = middle-1;
          break;
        }
      if (u  < Integral[middle-1]) nabove = middle;
      else                         nbelow = middle;
      ibin = nbelow-1;
    }
  
  //STEP4: retrurns the centered value of the energy at bin position
  // determined at STEP 3
  double ph = m_energy[ibin+minbin]+
    (m_energy[ibin+minbin+1]-m_energy[ibin+minbin])*
    (Integral[ibin+1] - u)/(Integral[ibin+1] - Integral[ibin]);
  //cout<< ph*1.0e-9<<endl;
  return ph*1.0e-9; //returns value in GeV
}
/*---------------------------------------------------------*/


