//
//    GRBShock: Class that describes the a Shock between 2 Shells
//    Authors: Nicola Omodei & Johann Cohen Tanugi 
//

#include <iostream>
#include <math.h>
#include "GRBShock.h"

using namespace std;

GRBShock::GRBShock(GRBShell Sh1, GRBShell Sh2, double time) 
  : m_sum(1.0)
{ 
  Sh1.setRadius(Sh2.getRadius());
  m_radius    = Sh1.getRadius();
  m_tobs      = time - m_radius/cst::c;
  //  cout<<"radius = "<<m_radius<<endl;
  //Kinematics:
  double m1 = Sh1.getMass();
  double m2 = Sh2.getMass();
  
  double g1 = Sh1.getGamma();
  double g2 = Sh2.getGamma();

  double dr1=Sh1.getThickness();
  double dr2=Sh2.getThickness();
  
  //Number and density of particles in the shocked material:
  m_npart =m1*cst::c2/(cst::mpc2)*cst::erg2MeV;
  m_n1=m1*cst::c2/(cst::mpc2*Sh1.getVolCom())*cst::erg2MeV;

  // relative gamma between the 2 shells:
  m_gr=(m1*g1+m2*g2)/(m1+m2);
  // Gamma of the resulting shell: 
  m_gf=sqrt((m1*g1+m2*g2)/(m1/g1+m2/g2));
  // Internal gamma:
  m_gi=m_gr-m_gf;
  //Gamma Shock
  m_gsh=sqrt(m_gi/2.0);
  if (m_gsh<1.0) {m_gsh=1.0;}

  // See Piran 1999
  // Internal Energy:
  m_Eint=m1*cst::c2*(g1-m_gf)+m2*cst::c2*(g2-m_gf);


  //Updating Shell Info. after shock:
  Sh1.setMass(m1+m2);
  Sh1.setGamma(m_gf);
  Sh1.setThickness(dr1+dr2);
  m_mass      = Sh1.getMass();
  m_thickness = Sh1.getThickness();
  m_VolCom    = Sh1.getVolCom(); // [cm^3]

  double E52=m_Eint/1e+52;

  //This will be computed when comptonization 
  //is taken into account
  double X=0.0;
  double Y=0.0;


  //B Field parametrization
  double eb=cst::alphab/0.01;//TO BE INQUIRED!
  m_Beq=(.23/5.9)*sqrt(eb)*m_gf*sqrt(m_n1);  
  


  //Gamma Lorenz e-
  m_gemin = cst::alphae*m_gf*(cst::mpc2)/(cst::mec2)*(cst::p-2.0)/(cst::p-1.0);
  m_gemax = 2.36e+8*1/sqrt(1.0+X+Y)*pow(eb,-1/4)*pow(m_gf,-1/2)*pow(m_n1,-1/4);
  m_gecoo = 5.24e+4/((1+X+Y)*eb)*pow(m_gf,-1/3)*pow(m_n1,-2/3)/pow(E52,1/3);
  
  //Rise time 
  m_riset = dr1/cst::c;
}

double GRBShock::Esyn(double gammae)
{
  return (4.3e+6)*cst::hplanck*(m_gf*m_Beq*pow(gammae,2));
}

double GRBShock::Eic(double gammae,double nic)
{
  if (gammae*cst::mec2*1.0e+6<pow(4/3*pow(gammae,2),nic)*Esyn(gammae))
    {
      return (gammae*cst::mec2)*1.0e+6;
    }
  else
    {
      return pow(4/3*pow(gammae,2),nic)*Esyn(gammae);
      //      return (4.3e+6)*(m_gf*m_Beq*pow(gammae,4))*cst::hplanck;
    }
}

double GRBShock::OpticalDepth(double ee)
{  
  double temp;
  if(m_thickness<cst::c*tsyn(ee,m_Beq))
    {temp=m_thickness;}
  else
    {temp=cst::c*tsyn(ee,m_Beq);}

  temp=m_thickness;
  if (cst::flagIC!=1)return 1;
  
  return cst::st*cst::csi*m_n1*temp; 
  //  return 1.0;
 
}
double ComptonOrder(double tau)
{
  if (tau*tau<1.0
)
    {return 1.0;}
  else
    {return tau*tau;}
}

//nrj is the energy of the observed photon
//B is the equipartition magnetic field
double GRBShock::tsyn(double nrj, double B)
{
  return 6.22e+11*pow(B,(-5./2.))*sqrt((m_gf*cst::hplanck)/nrj);
}

//ee is the energy of the observed photon
double GRBShock::fred(double ee,double tt)
{
  double decayt;
  double riset = (m_riset*sqrt(1.0e+10/pow(ee,2.0)));
  double tp    = m_tobs+riset;
  
  if (tsyn(ee,m_Beq)>riset)
    {
      decayt = tsyn(ee,m_Beq);
    }  else 
      {
	decayt = riset;
      } 

  double norma = riset/2.0 + decayt;
  //  double norma=decayt+riset-(riset*exp((m_tobs-tp)/riset));
  //  cout<<" "<<decayt<<" "<<riset<<" "<<tp<<" "<<norma<<endl;
  if (norma <=0) 
    {
      return 0.0;
    }  
  else if (tt<=m_tobs)
    {
      return 0.0;
    } else if (tt<=tp)
      {
	//Exponenetial Rise:
	//	return (exp(-(tp-tt)/riset))/norma;
	//Linear Rise:
	return (tt-m_tobs)/(riset*norma);
      } else 
	{
	  return (exp(-(tt-tp)/decayt))/norma;
	}
}

double GRBShock::Fsyn(double ee,double tt)
{
  if (m_sum<=0) return 0.0;
  double fmax=m_Beq*pow(m_gf,2);
  //erg/s/eV
  double flux;
  if (Esyn(m_gemin)>=Esyn(m_gecoo))
    {
      //      cout<<"Fast Cooling"<<endl;
      if (ee<=Esyn(m_gecoo))
	flux = pow(ee/Esyn(m_gecoo),1./3.);
      else if (ee<=Esyn(m_gemin)) 
	flux = pow(ee/Esyn(m_gecoo),-1./2.);
      else if (ee<=Esyn(m_gemax))
	flux = pow(Esyn(m_gemin)/Esyn(m_gecoo),-1./2.)*pow(ee/Esyn(m_gemin),-cst::p/2.);
      else 
	flux = 0.0;
    } else 
      { 
	//	cout<<"Slow Cooling"<<endl;
	if (ee<=Esyn(m_gemin))   
	  flux = pow(ee/Esyn(m_gemin),1/3);
	else if (ee<=Esyn(m_gecoo)) 
	  flux = pow(ee/Esyn(m_gemin),-(cst::p-1)/2);
	else if (ee<=Esyn(m_gemax)) 
	  flux = pow(Esyn(m_gecoo)/Esyn(m_gemin),-(cst::p-1)/2)*
	    pow(ee/Esyn(m_gecoo),-cst::p/2);
	else 
	  flux = 0.0;
      }
  //  cout<<"  "<<fmax<<" "<<flux<<"  "<<fred(ee,tt)<<"  "<<tt<<endl;
  return fmax*flux*fred(ee,tt);
}

double GRBShock::Fic(double ee,double tt)
{

  if (m_sum<=0) return 0.0;
  double tau = OpticalDepth(ee);
  double nic = ComptonOrder(tau);
  double fmax=tau*m_Beq*pow(m_gf,2);
  //erg/s/eV
  double flux;
  if (Eic(m_gemin,nic)>=Eic(m_gecoo,nic))
    {
      //      cout<<"Fast Cooling"<<endl;
      if (ee<=Eic(m_gecoo,nic))
	flux = pow(ee/Eic(m_gecoo,nic),1./3.);
      else if (Eic(m_gemin,nic)<Esyn(m_gemax))
	{
	  if (ee<=Eic(m_gemin,nic)) 
	    flux = pow(ee/Eic(m_gecoo,nic),-1./2.);
	  else if (ee<=Esyn(m_gemax)) //note the suppression at Asyn max approx KN energy !! 
	    flux = pow(Eic(m_gemin,nic)/Eic(m_gecoo,nic),-1./2.)*
	      pow(ee/Eic(m_gemin,nic),-cst::p/2.);
	  else
	    flux = 0.0;
	} 
      else 
	{
	  if (ee<=Esyn(m_gemax)) 
	    flux = pow(ee/Eic(m_gecoo,nic),-1./2.);
	  else
	    flux = 0.0;
	}
      
    } 
  else 
    {
      //      cout<<"IC SLOW Cooling"<<endl;
      
      if (ee<=Eic(m_gemin,nic))
	flux = pow(ee/Eic(m_gemin,nic),1/3);
      else if(Eic(m_gecoo,nic)<Esyn(m_gemax)) 
	{
	  if (ee<=Eic(m_gecoo,nic)) 
	    flux = pow(ee/Eic(m_gemin,nic),-(cst::p-1)/2);
	  else if (ee<=Esyn(m_gemax)) //note the suppression at Esyn max approx KN energy !! 
	    flux = pow(Eic(m_gecoo,nic)/Eic(m_gemin,nic),-(cst::p-1)/2)*
	      pow(ee/Eic(m_gecoo,nic),-cst::p/2);
	  else 
	    flux = 0.0;
	} 
      else 
	{
	  if (ee<=Esyn(m_gemax))
	    flux = pow(ee/Eic(m_gemin,nic),-(cst::p-1)/2);
	}
    }
  return fmax*flux*fred(ee/1.0e+5,tt);
}


double GRBShock::FluxAtT(double energy, 
			 double time, bool flagIC)
{
  if(m_sum <= 0.) return 0.0;
  double temp=0.0;

  // Fsyn & Fic are in erg/s/eV ; sum is in ergs
  double Flux = Fsyn(energy,time);
  if(flagIC) Flux += Fic(energy,time);
 
  //normalized to 1. here:
  temp = Flux/m_sum;

  return temp;
}

void GRBShock::FluxSum(std::vector<double> energy_step, 
			 std::vector<double> energy, 
			 double time_step, bool flagIC)
{
  double sum = 0.;
  for (int tt=0;tt<cst::nstep;tt++)
    {
      for (int en=0;en<cst::enstep;en++)
	{
	  // Fsyn & Fic are in erg/s/eV ; sum is in ergs
	  double Flux = Fsyn(energy[en],tt*time_step);
	  if(flagIC) Flux += Fic(energy[en],tt*time_step);
	  sum += Flux*energy_step[en]*time_step;
	}
    }
  m_sum = sum;
}

void GRBShock::Write()
{
  cout<< "--------------------Shock's parameters --------------" << endl;
  //  cout<< "Gamma 1    = "<< Sh1->Gamma() << " Gamma 2 = " << Sh2->Gamma() << " Mass 1   = " << Sh1->Mass() <<  " Mass 1   = " << Sh2->Mass() << endl;
  //  cout<< "Gamma fin. = "<< m_gf << " Mass f. = "<< (Sh1->Mass())+(Sh2->Mass()) << " G Shock = " << m_gsh <<  endl;
  cout<< "  Eint = "<< m_Eint <<"  E_rad ="<<m_sum<< " Beq = " << m_Beq   <<endl;
  cout<< "  G.E. min " << m_gemin << " G.E.max " << m_gemax << " G.E. Cooling " << m_gecoo << endl;
  cout<< "  Esyn. min " << Esyn(m_gemin) << " Esyn.max " << Esyn(m_gemax) << " Esyn. Cooling " << Esyn(m_gecoo) << endl;
  cout<< "  E IC min " << Eic(m_gemin) << " EIC max " << Eic(m_gemax) << " EIC Cooling " << Eic(m_gecoo) << endl;
  cout<< "  Vol Com = "<< m_VolCom <<" num ele "<<m_npart<< " n el = "<< m_npart/m_VolCom << endl;
  cout<<" Gamma fin. "<<m_gf<<endl;
  cout<<" ******* " <<m_radius<< "** T obs = "<<m_tobs<<endl; 
  cout<<m_riset<<endl;
}


