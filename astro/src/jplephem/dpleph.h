const double *dpleph (double *jd, int ntarg, int ncent);
int state (double *jd, int ntarg, int ncent, double *posn);
int getstate (int ntarg, double t1, double *posn);
int interp (double *buf, double t1, int ncf, int na, double *pv);
int readephem (long recnum);
int initephem (int ephnum, int *denum, double *c, double *radsol, double *msol);
double findcval (char **cnam, double *cval, long n, char *name);


const double *dpleph (double *jd, int ntarg, int ncent)
{
  static double posn[12] ;
  long jdint ;
  double jdtmp ;
  int i ;

/*
 *     Make sure jd is in proper range
 */
  jdint = (long) jd[0] ;
  jdtmp = (double) jd[0] - jdint ;
  jd[0] = (double) jdint ;
  jd[1] += jdtmp ;
  while ( jd[1] >= 0.5 ) {
    jd[1]-- ;
    jd[0]++ ;
  }
  while ( jd[1] < -0.5 ) {
    jd[1]++ ;
    jd[0]-- ;
  }

/*
 *      Get the positions and return
 */
  if ( state (jd, ntarg, ncent, posn) )
    return (const double *) NULL ;
  else {
    return (const double *) posn ;
  }
}