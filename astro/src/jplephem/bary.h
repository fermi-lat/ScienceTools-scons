#ifndef BARY_H
#define BARY_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
#include <time.h>
#include "fitsio.h"
//#include "longnam.h"

#define SECDAY	86400			/* Seconds per day */
#define EPHEM     "JPLEPH"

typedef struct {
  enum TimeSys ts ;
  long MJDint ;
  double MJDfr ;
} MJDTime ;


/*  Internally referenced functions  */
int initephem (int, int *, double *, double *, double *) ;
const double *dpleph (double *, int, int) ;
fitsfile *openFFile (const char *) ;

#endif


