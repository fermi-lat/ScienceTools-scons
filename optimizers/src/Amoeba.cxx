/** 
 * @file Amoeba.cxx
 * @brief Use Nelder-Mead to minimize a function object.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <iostream>

#include "optimizers/Amoeba.h"

namespace {
#define NR_END 1
#define FREE_ARG char*

void nrerror(char * error_text)
/* Numerical Recipes standard error handler */
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}

double *vector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
	double *v;

	v=(double *)malloc((unsigned int) ((nh-nl+1+NR_END)*sizeof(double)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

void free_vector(double *v, long nl, long nh)
/* free a double vector allocated with vector() */
{
	free((FREE_ARG) (v+nl-NR_END));
}
#undef FREE_ARG
#undef NR_END

#define NMAX 5000
#define GET_PSUM \
		for (j=1;j<=ndim;j++) {\
		for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];\
		psum[j]=sum;}
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

double amotry(double **p, double *y, double *psum, int ndim, 
             double (*funk)(double *), int ihi, double fac)
{
	int j;
	double fac1,fac2,ytry,*ptry;

	ptry=vector(1,ndim);
	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=1;j<=ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	ytry=(*funk)(ptry);
	if (ytry < y[ihi]) {
		y[ihi]=ytry;
		for (j=1;j<=ndim;j++) {
			psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	free_vector(ptry,1,ndim);
	return ytry;
}
/* (C) Copr. 1986-92 Numerical Recipes Software 0@.1Y.. */

void amoeba(double **p, double y[], int ndim, double ftol, 
            double (*funk)(double *), int *nfunk)
{
/* 	double amotry(); */
	int i,ihi,ilo,inhi,j,mpts=ndim+1;
	double rtol,sum,swap,ysave,ytry,*psum;

	psum=vector(1,ndim);
	*nfunk=0;
        for (j=1;j<=ndim;j++) {
           for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];
           psum[j]=sum;
        }
	for (;;) {
		ilo=1;
		ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
		for (i=1;i<=mpts;i++) {
			if (y[i] <= y[ilo]) ilo=i;
			if (y[i] > y[ihi]) {
				inhi=ihi;
				ihi=i;
			} else if (y[i] > y[inhi] && i != ihi) inhi=i;
		}
		rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo]));
		if (rtol < ftol) {
			SWAP(y[1],y[ilo])
			for (i=1;i<=ndim;i++) SWAP(p[1][i],p[ilo][i])
			break;
		}
		if (*nfunk >= NMAX) nrerror("NMAX exceeded");
		*nfunk += 2;
		ytry=amotry(p,y,psum,ndim,funk,ihi,-1.0);
		if (ytry <= y[ilo])
			ytry=amotry(p,y,psum,ndim,funk,ihi,2.0);
		else if (ytry >= y[inhi]) {
			ysave=y[ihi];
			ytry=amotry(p,y,psum,ndim,funk,ihi,0.5);
			if (ytry >= ysave) {
				for (i=1;i<=mpts;i++) {
					if (i != ilo) {
						for (j=1;j<=ndim;j++)
							p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
						y[i]=(*funk)(psum);
					}
				}
				*nfunk += ndim;
                                for (j=1;j<=ndim;j++) {
                                   for (sum=0.0,i=1;i<=mpts;i++) 
                                      sum += p[i][j];
                                   psum[j]=sum;
                                }
			}
		} else --(*nfunk);
	}
	free_vector(psum,1,ndim);
}
#undef SWAP
#undef GET_PSUM
#undef NMAX
/* (C) Copr. 1986-92 Numerical Recipes Software 0@.1Y.. */

} // anonymous namespace

namespace optimizers {

Amoeba * Amoeba::s_instance = 0;
Functor * Amoeba::s_functor = 0;
std::vector<std::vector<double> > Amoeba::s_startingSimplex;
int Amoeba::s_npars;

double Amoeba::findMin(std::vector<double> & params, double tol) {
   std::vector<double> yvalues;
   yvalues.reserve(s_npars + 1);
   for (int i = 0; i < s_npars + 1; i++) {
      double yval = (*s_functor)(s_startingSimplex[i]);
      yvalues.push_back(yval);
   }
   int nevals;
   std::vector<double*> my_simplex;
   my_simplex.reserve(s_npars + 1);
   for (int i = 0; i < s_npars + 1; i++) {
      my_simplex.push_back(&s_startingSimplex[i][0]-1);
   }
   ::amoeba(&my_simplex[0]-1, &yvalues[0]-1, s_npars, tol, &func, &nevals);
   int imin = 0;
   double ymin = yvalues[imin];
   for (int i = 1; i < s_npars + 1; i++) {
      if (yvalues[imin] < ymin) {
         imin = i;
         ymin = yvalues[imin];
      }
   }
   params = s_startingSimplex[imin];
   return ymin;
}

void Amoeba::buildStartingSimplex(const std::vector<double> & params,
                                  std::vector<std::vector<double> > & simplex,
                                  double frac, bool addfrac) {
   int npars = params.size();
   simplex.clear();
   simplex.reserve(npars);
   simplex.push_back(params);
   for (int i = 1; i < npars+1; i++) {
      simplex.push_back(params);
      double & value = simplex.back()[i-1];
      if (addfrac) {
         value += frac;
      } else {
         if (value == 0) { // moderately fragile kluge for this special case
            value = frac;
         } else {
            value *= (1. + frac);
         }
      }
   }
}

} // namespace optimizers
