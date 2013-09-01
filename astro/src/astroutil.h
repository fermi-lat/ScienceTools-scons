#ifndef ASTROUTIL_H
#define ASTROUTIL_H
//#include "myorbit.h"
#define OreToDec(x,y,z)  (x)+(y)/60.+(z)/3600.

#define R_TERRA    6378.145e3F /*m*/
#define PI 3.141592654F
#define PI2 (PI*2.F)
#define D2R 0.017453292F
#define R2D 57.29578122F
#define H2R 0.261799387F
#define J1900 2415020.0F
#define B1950 2433282.423F
#define J2000 2451545.0F
#define SECS2RADS 206264.806247096355F

#define  J2		1.0822e-3

#define MinutesPerDay (24.*60.0F)
const double SecondsPerDay=86400;
#define CLUCE 2.997925e+5

#define EarthFlat (1/298.25)            /* Earth Flattening Coeff. */
#define SiderealSolar 1.0027379093
#define SidRate (PI2*SiderealSolar/SecondsPerDay)	/* radians/second */
#define GM 398600.0F			/* Kilometers^3/seconds^2 */
 
#define Epsilon (D2R/3600)     /* 1 arc second */
#define SunRadius 695000		
#define SunSemiMajorAxis  149598845.0  	    /* Kilometers 		   */
#define SQR(x) ((x)*(x))

typedef struct Cart{
	double x, y, z;	
}CARTES;

typedef struct Sphe{
double l, b, r;
}SPHER;

void range (double *s, double m);
void sphcart (SPHER s, CARTES *c);
void car2sph (CARTES c,SPHER *s);

double GetJD(int An,int Me,int Gio,double utc);
double GetGMST(double J_D);
double Kepler(double MeanAnomaly,double Eccentricity);
void matrix_transpose(double a[3][3], double b[3][3]);
void matrix_multiply(double a[3][3], double b[3][3], double c[3][3]);
void calc_unit_vector(double a[3], double b[3]);
void vector_cross_product(double a[3], double b[3], double c[3]);
void vector_matrix_multiply(const double a[3], double b[3][3], double c[3]);
double modulo(double a, double b);
void eqtogal(double ra, double dec,double *l,double *b);
void galtoeq(double l,double b, double *ra, double *dec);
void vector_dot_product(double a[3],double b[3],double *mod,double *angle);
double modulo_vet(double a[3]);
int vector2radec (double *pos, double *ra, double *dec);
void Sfer2xyz (double ra, double dec,double *vector);
int InsideSAA(double lon, double lat);
void GetRockMat(const double pp[3],const double rock,const int orb,double ppr[3]);
#endif