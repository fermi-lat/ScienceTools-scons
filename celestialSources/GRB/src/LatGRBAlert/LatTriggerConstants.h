//
// File: LatTriggerConstants.h

#ifndef LATTRIGGER_CONSTANTS_H
#define LATTRIGGER_CONSTANTS_H 










namespace lattrigcst
{
	const double  ratebck     = 4.07;
	const double  degInPi     = 180.0 / M_PI;
	const double  twosigdist  = 34./degInPi;

	const long    nrange      = 8L;
//	const long    nrange      = 80L;
	const long    nmove       = nrange/4;
}



namespace lattrigtypes
{
	typedef std::vector<std::vector<double> >::const_iterator DoubleVecConstIter;
	typedef std::vector<double>::const_iterator DoubleConstIter;
	typedef std::vector<double>::iterator DoubleIter;
	typedef std::vector<double>::const_reverse_iterator DoubleConstRevIter;
}

#endif


