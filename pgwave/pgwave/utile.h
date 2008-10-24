/*
 *  utile.h
 *  di_servizio
 *
 *  Created by Gabriele on 20/10/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __UTILE_H
#define __UTILE_H

#include "pgwave/MyImage.h"
#include "pgwave/MyImageAnalisys.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iomanip>



#ifndef M_PI
//#undef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef M_E
#undef M_E
#define M_E 2.71828182845904523536
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

using namespace std;

template<class T> class array2d
{
public:
	array2d( int dim1, int dim2);
	~array2d();
	T& operator() (int i1, int i2);
	
	const int dim1, dim2;
	const long flatdim;
	T* array;
};

inline double round( double a );
inline double round( double a )
{
	return floor(a + 0.5);
}
/*** Template implementations below (cannot separate template implementations in this compiler) ***/

template<class T> array2d<T>::array2d( int d1, int d2) : dim1(d1), dim2(d2), flatdim(d1*d2)
{
	array = (T*) malloc( sizeof(T) * d1 * d2 );
}


template<class T> array2d<T>::~array2d()
{
	free(array);
}

template<class T> T& array2d<T>::operator() (int i1, int i2)
{
#ifdef CHECK_ARRAY_INDEX
	long i = (i2 + dim2*i1);
	if ( i1 >= dim1 || i2 >= dim2 || i >= flatdim ) {
		cout << "array index invalid: " << i1 << "," << i2 << endl;
		exit(0);
	}
	return array[i];
#else
	return array[i2 + dim2*i1];
#endif
}

template<class T> class array3d
{
public:
	array3d( int dim1, int dim2, int dim3);
	~array3d();
	T& operator() (int i1, int i2, int i3);
	
	const int dim1, dim2, dim3;
	const long flatdim;
	T* array;
};

template<class T> array3d<T>::array3d( int d1, int d2, int d3) : dim1(d1), dim2(d2), dim3(d3), flatdim(d1*d2*d3)
{
	array = (T*) malloc( sizeof(T) * d1 * d2 * d3 );
}

template<class T> array3d<T>::~array3d()
{
	free(array);
}

template<class T> T& array3d<T>::operator() (int i1, int i2, int i3)
{
#ifdef CHECK_ARRAY_INDEX
	long i = i3 + dim3*(i2 + dim2*i1);
	if ( i1 >= dim1 || i2 >= dim2 || i3 >= dim3 || i >= flatdim ) {
		cout << "array index invalid: " << i1 << "," << i2 << "," << i3 << endl;
		exit(0);
	}
	return array[i];
#else
	return array[i3 + dim3*(i2 + dim2*i1)];
#endif
}


inline double myrand( double max );
double exponential_distribution( double rate );
double power_distribution( double Emin, double alpha );

class detect_sources
{
public:
	void fill_coor(double xvalue, double yvalue, double dvalue, double wtvalue,
				   double magvalue, double bkgvalue, double
				   SNRvalue,double Sigvar=0,double merr=0,double
				   bkerr=0,double a=0,double b=0, double theta=0);
	double getx(int i){return x[i];};
	double gety(int i){return y[i];};
	double getd(int i){return d[i];};
	double getwt(int i){return wt[i];};
	double getmag(int i){return mag[i];};
	double getbkg(int i){return bkg[i];};
	double getmagerr(int i){return magerr[i];};
	double getbkgerr(int i){return bkgerr[i];};
	double getSNR(int i){return SNR[i];};
	double getSignif(int i){return signif[i];};
	double getA(int i){return A[i];};
	double getB(int i){return B[i];};
	double getTHETA(int i){return THETA[i];};
	int xdimension(){return int(x.size());};
	int ydimension(){return int(y.size());};
	int ddimension(){return int(d.size());};
	int wtdimension(){return int(wt.size());};
	int magdimension(){return int(mag.size());};
	int bkgdimension(){return int(bkg.size());};
	int magerrdimension(){return int(magerr.size());};
	int bkgerrdimension(){return int(bkgerr.size());};
	int SNRdimension(){return int(SNR.size());};
	int signdimension(){return int(signif.size());};
	int Adimension(){return int(A.size());};
	int Bdimension(){return int(B.size());};
	int THETAdimension(){return int(THETA.size());};
	void remove(int i);
	void set_clear();
	void print_scale(int step);
	void print_to_file(fitsfile *fptr, char *filenam,char *output_prefix);
private:
	vector<double> x;
	vector<double> y;
	vector<double> d;
	vector<double> wt;
	vector<double> mag;
	vector<double> bkg;
	vector<double> SNR;
	vector<double> signif;
	vector<double> magerr;
	vector<double> bkgerr;
	vector<double> A;
	vector<double> B;
	vector<double> THETA;

};

#endif
