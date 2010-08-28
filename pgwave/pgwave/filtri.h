/*
 *  filtri.h
 *  simul_21_oct
 *
 *  Created by Gabriele on 24/10/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __FILTRI_H
#define __FILTRI_H

#include "function_util.h"
#include "fftw/fftw3.h"


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


/** returns the median value in a box of size [2*xsize+1,2*ysize+1].
@param image the 2d row-major array
@param xmax the length of the first dimension of the array
@param ymax the length of the second dimension of the array
@param x the x coordinate of the center of the box
@param y the y coordinate of the center of the box
@param xsize the pixels to span on either side of x 
@param ysize the pixels to span on either side of y */
double median_2d( double* image, int xmax, int ymax, int x, int y, int xsize, int ysize );

void filtro_gaussiano(array2d<fftw_complex>& in, array2d<fftw_complex>& out, double sigma, double xbins, double ybins);

void wavelet_filter(array2d<fftw_complex>& in, array2d<fftw_complex>& out, double scale, double xbins, double ybins);

/** performs median filtering.
@param in the array4d to be filtered (left unchanged).
@param out the results of the filtering are put here.
@param size the square used is of size 2*scale+1. */
void median_filter_2d( array2d<double>& in, array2d<double>& out, int size, int xbins, int ybins );

double circle_sum_2d( double* image, int xmax, int ymax, int x, int y, int radius);

void threshold_map_2d( array2d<double>& bg_map, array2d<double>& out, array2d<double>& in,
					   array2d<double>& sig_out, double k, double sigma, long *nax, int it, int st, int print);

void MH_2d_transformed( fftw_complex result, double k1, double k2, double a1, double a2, double b1, double b2);

void Gauss_filter_2d(fftw_complex gauss, double kx, double ky, double sigma);

void conv_function(fftw_complex in_1, fftw_complex in_2, fftw_complex conv);

double factorial(int n);

#endif
