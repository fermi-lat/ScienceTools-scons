#include <iostream>
#include <algorithm>

#include "GRBobs/GRBobsengine.h"
#include "GRBobs/GRBobsPulse.h"

using namespace ObsCst;

GRBobsengine::GRBobsengine(GRBobsParameters *params)
  : m_params(params)
{
  m_dir = m_params->GetGalDir();
  
  std::cout<<" Create new GRBobs : "<<std::endl;
  std::cout<<" Seed           = "<<m_params->GetGRBNumber()<<std::endl;
  std::cout<<" Position    : l="<<m_dir.first<<", b = "<<m_dir.second<<std::endl;
}

std::vector<GRBobsPulse*> GRBobsengine::CreatePulsesVector()
{
  //////////////////////////////////////////////////
  const   int    Npulses            = m_params->GetNumberOfPulses();
  std::vector<GRBobsPulse*> thePulses;
  double tau, rt, dt, ph, nu, ep, a, b;
  double pt=0;
  GRBobsPulse *aPulse;

  m_params->GenerateParameters();
  m_params->PrintParameters();
  
  tau = m_params->GetPulseSeparation();
  rt  = m_params->GetRiseTime();
  dt  = m_params->GetDecayTime();
  ph  = m_params->GetPulseHeight();
  nu  = m_params->GetPeakedness();
  ep  = m_params->GetEpeak();
  a   = m_params->GetLowEnergy();
  b   = m_params->GetHighEnergy();
  pt  = pow(log(100.0),1./nu) * rt;
  aPulse = new GRBobsPulse(pt,rt,dt,ph,nu,ep,a,b);
  thePulses.push_back(aPulse);
  pt+=tau;

  for(int i = 0;i<Npulses-1;i++)
    {
      m_params->GenerateParameters();
      m_params->PrintParameters();

      tau = m_params->GetPulseSeparation();
      rt  = m_params->GetRiseTime();
      dt  = m_params->GetDecayTime();
      ph  = m_params->GetPulseHeight();
      nu  = m_params->GetPeakedness();
      ep  = m_params->GetEpeak();
      a   = m_params->GetLowEnergy();
      b   = m_params->GetHighEnergy();
      aPulse = new GRBobsPulse(pt,rt,dt,ph,nu,ep,a,b);
      thePulses.push_back(aPulse);
      pt  += tau;
      
    }
  return thePulses;
}


//////////////////////////////////////////////////

/*
  double GRBobsengine::getDistance()
  {
  double m_redshift = cst::red;
  double qo=(1.0+3.0*cst::wzel)/2.0;
  return ((cst::c/(cst::Hubble*1.0e+5)/pow(qo,2.0))*
  (m_redshift*qo+(qo-1.0)*(-1.0+sqrt(2.0*qo*m_redshift+1.0)))*cst::mpc2cm);
  }
*/


