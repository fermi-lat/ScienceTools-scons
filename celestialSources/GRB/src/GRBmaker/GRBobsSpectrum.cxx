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
	if (m_grbMaker->time().empty())
	{
		std::cout << "No more time values available to return" << std::endl;
		return -1.0;
	}

	else
		return m_grbMaker->time().back() - time;
}


    //JCT needs const to match pure virtual method
float GRBobsSpectrum::operator () (float randomNumber) const
{
	HepRandomEngine *engine = HepRandom::getTheEngine();
	HepRandom::setTheSeed(grbcst::seed);

	return (float) nextEnergy(engine);
}


// returns next available energy
double GRBobsSpectrum::nextEnergy(HepRandomEngine *engine) const
{
	double energy = -1.0;

	if (m_grbMaker->energy().empty())  // no more value left to return
		std::cout << "No more energy values to return" << std::endl;
	else
	{
		if (m_grbMaker->time().empty())  // last energy will correspond to the last returned time
		{
			energy = m_grbMaker->energy().front();
			m_grbMaker->energy().clear();
		}

		else  // pop energy corresponding to the last returned time
		{
			std::cout<< "PIPPO time: " << m_grbMaker->time().back() << std::endl;

			DoubleSize sz = m_grbMaker->time().size();
			if (m_grbMaker->energy().size() <= sz)  // more energies have been returned than time - return next energy
			{
				energy = m_grbMaker->energy().back();
				m_grbMaker->energy().pop_back();
			}

			else  // more times have been returned than energies - find energy corresponding to last returned time
			{
				while (m_grbMaker->energy().size() > sz)
				{
					energy = m_grbMaker->energy().back();
					m_grbMaker->energy().pop_back();
				}
			}
		}
	}

	return energy;
}


// returns next available energy to the simulation
double GRBobsSpectrum::energySrc(HepRandomEngine *engine, double time)
{
	return nextEnergy(engine);
}


// returns next available time
// this method will be useful once time stamp is added to the Ispectrum interface
double GRBobsSpectrum::nextTime(HepRandomEngine *engine) const
{
	double time = -1.0;

	if (m_grbMaker->time().empty())  // no more value left to return
		std::cout << "No more time values to return" << std::endl;
	else
	{
		if (m_grbMaker->energy().empty())  // last time will correspond to the last returned energy
		{
			time = m_grbMaker->time().front();
			m_grbMaker->time().clear();
		}

		else  // pop time corresponding to the last returned energy
		{
			std::cout<< "PIPPO energy: " << m_grbMaker->energy().back() << std::endl;

			DoubleSize sz = m_grbMaker->energy().size();
			if (m_grbMaker->time().size() <= sz)  // more times have been returned than energy - return next time
			{
				time = m_grbMaker->time().back();
				m_grbMaker->time().pop_back();
			}

			else  // more energies have been returned than times - find time corresponding to last returned energy
			{
				while (m_grbMaker->time().size() > sz)
				{
					time = m_grbMaker->time().back();
					m_grbMaker->time().pop_back();
				}
			}
		}
	}

	return time;
}


std::pair<float,float> GRBobsSpectrum::dir(float energy) const
{
  return m_grbMaker->dir();
}
    

std::pair<double,double> GRBobsSpectrum::dir(double energy, HepRandomEngine *engine)
{
  return dir(energy);
}