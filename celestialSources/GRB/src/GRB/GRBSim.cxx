#include <iterator>
#include <iostream>
//#include <stdio.h>
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>
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


double GRBSim::generateGamma(double gammaMin,double gammaMax) 
{
  HepRandom::setTheEngine(new RanluxEngine);
  double dgamma = gammaMax - gammaMin;
  return (gammaMin + (double (RandFlat::shoot(1.0)))*dgamma);
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
      catch (char * )
	{
	  std::cout<< "Failure initializing the GRB constants \n";
	  //TODO LIST: still need to remove this, without getting a core dump!
	  exit(1);
	}  
      
      // Step 1: Creation of the shells
      double ei = myParam->Etot()/myParam->Nshell(); //erg
      
      int i;
      
      double time=0.0;
      // Gap of time between the emission of shells (at the engine)
      double time_em=(myParam->R0()+myParam->T0())/cst::c;
      
      double ttee=0.0;
      double gmax2= pow(myParam->GammaMax(),2);
      double gmin2= pow(myParam->GammaMin(),2);
      double tmax = myParam->Nshell()*2.0*time_em*(gmax2*gmax2/(gmax2-gmin2));
      double ddt= time_em/10.0;
      int shell_created=0;

      while (time<tmax && nshock<myParam->Nshell()-1)
	{
	  if(shell_created<myParam->Nshell()) 
	    {
	      if (ttee==0)      // if true-> it is time to create a shell
		{
		  double gi = generateGamma(myParam->GammaMin(),myParam->GammaMax()); 
		  
		  if(myParam->Nshell()==2 && shell_created==1) 
		    {
		      while(gi<=theShells[0].getGamma()) 
			{
			  gi = generateGamma(myParam->GammaMin(),myParam->GammaMax()); 
			}
		    }
		  
		  double m = ei/(gi*cst::c2); // Shell mass
		  
		  GRBShell iShell(gi,m,myParam->T0(),0.0);
		  theShells.push_back(iShell);
		  shell_created++;
		}
	    }
	  else
	    {
	      ddt=tmax/1000.0 ;
	    }
	  
	  std::vector<GRBShell>::iterator itr;
	  for(itr=theShells.begin();itr != theShells.end();++itr)
	    {
	      (*itr).evolve(ddt);
	    }
	  
	  if(theShells.size()>1)
	    {
	      for(i=1;i<theShells.size();i++)
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
		      //		      iShock.Write();
		    }
		}
	    }
	  time+=ddt;
	  ttee+=ddt;
	  if(ttee>=time_em) ttee=0.0;
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
  
  cout<<"Dist  of the source = "<<Dist<<" cm "<<endl;
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
  m_tmax=1.2*(last.tobs()+last.duration());
  //if (m_tmax>1000.0) m_tmax=1000.0;
  cout<<"Duration of the Burst : "<<m_tmax<<endl;
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

/*------------------------------------------------------*/
std::vector<double> GRBSim::ComputeFlux(double time)
{
  //  cout<<"Compute the flux @ time ="<<time<<endl;
  double norma;
  double temp;
  std::vector<double> spectrum(enstep,0.);
  if (time<=0) return spectrum;
  std::vector<GRBShock>::iterator itr;
  for(itr=theShocks.begin();itr != theShocks.end();++itr)
    {
      norma = ((*itr).Eint())/(m_Area); //erg/cm^2
      std::vector<double>::iterator it;
      int en=0;
      for (it = spectrum.begin();it!=spectrum.end();++it)
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
   return spectrum;
}

/*------------------------------------------------------*/
double GRBSim::IFlux(std::vector<double> spctrmVec,double enmin,double enmax)
{
  if(spctrmVec.size()==0) return 0.0;
  std::vector<double>::iterator it_spec;
  std::vector<double>::iterator it_ene;
  std::vector<double>::iterator it_de;
  
  // rate of particle arrivals for energy > enmin
  double flux=0.0;
  it_ene=m_energy.begin();
  it_de=m_de.begin();
  for(it_spec= spctrmVec.begin();it_spec != spctrmVec.end();++it_spec)
    // for (int en=0;en<enstep;en++)
    {
      if((*it_ene)>=enmin)
	{  
	  flux += (*it_ene)*(*it_spec)*(*it_de)*(1.0e-6);
	  //eV/s/m^2
	}
      it_ene++;
      it_de++;
    }
  return flux;  
}

/*------------------------------------------------------*/
double GRBSim::IRate(std::vector<double> spctrmVec,double enmin)
{
  if(spctrmVec.size()==0) return 0.0;
  std::vector<double>::iterator it_spec;
  std::vector<double>::iterator it_ene;
  std::vector<double>::iterator it_de;
  
  // rate of particle arrivals for energy > enmin
  double rate=0.0;
  it_ene=m_energy.begin();
  it_de=m_de.begin();
  for(it_spec= spctrmVec.begin();it_spec != spctrmVec.end();++it_spec)
    // for (int en=0;en<enstep;en++)
    {
      if((*it_ene)>=enmin && (*it_ene)<=enmax)
	{  
	  rate += (*it_spec)*(*it_de)*(1.0e-6);
	  //ph/s/m^2
	}
      it_ene++;
      it_de++;
    }
  return rate;  
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
  if( Integral.back() <= 0 ){
      return -1; // invalid: seemss to happen sometimes (THB)??
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


