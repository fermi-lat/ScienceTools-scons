#include <iostream>

#include <stdio.h>

#include <string>



//#include <time.h>

#include <ctime>



//#include "src/FluxException.h" // defines FATAL_MACRO

#include "GRBConstants.h"

#include "facilities/Util.h"

#include "CLHEP/Random/RandFlat.h"

#include "CLHEP/Random/RandGauss.h"



using namespace std;



GRBConstants::GRBConstants()

{

  InitializeRandom();

  ReadParam();

}



void GRBConstants::InitializeRandom()

{

  int ini;

  time_t ctime;

  if (time(&ctime)==time_t(-1))

    {

      cout<<" Time() not defined for this processor "<<endl;

      ini=1;

    }

  ini=(ctime);

  HepRandom::setTheSeed(ini);

}



void GRBConstants::ReadParam(){    

  char buf[100];

  std::string paramFile = "$(GRBROOT)/src/test/GRBParam.txt";

  facilities::Util::expandEnvVar(&paramFile);

  ifstream f1(paramFile.c_str());

  if (! f1.is_open()) 

    {

      cout<<"Error Opening $(GRBROOT)/src/test/GRBParam.txt\n";

      exit(1);

    }

  f1.getline(buf,100);

  sscanf(buf,"%d",&m_nshell);

  

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_redshift);

  

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_etot);

  

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_r0);

  

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_t0);

    

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_g0);

   

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_g1);

  if(m_g1<=m_g0) m_g1=0;

  m_GenerateAgain=false;

  m_duration=0;

  if(m_nshell*m_redshift*m_etot*m_r0*m_t0*m_g0*m_g1==0)

    {

      m_GenerateAgain=true;

      f1.getline(buf,100);

      sscanf(buf,"%lf",&m_duration);

    }

  f1.getline(buf,100);

  f1.getline(buf,100);

  sscanf(buf,"%lf",&m_enph);

  f1.close();

  

}



double GRBConstants::MakeGRB()

{

  setDuration(m_duration);

  setNshell(m_nshell);

  setRedshift(m_redshift);

  setEtot(m_etot);

  setR0(m_r0);

  setT0(m_t0);

  setGammaMin(m_g0);

  setGammaMax(m_g1);

  setEnergyPh(m_enph);

  return 0;

}



double GRBConstants::SelectFlatRandom(double min, double max)

{

  return min+(max-min)*RandFlat::shoot(1.0);

}



double GRBConstants::SelectGaussRandom(double min, double max)

{

  double temp=0.0;

  while (temp<=0)

    {

      temp = RandGauss::shoot((max+min)/2,(max-min)/2*0.7);

    }

  return temp; 



}



void GRBConstants::Print()

{

  // cout<<"Read the file: "<<paramFile.c_str()<<endl;

  cout<<"********** Selected Parameters: **********"<<endl;

  cout<<" Number of shells               = "<<Nshell()<<endl;

  cout<<" Redshift of the Source         = "<<Redshift()<<endl;

  cout<<" Total Energy at the Source     = "<<Etot()<<endl;

  cout<<" Initial separation (cm)        = "<<R0()<<endl;

  cout<<" Initial thickness  (cm)        = "<<T0()<<endl;

  cout<<" Minimum Lorentz factor         = "<<GammaMin()<<endl;

  cout<<" Maximum Lorentz factor         = "<<GammaMax()<<endl;

  cout<<"*******************************************"<<endl;

}



void GRBConstants::Save(bool flag)

{

  if(flag)

    {

      m_paramFile=cst::paramFile;

      facilities::Util::expandEnvVar(&m_paramFile);

      cout<<"Save Parameters into the file: "<<m_paramFile.c_str()<<endl;

      ofstream f2(m_paramFile.c_str(),ios::app);

      if (! f2.is_open()) 

	{

	  cout<<"Error Opening "<<m_paramFile<<endl;

	  exit(1);

	}

      

      f2<<Nshell()<<endl;

      f2<<Redshift()<<endl;

      f2<<Etot()<<endl;

      f2<<R0()<<endl;

      f2<<T0()<<endl;

      f2<<GammaMin()<<endl;

      f2<<GammaMax()<<endl;

      f2.close();

    }

}



void GRBConstants::setNshell(int value){

  if (value == 0)

    {

      m_nshell=0;

      while(m_nshell<2)

	{

	  if (m_burst_type=="Short"){m_nshell=int(SelectGaussRandom(4,10));}

	  else {m_nshell=int(SelectGaussRandom(10,100));}

	}

    }

  else

    {

      m_nshell=value;

    }

  if(m_nshell<2) m_nshell=2;

}





void GRBConstants::setRedshift(double value){

  value==0 ? m_redshift=SelectFlatRandom(0.1,3):m_redshift=value;

}



void GRBConstants::setEtot(double value){

  double temp;

  if (value==0)

    {

      temp=SelectGaussRandom(51,55);

      m_etot=pow(10,temp);

    }

  else

    {

      m_etot=value;

    }

}



void GRBConstants::setR0(double value){

  if (value==0)

    {

      if(m_burst_type=="Short")

	{

	  double temp=SelectGaussRandom(pow(10,6.),pow(10,10.));

	  m_r0=temp;

	}

      else

	{

	  double temp=SelectGaussRandom(pow(10,6.),pow(10,10.));

	  m_r0=temp;

	}

    }

  else

    {

      m_r0=value;

    }  

}



void GRBConstants::setT0(double value){

  if (value==0)

    {

      if(m_burst_type=="Short")

	{

	  double temp=SelectGaussRandom(5.,10.);

	  m_t0=pow(10,temp);

	}

      else

	{

	  double temp=SelectGaussRandom(5.,10.);

	  m_t0=pow(10,temp);

	}

    }

  else

    {

      m_t0=value;

    }  

}



void GRBConstants::setGammaMin(double value){

  if (value==0)

    {

      if(m_burst_type=="Short")

	{

	  m_g0=SelectGaussRandom(50,200);

	}

      else

	{

	  m_g0=SelectGaussRandom(90,110);

	}

    }

  else

    {

      m_g0=value;

    }  

}







void GRBConstants::setGammaMax(double value){

  value==0 ? m_g1=SelectGaussRandom(2*m_g0,100*m_g0):m_g1=value;

}



void GRBConstants::setDuration(double value){

  m_duration=value;

  if (value<2.0)

    {m_burst_type="Short";}

  else

    {m_burst_type="Long";}  

}



/*

  bool GRBConstants::CompareResults(double duration, double ftot)

  {

  

  if(!m_GenerateAgain) 

  if (duration<=1000.0) return true;

  

  bool c1=false;

  //  bool c2=false;

  

  if (m_duration <= duration+duration/10. && m_duration > duration-duration/10.) c1=true;

  return c1;

  //  if (m_duration <= duration+duration/10. && m_duration > duration-duration/10.)  

  }

*/

