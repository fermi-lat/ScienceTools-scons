// $Id$

// File: GRBobsSpectrum.h
//
#ifndef GRB_OBS_SPECTRUM_H
#define GRB_OBS_SPECTRUM_H

#include "FluxSvc/ISpectrum.h"
//#include "Spectrum.h"
#include <vector>
#include <string>
#include <functional>  // for unary_function

//#include "facilities/Observer.h"
//#include "src/GPS.h"
#include "CLHEP/Random/RandomEngine.h"


class GRBmaker;


class GRBobsSpectrum : public ISpectrum
{
  
 public:
  GRBobsSpectrum();
  GRBobsSpectrum(const std::string &filename);
  GRBobsSpectrum(double duration, int npuls, double flux, double fraction, double powerLawIndex, bool flag);

	 // --- Need a destructor, copy constrctor and assignement operator to manage memory ---
	 ~GRBobsSpectrum();
	 GRBobsSpectrum &operator=(const GRBobsSpectrum &right);

	// --- Overridden function ---
    virtual std::string title() const;

    virtual const char * particleName() const;

    // calculate flux for the current cutoff
    //JCT pure virtual method takes time as argument
    virtual double flux(double time=0) const;

    float fraction(float energy);
    
    // return solid angle pair (costh, phi) for the given energy
    std::pair<float,float> dir(float energy) const;
    std::pair<double,double> dir(double energy, HepRandomEngine *engine);

    //double energySrc(HepRandomEngine *engine);
    double energySrc(HepRandomEngine *engine, double time=0);



	// --- Functions specific to GRBobsSpectrum class ---
    //virtual float operator() (float randomNumber) const;
    //JCT needs const to match pure virtual method
    virtual float operator() (float randomNumber) const;

    // this one asks the GPS for position
    int askGPS();
    
    //JCT pure virtual method 	double ISpectrum::interval(double) missing
    virtual double interval(double time) {return 0.5 /*dummy*/;}

 private:
	 GRBobsSpectrum(const GRBobsSpectrum &right);   // private copy constructor


	 double GRBobsSpectrum::nextEnergy(HepRandomEngine *engine) const;

	 void swap(GRBobsSpectrum &other) throw();

    //ObserverAdapter< GRBobsSpectrum > m_observer; //obsever tag


	// std::generate assigns the result of invoking gen, a function object that takes no arguments, 
	// to each element in the range [first, last).


	struct adder : public std::unary_function<double, double>
	{
		adder(double value) : m_value(value) {}
		double operator() (double x) { return x + m_value; }

		double m_value;
	};


	// The following definition is now in facilities package
	//template<class InputIterator,
	//		 class OutputIterator,
	//		 class Predicate>
	//OutputIterator copy_if(InputIterator begin,
	//					   InputIterator end,
	//					   OutputIterator destBegin,
	//					   Predicate p)
	//{
	//	while (begin != end)
	//	{
	//		if (p(*begin)) *destBegin++ = *begin;
	//		++begin;
	//	}
//
//		return destBegin;
//	}




	// Data members:
	std::string             m_title;
	std::string             m_particleName;

	GRBmaker               *m_grbMaker;

};
#endif // GRB_OBS_SPECTRUM_H

