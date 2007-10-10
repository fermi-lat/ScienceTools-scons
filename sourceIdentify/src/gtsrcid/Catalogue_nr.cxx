/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
------------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include "Catalogue.h"


/* Definitions ______________________________________________________________ */


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Type defintions __________________________________________________________ */


/* Globals __________________________________________________________________ */


/* Private Prototypes _______________________________________________________ */


/*----------------------------------------------------------------------------*/
/*                                 nr_gammln                                  */
/* -------------------------------------------------------------------------- */
/* Private method: ln(gamma)                                                  */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
double nr_gammln(double arg) {

    // Initialise
    double x   = arg;
    double y   = x;
    double tmp = x + 5.5;

    // Decrement
    tmp -= (x+0.5)*log(tmp);

    // Develop
    double ser = 1.000000000190015;
    ser += 76.18009172947146      / (++y);
    ser -= 86.50532032941677      / (++y);
    ser += 24.01409824083091      / (++y);
    ser -=  1.231739572450155     / (++y);
    ser +=  0.1208650973866179e-2 / (++y);
    ser -=  0.5395239384953e-5    / (++y);

    // Assign result
    double res = -tmp+log(2.5066282746310005*ser/x);

    // Return result
    return res;

}


/*----------------------------------------------------------------------------*/
/*                                 nr_gammp                                   */
/* -------------------------------------------------------------------------- */
/* Private method: returns incomplete gamma function P(a,x)                   */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
double nr_gammp(double a, double x) {

    // Check arguments
    if (x < 0.0 || a <= 0.0) {
      // nrerror("Invalid arguments in routine gammp");
      return (0.0);
    }

    // Determine result
    if (x < (a+1.0)) {
      double gamser;
      double gln;
      nr_gser(&gamser, a, x, &gln);
      return gamser;
    }
    else {
      double gammcf;
      double gln;
      nr_gcf(&gammcf, a, x, &gln);
      return (1.0-gammcf);
    }

}


/*----------------------------------------------------------------------------*/
/*                                 nr_gammq                                   */
/* -------------------------------------------------------------------------- */
/* Private method: returns incomplete gamma function Q(a,x) = 1 - P(a,x)      */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
double nr_gammq(double a, double x) {

    // Check arguments
    if (x < 0.0 || a <= 0.0) {
      // nrerror("Invalid arguments in routine gammp");
      return (0.0);
    }

    // Determine result
    if (x < (a+1.0)) {
      double gamser;
      double gln;
      nr_gser(&gamser, a, x, &gln);
      return 1.0 - gamser;
    }
    else {
      double gammcf;
      double gln;
      nr_gcf(&gammcf, a, x, &gln);
      return gammcf;
    }

}


/*----------------------------------------------------------------------------*/
/*                                  nr_gser                                   */
/* -------------------------------------------------------------------------- */
/* Private method: nr_gser                                                    */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
void nr_gser(double *gamser, double a, double x, double *gln) {

    // Set constants
    const int    ITMAX = 100;
    const double EPS   = 3.0e-7;

    // Evaluate function
    *gln = nr_gammln(a);

    // Return if x is non positive
    if (x <= 0.0) {
      *gamser = 0.0;
       return;
    }

    // Evaluate
    else {
      double ap  = a;
      double sum = 1.0/a;
      double del = sum;
      for (int n = 1; n <= ITMAX; ++n) {
        ++ap;
        del *= x/ap;
        sum += del;
        if (fabs(del) < fabs(sum)*EPS) {
          *gamser = sum*exp(-x+a*log(x)-(*gln));
          return;
        }
      }
      *gamser = sum*exp(-x+a*log(x)-(*gln));
      return;
    }

}

/*----------------------------------------------------------------------------*/
/*                                   nr_gcf                                   */
/* -------------------------------------------------------------------------- */
/* Private method: nr_gser                                                    */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
void nr_gcf(double *gammcf, double a, double x, double *gln) {

    // Set constants
    const int    ITMAX = 100;
    const double EPS   = 3.0e-7;
    const double FPMIN = 1.0e-30;

    // Evaluate function
    *gln = nr_gammln(a);

    // Initialise
    double b = x + 1.0 - a;
    double c = 1.0 / FPMIN;
    double d = 1.0 / b;
    double h = d;

    // Iterate
    int i;
    for (i = 1; i <= ITMAX; ++i) {
      double an = -i*(i-a);
      b += 2.0;
      d = an*d+b;
      if (fabs(d) < FPMIN) d = FPMIN;
      c = b+an/c;
      if (fabs(c) < FPMIN) c = FPMIN;
      d = 1.0/d;
      double del = d*c;
      h *= del;
      if (fabs(del-1.0) < EPS)
        break;
    }
    if (i > ITMAX) {
      //nrerror("a too large, ITMAX too small in gcf");
    }

    // Set result
    *gammcf = exp(-x+a*log(x)-(*gln))*h;

    // Return
    return;

}

/* Namespace ends ___________________________________________________________ */
}
