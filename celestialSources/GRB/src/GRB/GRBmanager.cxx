#include "GRBmanager.h"
#include <iostream>

GRBmanager::GRBmanager(const std::string& params)
: m_Time(0.0),m_timeToWait(1.0),m_params(params)
{

  m_GRB = new GRBSpectrum(m_params);
}

GRBmanager::~GRBmanager() 
{
  delete m_GRB;
}

double GRBmanager::solidAngle() const
{
  return 1.0;
}

///return flux, given a time
double GRBmanager::flux(double time) const
{
  return m_GRB->flux(time - m_Time);
}

double GRBmanager::rate(double time) const
{
  return m_GRB->rate(time - m_Time);
}

double GRBmanager::interval(double time)// const
{
  if (m_GRB->m_grbsim->Tmax()>= time - m_Time)
	return m_GRB->interval(time - m_Time);
  else
    {
     	m_GRB = new GRBSpectrum(m_params);
     	m_Time=m_timeToWait+time;
	return m_timeToWait;
    }
}

double GRBmanager::energySrc(HepRandomEngine* engine, double time)
{
  return m_GRB->energySrc(engine,time - m_Time);
}

float GRBmanager::operator() (float u) const
{
  return (*m_GRB)(u);
}

std::pair<float,float> GRBmanager::dir(float energy) const
{  
  return m_GRB->dir(energy);
}



