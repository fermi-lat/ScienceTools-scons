#include <iterator>
#include <iostream>
//#include <stdio.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include <string>
#include "CLHEP/Random/RandFlat.h"
#include "GRBSim.h"

//for windows to know about exit()
//#include <cstdlib>

/*------------------------------------------------------*/
using namespace cst;
using namespace std;
/*------------------------------------------------------*/

/*!
 *  Utility class for the sort() algorithm of Schock vector: sort in 
 *  decreasing order with respect to the observer time of arrival.
 */
class ShockCmp{
public:
  bool operator()(GRBShock* Sho1, GRBShock* Sho2)
  {
    return Sho1->tobs()<Sho2->tobs();    
  }
};
/*------------------------------------------------------*/


GRBSim::GRBSim()
{
  cout<<"******Staring The GRB Simulation******"<<endl;
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

  m_grbdir=std::make_pair(((RandFlat::shoot(1.0))*1.4)-0.4,(RandFlat::shoot(1.0))*2*M_PI);
  
  double qo=(1.0+3.0*cst::wzel)/2.0;
  double Dist=(cst::c/(Hubble*1.0e+5)/pow(qo,2.0))*
    (myParam->Redshift()*qo+(qo-1.0)*
     (-1.0+sqrt(2.0*qo*myParam->Redshift()+1.0)))*cst::mpc2cm;
  m_Area=(4.*cst::pi)*pow(Dist,2); // [cm^2]

  cout<<"Dist  of the source = "<<Dist<<endl;
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
}

/*------------------------------------------------------*/
GRBSim::~GRBSim()
{
  //  delete theShells;
  //  delete theShocks;
  cout<<"*******Exiting The GRB Simulation ******"<<endl;
}
/*------------------------------------------------------*/

void GRBSim::Start() 
{
   //! Step 1: Creation of the shells
  double ei = myParam->Etot()/myParam->Nshell(); //erg
  int i;
  for(i=myParam->Nshell();i>0;i--) {
    GRBShell* iShell = new GRBShell(ei);
    iShell->setThickness(myParam->T0());
    iShell->setRadius(i*(myParam->R0())+myParam->T0());
   
    cout << " Shell n: "<<myParam->Nshell()-(i-1)
	 <<" Gamma= "<<iShell->Gamma()
	 <<" Initiaml Radius "<<iShell->Radius()
	 << " Initial Thickness= " <<iShell->Thickness()<< endl;
    theShells.push_back(iShell);
  }
  double ssum;
  double tmax = dt1*nstep; 
  double time = 0.0;
  int nshock=0;
  
  //! Step 2: Calculation of the evolution
  while(time<tmax)
    {
      for(i=1;i<myParam->Nshell()-nshock;i++)
	{
	  theShells[i]->evolve(dt1);      
	}
      for(i=2;i<myParam->Nshell()-nshock;i++) 
	{
	  GRBShell* Sh1=theShells[i-1];
	  GRBShell* Sh2=theShells[i];
	  if(Sh1->Radius()<=(Sh2->Radius()+Sh2->Thickness())) 
	    {
	      GRBShock* iShock = new GRBShock(Sh1, Sh2);
	      iShock->setTime(time);
	      theShocks.push_back(iShock);	
	      theShells.erase(&theShells[i]);
	      nshock++;
	    }
	}
      time+=dt1;
    }
  cout<< "Number of Shocks = " <<nshock<< endl;
  if (nshock==0) throw "No shock event created!";
  //    {
  //      cout<< "Sorry no shocks events!! "<< endl;
  //      exit(1);
  //    }
  
  /*------------------------------------------------------*/
  /// Step 3: Sorting the shocks and setting t min=0
  std::sort(theShocks.begin(), theShocks.end(), ShockCmp());
  double t0 = theShocks[0]->tobs();
  for(i=0;i<nshock;i++)
    {
      double temp=theShocks[i]->tobs();
      theShocks[i]->setTobs(temp-t0);
      cout<<"Shock num " << i << " @ time obs = " << theShocks[i]->tobs()<<endl;
    }

  // Warning: tmax redeclared here!!
  m_tmax=1.2*theShocks[nshock-1]->tobs()+0.1;
  double dt=m_tmax/nstep;
  for(i=0;i<nshock;i++)
    {
      ssum=0.0;
      for (int tt=0;tt<nstep;tt++)
	{
	  for (int en=0;en<enstep;en++)
	    {
	      // Fsyn & Fic are in erg/s/eV ; sum is in ergs
	      ssum += (theShocks[i]->Fsyn(m_energy[en],tt*dt)+flagIC
		       *theShocks[i]->Fic(m_energy[en],tt*dt))*m_de[en]*dt;
	    }
	}
      theShocks[i]->setSum(ssum);
      //      theShocks[i]->Write();
    }
}

/*------------------------------------------------------*/
void GRBSim::ComputeFlux(double time)
{
  if (time<=1.0e-6) time=1.0e-6;
  double nshock=theShocks.size();
  double norma;
  double sum;
  double temp;
  m_spectrum.clear();
  
  for (int en=0;en<enstep;en++)
    {
      m_spectrum.push_back(0.0);
    }
  for (int j=0;j<nshock;j++)
    {
      sum = theShocks[j]->Sum(); /// erg
      norma = (theShocks[j]->Eint())/(m_Area); //erg/cm^2
      for (int en=0;en<enstep;en++)
	{
	  if(sum>0.0)
	    {
	      // temp is in erg/s/eV
	      temp=(theShocks[j]->Fsyn(m_energy[en],time)+
		    flagIC*theShocks[j]->Fic(m_energy[en],time));
	      
	      m_spectrum[en]+=(erg2MeV*1.0e+16)/m_energy[en]*(norma/sum)*temp;
	      // (eV/erg) (1/eV) (erg/cm^2) (1/erg) (erg/s/eV) = 1/cm^2/s/eV
	      // converted in ->photons/s/MeV/m^2
	    }else
	      {
		cout<<" Sum <= 0  !! "<<endl;
	      }
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
  // flux of particles for energy > enmin
  double flux=0.0;
  for (int en=0;en<enstep;en++)
    {
      if(m_energy[en]>=enmin)
	{  
	  flux += m_spectrum[en]*(m_de[en])*(1.0e-6);
	  //ph/s/m^2
	}
    }
  return flux;  
}
/*------------------------------------------------------*/
double GRBSim::IEnergy(double enmin)
{
  // Integrated flux of energy for energy > enmin
  // that flows in a time step dt .
  double flux=0.0;
  double dt=m_tmax/nstep;
  for (int en=0;en<enstep;en++)
    {
      if(m_energy[en]>=enmin)
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
  
  return ph*1.0e-9; //returns value in GeV
}
/*---------------------------------------------------------*/


