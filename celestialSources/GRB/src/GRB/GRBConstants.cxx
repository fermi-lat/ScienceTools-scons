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

using namespace std;

GRBConstants::GRBConstants()
{
  ReadParam();
}

void GRBConstants::ReadParam(){
  if (cst::RandomFlag==0)
    {
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
      cout<<"Read the file: "<<paramFile.c_str()<<endl;
      
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
    } 
  else 
    {
      double temp;
      cout<<"The parameters are choosing Randomly"<<endl;
      setNshell(SelectRandom(10,100));
      setRedshift(SelectRandom(0.01,5.0));
      temp=SelectRandom(50,52);
      setEtot(pow(10,temp));
      setR0(SelectRandom(1.0e+7,1.0e+9));
      setT0(SelectRandom(1.0e+7,1.0e+9));
      setGamma0(SelectRandom(10,100));
      setDGamma(SelectRandom(Gamma0(),100*Gamma0()));
      Save();
      
    }
}
double GRBConstants::SelectRandom(double min=0.0, double max=1.0)
{
  return min+(max-min)*RandFlat::shoot(1.0);      
}

void GRBConstants::Print()
{
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

void GRBConstants::Save()
{
  char buf[100];
  std::string paramFile = "$(GRBROOT)/src/test/GRBlog.txt";
  facilities::Util::expandEnvVar(&paramFile);
  ofstream f1(paramFile.c_str(),ios::app);
  if (! f1.is_open()) 
    {
      cout<<"Error Opening $(GRBROOT)/src/test/GRBlog.txt\n";
      //TODO LIST: still need to remove this exit, without gwtting a core dump!
      exit(1);
    }
  cout<<"Save into the file: "<<paramFile.c_str()<<endl;
  cout<<"*******************************************"<<endl;
  f1<<Nshell()<<endl;
  f1<<Redshift()<<endl;
  f1<<Etot()<<endl;
  f1<<R0()<<endl;
  f1<<T0()<<endl;
  f1<<Gamma0()<<endl;
  f1<<DGamma()<<endl;
  f1.close();
}


