//#include <iostream.h>
//#include <fstream.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <math.h>
#include "src/FluxException.h" // defines FATAL_MACRO
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
      cout<<" Time() not defined for this processor"<<endl;
      ini=1;
    }
  //  tm* gt=gmtime(&ctime);
  ini=(ctime);
  //  cout<<ini<<endl;
  //ini=int(gt->tm_sec);
  HepRandom::setTheSeed(ini);
  //  cout<<SelectFlatRandom(0.0,1.0)<<endl;
}
void GRBConstants::ReadParam(){  // It determines if, in case of random selection of the parameters,
  // the burst is long or short...
  //burst_type="Short";
  // burst_type="Long";
  burst_type="both";
  //    cst::kind_of_burst;
  if (burst_type!="Short" && burst_type!="Long")
    {
      if (SelectFlatRandom(0.0,1.0)<=0.3)
	{burst_type="Short";}
      else
	{burst_type="Long";}
    }
  
  char buf[100];
  double g0,g1;
  std::string paramFile = "$(GRBROOT)/src/test/GRBParam.txt";
  facilities::Util::expandEnvVar(&paramFile);
  ifstream f1(paramFile.c_str());
  if (! f1.is_open()) 
    {
      cout<<"Error Opening $(GRBROOT)/src/test/GRBParam.txt\n";
      exit(1);
    }
  f1.getline(buf,100);
  sscanf(buf,"%d",&nshell);
  setNshell(nshell);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&redshift);
  setRedshift(redshift);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&etot);
  setEtot(etot);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&r0);
  setR0(r0);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&t0);
  setT0(t0);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&g0);
  setGamma0(g0);
  
  f1.getline(buf,100);
  sscanf(buf,"%lf",&g1);
  setDGamma(g1);
  
  f1.close();
  //  if (cst::savef) Save();
}

double GRBConstants::SelectFlatRandom(double min=0.0, double max=1.0)
{
  return min+(max-min)*RandFlat::shoot(1.0);
}

double GRBConstants::SelectGaussRandom(double min=0.0, double max=2.0)
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
  cout<<" Minimum Lorentz factor         = "<<Gamma0()<<endl;
  cout<<" Maximum Lorentz factor         = "<<DGamma()<<endl;
  cout<<"*******************************************"<<endl;
}

void GRBConstants::Save(bool flag)
{
  if(flag)
    {
      /*
	std::string paramFile = "$(GRBROOT)/src/test/GRBlog.txt";
	facilities::Util::expandEnvVar(&paramFile);
	ofstream f1(paramFile.c_str(),ios::app);
	if (! f1.is_open()) 
	{
	cout<<"Error Opening $(GRBROOT)/src/test/GRBlog.txt\n";
	//TODO LIST: still need to remove this exit, without gwtting a core dump!
	exit(1);
	}
	cout<<"Save Parameters into the file: "<<paramFile.c_str()<<endl;
	f1<<"********************"<<endl;
	f1<<" Nshell   = "<<Nshell()<<endl;
	f1<<" Redshift = "<<Redshift()<<endl;
	f1<<" Etot     = "<<Etot()<<endl;
	f1<<" R0      = "<<R0()<<endl;
	f1<<" T0      = "<<T0()<<endl;
	f1<<" Gamma0   = "<<Gamma0()<<endl;
	f1<<" DGamma0  = "<<DGamma()<<endl;
	f1.close();
      */
      std::string paramFile2 = "$(GRBROOT)/src/test/GRBdata.txt";
      facilities::Util::expandEnvVar(&paramFile2);
      ofstream f2(paramFile2.c_str(),ios::app);
      if (! f2.is_open()) 
	{
	  cout<<"Error Opening $(GRBROOT)/src/test/GRBdata.txt\n";
	  exit(1);
	}
      f2<<Nshell()<<endl;
      f2<<Redshift()<<endl;
      f2<<Etot()<<endl;
      f2<<R0()<<endl;
      f2<<T0()<<endl;
      f2<<Gamma0()<<endl;
      f2<<DGamma()<<endl;
      f2.close();
    }
}

void GRBConstants::setNshell(int value){
  if (value == 0)
    {
      nshell=0;
      while(nshell<=6)
	{
	  if (burst_type=="Short"){nshell=int(SelectGaussRandom(6,11));}
	  else {nshell=int(SelectGaussRandom(30,80));}
	}
    }
  else
    {
      nshell=value;
    }
  if(nshell<2) nshell=2;
}


void GRBConstants::setRedshift(double value){
  value==0 ? redshift=SelectFlatRandom(0.1,3):redshift=value;
}

void GRBConstants::setEtot(double value){
  double temp;
  if (value==0)
    {
      temp=SelectGaussRandom(51,55);
      etot=pow(10,temp);
    }
  else
    {
      etot=value;
    }
}

void GRBConstants::setR0(double value){
  if (value==0)
    {
      if(burst_type=="Short")
	{
	  double temp=SelectGaussRandom(8,9);
	  r0=pow(temp,10);
	}
      else
	{
	  double temp=SelectGaussRandom(10,10.5);
	  r0=pow(temp,10);
	}
    }
  else
    {
      r0=value;
    }  
}

void GRBConstants::setT0(double value){
  if (value==0)
    {
      if(burst_type=="Short")
	{
	  double temp=SelectGaussRandom(8,9);
	  t0=pow(temp,10);
	}
      else
	{
	  double temp=SelectGaussRandom(10,10.5);
	  t0=pow(temp,10);
	}
    }
  else
    {
      t0=value;
    }  
}

void GRBConstants::setGamma0(double value){
  if (value==0)
    {
      if(burst_type=="Short")
	{
	  g0=SelectGaussRandom(90,110);
	}
      else
	{
	  g0=SelectGaussRandom(90,110);
	}
    }
  else
    {
      g0=value;
    }  
}



void GRBConstants::setDGamma(double value){
  value==0 ? g1=SelectGaussRandom(g0,100*g0):g1=value;
}
