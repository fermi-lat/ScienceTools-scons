// FILE: GRBobsSpectrum.cxx

#include "GRBmaker.h"
#include "GRBobsSpectrum.h"
#include "GRBobsConstants.h"
#include "src/GPS.h"
//#include "CLHEP/Random/RandEngine.h"
//#include "CLHEP/Random/Random.h"
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
//GRBobsSpectrum &GRBobsSpectrum::operator=(const GRBobsSpectrum &right)
//{
//	GRBobsSpectrum temp(right);   // does all the work
//	swap(temp);   // this can't throw
//	return *this;
//}


std::string GRBobsSpectrum::title() const
{
	return m_title;
}


const char *GRBobsSpectrum::particleName() const
{
	return m_particleName.c_str();
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


    //JCT needs const to match pure virtual method
float GRBobsSpectrum::operator () (float randomNumber) const
{
	HepRandomEngine *engine = HepRandom::getTheEngine();
	HepRandom::setTheSeed(grbcst::seed);

	return (float) nextEnergy(engine);
}


double GRBobsSpectrum::nextEnergy(HepRandomEngine *engine) const
{
	double energy = 0.0;

	if (m_grbMaker->energy().empty())
		std::cout << "No more energy values to pass back" << std::endl;

	else
	{
	    std::cout<< "PIPPO: " << m_grbMaker->time().back() << std::endl;

		energy = m_grbMaker->energy().back();
		m_grbMaker->energy().pop_back();
		m_grbMaker->time().pop_back();
	}

	return energy;
}


double GRBobsSpectrum::energySrc(HepRandomEngine *engine, double time)
{
	return nextEnergy(engine);
}


std::pair<float,float> GRBobsSpectrum::dir(float energy) const
{
  return m_grbMaker->dir();
}
    

std::pair<double,double> GRBobsSpectrum::dir(double energy, HepRandomEngine *engine)
{
  return dir(energy);
}


//int GRBobsSpectrum::askGPS()
//{
//    setPosition(GPS::instance()->lat(), GPS::instance()->lon());
//    return 0; // can't be void in observer pattern
//}
