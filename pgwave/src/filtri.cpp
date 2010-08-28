/*
 *  filtri.cpp
 *  simul_21_oct
 *
 *  Created by Gabriele on 24/10/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
#include "pgwave/filtri.h"
#include <iostream>
#include <cmath>
#include <cassert>


using namespace std;


/*
* The following code is public domain.
* Algorithm by Torben Mogensen, implementation by N. Devillard.
* This code in public domain.
*/
typedef double  elem_type ;
elem_type torben(vector<double> m, int n)
{
	int i, less, greater, equal;
	elem_type min, max, guess, maxltguess, mingtguess;
	min = max = m[0] ;
	for (i=1 ; i<n ; i++) {
		if (m[i]<min) min=m[i];
		if (m[i]>max) max=m[i];
	}
	while (1) {
		guess = (min+max)/2;
		less = 0; greater = 0; equal = 0;
		maxltguess = min ;
		mingtguess = max ;
		for (i=0; i<n; i++) {
		if (m[i]<guess) {
			less++;
			if (m[i]>maxltguess) 
				maxltguess = m[i] ;
			} else if (m[i]>guess) {
				greater++;
				if (m[i]<mingtguess) 
					mingtguess = m[i] ;
			} else equal++;
	}
		if (less <= (n+1)/2 && greater <= (n+1)/2) 
			break ;
		else if (less>greater) 
			max = maxltguess ;
		else 
			min = mingtguess;
	}
	if (less >= (n+1)/2) 
		return maxltguess;
	else if (less+equal >= (n+1)/2) 
		return guess;
	else 
		return mingtguess;
}

#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }

elem_type quick_select(vector<double> arr, int n)
{
	int low, high ;
	int median;
	int middle, ll, hh;
	low = 0 ; high = n-1 ; median = (low + high) / 2;
	for (;;) {
			if (high <= low) /* One element only */
				return arr[median] ;
			if (high == low + 1) { /* Two elements only */
				if (arr[low] > arr[high])
				ELEM_SWAP(arr[low], arr[high]) ;
				return arr[median] ;
			}
		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;
		if (arr[middle] > arr[high]) ELEM_SWAP(arr[middle], arr[high]) ;
		if (arr[low] > arr[high]) ELEM_SWAP(arr[low], arr[high]) ;
		if (arr[middle] > arr[low]) ELEM_SWAP(arr[middle], arr[low]) ;
		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(arr[middle], arr[low+1]) ;
		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;) {
			do ll++; while (arr[low] > arr[ll]) ;
			do hh--; while (arr[hh] > arr[low]) ;
			if (hh < ll)
			break;
			ELEM_SWAP(arr[ll], arr[hh]) ;
		}
		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(arr[low], arr[hh]) ;
		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}
#undef ELEM_SWAP


void filtro_gaussiano(array2d<fftw_complex>& in, array2d<fftw_complex>& out, double sigma, double xbins, double ybins)
{
	int i, j;
	double kx, ky;
	fftw_complex GAUSS;
	for (i = 0; i < xbins; i++) {
		for (j = 0; j < ybins/2+1; j++) {
			if ( i < xbins/2 ) kx = 2*M_PI*i/xbins; else kx = -2*M_PI*(xbins-i)/xbins;
			if ( j < ybins/2 ) ky = 2*M_PI*j/ybins; else ky = -2*M_PI*(ybins-j)/ybins;
			Gauss_filter_2d( GAUSS, kx, ky, sigma);
			
			// Normalization
			GAUSS[0] /= (xbins*ybins);
			GAUSS[1] /= (xbins*ybins);
			
			conv_function(in(i,j), GAUSS, out(i,j));
		}
	}
	return;
}

void wavelet_filter(array2d<fftw_complex>& in, array2d<fftw_complex>& out, double scale, double xbins, double ybins)
{
	int i, j;
	double kx, ky;
	fftw_complex MH;
	for (i = 0; i < int(xbins); i++) {
		for (j = 0; j < int(ybins)/2+1; j++) {
			if ( i < xbins/2 ) kx = 2*M_PI*i/xbins; else kx = -2*M_PI*(xbins-i)/xbins;
			if ( j < ybins/2 ) ky = 2*M_PI*j/ybins; else ky = -2*M_PI*(ybins-j)/ybins;

			MH_2d_transformed( MH, kx, ky, scale, scale, 0, 0 );
			
			// Normalization
			MH[0] /= (xbins*ybins);
			MH[1] /= (xbins*ybins);
			
			conv_function(in(i,j), MH, out(i,j));
		}
	}
	return;
}

double median_2d( double* image, int xmax, int ymax, int x, int y, int xsize, int ysize )
{
/*#ifdef _PPC_
	register double sum_pixel asm ("f0");
#else
	double sum_pixel;
#endif
	int n_pixel = 0;
	double *base;
	int imin = MAX(0, x-xsize);
	int imax = MIN(xmax-1,x+xsize);
	int jmin = MAX(0, y-ysize);
	int jmax = MIN(ymax-1,y+ysize);
	sum_pixel = 0.0;
	
	for( int i = imin; i < imax; i++ ) {
		base = &image[i*ymax];
		for(int j = jmin; j < jmax; j++ ) {
			sum_pixel += base[j];
			n_pixel++;
		}
	}
	return sum_pixel/double(n_pixel);
	*/
	vector<double> temp;
	int imin = MAX(0, x-xsize);
	int imax = MIN(xmax-1,x+xsize);
	int jmin = MAX(0, y-ysize);
	int jmax = MIN(ymax-1,y+ysize);
	for( int i = imin; i <=imax; i++ ) {
		for(int j = jmin; j <=jmax; j++ ) {
			temp.push_back( /*image[i+j*xmax]*/image[j+i*ymax] );
		}
	}
	//sort(temp.begin(),temp.end());
	//double med=torben(temp,temp.size());
	int temp_size=temp.size();
	double med=quick_select(temp,temp_size);
	//return 0.5*(temp[((temp_size+1)/2)-1]+temp[(temp_size/2)+1-1]);
	return med;
}


void median_filter_2d( array2d<double>& in, array2d<double>& out, int size, int xbins, int ybins )
{
	//assert( sameSize(in,out) );
	for( int i = 0; i < xbins; i++ ) {
		for( int j = 0; j < ybins; j++ ) {
			out(i,j) = median_2d( in.array, xbins, ybins, i, j, size, size );
		}
	}
	return;
}

void threshold_map_2d( array2d<double>& bg_map, array2d<double>& out, array2d<double>& in,
					   array2d<double>& sig_out, double k, double sigma, long *nax, int it, int st, int print)
{
	const long int ybins = *(nax+1), xbins = *(nax);
	//cout<<"XXXX="<<ybins<<"    "<<xbins<<endl;
	double c1 = -0.2336;
	double c2 = 0.0354;
	double c3 = 0.1830;
	double r_a = 5.0;
	double r_a1 = r_a*sigma;
	double r_a2 = 2./(r_a*r_a);
	double sum_arg = c1 + c2*k + c3*k*k;
	double c_cost = 2.73224; // = 1/(2.0*c3);
	double delta, SQR=0., sig_val;
	for( int i = 0; i < xbins; i++ ) {
		for( int j = 0; j < ybins; j++ ) {
			double n = circle_sum_2d( bg_map.array, xbins, ybins, i, j, int(r_a1));
			SQR = sqrt(n*r_a2);//M_PI
			out(i,j) = k*SQR + sum_arg;
			if (in(i,j) > 0) {
				delta = pow(SQR+c2, 2) - 4*c3*(c1- in(i,j));
				sig_val = (-(SQR + c2) + sqrt(delta))*c_cost;
				if (sig_val >k)
					//significat(i,j) = sig_val;
					sig_out(i,j)=sig_val;
				else
					//significat(i,j) = 0;
					sig_out(i,j)=0;
			}
			else
				//significat(i,j) = 0;
				sig_out(i,j)=0;

		}
	}
	//print_copy(significat, sig_out, st, nax);
	return;
}

double circle_sum_2d( double* image, int xmax, int ymax, int x, int y, int radius )
{
	double total = 0.;
	double base=0;
	int rad2 = radius*radius;
	int imin = MAX(0, x-radius);
	int imax = MIN(xmax-1,x+radius);
	int jmin = MAX(0, y-radius);
	int jmax = MIN(ymax-1,y+radius);
	
		for( int i = imin; i <= imax; i++ ) {
			for(int j = jmin; j <= jmax; j++ ) {
			base =image[j+i*ymax];//image[i+j*xmax]; ;
			if ( ((x-i)*(x-i) + (y-j)*(y-j)) <= rad2 ) {
				if (  base > 0.)
					//total += base[j]; //
						total +=base;
				else
					continue;
			}
		}
	}
	return total;
}


void MH_2d_transformed( fftw_complex result, double k1, double k2, double a1, double a2, double b1, double b2)
{
	double ka2 = pow(k1*a1,2) + pow(k2*a2,2);
	double real = 2.*M_PI*a1*a2*ka2*exp(-ka2*0.5);
	double imag = 0;
	result[0] = real;
	result[1] = imag;
	return;
}


void Gauss_filter_2d(fftw_complex gauss, double kx, double ky, double sigma)
{
	double arg = pow(kx*sigma, 2) + pow(ky*sigma, 2);
	double real = exp(-0.5*arg);
	double imag = 0;
	gauss[0] = real;
	gauss[1] = imag;
	return;
}


void conv_function(fftw_complex in_1, fftw_complex in_2, fftw_complex conv)
{
	conv[0] = in_1[0]*in_2[0] - in_1[1]*in_2[1];
	conv[1] = in_1[1]*in_2[0] + in_1[0]*in_2[1];
	return;
	
}



double factorial(int n)	
{			
	double result = 1.0;		
	if (n>0) {			
		do {
			result = result*double(n);
			--n;
		} while (n>1);
	}
	else if (n<0) {
		cout << "Errore nella funzione 'factorial'; e' stato introdotto un argomento = " <<
		n << "\n";
	}
	return (result);
}
