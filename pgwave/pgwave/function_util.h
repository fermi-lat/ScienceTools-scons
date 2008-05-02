/************************************************************
*	
*  function_util.h
*  Wave2D_7Feb
*
*  Created by Gabriele Discepoli on 7/02/06.
*  Copyright 2006 __Physics dept. University of Perugia__. All rights reserved.
*
************************************************************/
#ifndef __FUNCTION_UTIL_H
#define __FUNCTION_UTIL_H

#include "utile.h"

#ifndef M_PI
//#undef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
//#undef M_E
#define M_E 2.71828182845904523536
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

using namespace std;

/* These functions ask PIL to read parameters and return false, printing error message, if it failed. */
/*bool readParam_string( const char name[], char* result );
bool readParam_int( const char name[], int* result );
bool readParam_double( const char name[], double* result );
bool readParam_double_vector( const char name[], int len, double* result );
*/
void fitsio_all_in_one(fitsfile *fptr, char *filename, char *filename_header, array2d<double>& image, long *nax, int print);
void fitsio_all_in_one(fitsfile *fptr, char *filename, array2d<double>& image, long *nax, int print);
void fitsio_all_in_one(fitsfile *fptr, char *filename, char *filename_header, array3d<double>& image, long *nax, int print);
void fitsio_all_in_one(fitsfile *fptr, char *filename, array3d<double>& image, long *nax, int print);

void print_copy(array2d<double>& in, array3d<double>& print, int step, long *nax);

void cp_input(array2d<double>& service, array2d<double>& reas_image, long *nax);

void find(char *nome_file, MyImageAnalisys *im, int min_num, int side_box);

void find_stat(char *nome_file, MyImageAnalisys *im, fitsfile *fptr_in, int print);

void printerror( int status);

void remove_twin(detect_sources *good_list, double dist);

void erase_source(detect_sources *ogg, array2d<double>& input, double dist, long *nax, int print);

void erase_source(detect_sources *ogg, double dist);

void simul_bright(detect_sources *erase_list, array2d<double>& input, long *nax, int print);

void chi_quadro(detect_sources *list, array2d<double>& input, array2d<double>& simul, long *nax, int print);

void back_sub(array2d<double>& input, array2d<double>& clean, double x, double y, double sigma, long *nax);

void test(array2d<double>& clean, array2d<double>& simul, double x, double y, double sigma, long *nax, int print);

void fill_gauss(detect_sources *erase, int hh, array2d<double>& empty, long *nax);

void erase_border(detect_sources *ogg, int flag, long *nax, int far);

void SNR_cut(MyImageAnalisys *im, detect_sources *ogg, double SNR, double scale, double min_pix, array2d<double>& signi);

int m_num_fit(int s);

void select(detect_sources *list, detect_sources *flag, double min_pix, int print, int enable, int N_scale, double scala[]);

#endif
