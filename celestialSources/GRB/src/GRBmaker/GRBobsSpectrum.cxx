// FILE: GRBobsSpectrum.cxx

#include "GRBmaker.h"
#include "GRBobsSpectrum.h"
#include "GRBobsConstants.h"
#include "src/GPS.h"
#include "src/SpectrumFactory.h"
#include "CLHEP/Random/RandFlat.h"


//static SpectrumFactory<GRBobsSpectrum> factory;
//const ISpectrumFactory& GRBobsSpectrumFactory = factory;



// Constructors
GRBobsSpectrum::GRBobsSpectrum()
	:ISpectrum(),
	 m_title("GRBobsSpectrum"),
	 m_particleName("gamma"),
	 m_grbMaker(new GRBmaker)
{
}


GRBobsSpectrum::GRBobsSpectrum(const std::string &filename)
	:ISpectrum(),
	 m_title("GRBobsSpectrum"),
	 m_particleName("gamma"),
	 m_grbMaker(new GRBmaker)
{
}


GRBobsSpectrum::GRBobsSpectrum(double duration, int npuls, double flux, double fraction, double powerLawIndex, bool flag)
	:ISpectrum(),
	 m_title("GRBobsSpectrum"),
	 m_particleName("gamma"),
	 m_grbMaker(new GRBmaker(duration, npuls, flux, fraction, powerLawIndex, flag))
{
}


// Copy Constructor
GRBobsSpectrum::GRBobsSpectrum(const GRBobsSpectrum &right)
	:m_title(right.m_title),
	 m_particleName(right.m_particleName)
{
	if (right.m_grbMaker)
		m_grbMaker = right.m_grbMaker->clone();
}


// Destructor
GRBobsSpectrum::~GRBobsSpectrum()
{
	delete m_grbMaker;
	m_grbMaker = 0;
}


// Helper function for the assignment operator
void GRBobsSpectrum::swap(GRBobsSpectrum &other) throw()
{
	std::swap(m_title, other.m_title);
	std::swap(m_particleName, other.m_particleName);
	std::swap(m_grbMaker, other.m_grbMaker);
}


// Assignment Operator
GRBobsSpectrum &GRBobsSpectrum::operator=(const GRBobsSpectrum &right)
{
	GRBobsSpectrum temp(right);   // does all the work
	swap(temp);   // this can't throw
	return *this;
}


double GRBobsSpectrum::flux(double time) const
{ 
	return m_grbMaker->flux();
}


float GRBobsSpectrum::fraction(float energy) 
{
	std::cout<<"fraction"<< std::endl;
	return m_grbMaker->fraction();
}


double GRBobsSpectrum::interval(double time)
{
	if (m_grbMaker->photonlist().empty())
	{
		std::cout << "No more time values available to return" << std::endl;
		return -1.0;
	}

	else
		return nextTime() - time;
}


    //JCT needs const to match pure virtual method
float GRBobsSpectrum::operator () (float randomNumber) const
{
	return (float) nextEnergy();
}


// returns next available energy
double GRBobsSpectrum::nextEnergy() const
{
	static std::vector<TimeEnergy>::iterator it = m_grbMaker->photonlist().begin();

	return (*it++).energy();
}


// returns next available energy to the simulation
double GRBobsSpectrum::energySrc(HepRandomEngine *engine, double time)
{
	return nextEnergy();
}


// returns next available time
// this method will be useful once time stamp is added to the Ispectrum interface
double GRBobsSpectrum::nextTime() const
{
	static std::vector<TimeEnergy>::iterator it = m_grbMaker->photonlist().begin();

	return (*it++).time();
}


std::pair<float,float> GRBobsSpectrum::dir(float energy) const
{
  return m_grbMaker->dir();
}
    

std::pair<double,double> GRBobsSpectrum::dir(double energy, HepRandomEngine *engine)
{
  return dir(energy);
}