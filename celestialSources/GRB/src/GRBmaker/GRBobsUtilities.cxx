// FILE: GRBobsUtilities.cxx

#include "GRBobsUtilities.h"
#include <fstream>
#include <iomanip>
#include "GRBobsConstants.h"
#include "GRBsimvecCreator.h"
#include <algorithm>  // for transform
#include <numeric>  // for accumulate
#include "CLHEP/Random/Random.h"

using namespace grbobstypes;



// result(engine, lo, hi, p)
// returns interpolated values from lo and hi
//
// Input:
//		engine						:	pointer to a HepRandomEngine object
//		lo							:	lower value
//		hi							:	upper value
//		p							:	factor
//
// Output:
//		value						:	interpolated value generated from hi and lo
//
// Calls:
//		engine->flat				:	returns a random number between [0,1)
//
// Caller:
//		GRBmaker::computeFlux
//		GRBpulse::universalWidth

double GRBobsUtilities::result(HepRandomEngine *engine, const double lo, const double hi, const double p)
{
	return lo * pow((1.0 - engine->flat() * (1.0 - pow((hi/lo), -p))), (-1/p));
}




// multiplier::operator()
// returns product of x with multiplier::m_value
//
// Input:
//		x							:	input parameter
//		m_value						:	multiplier::data member
//
// Output:
//		value						:	x * m_value
//
// Calls:
//		--
//
// Caller:
//		GRBmaker::getTimes
//		GRBpulse::getPulse
//		GRBpulse::getTmax

double GRBobsUtilities::multiplier::operator () (double x)
{
	return x * m_value;
}




// randGen::operator()
// returns a random number between [0,1)
// used to fill a vector with random numbers
//
// Input:
//		--
//
// Output:
//		value						:	random number in [0,1)
//
// Calls:
//		engine->flat					:	returns a random number between [0,1)
//
// Caller:
//		GRBpulse::getAmplitude
//		GRBpulse::getTmax

double GRBobsUtilities::randGen::operator () (double x)
{
	return m_engine->flat();
}




// sortVector(index, in, sorted, out)
// sorts "out" array in the order of "in"
//
// Input:
//		index						:	offset for the indices into the output array 
//		in							:	input array - unsorted
//		sorted						:	input array - sorted
//		out							:	array to be sorted in the order of "in"
//
// Output:
//		out							:	output array sorted in same order as "in"
//
// Calls:
//		--
//
// Caller:
//		GRBmaker::makeTimes
//		GRBpulse::getAmplitude

// This version is lot slower than the following;  It takes ~8 minutes to run while the next version
// takes ~1.30 minutes.
//void GRBobsUtilities::sortVector(const long index, const std::vector<double> &in, const std::vector<double> &sorted, 
//						  std::vector<double> &out) 
//{
//	doubleSize sz = in.size();

//	std::vector<double> temp(out);

//	for (long i=0; i<sz; ++i)
//	{
//		doubleConstIter it = std::find(sorted.begin(), sorted.end(), in[i]);

//		doubleIter it_out = out.begin();
//		std::advance(it_out, std::distance(sorted.begin(), it));

//		*(it_out+index) = temp[i+index];
//	}
//}


void GRBobsUtilities::sortVector(const long index, const std::vector<double> &in, const std::vector<double> &sorted, 
								 std::vector<double> &out) 
{
	grbobstypes::DoubleSize sz = in.size();

	std::deque<bool> flag(sz,0);
	std::vector<double> temp(out);

	for (long i=0; i<sz; ++i)
	{
		long lowerBound=0, upperBound=sz-1; 
		long mid=(lowerBound + upperBound) / 2;
		long ich=-1;

		while (ich == -1)
		{
			bool next=0;
	
			if (in[i] <= sorted[mid])
			{
				if (in[i] == sorted[lowerBound])
				{
					while ((lowerBound <= mid) && (flag[lowerBound]))
						++lowerBound;

					if (lowerBound <= mid)
						ich = lowerBound;
				}

				else if (in[i] == sorted[mid])
				{
					if (!flag[mid])
						--mid;

					while ((mid > lowerBound) && (in[i] == sorted[mid]) && (!flag[mid]))
						--mid;

					ich = mid + 1;
				}

				else
				{
					upperBound = mid;
					next = 1;
				}
			}

			if ((!next) && (ich == -1))
			{
				++mid;

				if (in[i] == sorted[mid])
				{
					while ((mid <= upperBound) && (flag[mid]))
						++mid;

					ich = mid;
				}

				else if (in[i] == sorted[upperBound])
				{
					--upperBound;

					while ((upperBound > mid) && (in[i] == sorted[upperBound]) && (!flag[upperBound]))
						--upperBound;

					ich = upperBound + 1;
				}

				else
					lowerBound = mid + 1;
			}

			mid=(lowerBound + upperBound) / 2;
			if (ich != -1)
				flag[ich] = 1;
		}

		out[ich+index] = temp[i+index];
	}
}


