// SolSystem.cpp: implementation of the SolSystem class.
//
//////////////////////////////////////////////////////////////////////
#include "SolSystem.h"
#include <iostream>
#include <cmath>
#include <string>
#include "vsop87_data.h"
//#include "chap95_data.h"
//#include "CTimeClass.h"


#define A2000 2451545.00000000

#define Conv_Rad 0.017453292
#define Conv_Grad 57.29578122
#define Conv_Ora_Rad 0.261799387

#define	SPD		(24.0*3600.0)	/* seconds per day */
#define MJD0	2415020.0
#define J2000	(2451545.0 - MJD0) 
#define	MAU		(1.4959787e11)	/* m / au */
#define	ERAD	(6.37816e6)	/* earth equitorial radius, m */
#define	SIDRATE	.9972695677



extern "C" {void precess (double mjd1, double mjd2, double *ra, double *dec);
void range (double *v, double r);
int Conv_GST_UT(double jd,double gst,double *tu);
int Sun_Pos(double jd,double *ra,double *decs,double *lon,double *r);
int Conv_lb_ad(double jd,double l,double b,double *ra,double *dec);
void riset (double ra, double dec, double lat, double dis,double *lstr,double * lsts,
	   double *azr, double *azs, int *status);
int vsop87 (double mjd, int obj, double prec, double *ret);
void sunpos (double mjd, double *lsn, double *rsn, double *bsn);

void cal_mjd (int mn, double dy, int yr, double *mjd);
double mjd_day(double jd);
void rnd_second (double *t);
void year_mjd (double y, double *mjd);
void mjd_year (double mjd, double *yr);
void mjd_dpm (double mjd, int *ndays);
int mjd_dow (double mjd,int *dow);
void mjd_cal (double mjd, int *mn, double dy, int *yr);

void cartsph (double x,double  y,double  z,double *l, double *b, double *r);
void sphcart (double l, double b, double r, double *x, double *y, double *z);
void zero_mem (void *loc, unsigned len);

void reduce_elements (double mjd0, double mjd, double inc0, double ap0, double om0, double *inc, double *ap, double *om);
void obliquity (double mjd, double *eps);
void anomaly (double ma, double s, double *nu, double *ea);
void comet (double mjd, double ep, double inc, double ap, double qp, double om, double *lpd, double *psi, double *rp, double *rho, double *lam, double *bet);
int chap95 (double mjd, int obj, double prec, double *ret);
void moon (double mjd, double *lam, double *bet, double *rho,
    double *msp, double *mdp);
void nut_eq (double mjd, double *ra, double *dec);
void nutation (double mjd, double *deps, double *dpsi);
void ta_par (double tha, double tdec, double phi, double ht, double *rho, double *aha, double *adec);
void refract (double pr,double tr, double ta,double *aa);
void ab_eq (double mjd, double lsn, double *ra, double *dec);
void ab_ecl (double mjd, double lsn, double *lam, double *bet);
void unrefract (double pr, double tr, double aa, double *ta);

void eq_ecl (double mjd, double ra, double dec, double *lat, double *lng);
void ecl_eq (double mjd, double lat, double lng, double *ra, double *dec);

void aa_hadec (double lat, double alt,double  az, double *ha, double *dec);
void hadec_aa (double lat, double ha, double dec, double *alt, double *az);

void solve_sphere (double A, double b, double cc, double sc, double *cap, double *Bp);
double deltat(double mjd);
double mm_mjed (double mjd);
double Tempo_Siderale(double J_D,double Ora_Un_Dec);
void now_lst(double mdj,double lng, double *lstp);
void plans (double mjd, int p, double *lpd0, double *psi0, double *rp0, double *rho0, double *lam, double *bet, double *dia, double *mag);
}
#define	TMACC	(10./3600./24.0)	/* convergence accuracy, days */


/*static void SolSystem::elongation (double lam, double bet, double lsn, double *el);
static void SolSystem::ephi_pos(double mdj,int obj,double lng,double lat,double elev, double hlong, double hlat,double bet, double lam, double *rho, double *ra1, double *dec1);
static int SolSystem::ephi_moon(double jd,double lng, double lat,double elev, double *ra, double *dec, double *phase, double *dist);
static void SolSystem::deflect (double mjd1, double lpd, double psi, double lsn, double rsn, double rho, double *ra,double *dec);
static int SolSystem::ephi_sun(double *ra, double *dec, double *dist);
static void SolSystem::ephi_planet(double jd,int obj,double lng, double lat,double elev, double *ra, double *dec, double *phase, double *dist);
static int SolSystem::find_transit (double mjd, double lat,double lng, double dt, double dis, double *alt, double *mjdt);
static int SolSystem::find_0alt (double mjd, double lat,double lng,  double dt, double dis, double *az, double *mjdt);

*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SolSystem::SolSystem()
{
	LOCSET=-1;	
	dis=9.89e-3;
	lat=degrad(43.);
	lng=0.0;
	elev=0.0;
	GG=A2000;
	Obj=8;
}

SolSystem::~SolSystem()
{
	LOCSET=-1;
}

void SolSystem::RiseSet()
{
	ephi_riset ();

}

void SolSystem::RiseSet(double diso)
{
	dis=degrad(diso);
	ephi_riset ();

}


void SolSystem::CalculatePos(double jd, int obo)
{
	if(LOCSET){
		GG=jd;
		if(obo>=0 && obo<=9)
		{
			Obj=obo;
		switch(Obj){
		case 8: ephi_sun();
			break;
		case 9:ephi_moon();
			break;
		default:
			ephi_planet();
		break;
		}	
		}
	}
}

void SolSystem::CalculatePos(double JD)
{
	if(LOCSET){
		//Obj=obo;
		SetJD(JD);
		switch(Obj){
		case 8: ephi_sun();
			break;
		case 9:ephi_moon();
			break;
		default:
				ephi_planet();
		break;
		}	
	}
}

void SolSystem::SetLocation(double lati, double loni, double eleva)
{
	lat=degrad(lati);
	lng=degrad(loni);
	elev=eleva;
	
	LOCSET=1;
}
void SolSystem::SetJD(double jd)
{
	GG=jd;
}

void SolSystem::SetObj(int nob)
{	
	if(nob>=0 && nob<=9)
		Obj=nob;
}

void SolSystem::elongation (double lam, double bet, double lsn, double *el)
{
	*el = acos(cos(bet)*cos(lam-lsn));
	if (lam>lsn+PI || (lam>lsn-PI && lam<lsn)) *el = - *el;
}

void SolSystem::deflect (double mjd1, double lpd, double psi, double lsn, double rsn, double rho, double *ra,double *dec)
/* double mjd1;		 epoch 
double lpd, psi;heliocentric ecliptical long / lat 
double rsn, lsn;	 distance and longitude of sun 
double rho;		geocentric distance 
double *ra, *dec;	 geocentric equatoreal */
{
	double hra, hdec;	/* object heliocentric equatoreal */
	double el;		/* HELIOCENTRIC elongation object--earth */
	double g1, g2;		/* relativistic weights */
	double u[3];		/* object geocentric cartesian */
	double q[3];		/* object heliocentric cartesian unit vect */
	double e[3];		/* earth heliocentric cartesian unit vect */
	double qe, uq, eu;	/* scalar products */
	int i;			/* counter */

#define G	1.32712438e20	/* heliocentric grav const; in m^3*s^-2 */
#define c	299792458.0	/* speed of light in m/s */

	elongation(lpd, psi, lsn-PI, &el);
	el = fabs(el);
	/* only continue if object is within about 10 deg around the sun
	 * and not obscured by the sun's disc (radius 0.25 deg)
	 *
	 * precise geocentric deflection is:  g1 * tan(el/2)
	 *	radially outwards from sun;  the vector munching below
	 *	just applys this component-wise
	 *	Note:	el = HELIOCENTRIC elongation.
	 *		g1 is always about 0.004 arc seconds
	 *		g2 varies from 0 (highest contribution) to 2
	 */
	if (el<degrad(170) || el>degrad(179.75)) return;

	/* get cartesian vectors */
	sphcart(*ra, *dec, rho, u, u+1, u+2);

	ecl_eq(mjd1, psi, lpd, &hra, &hdec);
	sphcart(hra, hdec, 1.0, q, q+1, q+2);

	ecl_eq(mjd1, 0.0, lsn-PI, &hra, &hdec);
	sphcart(hra, hdec, 1.0, e, e+1, e+2);

	/* evaluate scalar products */
	qe = uq = eu = 0.0;
	for(i=0; i<=2; ++i) {
	    qe += q[i]*e[i];
	    uq += u[i]*q[i];
	    eu += e[i]*u[i];
	}

	g1 = 2*G/(c*c*MAU)/rsn;
	g2 = 1 + qe;

	/* now deflect geocentric vector */
	g1 /= g2;
	for(i=0; i<=2; ++i)
	    u[i] += g1*(uq*e[i] - eu*q[i]);
	
	/* back to spherical */
	cartsph(u[0], u[1], u[2], ra, dec, &rho);	/* rho thrown away */
#undef G
#undef c
}


int SolSystem::ephi_sun()
{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double bsn;		/* true latitude beta of sun */
	double dhlong;
	double mjd, mjed;
	double ra,dec;

	mjd=GG-MJD0;
	mjed=mm_mjed(mjd);


	sunpos (mjed, &lsn, &rsn, &bsn);/* sun's true coordinates; mean ecl. */

	Sdist = 0.0;
	Elong = 0.0;
	Phase = 100.0;
	Mag= -26.8;	/* TODO */
	dhlong = lsn-PI;	/* geo- to helio- centric */
	range (&dhlong, 2*PI);
	Hlong = dhlong;
	Hlat = -bsn;
	ephi_pos(mjd,SUN,bsn, lsn, &rsn, &ra,&dec);
	/* fill sun's ra/dec, alt/az in op */
	//cir_pos (np, bsn, lsn, &rsn, op);
	Edist = rsn;
	Size = raddeg(4.65242e-3/rsn)*3600*2;
	HRa=radhr(ra);
	DDec=raddeg(dec);
	Ra=ra;
	Dec=dec;
	return (0);
}




int SolSystem::ephi_moon()

{
	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double lam;    		/* geocentric ecliptic longitude */
	double bet;    		/* geocentric ecliptic latitude */
	double edistau;		/* earth-moon dist, in au */
	double el;		/* elongation, rads east */
	double ms;		/* sun's mean anomaly */
	double md;		/* moon's mean anomaly */
	double i;
	double mjd, mjed;
	double  ra,dec;

	mjd=GG-MJD0;
	mjed=mm_mjed(mjd);

	moon (mjed, &lam, &bet, &edistau, &ms, &md);	/* mean ecliptic & EOD*/
	sunpos (mjed, &lsn, &rsn, NULL);		/* mean ecliptic & EOD*/

	//op->s_hlong = lam;			/* save geo in helio fields */
	//op->s_hlat = bet;
	Hlong=lam;
	Hlat=bet;
	/* find angular separation from sun */
	elongation (lam, bet, lsn, &el);
	Elong = raddeg(el);		/* want degrees */

	/* solve triangle of earth, sun, and elongation for moon-sun dist */
	Sdist= sqrt (edistau*edistau + rsn*rsn - 2.0*edistau*rsn*cos(el));
		//*dist= sqrt (edistau*edistau + rsn*rsn - 2.0*edistau*rsn*cos(el));


	/* TODO: improve mag; this is based on a flat moon model. */
	//set_smag (op, -12.7 + 2.5*(log10(PI) - log10(PI/2*(1+1.e-6-cos(el)))));

	/* find phase -- allow for projection effects */
	i = 0.1468*sin(el)*(1 - 0.0549*sin(md))/(1 - 0.0167*sin(ms));
	//op->s_phase = (1+cos(PI-el-degrad(i)))/2*100;

		Phase = (1+cos(PI-el-degrad(i)))/2*100;

	/* fill moon's ra/dec, alt/az in op and update for topo dist */
	//cir_pos (np, bet, lam, &edistau, op);
	ephi_pos(mjd,MOON,bet, lam, &edistau, &ra,&dec);
	//op->s_edist = edistau;
	HRa=radhr(ra);
	DDec=raddeg(dec);
	Ra=ra;
	Dec=dec;
	Edist=edistau;
	//Size = 3600*2.0*raddeg(asin(MRAD/MAU/edistau));
						/* moon angular dia, seconds */

	return (0);
}

void SolSystem::ephi_planet()
{
#if 0
    
	double el;		/* elongation */
	double f;		/* fractional phase from earth */
	double mjd, mjed;


	double lsn, rsn;	/* true geoc lng of sun; dist from sn to earth*/
	double lpd, psi;	/* heliocentric ecliptic long and lat */
	double rp;		/* dist from sun */
	double rho;		/* dist from earth */
	double lam, bet;	/* geocentric ecliptic long and lat */
	double dia, mag;	/* angular diameter at 1 AU and magnitude */
	double ra, dec;
	mjd=GG-MJD0;
	mjed=mm_mjed(mjd);
	/* compute elongation and phase */
	sunpos (mjed, &lsn, &rsn, 0);

	/* find helio long/lat; sun/planet and earth/plant dist; ecliptic
	 * long/lat; diameter and mag.
	 */
	plans(mjed, Obj, &lpd, &psi, &rp, &rho, &lam, &bet, &dia, &mag);
	Size=dia;
	Mag=mag;

	elongation (lam, bet, lsn, &el);
	Elong=el = raddeg(el);
	//op->s_elong = el;
	 f = 0.25 * ((rp+rho)*(rp+ rho) - rsn*rsn)/(rp*rho);
	Phase= f*100.0; /* percent */

	/* set heliocentric long/lat; mean ecliptic and EOD */
	Hlong = lpd;
	Hlat = psi;

	/* fill solar sys body's ra/dec, alt/az in op */
	//cir_pos (np, bet, lam, rho, op);        /* updates rho */
	ephi_pos(mjd,Obj,bet, lam, &rho, &ra,&dec);
	HRa=radhr(ra);
	DDec=raddeg(dec);
	Ra=ra;
	Dec=dec;
	/* set earth/planet and sun/planet distance */
	Edist = rho;
	//op->s_sdist = rp;
#endif
}



void SolSystem::ephi_pos(double mjd,int obj, double bet, double lam, double *rho, double *ra1, double *dec1)
//cir_pos (np, bet, lam, rho, op)
//Now *np;
//double bet, lam;/* geo lat/long (mean ecliptic of date) */
//double *rho;	/* in: geocentric dist in AU; out: geo- or topocentic dist */
//Obj *op;	/* object to set s_ra/dec as per epoch */
{
	double ra, dec;		/* apparent ra/dec, corrected for nut/ab */
	double tra, tdec;	/* astrometric ra/dec, no nut/ab */
	double lsn, rsn;	/* solar geocentric (mean ecliptic of date) */
	double ha_in, ha_out;	/* local hour angle before/after parallax */
	double dec_out;		/* declination after parallax */
	double dra, ddec;	/* parallax correction */
	double alt, az;		/* current alt, az */
	double lst;             /* local sidereal time */
	double rho_topo;        /* topocentric distance in earth radii */
	double temp_ra,temp_dec;
	double mjed;
	
	/* convert to equatoreal [mean equator, with mean obliquity] */
	ecl_eq (mjd, bet, lam, &ra, &dec);
	tra = ra;	/* keep mean coordinates */
	tdec = dec;

	/* get sun position */
	mjed=mm_mjed(mjd);
	sunpos(mjed, &lsn, &rsn, NULL);

	/* allow for relativistic light bending near the sun.
	 * (avoid calling deflect() for the sun itself).
	 */
	//if (!is_planet(op,SUN) && !is_planet(op,MOON))
		 if((obj!=SUN)&&(obj!=MOON))
	deflect (mjd, Hlong, Hlat, lsn, rsn, *rho, &ra, &dec);

	/* correct ra/dec to form geocentric apparent */
	nut_eq (mjd, &ra, &dec);
	//if (!is_planet(op,MOON))
	if(obj!=MOON)
		ab_eq (mjd, lsn, &ra, &dec);
	GRa = raddeg(ra);
	GDec = raddeg(dec);
	temp_ra = ra;	/* keep geocentric apparent  coordinates */
	temp_dec = dec;
	//cout<<"NNNNNNNNN  "<<radhr(ra)<<"   "<<raddeg(dec)<<endl;
	//printang(radhr(ra));
	//printang(raddeg(dec));
	/* find parallax correction for equatoreal coords */
/* inserire last..
	now_lst (np, &lst);
*/
	now_lst(mjd,lng,&lst);
	ha_in = hrrad(lst) - ra;
	rho_topo = *rho * MAU/ERAD;             /* convert to earth radii */
	double dd=(elev/1000.)/ERAD;
	ta_par (ha_in, dec, lat, dd, &rho_topo, &ha_out, &dec_out);

	/* transform into alt/az and apply refraction */
	ha_in = hrrad(lst) - ra;
	hadec_aa (lat, ha_out, dec_out, &alt, &az);
	//refract (pressure, temp, alt, &alt);

	Alt = alt;
	Az = az;

	/* Get parallax differences and apply to apparent or astrometric place
	 * as needed.  For the astrometric place, rotating the CORRECTIONS
	 * back from the nutated equator to the mean equator will be
	 * neglected.  This is an effect of about 0.1" at moon distance.
	 * We currently don't have an inverse nutation rotation.
	 */
//	if (pref_get(PREF_EQUATORIAL) == PREF_GEO) {
	    /* no topo corrections to eq. coords */
//	    dra = ddec = 0.0;
//	} else {
	    dra = ha_in - ha_out;	/* ra sign is opposite of ha */
	    ddec = dec_out - dec;
	    *rho = rho_topo * ERAD/MAU; /* return topocentric distance in AU */
//	}

	/* fill in ra/dec fields */
//	if (epoch == EOD) {		/* apparent geo/topocentric */
	    ra = ra + dra;
	    dec = dec + ddec;
//	} else {			/* astrometric geo/topocent */
//	    ra = tra + dra;
//	    dec = tdec + ddec;
//	    precess (mjd, epoch, &ra, &dec);
//	}
	range(&ra, 2*PI);
	//op->s_ra = ra;
	//op->s_dec = dec;
	*ra1=ra;
	*dec1=dec;
	//printang(radhr(ra));
}

/* find where and when an object, op, will rise and set and
 *   it's transit circumstances. all times are utc mjd, angles rads e of n.
 * dis is the angle down from an ideal horizon, in rads (see riset()).
 * N.B. dis should NOT include refraction, we do that here.
 */
void SolSystem::ephi_riset ()
{
	double mjdn;	/* mjd of local noon */
	double lstn;	/* lst at local noon */
	double lr, ls;	/* lst rise/set times */
	double ar, as;	/* az of rise/set */
	double ran;	/* RA at noon */
	int rss;	/* temp status */
	
	double mjd=GG-MJD0;
	/* assume no problems initially */
	Flags = 0;

	/* start the iteration at local noon */
	mjdn = mjd_day(mjd)+0.5;
	now_lst (mjdn,lng, &lstn);
	//ephi_sun(2451690.0,lng,lat,.0, &ra1, &dec1, &ls1);
	SetJD(mjdn+MJD0);
	//cout<<Obj<<endl;
	switch(Obj){
		case 8: ephi_sun();
			break;
		case 9:	ephi_moon();
			break;
		default:
			   ephi_planet();
		break;
		}	
	ran=Ra;
	//cout<<"MMMM "<<lstn<<"  "<<mjd_hr(mjdn)<<endl;
	//printang(ra1);
	/* first approximation is to find rise/set times of a fixed object
	 * at the current epoch in its position at local noon.
	 * N.B. add typical refraction for initial go/no-go test. if it
	 *   passes, real code does refraction rigorously.
	 */
	riset (Ra,Dec, lat, dis+0.01, &lr, &ls, &ar, &as, &rss);
	switch (rss) {
	case  0:  break;
	case  1: Flags = RS_NEVERUP; RiseTu=0.0; RiseAz=0.0;return;
	case -1: Flags = RS_CIRCUMPOLAR; goto dotransit;
	default: Flags = RS_ERROR; RiseTu=0.0; RiseAz=0.0;return;
	}

	/* iterate to find better rise time */

	double mjdt;
	switch (find_0alt (mjdn,(lr - lstn)/SIDRATE, dis, &ar, &mjdt)) {
	case 0: /* ok */
	    RiseTu = mjd_hr(mjdt);
	    RiseAz = raddeg(ar);
	    break;
	case -1: /* obj_cir error */
	    Flags |= RS_RISERR;
		RiseTu=0.0; RiseAz=0.0;
	    break;
	case -2: /* converged but not today */ /* FALLTHRU */
	case -3: /* probably never up */
	    Flags |= RS_NORISE;
		RiseTu=0.0; RiseAz=0.0;
	    break;
	}

	/* iterate to find better set time */
	switch (find_0alt (mjdn, (ls - lstn)/SIDRATE, dis, &ar, &mjdt)) {
	case 0: /* ok */
	    SetTu =  mjd_hr(mjdt);
	    SetAz = raddeg(ar);
	    break;
	case -1: /* obj_cir error */
	    Flags |= RS_SETERR;
		SetTu=-1.0; SetAz=0.0;
	    break;
	case -2: /* converged but not today */ /* FALLTHRU */
	case -3: /* probably circumpolar */
	    Flags |= RS_NOSET;
		SetTu=-1.0; SetAz=0.0;
	    break;
	}

	/* can try transit even if rise or set failed */
    dotransit:
	switch (find_transit (mjdn,(radhr(ran) - lstn)/SIDRATE, dis, &as, &mjdt)) {
	case 0: /* ok */
	    TranTu = mjd_hr(mjdt);
	    TranAlt = raddeg(as);
	    break;
	case -1: /* did not converge */
	    Flags |= RS_TRANSERR;
		TranTu=-1.0;TranAlt=0.0;
	    break;
	case -2: /* converged but not today */
	    Flags |= RS_NOTRANS;
		TranTu=-1.0;TranAlt=0.0;
	    break;
	}
}

int SolSystem::find_0alt (double mjd, double dt, double dis, double *az, double *mjdt)
{
#define	MAXPASSES	20		/* max iterations to try */
#define	FIRSTSTEP	(1.0/60.0/24.0)	/* first time step, days */

	double a0 = 0, alt=0;
	double mjdn = mjd;
	int npasses;
	/* insure initial guess is today -- if not, move by 24 hours */
	if (dt < -12.0)
	    dt += 24.0;
	if (dt > 12.0)
	    dt -= 24.0;
	//cout<<"DT="<<dt<<endl;
	/* convert dt to days for remainder of algorithm */
	dt /= 24.0;

	/* use secant method to look for s_alt + dis == 0 */
	
	npasses = 0;
	do {
	    double a1,lst,ha;
		
	    mjd += dt;
		*mjdt=mjd;
		//now_lst (mjd,lng, &lst);
		SetJD(mjd+MJD0);
		switch(Obj){
		case 8: ephi_sun();
			break;
		case 9:ephi_moon();
			break;
		default:
				ephi_planet();
		break;
		}	
		now_lst (mjd,lng, &lst);
		//printang(lst);
		ha= hrrad(lst) - Ra;
		
	    hadec_aa (lat, ha, Dec, &alt, az);
		//cout<<"MMMM "<<raddeg(*az)<<" ALT= "<<mjd_hr(mjd)<<endl;
	    a1 = alt;
		
	    dt = (npasses == 0) ? FIRSTSTEP : (dis+a1)*dt/(a0-a1);
	    a0 = a1;

	} while (++npasses < MAXPASSES && fabs(dt) > TMACC);

	/* return codes */
	if (npasses == MAXPASSES)
	    return (-3);
	
	return (fabs(mjdn-mjd) < .5 ? 0 : -2);

#undef	MAXPASSES
#undef	FIRSTSTEP
}

/* find when the given object transits. start the search when LST matches the
 *   object's RA at noon.
 * if ok, return 0 with np and op set to the transit conditions; if can't
 *   converge return -1; if converges ok but not today return -2.
 * N.B. we assume np is passed set to local noon.
 */
int SolSystem::find_transit (double mjd, double dt, double /*dis*/, double *alt, double *mjdt)
{
#define	MAXLOOPS	10
#define	MAXERR		(0.25/60.)		/* hours */
	double mjdn = mjd;
	double lst,az;
	int i;

	/* insure initial guess is today -- if not, move by 24 hours */
	if (dt < -12.0)
	    dt += 24.0;
	if (dt > 12.0)
	    dt -= 24.0;

	i = 0;
	double ha;
	do {
	    mjd += dt/24.0;
		SetJD(mjd+MJD0);
		switch(Obj){
		case 8: ephi_sun();
			break;
		case 9:ephi_moon();
			break;
		default:
			  ephi_planet();
		break;
		}	
	    now_lst (mjd,lng, &lst);
		ha= hrrad(lst) - Ra;
		hadec_aa (lat, ha, Dec, alt, &az);
	    dt = (radhr(Ra) - lst);
	    if (dt < -12.0)
		dt += 24.0;
	    if (dt > 12.0)
		dt -= 24.0;
	} while (++i < MAXLOOPS && fabs(dt) > MAXERR);

	/* return codes */
	if (i == MAXLOOPS)
	    return (-1);
	*mjdt=mjd;
	return (fabs(mjd - mjdn) < 0.5 ? 0 : -2);

#undef	MAXLOOPS
#undef	MAXERR
}




/*
*****************************************************************************************
				R O U T I N E S ASTRO

*****************************************************************************************
*/


double Tempo_Siderale(double J_D,double Ora_Un_Dec)
{
	double M,T,T1,Tempo_Siderale_0,Tempo_Siderale_Ora,Tempo_Siderale_Loc;

	T = ((J_D) - A2000) / 36525.;
	T1 = (24110.54841 + 8640184.812866 * T + 0.0093103 * T * T)/86400.0;
	Tempo_Siderale_0 = modf(T1,&M) * 24.;
	Tempo_Siderale_Ora = Tempo_Siderale_0 + Ora_Un_Dec * 1.00273790935;
	if (Tempo_Siderale_Ora < 0.) Tempo_Siderale_Ora = Tempo_Siderale_Ora + 24.;
	if (Tempo_Siderale_Ora >= 24.) Tempo_Siderale_Ora = Tempo_Siderale_Ora - 24.;
	Tempo_Siderale_Loc = Tempo_Siderale_Ora;
	if (Tempo_Siderale_Loc < 0.) Tempo_Siderale_Loc = Tempo_Siderale_Loc + 24.;
	if (Tempo_Siderale_Loc >= 24.) Tempo_Siderale_Loc = Tempo_Siderale_Loc - 24.;
	return Tempo_Siderale_Loc;
}   /* Calcolo_Tempo_Siderale */







void now_lst(double mjd,double lng, double *lstp)
//now_lst (np, lstp)
//Now *np;
//double *lstp;
{
	static double last_mjd = -23243, last_lng = 121212, last_lst;
	double eps, lst, deps, dpsi;

	if (last_mjd == mjd && last_lng == lng) {
	    *lstp = last_lst;
	    return;
	}

	//utc_gst (mjd_day(mjd), mjd_hr(mjd), &lst);
	lst=Tempo_Siderale(mjd_day(mjd)+MJD0,mjd_hr(mjd));
	lst += radhr(lng);

	obliquity(mjd, &eps);
	nutation(mjd, &deps, &dpsi);
	lst += radhr(dpsi*cos(eps+deps));

	range (&lst, 24.0);

	last_mjd = mjd;
	last_lng = lng;
	*lstp = last_lst = lst;
}
//////////////////
double mm_mjed (double mjd)
{
	return (mjd + deltat(mjd)/86400.0);
}


int Conv_GST_UT(double jd,double gst,double *tu)
/* GST deve essere il tempo siderale medio di greenwich;*/
{
	double t,t0;
	t=(jd-A2000)/36525.;
	t0=fmod(6.697374558 + 2400.051336 * t + 0.000025862 * t * t,24.);
	if(t0<0.)t0+=24.;
	*tu=fmod((gst-t0),24.);
	if(*tu<0.)*tu+=24.;
	(*tu)=(*tu)*0.9972695663;
	return 0;
}

int Sun_Pos(double jd,double *ra,double *decs,double *lon,double *r)
{
	double TW=2.*PI,t,m,tl,tl0,m1;
	t=jd-A2000;
	tl=4.8949504+0.017202792*t; /*Long media sole*/
	while(tl<0.)tl+=TW;
	tl0= 6.240040768+.01720197*t; /*Anomalia media sole*/
	while(tl0<0.)tl0+=TW;
	m=tl+0.033423055*sin(tl0)+0.000349065*sin(2.*tl0); /*Oss.Long sole*/
	m1=fmod(m,TW);
	*r=1.00014-0.01671*cos(tl0)-0.00014*cos(2.*tl0);
	*lon=(m1 -9.93e-5)/Conv_Rad;
	Conv_lb_ad(jd,*lon,0.,ra,decs);
	return 0;
}

int Conv_lb_ad(double jd,double l,double b,double *ra,double *dec)
	/*l,b,in gradi ridotti tra 0-360 per l
   ritorna ra, dec in ore , gradi decimali*/
{
	double e,ras,a,c1;

	e=0.409087723-0.069813e-9*(jd-A2000);/*obliquita eclittica*/
	a=cos(e)*sin(l*Conv_Rad)-tan(b*Conv_Rad)*sin(e);
	c1=cos(l*Conv_Rad);
	ras=atan(a/c1);
	if(l>90.)ras+=PI;
	if(l>270.)ras+=PI;
	*ra=(ras/Conv_Ora_Rad);
	a=sin(e)*sin(l*Conv_Rad)*cos(b*Conv_Rad);
	c1=sin(b*Conv_Rad)*cos(e) ;
	*dec=asin( a + c1 )/Conv_Rad;
	return 0;
}

void riset (double ra, double dec, double lat, double dis, double *lstr,double * lsts,
	   double *azr, double *azs, int *status)
{
#define	EPS	(1e-9)	/* math rounding fudge - always the way, eh? */
	double h;		/* hour angle */
	double cos_h;		/* cos h */
	double z;		/* zenith angle */
	double zmin, zmax;	/* Minimum and maximum zenith angles */
	double xaz, yaz;	/* components of az */
	int shemi;		/* flag for southern hemisphere reflection */

	/* reflect lat and dec if in southern hemisphere, then az back later */
	if ((shemi= (lat < 0.)) != 0) {
	    lat = -lat;
	    dec = -dec;
	}

	/* establish zenith angle, and its extrema */
	z = (PI/2.) + dis;
	zmin = fabs (dec - lat);
	zmax = PI - fabs(dec + lat);

	/* first consider special cases.
	 * these also avoid any boundary problems in subsequent computations.
	 */
	if (zmax <= z + EPS) {
	    *status = -1;	/* never sets */
	    return;
	}
	if (zmin >= z - EPS) {
	    *status = 1;	/* never rises */
	    return;
	}

	/* compute rising hour angle -- beware found off */
	cos_h = (cos(z)-sin(lat)*sin(dec))/(cos(lat)*cos(dec));
	if (cos_h >= 1.)
	    h =  0.;
	else if (cos_h <= -1.)
	    h = PI;
	else
	    h = acos (cos_h);

	/* compute setting azimuth -- beware found off */
	xaz = sin(dec)*cos(lat)-cos(dec)*cos(h)*sin(lat);
	yaz = -1.*cos(dec)*sin(h);
	if (xaz == 0.) {
	    if (yaz > 0)
		*azs = PI/2;
	    else
		*azs = -PI/2;
	} else
	    *azs = atan2 (yaz, xaz);

	/* reflect az back if southern */
	if (shemi)
	    *azs = PI - *azs;
	range(azs, 2.*PI);

	/* rising is just the opposite side */
	*azr = 2.*PI - *azs;
	range(azr, 2.*PI);

	/* rise and set are just ha either side of ra */
	*lstr = radhr(ra-h);
	range(lstr,24.0);
	*lsts = radhr(ra+h);
	range(lsts,24.0);

	/* OK */
	*status = 0;
}

void range (double *v, double r)
{
	*v -= r*floor(*v/r);
}


#define VSOP_ASCALE	1e8	/* amplitude factor as stored */

/* coding flags */
#define VSOP_SPHERICAL	1	/* version in data.c uses spherical coords */
#define VSOP_GETRATE	0	/* calculate time derivatives of coordinates */

#define VSOP_A1000	365250.0	/* days per millenium */
#define VSOP_MAXALPHA	5		/* max degree of time */


/******************************************************************
 * adapted from BdL FORTRAN Code; stern
 *
 *    Reference : Bureau des Longitudes - PBGF9502
 *
 *    Object :  calculate a VSOP87 position for a given time.
 *
 *    Input :
 *
 *    mjd      modified julian date, counted from J1900.0
 *             time scale : dynamical time TDB.
 *
 *    obj	object number as in astro.h, NB: not for pluto
 *
 *    prec     relative precision
 *
 *             if prec is equal to 0 then the precision is the precision
 *                p0 of the complete solution VSOP87.
 *                Mercury    p0 =  0.6 10**-8
 *                Venus      p0 =  2.5 10**-8
 *                Earth      p0 =  2.5 10**-8
 *                Mars       p0 = 10.0 10**-8
 *                Jupiter    p0 = 35.0 10**-8
 *                Saturn     p0 = 70.0 10**-8
 *                Uranus     p0 =  8.0 10**-8
 *                Neptune    p0 = 42.0 10**-8
 *
 *             if prec is not equal to 0, let us say in between p0 and
 *             10**-3, the precision is :
 *                for the positions :
 *                - prec*a0 au for the distances.
 *                - prec rad for the other variables.
 *                for the velocities :
 *                - prec*a0 au/day for the distances.
 *                - prec rad/day for the other variables.
 *                  a0 is the semi-major axis of the body.
 *
 *    Output :
 *
 *    ret[6]     array of the results (double).
 *
 *             for spherical coordinates :
 *                 1: longitude (rd)
 *                 2: latitude (rd)
 *                 3: radius (au)
 *		#if VSOP_GETRATE:
 *                 4: longitude velocity (rad/day)
 *                 5: latitude velocity (rad/day)
 *                 6: radius velocity (au/day)
 *
 *    return:     error index (int)
 *                 0: no error.
 *		   2: object out of range [MERCURY .. NEPTUNE, SUN]
 *		   3: precision out of range [0.0 .. 1e-3]
 ******************************************************************/
int vsop87 (double mjd, int obj, double prec, double *ret)
{
    static double (*vx_map[])[3] = {		/* data tables */
		vx_mercury, vx_venus, vx_mars, vx_jupiter,
		vx_saturn, vx_uranus, vx_neptune, 0, vx_earth,
	};
    static int (*vn_map[])[3] = {		/* indexes */
		vn_mercury, vn_venus, vn_mars, vn_jupiter,
		vn_saturn, vn_uranus, vn_neptune, 0, vn_earth,
	};
    static double a0[] = {	/* semimajor axes; for precision ctrl only */
	    0.39, 0.72, 1.5, 5.2, 9.6, 19.2, 30.1, 39.5, 1.0,
	};
    double (*vx_obj)[3] = vx_map[obj];		/* VSOP87 data and indexes */
    int (*vn_obj)[3] = vn_map[obj];

    double t[VSOP_MAXALPHA+1];			/* powers of time */
    double t_abs[VSOP_MAXALPHA+1];		/* powers of abs(time) */
    double q;					/* aux for precision control */
    int i, cooidx, alpha;			/* misc indexes */

    if (obj == PLUTO || obj > SUN)
	return (2);

    if (prec < 0.0 || prec > 1e-3)
	return(3);

    /* zero result array */
    for (i = 0; i < 6; ++i) ret[i] = 0.0;

    /* time and its powers */
    t[0] = 1.0;
    t[1] = (mjd - J2000)/VSOP_A1000;
    for (i = 2; i <= VSOP_MAXALPHA; ++i) t[i] = t[i-1] * t[1];
    t_abs[0] = 1.0;
    for (i = 1; i <= VSOP_MAXALPHA; ++i) t_abs[i] = fabs(t[i]);

    /* precision control */
    q = -log10(prec + 1e-35) - 2;	/* decades below 1e-2 */
    q = VSOP_ASCALE * prec / 10.0 / q;	/* reduce threshold progressively
					 * for higher precision */

    /* do the term summation; first the spatial dimensions */
    for (cooidx = 0; cooidx < 3; ++cooidx) {

	/* then the powers of time */
	for (alpha = 0; vn_obj[alpha+1][cooidx] ; ++alpha) {
	    double p, term, termdot;

	    /* precision threshold */
	    p = q/(t_abs[alpha] + alpha * t_abs[alpha-1] * 1e-4 + 1e-35);
#if VSOP_SPHERICAL
	    if (cooidx == 2)	/* scale by semimajor axis for radius */
#endif
		p *= a0[obj];

	    term = termdot = 0.0;
	    for (i = vn_obj[alpha][cooidx]; i < vn_obj[alpha+1][cooidx]; ++i) {
		double a, b, c, arg;

		a = vx_obj[i][0];
		if (a < p) continue;	/* ignore small terms */

		b = vx_obj[i][1];
		c = vx_obj[i][2];

		arg = b + c * t[1];
		term += a * cos(arg);
#if VSOP_GETRATE
		termdot += -c * a * sin(arg);
#endif
	    }

	    ret[cooidx] += t[alpha] * term;
#if VSOP_GETRATE
	    ret[cooidx + 3] += t[alpha] * termdot +
		    ((alpha > 0) ? alpha * t[alpha - 1] * term : 0.0);
#endif
	} /* alpha */
    } /* cooidx */

    for (i = 0; i < 6; ++i) ret[i] /= VSOP_ASCALE;

#if VSOP_SPHERICAL
    /* reduce longitude to 0..2pi */
    ret[0] -= floor(ret[0]/(2.*PI)) * (2.*PI);
#endif

#if VSOP_GETRATE
    /* convert millenium rate to day rate */
    for (i = 3; i < 6; ++i) ret[i] /= VSOP_A1000;
#endif

#if VSOP_SPHERICAL
    /* reduction from dynamical equinox of VSOP87 to FK5;
     */
    if (prec < 5e-7) {		/* 5e-7 rad = 0.1 arc seconds */
	double L1, c1, s1;
	L1 = ret[0] - degrad(13.97 * t[1] - 0.031 * t[2]);
	c1 = cos(L1); s1 = sin(L1);
	ret[0] += degrad(-0.09033 + 0.03916 * (c1 + s1) * tan(ret[1]))/3600.0;
	ret[1] += degrad(0.03916 * (c1 - s1))/3600.0;
    }
#endif

    return (0);
}

void sunpos (double mjd, double *lsn, double *rsn, double *bsn)
{
	static double last_mjd = -3691, last_lsn, last_rsn, last_bsn;
	double ret[6];

	if (mjd == last_mjd) {
	    *lsn = last_lsn;
	    *rsn = last_rsn;
	    if (bsn) *bsn = last_bsn;
	    return;
	}

	vsop87(mjd, SUN, 0.0, ret);	/* full precision earth pos */

	*lsn = ret[0] - PI;		/* revert to sun pos */
	range (lsn, 2*PI);		/* normalise */

	last_lsn = *lsn;		/* memorise */
	last_rsn = *rsn = ret[2];
	last_bsn = -ret[1];
	last_mjd = mjd;

	if (bsn) *bsn = last_bsn;	/* assign only if non-NULL pointer */

}



#define	DCOS(x)		cos(degrad(x))
#define	DSIN(x)		sin(degrad(x))
#define	DASIN(x)	raddeg(asin(x))
#define	DATAN2(y,x)	raddeg(atan2((y),(x)))

static void precess_hiprec (double mjd1, double mjd2, double *ra,
    double *dec);

/* corrects ra and dec, both in radians, for precession from epoch 1 to epoch 2.
 * the epochs are given by their modified JDs, mjd1 and mjd2, respectively.
 * N.B. ra and dec are modifed IN PLACE.
 */
void precess (double mjd1, double mjd2, double *ra, double *dec)
/*mjd1, mjd2;	initial and final epoch modified JDs 
double *ra, *dec;	/ ra/dec for mjd1 in, for mjd2 out */
{
	precess_hiprec (mjd1, mjd2, ra, dec);
}

/*
 * Copyright (c) 1990 by Craig Counterman. All rights reserved.
 *
 * This software may be redistributed freely, not sold.
 * This copyright notice and disclaimer of warranty must remain
 *    unchanged. 
 *
 * No representation is made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty, to the extent permitted by applicable law.
 *
 * Rigorous precession. From Astronomical Ephemeris 1989, p. B18
 *
 * 96-06-20 Hayo Hase <hase@wettzell.ifag.de>: theta_a corrected
 */
static void precess_hiprec (double mjd1, double mjd2, double *ra, double *dec)
/*double mjd1, mjd2;	 initial and final epoch modified JDs 
ra, *dec;	ra/dec for mjd1 in, for mjd2 out */
{
	static double last_mjd1 = -213.432, last_from;
	static double last_mjd2 = -213.432, last_to;
	double zeta_A, z_A, theta_A;
	double T;
	double A, B, C;
	double alpha, delta;
	double alpha_in, delta_in;
	double from_equinox, to_equinox;
	double alpha2000, delta2000;

	/* convert mjds to years;
	 * avoid the remarkably expensive calls to mjd_year()
	 */
	if (last_mjd1 == mjd1)
	    from_equinox = last_from;
	else {
	    mjd_year (mjd1, &from_equinox);
	    last_mjd1 = mjd1;
	    last_from = from_equinox;
	}
	if (last_mjd2 == mjd2)
	    to_equinox = last_to;
	else {
	    mjd_year (mjd2, &to_equinox);
	    last_mjd2 = mjd2;
	    last_to = to_equinox;
	}

	/* convert coords in rads to degs */
	alpha_in = raddeg(*ra);
	delta_in = raddeg(*dec);

	/* precession progresses about 1 arc second in .047 years */
	/* From from_equinox to 2000.0 */
	if (fabs (from_equinox-2000.0) > .02) {
	    T = (from_equinox - 2000.0)/100.0;
	    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
	    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
	    theta_A = 0.5567530* T - 0.0001185* T*T - 0.0000116* T*T*T;

	    A = DSIN(alpha_in - z_A) * DCOS(delta_in);
	    B = DCOS(alpha_in - z_A) * DCOS(theta_A) * DCOS(delta_in)
	      + DSIN(theta_A) * DSIN(delta_in);
	    C = -DCOS(alpha_in - z_A) * DSIN(theta_A) * DCOS(delta_in)
	      + DCOS(theta_A) * DSIN(delta_in);

	    alpha2000 = DATAN2(A,B) - zeta_A;
	    range (&alpha2000, 360.0);
	    delta2000 = DASIN(C);
	} else {
	    /* should get the same answer, but this could improve accruacy */
	    alpha2000 = alpha_in;
	    delta2000 = delta_in;
	};


	/* From 2000.0 to to_equinox */
	if (fabs (to_equinox - 2000.0) > .02) {
	    T = (to_equinox - 2000.0)/100.0;
	    zeta_A  = 0.6406161* T + 0.0000839* T*T + 0.0000050* T*T*T;
	    z_A     = 0.6406161* T + 0.0003041* T*T + 0.0000051* T*T*T;
	    theta_A = 0.5567530* T - 0.0001185* T*T - 0.0000116* T*T*T;

	    A = DSIN(alpha2000 + zeta_A) * DCOS(delta2000);
	    B = DCOS(alpha2000 + zeta_A) * DCOS(theta_A) * DCOS(delta2000)
	      - DSIN(theta_A) * DSIN(delta2000);
	    C = DCOS(alpha2000 + zeta_A) * DSIN(theta_A) * DCOS(delta2000)
	      + DCOS(theta_A) * DSIN(delta2000);

	    alpha = DATAN2(A,B) + z_A;
	    range(&alpha, 360.0);
	    delta = DASIN(C);
	} else {
	    /* should get the same answer, but this could improve accruacy */
	    alpha = alpha2000;
	    delta = delta2000;
	};

	*ra = degrad(alpha);
	*dec = degrad(delta);
}

void cal_mjd (int mn, double dy, int yr, double *mjd)
{
	static double last_mjd, last_dy;
	static int last_mn, last_yr;
	int b, d, m, y;
	long c;

	if (mn == last_mn && yr == last_yr && dy == last_dy) {
	    *mjd = last_mjd;
	    return;
	}

	m = mn;
	y = (yr < 0) ? yr + 1 : yr;
	if (mn < 3) {
	    m += 12;
	    y -= 1;
	}

	if (yr < 1582 || (yr == 1582 && (mn < 10 || (mn == 10 && dy < 15))))
	    b = 0;
	else {
	    int a;
	    a = y/100;
	    b = 2 - a + a/4;
	}

	if (y < 0)
	    c = (long)((365.25*y) - 0.75) - 694025L;
	else
	    c = (long)(365.25*y) - 694025L;

	d = (int)30.6001*(m+1);

	*mjd = b + c + d + dy - 0.5;

	last_mn = mn;
	last_dy = dy;
	last_yr = yr;
	last_mjd = *mjd;
}


/* given the modified Julian date (number of days elapsed since 1900 jan 0.5,),
 * mjd, return the calendar date in months, *mn, days, *dy, and years, *yr.
 */

void mjd_cal (double mjd, int *mn, double *dy, int *yr)
{
	static double last_mjd, last_dy;
	static int last_mn, last_yr;
	double d, f;
	double i, a, b, ce, g;

	/* we get called with 0 quite a bit from unused epoch fields.
	 * 0 is noon the last day of 1899.
	 */
	if (mjd == 0.0) {
	    *mn = 12;
	    *dy = 31.5;
	    *yr = 1899;
	    return;
	}

	if (mjd == last_mjd) {
	    *mn = last_mn;
	    *yr = last_yr;
	    *dy = last_dy;
	    return;
	}

	d = mjd + 0.5;
	i = floor(d);
	f = d-i;
	if (f == 1) {
	    f = 0;
	    i += 1;
	}

	if (i > -115860.0) {
	    a = floor((i/36524.25)+.99835726)+14;
	    i += 1 + a - floor(a/4.0);
	}

	b = floor((i/365.25)+.802601);
	ce = i - floor((365.25*b)+.750001)+416;
	g = floor(ce/30.6001);
	*mn = (int)(g - 1);
	*dy = ce - floor(30.6001*g)+f;
	*yr = (int)(b + 1899);

	if (g > 13.5)
	    *mn = (int)(g - 13);
	if (*mn < 2.5)
	    *yr = (int)(b + 1900);
	if (*yr < 1)
	    *yr -= 1;

	last_mn = *mn;
	last_dy = *dy;
	last_yr = *yr;
	last_mjd = mjd;
}

/* given an mjd, set *dow to 0..6 according to which day of the week it falls
 * on (0=sunday).
 * return 0 if ok else -1 if can't figure it out.
 */

int mjd_dow (double mjd,int *dow)

{
	/* cal_mjd() uses Gregorian dates on or after Oct 15, 1582.
	 * (Pope Gregory XIII dropped 10 days, Oct 5..14, and improved the leap-
	 * year algorithm). however, Great Britian and the colonies did not
	 * adopt it until Sept 14, 1752 (they dropped 11 days, Sept 3-13,
	 * due to additional accumulated error). leap years before 1752 thus
	 * can not easily be accounted for from the cal_mjd() number...
	 */
	if (mjd < -53798.5) {
	    /* pre sept 14, 1752 too hard to correct |:-S */
	    return (-1);
	}
	*dow = ((long)floor(mjd-.5) + 1) % 7;/* 1/1/1900 (mjd 0.5) is a Monday*/
	if (*dow < 0)
	    *dow += 7;
	return (0);
}

/* given a mjd, return the the number of days in the month.  */

void mjd_dpm (double mjd, int *ndays)
{
	static short dpm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int m, y;
	double d;

	mjd_cal (mjd, &m, &d, &y);
	*ndays = (m==2 && ((y%4==0 && y%100!=0)||y%400==0)) ? 29 : dpm[m-1];
}

/* given a mjd, return the year as a double. */

void mjd_year (double mjd, double *yr)
{
	static double last_mjd, last_yr;
	int m, y;
	double d;
	double e0, e1;	/* mjd of start of this year, start of next year */

	if (mjd == last_mjd) {
	    *yr = last_yr;
	    return;
	}

	mjd_cal (mjd, &m, &d, &y);
	if (y == -1) y = -2;
	cal_mjd (1, 1.0, y, &e0);
	cal_mjd (1, 1.0, y+1, &e1);
	*yr = y + (mjd - e0)/(e1 - e0);

	last_mjd = mjd;
	last_yr = *yr;
}

/* given a decimal year, return mjd */

void year_mjd (double y, double *mjd)
{
	double e0, e1;	/* mjd of start of this year, start of next year */
	int yf =(int) floor (y);
	if (yf == -1) yf = -2;

	cal_mjd (1, 1.0, yf, &e0);
	cal_mjd (1, 1.0, yf+1, &e1);
	*mjd = e0 + (y - yf)*(e1-e0);
}

/* round a time in days, *t, to the nearest second, IN PLACE. */

void rnd_second (double *t)
{
	*t = floor(*t*SPD+0.5)/SPD;
}
	
/* given an mjd, truncate it to the beginning of the whole day */

double mjd_day(double jd)

{
	return (floor(jd-0.5)+0.5);
}

/* given an mjd, return the number of hours past midnight of the whole day */
double mjd_hr(double jd)
{
	return ((jd-mjd_day(jd))*24.0);
}

void sphcart (double l, double b, double r, double *x, double *y, double *z)
{
	double rcb = r * cos(b);

	*x = rcb * cos(l);
	*y = rcb * sin(l);
	*z = r * sin(b);
}

/* transformation from cartesian to spherical coordinates */
void cartsph (double x,double  y,double  z,double *l, double *b, double *r)
/*double x, y, z;	source: rectangular coordinates 
double *l, *b, *r;	result: spherical coordinates */
{
	double rho = x*x + y*y;

	if (rho > 1e-35) {	/* standard case: off axis */
	    *l = atan2(y, x);
	    range (l, 2*PI);
	    *b = atan2(z, sqrt(rho));
	    *r = sqrt(rho + z*z);
	} else {		/* degenerate case; avoid math error */
	    *l = 0.0;
	    if (z == 0.0)
		*b = 0.0;
	    else
		*b = (z > 0.0) ? PI/2. : -PI/2.;
	    *r = fabs(z);
	}
}

/* convert those orbital elements that change from epoch mjd0 to epoch mjd.
 */
void reduce_elements (double mjd0, double mjd, double inc0, double ap0, double om0, double *inc, double *ap, double *om)
/*double mjd0;	initial epoch
double mjd;	desired epoch 
double inc0;	 initial inclination, rads 
double ap0;	initial argument of perihelion, as an mjd 
double om0;	 initial long of ascending node, rads 
double *inc;	desired inclination, rads 
double *ap;	desired epoch of perihelion, as an mjd 
double *om;	desired long of ascending node, rads */
{
	double t0, t1;
	double tt, tt2, t02, tt3;
	double eta, th, th0;
	double a, b;
	double dap;
	double cinc, sinc;
	double ot, sot, cot, ot1;
	double seta, ceta;

	if (fabs(mjd - mjd0) < 1e-5) {
	    /* sin(eta) blows for inc < 10 degrees -- anyway, no need */
	    *inc = inc0;
	    *ap = ap0;
	    *om = om0;
	    return;
	}

	t0 = mjd0/365250.0;
	t1 = mjd/365250.0;

	tt = t1-t0;
	tt2 = tt*tt;
        t02 = t0*t0;
	tt3 = tt*tt2;
        eta = (471.07-6.75*t0+.57*t02)*tt+(.57*t0-3.37)*tt2+.05*tt3;
        th0 = 32869.0*t0+56*t02-(8694+55*t0)*tt+3*tt2;
        eta = degrad(eta/3600.0);
        th0 = degrad((th0/3600.0)+173.950833);
        th = (50256.41+222.29*t0+.26*t02)*tt+(111.15+.26*t0)*tt2+.1*tt3;
        th = th0+degrad(th/3600.0);
	cinc = cos(inc0);
        sinc = sin(inc0);
	ot = om0-th0;
	sot = sin(ot);
        cot = cos(ot);
	seta = sin(eta);
        ceta = cos(eta);
	a = sinc*sot;
        b = ceta*sinc*cot-seta*cinc;
	ot1 = atan(a/b);
        if (b<0) ot1 += PI;
        b = sinc*ceta-cinc*seta*cot;
        a = -1*seta*sot;
	dap = atan(a/b);
        if (b<0) dap += PI;

        *ap = ap0+dap;
	range (ap, 2*PI);
        *om = ot1+th;
	range (om, 2*PI);

        if (inc0<.175)
	    *inc = asin(a/sin(dap));
	else
	    *inc = 1.570796327-asin((cinc*ceta)+(sinc*seta*cot));
}

/* given a modified Julian date, mjd, and a set of heliocentric parabolic
 * orbital elements referred to the epoch of date (mjd):
 *   ep:   epoch of perihelion,
 *   inc:  inclination,
 *   ap:   argument of perihelion (equals the longitude of perihelion minus the
 *	   longitude of ascending node)
 *   qp:   perihelion distance,
 *   om:   longitude of ascending node;
 * find:
 *   lpd:  heliocentric longitude, 
 *   psi:  heliocentric latitude,
 *   rp:   distance from the sun to the planet, 
 *   rho:  distance from the Earth to the planet,
 *   lam:  geocentric ecliptic longitude, 
 *   bet:  geocentric ecliptic latitude,
 *         none are corrected for light time, ie, they are the true values for
 *	   the given instant.
 *
 * all angles are in radians, all distances in AU.
 * mutual perturbation corrections with other solar system objects are not
 * applied. corrections for nutation and abberation must be made by the caller.
 * The RA and DEC calculated from the fully-corrected ecliptic coordinates are
 * then the apparent geocentric coordinates. Further corrections can be made,
 * if required, for atmospheric refraction and geocentric parallax.
 */
void comet (double mjd, double ep, double inc, double ap, double qp, double om, double *lpd, double *psi, double *rp, double *rho, double *lam, double *bet)
{
	double w, s, s2;
	double l, sl, cl, y;
	double spsi, cpsi;
	double rd, lsn, rsn;
	double lg, re, ll;
	double cll, sll;
	double nu;

#define	ERRLMT	0.0001
        w = ((mjd-ep)*3.649116e-02)/(qp*sqrt(qp));
        s = w/3;
	for (;;) {
	    double d;
	    s2 = s*s;
	    d = (s2+3)*s-w;
	    if (fabs(d) <= ERRLMT)
		break;
	    s = ((2*s*s2)+w)/(3*(s2+1));
	}

        nu = 2*atan(s);
	*rp = qp*(1+s2);
	l = nu+ap;
        sl = sin(l);
	cl = cos(l);
	spsi = sl*sin(inc);
        *psi = asin(spsi);
	y = sl*cos(inc);
        *lpd = atan(y/cl)+om;
	cpsi = cos(*psi);
        if (cl<0) *lpd += PI;
	range (lpd, 2*PI);
        rd = *rp * cpsi;
	sunpos (mjd, &lsn, &rsn, 0);
	lg = lsn+PI;
        re = rsn;
	ll = *lpd - lg;
        cll = cos(ll);
	sll = sin(ll);
        *rho = sqrt((re * re)+(*rp * *rp)-(2*re*rd*cll));
        if (rd<re) 
            *lam = atan((-1*rd*sll)/(re-(rd*cll)))+lg+PI;
	else
	    *lam = atan((re*sll)/(rd-(re*cll)))+*lpd;
	range (lam, 2*PI);
        *bet = atan((rd*spsi*sin(*lam-*lpd))/(cpsi*re*sll));
}

void obliquity (double mjd, double *eps)
{
	static double lastmjd = -16347, lasteps;

	if (mjd != lastmjd) {
	    double t = (mjd - J2000)/36525.;	/* centuries from J2000 */
	    lasteps = degrad(23.4392911 +	/* 23^ 26' 21".448 */
			    t * (-46.8150 +
			    t * ( -0.00059 +
			    t * (  0.001813 )))/3600.0);
	    lastmjd = mjd;
	}
	*eps = lasteps;
}
#if 0 // disable planet stuff
static void pluto_ell (double mjd, double *ret);
static void chap_trans (double mjd, double *ret);
static void planpos (double mjd, int obj, double prec, double *ret);


static void chap_trans (double mjd, double *ret)
/*double mjd;	 destination epoch 
double *ret;	vector to be transformed _IN PLACE_ */
{
	double ra, dec, r, eps;
	double sr, cr, sd, cd, se, ce;

	cartsph(ret[0], ret[1], ret[2], &ra, &dec, &r);
	precess(J2000, mjd, &ra, &dec);
	obliquity(mjd, &eps);
	sr = sin(ra); cr = cos(ra);
	sd = sin(dec); cd = cos(dec);
	se = sin(eps); ce = cos(eps);
	ret[0] = atan2( sr * ce + sd/cd * se, cr);	/* long */
	ret[1] = asin( sd * ce - cd * se * sr);		/* lat */
	ret[2] = r;					/* radius */
}

/* low precision ecliptic coordinates of Pluto from mean orbit.
 * Only for sake of completeness outside available perturbation theories.
 */
static void pluto_ell (double mjd, double *ret)
/*double mjd epoch 
double *ret;	ecliptic coordinates {l,b,r} at equinox of date */
{
	/* mean orbital elements of Pluto.
	 * The origin of these is somewhat obscure.
	 */
	double	a = 39.543,			/* semimajor axis, au */
		e = 0.2490,			/* excentricity */
		inc0 = 17.140,			/* inclination, deg */
		Om0 = 110.307,			/* long asc node, deg */
		omeg0 = 113.768,		/* arg of perihel, deg */
		mjdp = 2448045.539 - MJD0,	/* epoch of perihel */
		mjdeq = J2000,			/* equinox of elements */
		n = 144.9600/36525.;            /* daily motion, deg */

	double inc, Om, omeg;	/* orbital elements at epoch of date */
	double ma, ea, nu;	/* mean, excentric and true anomaly */
	double lo, slo, clo;	/* longitude in orbit from asc node */

	reduce_elements(mjdeq, mjd, degrad(inc0), degrad(omeg0), degrad(Om0),
				&inc, &omeg, &Om);
	ma = degrad((mjd - mjdp) * n);
	anomaly(ma, e, &nu, &ea);
	ret[2] = a * (1.0 - e*cos(ea));			/* r */
	lo = omeg + nu;
	slo = sin(lo);
	clo = cos(lo);
	ret[1] = asin(slo * sin(inc));			/* b */
	ret[0] = atan2(slo * cos(inc), clo) + Om;	/* l */
}

/*************************************************************/

/* geometric heliocentric position of planet, mean ecliptic of date
 * (not corrected for light-time)
 */
static void planpos (double mjd, int obj, double prec, double *ret)
{
#if 0
	if (mjd >= CHAP_BEGIN && mjd <= CHAP_END) {
	    if (obj >= JUPITER) {		/* prefer Chapront */
		chap95(mjd, obj, prec, ret);
		chap_trans (mjd, ret);
	    } else {				/* VSOP for inner planets */
		vsop87(mjd, obj, prec, ret);
	    }
	} else {				/* outside Chapront time: */
	    if (obj != PLUTO) {			/* VSOP for all but Pluto */
		vsop87(mjd, obj, prec, ret);
	    } else {				/* Pluto mean elliptic orbit */
		pluto_ell(mjd, ret);
	    }
	}
#else
		vsop87(mjd, obj, prec, ret);
#endif
}
/*************************************************************/

/* visual elements of planets
 * [planet][0] = angular size at 1 AU
 * [planet][1] = magnitude at 1 AU from sun and earth and 0 deg phase angle
 */
static double vis_elements[8][2] = {
	/* Mercury */	{ 6.74, -0.42, },
	/* Venus */	{ 16.92, -4.34, },
	/* Mars */	{ 9.36, -1.20, },
	/* Jupiter */	{ 196.74, -9.4, },
	/* Saturn */	{ 165.6, -8.88, },
	/* Uranus */	{ 65.8, -7.19, },
	/* Neptune */	{ 62.2, -6.87, },
	/* Pluto */	{ 8.2, -1.0, }
};

/* given a modified Julian date, mjd, and a planet, p, find:
 *   lpd0: heliocentric longitude, 
 *   psi0: heliocentric latitude,
 *   rp0:  distance from the sun to the planet, 
 *   rho0: distance from the Earth to the planet,
 *         none corrected for light time, ie, they are the true values for the
 *         given instant.
 *   lam:  geocentric ecliptic longitude, 
 *   bet:  geocentric ecliptic latitude,
 *         each corrected for light time, ie, they are the apparent values as
 *	   seen from the center of the Earth for the given instant.
 *   dia:  angular diameter in arcsec at 1 AU, 
 *   mag:  visual magnitude when 1 AU from sun and earth at 0 phase angle.
 *
 * all angles are in radians, all distances in AU.
 *
 * corrections for nutation and abberation must be made by the caller. The RA 
 *   and DEC calculated from the fully-corrected ecliptic coordinates are then
 *   the apparent geocentric coordinates. Further corrections can be made, if
 *   required, for atmospheric refraction and geocentric parallax.
 */
void plans (double mjd, int p, double *lpd0, double *psi0, double *rp0, double *rho0, double *lam, double *bet, double *dia, double *mag)
{
	static double lastmjd = -10000;
	static double lsn, bsn, rsn;	/* geometric geocentric coords of sun */
	static double xsn, ysn, zsn;
	double lp, bp, rp;		/* heliocentric coords of planet */
	double xp, yp, zp, rho;		/* rect. coords and geocentric dist. */
	double dt;			/* light time */
	int pass;

	/* get sun cartesian; needed only once at mjd */
	if (mjd != lastmjd) {
	    sunpos (mjd, &lsn, &rsn, &bsn);
	    sphcart (lsn, bsn, rsn, &xsn, &ysn, &zsn);
            lastmjd = mjd;
        }

	/* first find the true position of the planet at mjd.
	 * then repeat a second time for a slightly different time based
	 * on the position found in the first pass to account for light-travel
	 * time.
	 */
	dt = 0.0;
	for (pass = 0; pass < 2; pass++) {
	    double ret[6];

	    /* get spherical coordinates of planet from precision routines,
	     * retarded for light time in second pass;
	     * alternative option:  vsop allows calculating rates.
	     */
	    planpos(mjd - dt, p, 0.0, ret);

	    lp = ret[0];
	    bp = ret[1];
	    rp = ret[2];

	    sphcart (lp, bp, rp, &xp, &yp, &zp);
	    cartsph (xp + xsn, yp + ysn, zp + zsn, lam, bet, &rho);

	    if (pass == 0) {
		/* save heliocentric coordinates at first pass since, being
		 * true, they are NOT to be corrected for light-travel time.
		 */
		*lpd0 = lp;
		range (lpd0, 2.*PI);
		*psi0 = bp;
		*rp0 = rp;
		*rho0 = rho;
	    }

	    /* when we view a planet we see it in the position it occupied
	     * dt days ago, where rho is the distance between it and earth,
	     * in AU. use this as the new time for the next pass.
	     */
	    dt = rho * 5.7755183e-3;
	}

	*dia = vis_elements[p][0];
	*mag = vis_elements[p][1];
}

#endif // planets

#define TWOPI   	(2*PI)
#define	STOPERR		(1e-8)

/* given the mean anomaly, ma, and the eccentricity, s, of elliptical motion,
 * find the true anomaly, *nu, and the eccentric anomaly, *ea.
 * all angles in radians.
 */
void anomaly (double ma, double s, double *nu, double *ea)
{
        double m, fea, corr;

        if (s < 1.0) {
            /* elliptical */
            double dla;

            m = ma-TWOPI*(long)(ma/TWOPI);
            if (m > PI) m -= TWOPI;
            if (m < -PI) m += TWOPI;
            fea = m;

            for (;;) {
                dla = fea-(s*sin(fea))-m;
                if (fabs(dla)<STOPERR)
                    break;
                /* avoid runnaway corrections for e>.97 and M near 0*/
                corr = 1-(s*cos(fea));
                if (corr < .1) corr = .1;
                dla /= corr;
                fea -= dla;
            }
            *nu = 2*atan(sqrt((1+s)/(1-s))*tan(fea/2));
        } else {
            /* hyperbolic */
	    double fea1;

            m = fabs(ma);
            fea = m / (s-1.);
	    fea1 = pow(6*m/(s*s),1./3.);
            /* whichever is smaller is the better initial guess */
            if (fea1 < fea) fea = fea1;

	    corr = 1;
            while (fabs(corr) > STOPERR) {
		corr = (m - s * sinh(fea) + fea) / (s*cosh(fea) - 1);
		fea += corr;
            }
            if (ma < 0.) fea = -fea;
            *nu = 2*atan(sqrt((s+1)/(s-1))*tanh(fea/2));
        }
        *ea = fea;
}



/* heliocentric rectangular equatorial coordinates of Jupiter to Pluto;
 * from Chapront's expansion of DE200/extension of DE200;  mean equator J2000.0
 *
 * calculation time (milliseconds) on an HP 715/75, Jupiter to Pluto:
 * (each coordinate component counted as 1 term,
 * secular terms included for JD 2448908.5 = 1992 Oct 13.0)
 *
 *      prec	terms	rates	no rates
 *	0.0	2256	5.1	4.6
 *
 *	1e-7	792	2.6	2.4	--> nominal precision rel. to DE200
 *	1e-6	535	2.1	2.0
 *	1e-5	350	1.8	1.6
 *	1e-4	199	1.5	1.4
 *	1e-3	96	1.2	1.1
 *
 *	no drop	2256	4.5	3.9	(code without test criterion)
 */


#define CHAP_MAXTPOW	2	/* NB: valid for all 5 outer planets */
#if 0
/* chap95()
 *
 * input:
 *	mjd	modified JD; days from J1900.0 = 2415020.0
 *
 *	prec	precision level, in radians.
 *		if (prec = 0.0), you get the full precision, namely
 *		a deviation of not more than 0.02 arc seconds (1e-7 rad)
 *		from the JPL DE200 integration, on which this expansion
 *		is based.
 *
 *	obj	object number as in astro.h (jupiter=3, saturn=4, ...)
 *
 * output:
 *	ret[6]	cartesian components of position and velocity
 *
 * return:
 *	0	Ok
 *	1	time out of range [CHAP_BEGIN .. CHAP_END]
 *	2	object out of range [JUPITER .. PLUTO]
 *	3	precision out of range [0.0 .. 1e-3]
 */
int chap95 (double mjd, int obj, double prec, double *ret)
{
	static double a0[] = {		/* semimajor axes for precision ctrl */
	    0.39, 0.72, 1.5, 5.2, 9.6, 19.2, 30.1, 39.5, 1.0
	};
	double sum[CHAP_MAXTPOW+1][6];	/* [T^0, ..][X,Y,Z,X',Y',Z'] */
	double T, t;			/* time in centuries and years */
	double ca, sa, Nu;		/* aux vars for terms */
	double precT[CHAP_MAXTPOW+1];	/* T-augmented precision threshold */
	chap95_rec *rec;		/* term coeffs */
	int cooidx;

	/* check parameters */
	if (mjd < CHAP_BEGIN || mjd > CHAP_END)
		return (1);

	if (obj < JUPITER || obj > PLUTO)
		return (2);

	if (prec < 0.0 || prec > 1e-3)
		return (3);

	/* init the sums */
	zero_mem ((void *)sum, sizeof(sum));

	T = (mjd - J2000)/36525.0;	/* centuries since J2000.0 */

	/* modify precision treshold for
	 * a) term storing scale
	 * b) convert radians to au
	 * c) account for skipped terms (more terms needed for better prec)
	 *    threshold empirically established similar to VSOP; stern
	 * d) augment for secular terms
	 */
	precT[0] = prec * CHAP_SCALE				/* a) */
			* a0[obj]				/* b) */
			/ (10. * (-log10(prec + 1e-35) - 2));	/* c) */
	t = 1./(fabs(T) + 1e-35);				/* d) */
	precT[1] = precT[0]*t;
	precT[2] = precT[1]*t;

	t = T * 100.0;		/* YEARS since J2000.0 */

	ca = sa = Nu = 0.;	/* shut up compiler warning 'uninitialised' */

	switch (obj) {		/* set initial term record pointer */
	    case JUPITER:	rec = chap95_jupiter;	break;
	    case SATURN:	rec = chap95_saturn;	break;
	    case URANUS:	rec = chap95_uranus;	break;
	    case NEPTUNE:	rec = chap95_neptune;	break;
	    case PLUTO:		rec = chap95_pluto;	break;
	    default:
		return (2);	/* wrong object: severe internal trouble */
	}

	/* do the term summation into sum[T^n] slots */
	for (; rec->n >= 0; ++rec) {
	    double *amp;

	    /* NOTE:  The formula
	     * X = SUM[i=1,Records] T**n_i*(CX_i*cos(Nu_k*t)+SX_i*sin(Nu_k*t))
	     * could be rewritten as  SUM( ... A sin (B + C*t) )
	     * "saving" trigonometric calls.  However, e.g. for Pluto,
	     * there are only 65 distinct angles NU_k (130 trig calls).
	     * With that manipulation, EVERY arg_i would be different for X,
	     * Y and Z, which is 3*96 terms.  Hence, the formulation as
	     * given is good (optimal?).
	     */

	    for (cooidx = 0, amp = rec->amp; cooidx < 3; ++cooidx) {
		double C, S, term, termdot;
		short n;		/* fast access */

		C = *amp++;
		S = *amp++;
		n = rec->n;

		/* drop term if too small
		 * this is quite expensive:  17% of loop time
		 */
		if (fabs(C) + fabs(S) < precT[n])
			continue;

		if (n == 0 && cooidx == 0) {	/* new Nu only here */
		    double arg;

		    Nu = rec->Nu;
		    arg = Nu * t;
		    arg -= floor(arg/(2.*PI))*(2.*PI);
		    ca = cos(arg);	/* blast it - even for Nu = 0.0 */
		    sa = sin(arg);
		}

		term = C * ca + S * sa;
		sum[n][cooidx] += term;
#if CHAP_GETRATE
		termdot = (-C * sa + S * ca) * Nu;
		sum[n][cooidx+3] += termdot;
		if (n > 0) sum[n - 1][cooidx+3] += n/100.0 * term;
#endif
	    } /* cooidx */
	} /* records */

	/* apply powers of time and sum up */
	for (cooidx = 0; cooidx < 6; ++cooidx) {
	    ret[cooidx] = (sum[0][cooidx] +
			T * (sum[1][cooidx] +
			T * (sum[2][cooidx] )) )/CHAP_SCALE;
	}

	/* TEST: if the MAIN terms are dropped, get angular residue
	ret[0] = sqrt(ret[0]*ret[0] + ret[1]*ret[1] + ret[2]*ret[2])/a0[obj];
	*/

#if CHAP_GETRATE
	for (cooidx = 3; cooidx < 6; ++cooidx) {
	    ret[cooidx] /= 365.25;	/* yearly to daily rate */
	}
#endif

    return (0);
}
#endif
void zero_mem (void *loc, unsigned len)
{
	(void) memset (loc, 0, len);
}


#define CHAR short

#define NARGS 18
 
struct plantbl {
  char max_harmonic[NARGS];
  char max_power_of_t;
  CHAR *arg_tbl;
  long *lon_tbl;
  long *lat_tbl;
  long *rad_tbl;
  double distance;
  double timescale;
  double trunclvl;
};

static double mods3600 (double x);
static void mean_elements (double JED);
static int sscc (int k, double arg, int n);
static int g2plan (double J, struct plantbl *plan, double *pobj, int flag);
static double g1plan (double J, struct plantbl *plan);
static int gecmoon (double J, struct plantbl *lrtab,
	struct plantbl *lattab, double *pobj);

/* time points */
#define MOSHIER_J2000 (2451545.0)

#define MOSHIER_BEGIN (1221000.5 - MJD0) /* directly from above */
#define MOSHIER_END   (2798525.5 - MJD0) /* 2950.0; from libration table */


static double Args[NARGS];
static double LP_equinox;
static double NF_arcsec;
static double Ea_arcsec;
static double pA_precession;


/* This storage ought to be allocated dynamically.  */
double ss[NARGS][30];
double cc[NARGS][30];

/* Time, in units of 10,000 Julian years from JED 2451545.0.  */
static double T;

/* Conversion factors between degrees and radians */
#define DTR 1.7453292519943295769e-2
#define RTD 5.7295779513082320877e1
#define RTS 2.0626480624709635516e5	/* arc seconds per radian */
#define STR 4.8481368110953599359e-6	/* radians per arc second */
#define AUKM 1.4959787e8


static long lrtabl[] = {
    175667,     66453,      5249,       -42,
     20057,       403,     -2360,      6148,
     -7644,     24646,     -1273,      9127,
     -1395,      1958,
       232,      -289,
       -97,       553,        69,       130,
       -80,         6,
       129,      -868,        26,       -89,
      1042,      1172,       194,      -112,
    -47433,   -241666,    224626,   -103752,
     63419,    127606,
      2294,      -691,     -1827,     -1254,
        -1,      -119,
      1057,       324,
       505,      -195,       254,      -641,
       -36,      1008,     -1082,        -3,
       -87,       122,
       161,        11,
         2,      -106,
        29,      -123,
       -32,        41,
      -524,       -35,
       133,      -595,
       225,       837,      -108,      -191,
     -2294,       841,      -340,      -394,
      -351,     -1039,       238,      -108,
       -66,        21,
      1405,       869,       520,      2776,
      -174,        71,
       425,       652,     -1260,       -80,
       249,        77,
      -192,       -17,
       -97,       134,
        -7,       -54,
      -802,     -7436,     -2824,     70869,
       -35,      2481,
      1865,      1749,     -2166,      2415,
        33,      -183,
      -835,       283,
        27,       -45,
        56,       235,
         2,       718,
     -1206,       275,       -87,      -158,
        -7,     -2534,         0,     10774,
         1,      -324,
      -208,       821,
       281,      1340,      -797,       440,
       224,        72,
       -65,        -5,
        -7,       -44,
       -48,        66,
      -151,       -40,
       -41,       -45,
        76,      -108,
       -18,      1202,         0,     -2501,
      1438,      -595,       900,      3040,
     -3435,        -5,
      -100,       -26,
         0,    -13714,
      -183,        68,
       453,       -83,
      -228,       325,
        97,        13,
         2,       105,
       -61,       257,
         0,        57,
        88,       -11,
        -1,     -8220,
         0,       275,
       -43,       -10,
      -199,       105,
         1,     -5849,         2,     24887,
      -128,        48,
       712,       970,     -1407,       845,
      -266,       378,
       311,      1526,     -1751,        27,
         0,   -185858,
       133,      6383,
      -108,        25,
        -7,      1944,
         5,       390,
       -11,        31,
       277,      -384,       158,        72,
       -81,       -41,       -13,      -111,
     -2332,    -65804,      -698,    505812,
        34,   1676716,       -72,  -6664384,
       154,       -57,        52,        95,
        -4,        -5,
        -7,        37,
       -63,       -32,
         4,      3349,         1,    -14370,
        16,       -83,
         0,      -401,
        13,      3013,
        48,       -20,
         0,       250,
        51,       -79,
        -7,      -146,
       148,         9,
         0,       -64,
       -17,       -59,
       -67,      -492,
        -2,   2116601,
       -12,     -1848,
         8,      -436,
        -6,       324,         0,     -1363,
      -163,         9,
         0,       -74,
        63,      8167,       -29,     37587,
       -22,    -74501,
       -71,       497,
        -1,    551747,
       -87,       -22,
         0,       -51,
        -1,      -463,
         0,      -444,
         3,        89,
        15,       -84,
       -36,     -6829,        -5,    -21663,
         0,     86058,
         0,      -298,
        -2,       751,        -2,     -1015,
         0,        69,
         1,     -4989,         0,     21458,
         0,      -330,
         0,        -7,
         0,      -226,
         0,     -1407,         0,      2942,
         0,        66,
         0,       667,
         0,      -155,
         0,       105,
         0,      -107,
         0,       -74,
         0,       -52,
         0,        91,
         0,        59,
         0,       235,
        -1,     -1819,         0,      2470,
        71,        13,
         0,      1026,
        14,       -54,
         0,      -174,
      -121,       -19,
         0,      -200,
         0,      3008,
       -16,     -8043,       -10,    -37136,
        -3,     73724,
      -157,        -5,
         0,      -854,
         8,       147,
       -13,      -893,
         0,     11869,
       -23,      -172,
        89,        14,
        -1,       872,         0,     -3744,
        11,      1606,
         0,      -559,
        -1,     -2530,
         0,       454,
         0,      -193,
       -60,       -10,
       -82,       -13,
       -75,         6,
        36,        81,
       354,   -162836,       148,   -516569,
         4,   2054441,
         4,       -94,
        39,        38,
        61,       -30,
         2,       121,
       -11,       590,
        62,      2108,
         0,    -12242,
      -476,       -42,
       -84,       113,
      -394,       236,
         0,       276,
       -49,        31,
         0,        86,
         1,     -1313,
         1,        69,
       -60,        88,
       -46,        18,
         0,    -63818,
        14,       -93,
       113,       547,      -618,        17,
        -7,     12290,        -1,    -25679,
         0,        92,
      -115,        50,
       -48,       233,
         4,      1311,         1,     -5567,
         3,      1251,
        29,       548,
      -244,       257,
        -2,      1825,
        42,       637,
       -46,        68,
       -62,         8,
         3,       110,
       445,      -100,      -316,      -202,
      2925,      -621,       763,      1495,
      -169,      -184,        20,       -76,
      -475,      -138,         8,      -141,
      -197,      1351,     -1284,       422,
      -129,      1879,      -102,      8382,
        -9,  45864958,
      -215,      1350,     -1285,       422,
      -481,      -136,         8,      -140,
        40,       -53,
      2622,      -543,       700,      1406,
       402,       -95,      -318,      -194,
       122,        13,
       -30,       147,
      -121,      -902,
        61,       -23,
       -63,         7,
        69,       479,
      -224,       228,
        -7,       500,
         0,      -429,
       -42,       193,
       -92,        37,
        67,         5,
      -350,       -31,
         0,        67,
       -55,        -5,
         0,        47,
       -36,        53,
         5,       561,
         0,      -126,
         0,       871,
       -52,         4,
      -201,    116922,       -22,    371352,
       -12,  -1473285,
         0,        87,
      -164,        84,
        -3,       422,
        30,      1434,
       -26,        38,
         2,  -1249943,
      -404,       -34,
       -57,        79,
         5,       509,
         1,       131,
      -344,       168,
       112,     22540,        30,     71218,
        18,   -283983,
         0,      -851,
         0,     -1538,
         0,      1360,
       -12,        51,
       -48,        68,
        88,       -20,
         1,        63,
         0,      -568,
       303,        25,
         0,      -122,
        87,       586,      -606,       -14,
         0,      -100,
       -85,         8,
      -165,        54,
       -45,       140,
         0,       -54,
         4,      -831,         1,      3495,
        31,       116,
       -46,       -11,
      -371,       190,
      -507,       399,
        -2,        57,
       -60,        36,
      -198,     -1174,      -613,      4988,
       -87,        -4,
       141,       560,      -276,       187,
      1876,      1379,       778,      4386,
        24,       -15,
       167,      -774,
       -71,        -9,
       -62,        90,
        98,       580,      -663,        -7,
        34,      -112,
        57,        15,
      -355,      -214,
     -3240,    -13605,     12229,     -5723,
      3496,      7063,
        33,       -51,
      1908,      1160,      -226,       715,
       964,      1170,     -1264,       623,
     14071,      5280,      5614,      3026,
       488,      1576,        -2, 226395859,
       824,      1106,     -1287,       617,
      1917,      1156,      -214,       718,
        90,       -97,
     12078,     -2366,      3282,      6668,
      -219,      9179,       593,      2015,
      -282,      -186,
        57,        25,
        31,      -102,
       -77,        -4,
      -268,      -341,        -7,       -45,
        -3,        74,
        15,      -615,
       -88,        -7,
       234,      -353,
         1,      -119,
      -163,     -1159,      -601,      4969,
        22,       -58,
       -17,    -11434,
        17,        54,
       348,       348,      -460,       434,
      -371,       175,
       -11,      -204,
         4,     -6440,
        -5,       -53,
        -4,    -14388,       -37,    -45231,
        -7,    179562,
       -44,       136,
      -160,        49,
      -101,        81,
        -1,      -188,
         0,         2,
        -4,     12124,       -11,    -25217,
        71,       543,      -557,       -14,
       -75,       526,
         0,    395274,
      -233,       -16,
        93,       -20,
       -43,        61,
         0,     -1275,
         0,      -824,
         1,      -415,         0,      1762,
      -261,       131,
       -45,        64,
      -297,       -25,
         0,    -17533,
        -6,       -56,
        21,      1100,
         1,       327,
         1,        66,
        23,      -217,
       -83,        -7,
        83,     86847,        49,    275754,
        -4,  -1093857,
       -46,         2,
         0,       -24,
         0,      -419,
         0,     -5833,
         1,       506,
         0,      -827,
        -1,      -377,
       -11,       -78,
         0,    131945,
        -2,      -334,
         1,       -75,
         0,       -72,
         0,      -213,
        -6,      5564,        -2,    -11618,
         0,      1790,
         0,      -131,
         0,         6,
         0,       -76,
         0,      -130,
         0,     -1115,         0,      4783,
         0,      -195,
         0,      -627,
         0,       -55,
         0,       -83,
         0,       163,
         0,       -54,
         0,        82,
         0,       149,
         0,      -754,         0,      1578,
         0,       138,
         0,        68,
         2,     -2506,         0,      3399,
         0,      -125,
        86,        16,
         0,     -6350,         0,     27316,
        18,       -63,
         0,      -169,
        -1,        46,
      -136,       -21,
         0,      -239,
       -30,     -8788,       -15,    -40549,
        -4,     80514,
       -46,        -8,
      -168,        -6,
        -1,       536,         0,     -2314,
         9,       148,
       -13,      -842,
        -1,    307713,
       -23,      -175,
        95,        15,
         0,      -297,
        11,      1341,
         0,      -106,
         0,         5,
        -4,        68,
      -114,        10,
        32,        75,
       159,   -130487,        98,   -413967,
         2,   1647339,
        -4,       -85,
       100,       -46,
         2,        95,
       -11,       461,
        51,      1647,
         0,    -32090,
      -375,       -33,
       -65,        86,
      -300,       180,
         0,       836,         0,     -3576,
         0,      -222,
         0,      -993,
       -41,        60,
         0,     -4537,
      -431,       -34,
         2,       927,         0,     -1931,
       -79,        33,
       -31,       144,
        -1,       284,         0,     -1207,
         0,        88,
       -11,       315,
      -178,       177,
        -1,       144,
       -58,       986,
        11,        86,
      -228,      -110,
      2636,      -494,       718,      1474,
        28,       -35,
       -24,       782,      -797,       277,
      2142,     -1231,       856,      1853,
        74,     10797,         0,  23699298,
       -21,       786,      -796,       277,
        27,       -34,
      2615,      -494,       712,      1461,
      -226,      -109,
       -11,       663,
         0,      -123,
      -169,       157,
       -54,       266,
         0,       -76,
         1,      -634,         0,      2738,
       -25,       106,
       -63,        24,
         0,      -372,
      -221,       -24,
         0,     -5356,
         0,      -219,
         0,        91,
       -28,      7684,        -6,     24391,
        -1,    -96795,
       -77,        43,
         2,        95,
       -47,        -3,
         0,    -84530,
         2,       310,
         1,        88,
       111,     19331,        32,     61306,
         4,   -243595,
         0,       770,
         0,      -103,
         0,       160,
         0,       356,
         0,       236,
       -41,       354,
        39,       303,
        12,       -56,
       873,      -143,       238,       482,
       -28,        35,
       -93,        31,
        -3,   7690211,
       -91,        33,
       -34,        43,
       824,      -130,       226,       450,
       -39,       341,
        -1,      -687,
         0,      -303,
        11,     -2935,         1,     12618,
       121,       924,         9,     -1836,
      -268,     -1144,      -678,      3685,
       -69,      -261,
         0,  -4115951,
       -69,      -261,
         5,      -151,
         0,       -88,
         0,        91,
         0,       187,
         0,     -1281,
         1,        77,
         1,      6059,         3,     19238,
         0,    -76305,
         0,       -90,
         0,      -238,
         0,      -962,         0,      4133,
         0,        96,
         0,      9483,
         0,        85,
         0,      -688,
         0,     -5607,
         0,        55,
         0,      -752,
         0,        71,
         0,       303,
         0,      -288,
         0,        57,
         0,        45,
         0,       189,
         0,       401,
         0,     -1474,         0,      3087,
         0,       -71,
         0,      2925,
         0,       -75,
         0,       359,
         0,        55,
         1,    -10155,         0,     43735,
         0,      -572,
         0,       -49,
         0,      -660,
         0,     -3591,         0,      7516,
         0,       668,
        -1,       -53,
        -2,    384259,
         0,      -163,
         0,       -93,
         1,       112,
       -95,    -11528,       -22,    -36505,
        -1,    145308,
         5,       145,
         0,      4047,
         1,      1483,         0,     -6352,
         0,       991,         0,     -4262,
         0,       -93,
         0,      -334,
         0,      -160,
         0,      -153,
       -10,       127,
        51,       185,
       -77,        18,
        56,      1217,         6,   1919574,
       -74,        17,
        50,       180,
        -5,        93,
         0,      -104,
         0,       -58,
        -3,      -353,        -1,      1499,
         0,      -229,
       -15,        86,
         0,    -93657,
         0,      1561,         0,     -6693,
         0,     -5839,
         1,      6791,         0,    -29143,
         1,      -701,         0,      3015,
         0,      2543,
         0,       693,
        -1,    361233,
         0,       -50,
         0,       946,
        -1,      -140,
       -70,       407,
         0,   -450995,
         0,      -368,
         0,        54,
         0,      -802,
         0,       -96,
         0,      1274,         0,     -5459,
         0,      -614,         0,      2633,
         0,       685,
         0,      -915,
         0,       -85,
         0,        88,
         0,       106,
         0,       928,
         0,      -726,         0,      1523,
         0,      5715,
         0,     -4338,         0,     18706,
         0,      -135,
         0,      -132,
         0,      -158,
         0,       -98,
         0,       680,
        -1,    138968,
         0,      -192,
         0,     -1698,
         0,     -2734,         0,     11769,
         0,         4,
         0,       673,         0,     -2891,
         0,       889,         0,     -3821,
         0,       121,
        -1,    143783,
         0,       231,
        -9,        51,
         0,    -57413,
         0,      -483,
         0,      -407,
         0,       676,         0,     -2902,
         0,       531,
         0,       445,
         0,       672,
         0,     19336,
         0,        70,
         0,    -39976,
         0,       -68,
         0,      4203,
         0,      -406,
         0,       446,
         0,      -108,
         0,        79,
         0,        84,
         0,       734,
         0,       255,
         0,      3944,
         0,      -655,         0,      2825,
         0,      -109,
         0,      -234,
         0,        57,
         0,     19773,
         0,     -2013,
         0,       958,
         0,      -521,
         0,      -757,
         0,     10594,
         0,     -9901,
         0,       199,
         0,      -275,
         0,        64,
         0,        54,
         0,       165,
         0,      1110,
         0,     -3286,
         0,       909,
         0,        54,
         0,        87,
         0,       258,
         0,      1261,
         0,       -51,
         0,       336,
         0,      -114,
         0,      2185,
         0,      -850,
         0,        75,
         0,       -69,
         0,      -103,
         0,       776,
         0,     -1238,
         0,       137,
         0,        67,
         0,      -260,
         0,       130,
         0,        49,
         0,       228,
         0,       215,
         0,      -178,
         0,        57,
         0,      -133,
};
static long lrtabb[] = {-1};
static long lrtabr[] = {
     -5422,     -2120,      1077,       772,
        39,        75,         3,        10,
      -468,      -326,      -113,       -78,
        -4,        -2,
         1,         3,
        29,        24,         4,         2,
         1,         0,
        -9,         7,        -2,         0,
       -32,       -13,        -3,        -3,
       233,       126,        89,        77,
       -33,        16,
         3,        -3,         0,        -1,
         2,         0,
         0,         1,
         4,         9,         1,         1,
        16,        -1,         0,        18,
         3,         2,
         0,         0,
         0,         0,
         0,         0,
         0,         0,
         0,        -1,
       -22,        -5,
        10,         3,         1,         1,
       -15,         7,        -2,         1,
        -8,       -11,        -1,        -2,
        -1,         1,
        46,       -58,       126,       -23,
         4,         8,
        35,         8,        10,       -17,
         0,         0,
         0,         0,
       -10,        -7,
         0,         0,
       -23,         3,       151,        10,
      -327,         0,
         4,        -5,         6,         5,
         1,         0,
        -1,        -3,
         0,         0,
         0,         1,
      -185,         0,
        -3,       -24,        -5,        -2,
     -1062,         3,      4560,         0,
        -3,         0,
         4,         1,
         8,        -1,         2,         4,
         0,         1,
         0,        -1,
         0,         0,
        -1,         0,
         0,         1,
         0,         0,
        -1,        -1,
       277,         3,      -583,         1,
        -1,         4,       -32,         7,
         0,       -34,
         1,        -1,
    -23685,         0,
        -1,        -2,
        -1,        -7,
        -5,        -4,
         0,         2,
        -2,         0,
        -5,        -1,
        35,         0,
         0,         2,
       202,         0,
       180,         0,
         0,        -1,
        -3,        -6,
      -193,         0,       770,        -1,
        -2,        -4,
       -32,        23,       -28,       -46,
       -13,        -9,
       -54,        10,        -1,       -61,
    -44895,         0,
      -230,         5,
        -1,        -4,
       -71,         0,
       -15,         0,
         1,         0,
        15,        11,        -3,         6,
         2,        -3,         4,        -1,
      2576,      -138,    -19881,       -47,
    -65906,        -1,    261925,        -4,
        -2,        -7,         4,        -2,
         0,         0,
        -1,         0,
         1,        -3,
       172,        -2,      -727,         0,
         4,         1,
       324,         0,
      -139,         1,
         1,         3,
      -276,         0,
         5,         3,
         9,         0,
        -1,        10,
       -37,         0,
         5,        -1,
        76,       -10,
   1318810,         1,
        12,        -1,
       -38,         1,
      -141,         0,       611,         0,
         0,       -11,
         4,         0,
      -627,         2,     -2882,        -3,
      5711,        -2,
       -48,        -7,
     55294,         0,
         2,        -7,
        31,         0,
        34,         0,
      -259,         0,
       -55,         2,
         6,         3,
     -4273,        20,    -13554,         3,
     53878,         0,
       -46,         0,
       -85,         0,       114,         0,
       -45,         0,
      -818,         0,      3520,         0,
        34,         0,
      -157,         0,
        29,         0,
      -878,         0,      1838,         0,
      -428,         0,
       161,         0,
        24,         0,
        65,         0,
        19,         0,
        15,         0,
        12,         0,
       -26,         0,
       -14,         0,
      -149,         0,
       584,         0,      -793,         0,
         4,       -23,
      -238,         0,
       -18,        -5,
        45,         0,
        -7,        42,
        79,         0,
     -1723,         0,
      2895,        -6,     13362,        -4,
    -26525,        -1,
        -2,        57,
       291,         0,
        52,        -3,
      -327,         5,
     -2755,         0,
       -63,         9,
         5,       -33,
      -261,        -1,      1122,         0,
       621,        -4,
      -227,         0,
      1077,         0,
      -167,         0,
        85,         0,
        -4,        23,
        -5,        32,
         3,        30,
       -32,        14,
     64607,       141,    204958,        59,
   -815115,         2,
       -37,        -1,
        15,       -15,
        12,        24,
        48,        -1,
       235,         4,
       843,       -25,
      4621,         0,
       -17,       191,
        45,        34,
        95,       159,
      -132,         0,
        13,        20,
        32,         0,
      -540,         0,
        29,         0,
        37,        25,
         8,        19,
     22127,         0,
       -35,        -5,
       232,       -48,         7,       262,
      5428,         3,    -11342,         1,
       -45,         0,
       -21,       -49,
      -100,       -21,
      -626,         1,      2665,         0,
       532,        -2,
       235,       -12,
      -111,      -105,
       774,         1,
      -283,        17,
        29,        20,
         3,        27,
        47,        -2,
       -43,      -192,       -87,       136,
      -269,     -1264,       646,      -330,
       -79,        73,       -33,        -9,
        60,      -205,        61,         4,
      -584,       -85,      -182,      -555,
      -780,       -57,     -3488,       -45,
 -19818328,        -4,
       583,        93,       182,       555,
       -59,       208,       -60,        -4,
        23,        17,
       235,      1133,      -608,       302,
        41,       174,        84,      -137,
         6,       -53,
        63,        13,
      -392,        52,
       -10,       -27,
        -3,       -27,
       199,       -31,
        99,        97,
      -218,        -3,
       209,         0,
        84,        18,
        16,        40,
         2,       -30,
        14,      -154,
        30,         0,
        -2,        24,
      -108,         0,
       -24,       -16,
       262,        -2,
        55,         0,
      -304,         0,
         2,        25,
     55112,        95,    175036,        11,
   -694477,         5,
        41,         0,
       -38,       -76,
       199,         1,
       679,       -14,
       -17,       -12,
    582619,         1,
       -16,       191,
        38,        27,
      -234,         2,
       -60,         0,
        80,       163,
    -10296,        48,    -32526,        13,
    129703,         8,
     -1366,         0,
      -741,         0,
      -646,         0,
        25,         6,
        33,        23,
        10,        43,
       -31,         0,
        -6,         0,
       -12,       147,
        59,         0,
       287,       -42,        -7,       297,
       -59,         0,
        -4,       -42,
       -27,       -81,
       -69,       -22,
        27,         0,
      -423,        -2,      1779,        -1,
       -57,        15,
         5,       -23,
        94,       182,
      -197,      -250,
        24,         1,
       -18,       -30,
       581,       -98,     -2473,      -303,
        -2,        43,
      -277,        70,       -92,      -136,
      -681,       925,     -2165,       384,
        -8,       -12,
       382,        82,
        -4,        35,
       -45,       -31,
      -286,        48,         3,      -328,
       -55,       -17,
         8,       -28,
      -106,       175,
     -6735,      1601,     -2832,     -6052,
      3495,     -1730,
       -25,       -17,
      -574,       944,      -354,      -112,
      -579,       476,      -308,      -625,
     -2411,      7074,     -1529,      2828,
     -1335,       247,-112000844,        -1,
       545,      -409,       305,       637,
       572,      -950,       356,       106,
        48,        44,
      1170,      5974,     -3298,      1624,
     -4538,      -106,      -996,       294,
        92,      -139,
       -12,        28,
        50,        16,
         2,       -38,
       169,      -133,        22,        -3,
        38,         1,
       305,         7,
         4,       -44,
       175,       116,
        59,         1,
      -573,        81,      2453,       297,
        29,        11,
      5674,        -8,
       -27,         9,
       173,      -173,       215,       228,
       -87,      -184,
       102,        -5,
      3206,         2,
       -53,         2,
      7159,        -7,     22505,       -19,
    -89344,        -3,
        67,        22,
        24,        79,
       -40,       -50,
        94,         0,
       186,         0,
     -6063,         0,     12612,        -5,
      -271,        35,         7,      -278,
      -479,       -74,
    426754,         0,
         8,      -116,
       -10,       -47,
       -31,       -22,
       645,         0,
       426,         0,
      -213,         0,       903,         0,
       -67,      -133,
       -33,       -23,
        13,      -152,
     -9316,         0,
        29,        -3,
      -564,        11,
      -167,         0,
       -34,         0,
       114,        12,
         4,       -44,
    -44561,        42,   -141493,        25,
    561256,        -2,
        -1,       -24,
      -261,         0,
       211,         0,
     -4263,         0,
      -262,         1,
      1842,         0,
       202,         0,
        41,        -6,
     77165,         0,
       176,        -1,
        39,         1,
       -24,         0,
       118,         0,
     -2991,        -4,      6245,        -1,
     46886,         0,
       -75,         0,
      -100,         0,
        40,         0,
        75,         0,
      -618,         0,      2652,         0,
       112,         0,
      1780,         0,
        30,         0,
        49,         0,
        86,         0,
        33,         0,
       -30,         0,
       -95,         0,
       277,         0,      -580,         0,
       -35,         0,
      -319,         0,
      1622,         1,     -2201,         0,
        79,         0,
        10,       -57,
      2363,         0,    -10162,         0,
       -41,       -12,
        62,         0,
        30,         1,
       -14,        89,
     -2721,         0,
      5780,       -19,     26674,       -10,
    -52964,        -2,
        -5,        30,
        -4,       111,
      -317,        -1,      1369,         0,
        93,        -6,
      -564,         9,
   -115913,         0,
      -113,        15,
        10,       -62,
        99,         0,
       891,        -7,
        36,         0,
       108,         0,
       -42,        -2,
         7,        75,
       -50,        21,
     86822,       104,    275441,        65,
  -1096109,         1,
       -56,         3,
        31,        66,
        63,        -1,
       307,         7,
      1097,       -34,
     17453,         0,
       -22,       250,
        57,        43,
       120,       200,
      -297,         0,      1269,         0,
       166,         0,
      -662,         0,
        40,        28,
      1521,         0,
       -23,       288,
       351,        -2,      -729,         0,
       -22,       -52,
       -96,       -21,
      -139,        -1,       589,         0,
        35,         0,
       210,         7,
      -118,      -119,
        62,         0,
      -583,       -26,
       -42,         5,
       -73,       152,
      -330,     -1759,       983,      -479,
       -23,       -19,
      -522,       -15,      -185,      -533,
       739,      1559,     -1300,       614,
     -7332,        52, -15836758,         0,
       524,        16,       185,       532,
        23,        18,
       330,      1751,      -978,       476,
        73,      -151,
       519,        18,
        38,         0,
       105,       113,
      -178,       -37,
        26,         0,
       262,         1,     -1139,         0,
        71,        17,
        16,        42,
       151,         0,
        16,      -148,
      4147,         0,
       149,         0,
       -30,         0,
      2980,         9,      9454,         2,
    -37519,         0,
       -28,       -49,
        37,        -1,
         2,       -31,
     33870,         0,
      -208,         1,
       -59,         1,
    -13105,        68,    -41564,        21,
    165148,         3,
     -1022,         0,
       -40,         0,
      -132,         0,
      -228,         0,
        95,         0,
      -138,       -16,
      -126,        16,
        24,         5,
       -57,      -346,       191,       -94,
       -14,       -11,
       -12,       -37,
  -3053364,        -1,
        13,        36,
        17,        13,
        51,       327,      -179,        90,
       138,        16,
       233,         0,
        62,         0,
      1164,         0,     -5000,         0,
      -407,       117,       770,         9,
        -4,         1,        21,         2,
         1,         0,
    -16869,         0,
        -1,         0,
         1,         0,
        35,         0,
       -78,         0,
        78,         0,
      -533,         0,
       -31,         1,
     -2448,        -6,     -7768,        -1,
     30812,         0,
        37,         0,
      -227,         0,
       197,         0,      -846,         0,
       -77,         0,
      4171,         0,
       -67,         0,
       287,         0,
      2532,         0,
       -19,         0,
       -40,         0,
       -56,         0,
       128,         0,
        83,         0,
       -45,         0,
       -36,         0,
       -92,         0,
      -134,         0,
       714,         0,     -1495,         0,
        32,         0,
      -981,         0,
        15,         0,
      -166,         0,
       -59,         0,
      4923,         0,    -21203,         0,
       246,         0,
        15,         0,
       104,         0,
      1683,         0,     -3523,         0,
      -865,         0,
       -25,         1,
   -186329,        -1,
        10,         0,
        50,         0,
        53,         0,
      5455,       -45,     17271,       -10,
    -68747,         0,
        69,        -2,
     -7604,         0,
      -724,         1,      3101,         0,
       -46,         0,       200,         0,
       -44,         0,
        97,         0,
       -53,         0,
        62,         0,
       -54,        -4,
        88,       -24,
        -9,       -36,
      -581,        27,   -914711,         3,
         8,        35,
       -86,        24,
        51,         3,
        48,         0,
        26,         0,
       133,         1,      -577,         0,
       105,         0,
        -3,        -1,
      3194,         0,
       528,         0,     -2263,         0,
      2028,         0,
     -3266,         1,     14016,         0,
        10,         0,       -41,         0,
      -100,         0,
       -32,         0,
   -124348,         0,
        16,         0,
      -325,         0,
        50,        -1,
         1,         0,
      -553,         0,
         0,         0,
         0,         0,
         2,         0,
       -34,         0,
      -444,         0,      1902,         0,
         9,         0,       -37,         0,
       254,         0,
       156,         0,
        -2,         0,
       -35,         0,
       -48,         0,
      -368,         0,
       327,         0,      -686,         0,
     -2263,         0,
      1952,         0,     -8418,         0,
       -13,         0,
        52,         0,
         9,         0,
        21,         0,
      -261,         0,
    -62404,         0,
         0,         0,
        79,         0,
      1056,         0,     -4547,         0,
      -351,         0,
      -305,         0,      1310,         0,
        -1,         0,         6,         0,
         0,         0,
    -55953,         0,
       -80,         0,
         0,         0,
       168,         0,
      -147,         0,
       127,         0,
      -265,         0,      1138,         0,
        -1,         0,
        -9,         0,
        -8,         0,
     -5984,         0,
       -22,         0,
        -5,         0,
         0,         0,
         0,         0,
       127,         0,
        -2,         0,
        10,         0,
       -31,         0,
       -29,         0,
      -286,         0,
       -98,         0,
     -1535,         0,
       252,         0,     -1087,         0,
        43,         0,
         4,         0,
       -19,         0,
     -7620,         0,
        29,         0,
      -322,         0,
       203,         0,
         0,         0,
     -3587,         0,
        10,         0,
         0,         0,
        94,         0,
         0,         0,
        -1,         0,
        -1,         0,
      -315,         0,
         1,         0,
         0,         0,
         0,         0,
       -30,         0,
       -94,         0,
      -460,         0,
         1,         0,
      -114,         0,
         0,         0,
      -746,         0,
         4,         0,
       -23,         0,
        24,         0,
         0,         0,
      -237,         0,
         1,         0,
         0,         0,
       -18,         0,
         0,         0,
         0,         0,
       -16,         0,
       -76,         0,
       -67,         0,
         0,         0,
       -16,         0,
         0,         0,
};
static CHAR lrargs[] = {
  0,  3,
  3,  4,  3, -8,  4,  3,  5,  1,
  2,  2,  5, -5,  6,  2,
  5, -1, 10,  2, 13, -1, 11,  3,  3, -7,  4,  0,
  3,  1, 13, -1, 11,  2,  5,  1,
  2,  4,  5,-10,  6,  0,
  4,  2, 10, -2, 13, 14,  3,-23,  4,  1,
  3,  3,  2, -7,  3,  4,  4,  1,
  3, -1, 13, 18,  2,-16,  3,  2,
  2,  8,  2,-13,  3,  1,
  5,  2, 10, -2, 13,  2,  3, -3,  5,  1,  6,  0,
  3, -1, 13, 26,  2,-29,  3,  0,
  3,  1, 10, -1, 11,  2,  4,  1,
  4,  1, 10, -1, 13,  3,  2, -4,  3,  1,
  4,  1, 10, -1, 13,  3,  3, -4,  4,  0,
  3, -1, 10, 15,  2,-12,  3,  0,
  4,  2, 10, -3, 13, 24,  2,-24,  3,  0,
  3, -1, 10, 23,  2,-25,  3,  0,
  4,  1, 10, -1, 11,  1,  3,  1,  6,  0,
  4,  2, 10, -2, 11,  5,  2, -6,  3,  0,
  4,  2, 10, -2, 13,  6,  2, -8,  3,  0,
  4, -2, 10,  1, 13, 12,  2, -8,  3,  1,
  5, -1, 10,  1, 13, -1, 11, 20,  2,-20,  3,  1,
  4, -2, 10,  1, 13,  3,  1, -1,  3,  1,
  5,  2, 10, -2, 13,  2,  3, -5,  5,  5,  6,  0,
  4,  2, 10, -2, 13,  2,  3, -3,  5,  1,
  4,  2, 10, -2, 13,  6,  3, -8,  4,  0,
  4, -2, 10,  1, 13, 20,  2,-21,  3,  1,
  4,  1, 10, -1, 11,  1,  3,  1,  5,  0,
  1,  1,  6,  0,
  4,  2, 10, -2, 13,  5,  3, -6,  4,  0,
  3,  3,  2, -5,  3,  2,  5,  0,
  2, -1, 11,  1, 14,  1,
  4,  2, 10, -2, 13,  2,  3, -2,  5,  0,
  2,  1,  3, -2,  4,  1,
  4,  1, 10, -1, 11,  5,  2, -7,  3,  0,
  1,  1,  5,  0,
  2,  7,  3,-13,  4,  0,
  4, -2, 10,  1, 13, 15,  2,-13,  3,  0,
  4,  2, 10, -2, 13,  3,  2, -3,  3,  0,
  2, -2, 11,  2, 14,  1,
  3,  1, 10,  1, 12, -1, 13,  1,
  3, -1, 13, 21,  2,-21,  3,  0,
  2,  3,  2, -5,  3,  0,
  2,  2,  3, -4,  4,  1,
  2,  5,  2, -8,  3,  0,
  3, -1, 13, 23,  2,-24,  3,  0,
  2,  6,  3,-11,  4,  0,
  1,  2,  5,  0,
  2,  3,  3, -6,  4,  0,
  2,  5,  3, -9,  4,  0,
  4,  1, 10, -1, 11,  1,  3, -2,  5,  0,
  3,  2, 10,  2, 12, -2, 13,  1,
  2,  2,  2, -3,  3,  2,
  2,  4,  3, -7,  4,  0,
  2,  2, 13, -2, 11,  0,
  2,  3,  3, -5,  4,  0,
  2,  1,  2, -2,  3,  0,
  2,  2,  3, -3,  4,  0,
  4,  1, 10, -1, 11,  4,  2, -5,  3,  0,
  2,  1,  3, -1,  4,  0,
  2,  4,  2, -6,  3,  0,
  4,  2, 10, -2, 13,  2,  2, -2,  3,  0,
  3,  1, 10, -1, 11,  1,  2,  0,
  2,  1,  2, -1,  3,  0,
  3,  1, 12,  2, 13, -2, 11,  0,
  2,  5,  3, -8,  4,  0,
  2,  1,  3, -3,  5,  0,
  3,  2, 10,  1, 12, -2, 13,  1,
  2,  4,  3, -6,  4,  0,
  2,  1,  3, -2,  5,  1,
  2,  3,  3, -4,  4,  0,
  2,  3,  2, -4,  3,  1,
  2,  1, 10, -1, 13,  0,
  2,  1,  3, -1,  5,  0,
  2,  1,  3, -2,  6,  0,
  2,  2,  3, -2,  4,  0,
  2,  1,  3, -1,  6,  0,
  2,  8,  2,-14,  3,  0,
  3,  1,  3,  2,  5, -5,  6,  1,
  3,  5,  3, -8,  4,  3,  5,  1,
  1,  1, 12,  3,
  3,  3,  3, -8,  4,  3,  5,  1,
  3,  1,  3, -2,  5,  5,  6,  0,
  2,  8,  2,-12,  3,  0,
  2,  1,  3,  1,  5,  0,
  3,  2, 10,  1, 12, -2, 11,  1,
  2,  5,  2, -7,  3,  0,
  3,  1, 10,  1, 13, -2, 11,  0,
  2,  2,  2, -2,  3,  0,
  2,  5,  3, -7,  4,  0,
  3,  1, 12, -2, 13,  2, 11,  0,
  2,  4,  3, -5,  4,  0,
  2,  3,  3, -3,  4,  0,
  1,  1,  2,  0,
  3,  3, 10,  1, 12, -3, 13,  0,
  2,  2,  3, -4,  5,  0,
  2,  2,  3, -3,  5,  0,
  2,  2, 10, -2, 13,  0,
  2,  2,  3, -2,  5,  0,
  2,  3,  2, -3,  3,  0,
  3,  1, 10, -1, 12, -1, 13,  1,
  2,  2,  3, -1,  5,  0,
  2,  2,  3, -2,  6,  0,
  1,  2, 12,  2,
  3, -2, 10,  1, 11,  1, 14,  0,
  2,  2, 10, -2, 11,  0,
  2,  2,  2, -1,  3,  0,
  4, -2, 10,  2, 13,  1,  2, -1,  3,  0,
  2,  4,  2, -4,  3,  0,
  2,  3, 10, -3, 13,  0,
  4, -2, 10,  2, 13,  1,  3, -1,  5,  0,
  2,  3,  3, -3,  5,  0,
  3,  2, 10, -1, 12, -2, 13,  2,
  3,  3, 10, -1, 13, -2, 11,  0,
  1,  3, 12,  1,
  4, -2, 10,  2, 13,  2,  2, -2,  3,  0,
  3,  2, 10, -1, 12, -2, 11,  1,
  2,  5,  2, -5,  3,  0,
  2,  4, 10, -4, 13,  0,
  2,  6,  2, -6,  3,  0,
  3,  2, 10, -2, 12, -2, 13,  1,
  3,  4, 10, -2, 13, -2, 11,  0,
  3,  2, 10, -2, 12, -2, 11,  0,
  2,  7,  2, -7,  3,  0,
  3,  2, 10, -3, 12, -2, 13,  0,
  2,  8,  2, -8,  3,  0,
  2,  9,  2, -9,  3,  0,
  2, 10,  2,-10,  3,  0,
  3,  2, 10, -4, 12, -1, 13,  0,
  3,  4, 10, -2, 12, -3, 13,  0,
  4,  4, 10, -1, 12, -1, 13, -2, 11,  0,
  3,  2, 10, -3, 12, -1, 13,  1,
  4, -2, 10,  1, 13,  3,  3, -2,  5,  0,
  3,  4, 10, -1, 12, -3, 13,  0,
  4, -2, 10,  1, 13,  3,  3, -3,  5,  0,
  4,  2, 10, -2, 12,  1, 13, -2, 11,  0,
  4, -2, 10,  1, 13,  2,  2, -1,  3,  0,
  3,  3, 10, -1, 12, -2, 11,  0,
  3,  4, 10, -1, 13, -2, 11,  0,
  3,  2, 10, -2, 12, -1, 13,  2,
  4, -2, 10,  1, 13,  2,  3, -1,  5,  0,
  3,  3, 10, -1, 12, -2, 13,  0,
  4, -2, 10,  1, 13,  3,  2, -3,  3,  0,
  4, -2, 10,  1, 13,  2,  3, -2,  5,  0,
  2,  4, 10, -3, 13,  0,
  4, -2, 10,  1, 13,  2,  3, -3,  5,  0,
  3, -2, 10,  1, 13,  1,  2,  0,
  4,  2, 10, -1, 12,  1, 13, -2, 11,  1,
  4, -2, 10,  1, 13,  2,  2, -2,  3,  0,
  2,  3, 12, -1, 13,  0,
  2,  3, 10, -2, 11,  0,
  2,  1, 10, -2, 12,  0,
  4,  4, 10,  1, 12, -1, 13, -2, 11,  0,
  3, -1, 13,  3,  2, -2,  3,  0,
  3, -1, 13,  3,  3, -2,  5,  0,
  3, -2, 10, 18,  2,-15,  3,  0,
  5,  2, 10, -1, 13,  3,  3, -8,  4,  3,  5,  0,
  3,  2, 10, -1, 12, -1, 13,  2,
  5, -2, 10,  1, 13,  5,  3, -8,  4,  3,  5,  0,
  5, -2, 10,  1, 13,  1,  3,  2,  5, -5,  6,  0,
  4,  2, 10, -2, 13, 18,  2,-17,  3,  0,
  4, -2, 10,  1, 13,  1,  3, -1,  6,  0,
  4, -2, 10,  1, 13,  2,  3, -2,  4,  0,
  4, -2, 10,  1, 13,  1,  3, -1,  5,  0,
  2,  3, 10, -2, 13,  0,
  4, -2, 10,  1, 13,  3,  2, -4,  3,  0,
  4, -2, 10,  1, 13,  3,  3, -4,  4,  0,
  4, -2, 10,  1, 13,  1,  3, -2,  5,  0,
  3,  4, 10,  1, 12, -3, 13,  0,
  4, -2, 10,  1, 13,  1,  3, -3,  5,  0,
  3, -1, 13,  4,  2, -4,  3,  0,
  4, -2, 10,  1, 13,  1,  2, -1,  3,  0,
  4, -2, 10,  1, 13,  1,  3, -1,  4,  0,
  4, -2, 10,  1, 13,  2,  3, -3,  4,  0,
  4, -2, 10,  1, 13,  3,  3, -5,  4,  0,
  3,  2, 10,  1, 13, -2, 11,  0,
  4, -2, 10, -1, 13,  1, 11,  1, 14,  0,
  4, -2, 10,  1, 13,  2,  2, -3,  3,  1,
  2,  2, 12, -1, 13,  1,
  3,  3, 10,  1, 12, -2, 11,  0,
  4,  2, 10, -1, 13,  2,  3, -4,  4,  0,
  4,  2, 10, -1, 13,  3,  2, -5,  3,  0,
  2,  1, 10, -1, 12,  1,
  3, -1, 13,  3,  2, -3,  3,  0,
  3, -2, 10,  1, 13,  1,  5,  0,
  4,  2, 10, -1, 13,  1,  3, -2,  4,  0,
  3, -1, 13,  2,  3, -2,  5,  0,
  4,  2, 10, -1, 13, -1, 11,  1, 14,  0,
  3, -1, 13,  5,  3, -6,  4,  0,
  3, -2, 10,  1, 13,  1,  6,  0,
  3, -1, 10,  1,  3, -1,  5,  0,
  4, -2, 10,  1, 13,  8,  2,-13,  3,  1,
  3, -2, 10, 18,  2,-16,  3,  1,
  5, -2, 10,  1, 13,  3,  2, -7,  3,  4,  4,  1,
  4,  2, 10, -1, 13,  2,  5, -5,  6,  1,
  5,  2, 10, -1, 13,  4,  3, -8,  4,  3,  5,  1,
  2,  2, 10, -1, 13,  2,
  5, -2, 10,  1, 13,  4,  3, -8,  4,  3,  5,  1,
  4, -2, 10,  1, 13,  2,  5, -5,  6,  1,
  5,  2, 10, -1, 13,  3,  2, -7,  3,  4,  4,  0,
  4,  2, 10, -2, 13, 18,  2,-16,  3,  1,
  4,  2, 10, -1, 13,  8,  2,-13,  3,  1,
  3, -1, 10,  3,  2, -4,  3,  0,
  3, -1, 13,  6,  2, -8,  3,  0,
  3, -1, 13,  2,  3, -3,  5,  0,
  3, -1, 13,  6,  3, -8,  4,  0,
  3,  2, 10, -1, 13,  1,  6,  0,
  4, -2, 10,  1, 13, -1, 11,  1, 14,  0,
  4, -2, 10,  1, 13,  1,  3, -2,  4,  0,
  3,  2, 10, -1, 13,  1,  5,  0,
  3,  3, 10,  1, 12, -2, 13,  0,
  4, -2, 10,  1, 13,  3,  2, -5,  3,  0,
  4, -2, 10,  1, 13,  2,  3, -4,  4,  0,
  2, -1, 13,  1,  2,  0,
  4,  2, 10, -1, 13,  2,  2, -3,  3,  0,
  3, -1, 10,  1,  2, -1,  3,  0,
  3, -1, 13,  4,  2, -5,  3,  0,
  3,  2, 10, -3, 13,  2, 11,  0,
  4,  2, 10, -1, 13,  2,  3, -3,  4,  0,
  3, -1, 13,  2,  2, -2,  3,  0,
  4,  2, 10, -1, 13,  1,  2, -1,  3,  0,
  4,  2, 10,  1, 12,  1, 13, -2, 11,  0,
  3, -2, 13, 18,  2,-15,  3,  0,
  2,  1, 12, -1, 13,  2,
  3, -1, 13,  1,  3, -1,  6,  0,
  4,  2, 10, -1, 13,  1,  3, -2,  5,  0,
  3, -1, 13,  2,  3, -2,  4,  0,
  3, -1, 13,  1,  3, -1,  5,  0,
  4,  2, 10, -1, 13,  3,  3, -4,  4,  0,
  1,  1, 10,  0,
  3, -1, 13,  3,  2, -4,  3,  0,
  3, -1, 13,  3,  3, -4,  4,  0,
  4,  2, 10, -1, 13,  1,  3, -1,  5,  0,
  4,  2, 10, -1, 13,  2,  3, -2,  4,  0,
  3, -1, 13,  1,  3, -2,  5,  0,
  3,  2, 10,  1, 12, -1, 13,  2,
  3,  1, 12,  1, 13, -2, 11,  0,
  3, -1, 13,  1,  2, -1,  3,  0,
  4,  2, 10, -1, 13,  2,  2, -2,  3,  0,
  3, -1, 13,  4,  2, -6,  3,  0,
  3, -1, 13,  2,  3, -3,  4,  0,
  3,  1, 13,  1,  2, -2,  3,  0,
  4,  2, 10, -1, 13,  3,  3, -3,  4,  0,
  2,  3, 13, -2, 11,  0,
  4,  2, 10, -1, 13,  4,  2, -5,  3,  0,
  3,  1, 10,  1,  2, -1,  3,  0,
  3, -1, 13,  2,  2, -3,  3,  1,
  3,  2, 10,  2, 12, -3, 13,  0,
  3,  2, 10, -1, 13,  1,  2,  0,
  3,  1, 13,  2,  3, -4,  4,  0,
  3,  1, 13,  3,  2, -5,  3,  0,
  2, 21,  2,-21,  3,  0,
  3,  1, 10,  1, 12, -2, 13,  1,
  4,  2, 10, -1, 13,  2,  3, -4,  5,  0,
  4,  2, 10, -1, 13,  7,  3,-10,  4,  0,
  2, -1, 13,  1,  5,  0,
  3,  1, 13,  1,  3, -2,  4,  0,
  4,  2, 10, -3, 13,  2,  3, -2,  5,  0,
  3,  1, 10,  1,  3, -2,  5,  0,
  3,  1, 13, -1, 11,  1, 14,  1,
  2, -1, 13,  1,  6,  0,
  4,  2, 10, -1, 13,  6,  3, -8,  4,  1,
  4,  2, 10, -1, 13,  2,  3, -3,  5,  1,
  3, -1, 13,  8,  3,-15,  4,  0,
  4,  2, 10, -1, 13,  6,  2, -8,  3,  0,
  5,  2, 10, -1, 13, -2, 11,  5,  2, -6,  3,  0,
  3,  1, 10,  3,  3, -4,  4,  0,
  3,  1, 10,  3,  2, -4,  3,  1,
  4,  1, 10, -1, 13, -1, 11,  2,  4,  0,
  3, -2, 13, 26,  2,-29,  3,  0,
  3, -1, 13,  8,  2,-13,  3,  0,
  3, -2, 13, 18,  2,-16,  3,  2,
  4, -1, 13,  3,  2, -7,  3,  4,  4,  0,
  3,  1, 13,  2,  5, -5,  6,  1,
  4,  1, 13,  4,  3, -8,  4,  3,  5,  1,
  1,  1, 13,  3,
  4, -1, 13,  4,  3, -8,  4,  3,  5,  1,
  3, -1, 13,  2,  5, -5,  6,  1,
  4,  1, 13,  3,  2, -7,  3,  4,  4,  0,
  2, 18,  2,-16,  3,  1,
  3,  1, 13,  8,  2,-13,  3,  2,
  2, 26,  2,-29,  3,  0,
  4,  1, 10,  1, 13, -1, 11,  2,  4,  0,
  5,  2, 10,  1, 13, -2, 11,  5,  2, -6,  3,  0,
  3,  1, 13,  8,  3,-15,  4,  1,
  4,  2, 10, -3, 13,  2,  3, -3,  5,  0,
  3,  1, 10,  1,  3, -1,  5,  0,
  2,  1, 13,  1,  6,  0,
  4,  2, 10, -1, 13,  5,  3, -6,  4,  0,
  3,  1, 10,  2,  3, -2,  4,  0,
  3, -1, 13, -1, 11,  1, 14,  1,
  4,  2, 10, -1, 13,  2,  3, -5,  6,  0,
  4,  2, 10, -1, 13,  2,  3, -2,  5,  0,
  5,  2, 10, -1, 13,  2,  3, -4,  5,  5,  6,  0,
  3, -1, 13,  1,  3, -2,  4,  1,
  2,  1, 13,  1,  5,  0,
  4,  2, 10, -1, 13,  4,  3, -4,  4,  0,
  4,  2, 10, -1, 13,  3,  2, -3,  3,  0,
  4,  2, 10,  2, 12, -1, 13, -2, 11,  0,
  2,  1, 10,  1, 12,  2,
  3, -1, 13,  3,  2, -5,  3,  0,
  3, -1, 13,  2,  3, -4,  4,  0,
  4,  2, 10, -1, 13,  2,  3, -1,  5,  0,
  4,  2, 10, -1, 13,  2,  3, -2,  6,  0,
  3,  1, 10,  1, 12, -2, 11,  0,
  3,  2, 10,  2, 12, -1, 13,  1,
  3,  1, 13,  2,  2, -3,  3,  1,
  3, -1, 13,  1, 11,  1, 14,  0,
  2,  1, 13, -2, 11,  0,
  4,  2, 10, -1, 13,  5,  2, -6,  3,  0,
  3, -1, 13,  1,  2, -2,  3,  0,
  3,  1, 13,  2,  3, -3,  4,  0,
  3,  1, 13,  1,  2, -1,  3,  0,
  4,  2, 10, -1, 13,  4,  2, -4,  3,  0,
  3,  2, 10,  1, 12, -3, 13,  1,
  3,  1, 13,  1,  3, -2,  5,  0,
  3,  1, 13,  3,  3, -4,  4,  0,
  3,  1, 13,  3,  2, -4,  3,  0,
  2,  1, 10, -2, 13,  0,
  4,  2, 10, -1, 13,  3,  3, -4,  5,  0,
  3,  1, 13,  1,  3, -1,  5,  0,
  3,  1, 13,  2,  3, -2,  4,  0,
  3,  1, 13,  1,  3, -1,  6,  0,
  4,  2, 10, -1, 13,  3,  3, -3,  5,  0,
  4,  2, 10, -1, 13,  6,  2, -7,  3,  0,
  2,  1, 12,  1, 13,  2,
  4,  2, 10, -1, 13,  3,  3, -2,  5,  0,
  4,  2, 10,  1, 12, -1, 13, -2, 11,  0,
  2,  1, 10,  2, 12,  0,
  2,  1, 10, -2, 11,  0,
  3,  1, 13,  2,  2, -2,  3,  0,
  3,  1, 12, -1, 13,  2, 11,  0,
  4,  2, 10, -1, 13,  5,  2, -5,  3,  0,
  3,  1, 13,  2,  3, -3,  5,  0,
  2,  2, 10, -3, 13,  0,
  3,  1, 13,  2,  3, -2,  5,  0,
  3,  1, 13,  3,  2, -3,  3,  0,
  3,  1, 10, -1, 12, -2, 13,  0,
  4,  2, 10, -1, 13,  6,  2, -6,  3,  0,
  2,  2, 12,  1, 13,  1,
  3,  2, 10, -1, 13, -2, 11,  0,
  3,  1, 10, -1, 12, -2, 11,  0,
  3,  2, 10,  1, 13, -4, 11,  0,
  3,  1, 13,  4,  2, -4,  3,  0,
  4,  2, 10, -1, 13,  7,  2, -7,  3,  0,
  3,  2, 10, -1, 12, -3, 13,  1,
  2,  3, 12,  1, 13,  0,
  4,  2, 10, -1, 12, -1, 13, -2, 11,  0,
  3,  1, 13,  5,  2, -5,  3,  0,
  4,  2, 10, -1, 13,  8,  2, -8,  3,  0,
  3,  2, 10, -2, 12, -3, 13,  0,
  4,  2, 10, -1, 13,  9,  2, -9,  3,  0,
  3,  4, 10, -3, 12, -2, 13,  0,
  2,  2, 10, -4, 12,  0,
  3,  4, 10, -2, 12, -2, 13,  1,
  2,  6, 10, -4, 13,  0,
  3,  4, 10, -1, 12, -2, 11,  0,
  2,  2, 10, -3, 12,  1,
  3,  3, 10, -2, 12, -1, 13,  0,
  3, -2, 10,  3,  3, -2,  5,  0,
  3,  4, 10, -1, 12, -2, 13,  1,
  3, -2, 10,  3,  3, -3,  5,  0,
  2,  5, 10, -3, 13,  0,
  3, -2, 10,  4,  2, -4,  3,  0,
  3, -2, 10,  2,  2, -1,  3,  0,
  2,  4, 10, -2, 11,  0,
  2,  2, 10, -2, 12,  2,
  3, -2, 10,  3,  3, -2,  4,  0,
  3, -2, 10,  2,  3, -1,  5,  0,
  3,  3, 10, -1, 12, -1, 13,  1,
  3, -2, 10,  3,  2, -3,  3,  0,
  3, -2, 10,  2,  3, -2,  5,  0,
  2,  4, 10, -2, 13,  0,
  3, -2, 10,  2,  3, -3,  5,  0,
  2, -2, 10,  1,  2,  0,
  4,  2, 10, -1, 12,  2, 13, -2, 11,  0,
  3, -2, 10,  2,  2, -2,  3,  0,
  3,  3, 10,  1, 13, -2, 11,  0,
  3,  4, 10,  1, 12, -2, 11,  0,
  4,  2, 10, -1, 12, -1, 11,  1, 14,  0,
  4, -2, 10, -1, 13, 18,  2,-15,  3,  0,
  4,  2, 10,  3,  3, -8,  4,  3,  5,  0,
  2,  2, 10, -1, 12,  2,
  4, -2, 10,  5,  3, -8,  4,  3,  5,  0,
  4,  2, 10, -1, 13, 18,  2,-17,  3,  0,
  3, -2, 10,  1,  3, -1,  6,  0,
  3, -2, 10,  2,  3, -2,  4,  0,
  3, -2, 10,  1,  3, -1,  5,  0,
  2,  3, 10, -1, 13,  0,
  3, -2, 10,  3,  2, -4,  3,  0,
  3, -2, 10,  3,  3, -4,  4,  0,
  3, -2, 10,  1,  3, -2,  5,  0,
  3,  4, 10,  1, 12, -2, 13,  1,
  4,  2, 10, -1, 12, -2, 13,  2, 11,  0,
  3, -2, 10,  1,  2, -1,  3,  0,
  3, -2, 10,  2,  3, -3,  4,  0,
  3,  2, 10,  2, 13, -2, 11,  0,
  3, -2, 10,  2,  2, -3,  3,  0,
  2,  2, 12, -2, 13,  1,
  3,  2, 10,  2,  3, -4,  4,  0,
  3,  2, 10,  3,  2, -5,  3,  0,
  3,  1, 10, -1, 12,  1, 13,  1,
  3, -2, 13,  3,  2, -3,  3,  0,
  2, -2, 10,  1,  5,  0,
  3,  2, 10,  1,  3, -2,  4,  0,
  3, -2, 13,  2,  3, -2,  5,  0,
  3,  2, 10, -1, 11,  1, 14,  0,
  4,  4, 10, -2, 13,  2,  3, -3,  5,  0,
  3, -2, 10,  8,  2,-13,  3,  0,
  4, -2, 10, -1, 13, 18,  2,-16,  3,  1,
  4, -2, 10,  3,  2, -7,  3,  4,  4,  0,
  4,  2, 10,  4,  3, -8,  4,  3,  5,  1,
  1,  2, 10,  3,
  4, -2, 10,  4,  3, -8,  4,  3,  5,  1,
  4,  2, 10,  3,  2, -7,  3,  4,  4,  0,
  4,  2, 10, -1, 13, 18,  2,-16,  3,  1,
  3,  2, 10,  8,  2,-13,  3,  0,
  3, -2, 10, -1, 11,  1, 14,  0,
  4,  4, 10, -2, 13,  2,  3, -2,  5,  0,
  3, -2, 10,  1,  3, -2,  4,  0,
  2,  2, 10,  1,  5,  0,
  4,  4, 10, -2, 13,  3,  2, -3,  3,  0,
  3,  3, 10,  1, 12, -1, 13,  1,
  3, -2, 10,  3,  2, -5,  3,  0,
  3, -2, 10,  2,  3, -4,  4,  0,
  3,  4, 10,  2, 12, -2, 13,  0,
  3,  2, 10,  2,  2, -3,  3,  0,
  3,  2, 10, -2, 13,  2, 11,  0,
  3,  2, 10,  1,  2, -1,  3,  0,
  4,  2, 10,  1, 12,  2, 13, -2, 11,  0,
  2,  1, 12, -2, 13,  2,
  3,  2, 10,  1,  3, -2,  5,  0,
  3, -2, 13,  1,  3, -1,  5,  0,
  3,  2, 10,  3,  2, -4,  3,  0,
  2,  1, 10,  1, 13,  0,
  3,  2, 10,  1,  3, -1,  5,  0,
  3,  2, 10,  2,  3, -2,  4,  0,
  2,  2, 10,  1, 12,  2,
  2,  1, 12, -2, 11,  0,
  3, -2, 13,  1,  2, -1,  3,  0,
  3,  1, 10, -1, 13,  2, 11,  0,
  3,  2, 10,  2,  2, -2,  3,  0,
  3,  1, 10,  1, 12, -3, 13,  0,
  3,  2, 13, -1, 11,  1, 14,  0,
  3,  2, 10,  2,  3, -3,  5,  0,
  3,  2, 10,  6,  2, -8,  3,  0,
  3, -3, 13, 18,  2,-16,  3,  1,
  3,  2, 13,  2,  5, -5,  6,  0,
  4,  2, 13,  4,  3, -8,  4,  3,  5,  0,
  1,  2, 13,  0,
  4, -2, 13,  4,  3, -8,  4,  3,  5,  0,
  3, -2, 13,  2,  5, -5,  6,  0,
  3,  1, 13, 18,  2,-16,  3,  1,
  3, -2, 13, -1, 11,  1, 14,  0,
  3,  2, 10,  2,  3, -2,  5,  0,
  3,  2, 10,  3,  2, -3,  3,  0,
  3,  1, 10,  1, 12,  1, 13,  1,
  2,  2, 10,  2, 12,  1,
  2,  1, 11,  1, 14,  1,
  4, -1, 13, -2, 11, 18,  2,-16,  3,  0,
  1,  2, 11,  0,
  4, -1, 13,  2, 11, 18,  2,-16,  3,  0,
  2, -3, 11,  1, 14,  0,
  3,  2, 13,  1,  2, -1,  3,  0,
  3,  2, 10,  4,  2, -4,  3,  0,
  3,  2, 10,  1, 12, -4, 13,  0,
  2,  1, 10, -3, 13,  0,
  3,  2, 13,  1,  3, -1,  5,  0,
  2,  1, 12,  2, 13,  2,
  3,  1, 10,  2, 12,  1, 13,  0,
  3,  1, 10, -1, 13, -2, 11,  0,
  2,  1, 12,  2, 11,  1,
  3,  2, 10,  5,  2, -5,  3,  0,
  2,  2, 10, -4, 13,  0,
  3,  2, 10,  6,  2, -6,  3,  0,
  2,  2, 12,  2, 13,  0,
  3,  2, 10, -2, 13, -2, 11,  0,
  2,  2, 12,  2, 11,  0,
  2,  2, 10, -4, 11,  0,
  3,  2, 10,  7,  2, -7,  3,  0,
  3,  2, 10, -1, 12, -4, 13,  0,
  4,  2, 10, -1, 12, -2, 13, -2, 11,  0,
  3,  2, 10,  8,  2, -8,  3,  0,
  3,  2, 10,  9,  2, -9,  3,  0,
  3,  4, 10, -3, 12, -1, 13,  0,
  3,  6, 10, -1, 12, -3, 13,  0,
  3,  4, 10, -2, 12, -1, 13,  1,
  3,  5, 10, -1, 12, -2, 13,  0,
  2,  6, 10, -3, 13,  0,
  4,  4, 10, -1, 12,  1, 13, -2, 11,  0,
  3,  2, 10, -3, 12,  1, 13,  0,
  2,  3, 10, -2, 12,  0,
  3,  4, 10, -1, 12, -1, 13,  1,
  2,  5, 10, -2, 13,  0,
  3,  6, 10,  1, 12, -3, 13,  0,
  3,  4, 10,  1, 13, -2, 11,  0,
  3,  2, 10, -2, 12,  1, 13,  1,
  2,  3, 10, -1, 12,  0,
  4, -2, 10, -1, 13,  2,  3, -2,  5,  0,
  2,  4, 10, -1, 13,  0,
  4,  2, 10, -2, 12, -1, 13,  2, 11,  0,
  3,  4, 10, -3, 13,  2, 11,  0,
  4, -2, 10, -1, 13,  2,  2, -2,  3,  0,
  3,  2, 10, -1, 12,  1, 13,  2,
  4, -2, 10, -1, 13,  1,  3, -1,  5,  0,
  1,  3, 10,  0,
  3,  4, 10,  1, 12, -1, 13,  1,
  4,  2, 10, -1, 12, -1, 13,  2, 11,  1,
  4, -2, 10, -1, 13,  1,  2, -1,  3,  0,
  3,  2, 10,  3, 13, -2, 11,  0,
  2,  2, 12, -3, 13,  0,
  3,  1, 10, -1, 12,  2, 13,  0,
  4,  2, 10,  1, 13, -1, 11,  1, 14,  0,
  4, -2, 10, -2, 13, 18,  2,-16,  3,  0,
  5,  2, 10,  1, 13,  4,  3, -8,  4,  3,  5,  0,
  2,  2, 10,  1, 13,  1,
  5, -2, 10, -1, 13,  4,  3, -8,  4,  3,  5,  0,
  3,  2, 10, 18,  2,-16,  3,  0,
  4, -2, 10, -1, 13, -1, 11,  1, 14,  0,
  4,  4, 10, -1, 13,  2,  3, -2,  5,  0,
  4,  4, 10, -1, 13,  3,  2, -3,  3,  0,
  2,  3, 10,  1, 12,  1,
  3,  4, 10,  2, 12, -1, 13,  0,
  4,  2, 10, -1, 13,  1, 11,  1, 14,  0,
  3,  2, 10, -1, 13,  2, 11,  0,
  2,  1, 12, -3, 13,  1,
  2,  1, 10,  2, 13,  0,
  3,  2, 10,  1, 12,  1, 13,  1,
  3,  1, 12, -1, 13, -2, 11,  1,
  2,  1, 10,  2, 11,  0,
  4,  2, 10,  1, 12, -1, 13,  2, 11,  0,
  1,  3, 13,  0,
  4,  2, 10,  1, 13,  2,  3, -2,  5,  0,
  3,  1, 10,  1, 12,  2, 13,  0,
  3,  2, 10,  2, 12,  1, 13,  0,
  3,  1, 13,  1, 11,  1, 14,  0,
  2,  1, 13,  2, 11,  0,
  3,  1, 10,  1, 12,  2, 11,  0,
  4,  2, 10,  2, 12, -1, 13,  2, 11,  0,
  2,  1, 13, -4, 11,  0,
  2,  1, 10, -4, 13,  0,
  2,  1, 12,  3, 13,  1,
  3,  1, 12,  1, 13,  2, 11,  1,
  2,  2, 10, -5, 13,  0,
  3,  2, 10, -3, 13, -2, 11,  0,
  3,  2, 10, -1, 13, -4, 11,  0,
  3,  6, 10, -2, 12, -2, 13,  0,
  2,  4, 10, -3, 12,  0,
  3,  6, 10, -1, 12, -2, 13,  0,
  2,  4, 10, -2, 12,  1,
  2,  6, 10, -2, 13,  0,
  2,  4, 10, -1, 12,  1,
  2,  5, 10, -1, 13,  0,
  3,  6, 10,  1, 12, -2, 13,  0,
  4,  4, 10, -1, 12, -2, 13,  2, 11,  0,
  3,  4, 10,  2, 13, -2, 11,  0,
  3,  2, 10, -2, 12,  2, 13,  0,
  1,  4, 10,  0,
  3,  2, 10, -2, 12,  2, 11,  0,
  3,  4, 10, -2, 13,  2, 11,  0,
  3,  2, 10, -1, 12,  2, 13,  1,
  2,  3, 10,  1, 13,  0,
  2,  4, 10,  1, 12,  1,
  3,  2, 10, -1, 12,  2, 11,  1,
  3,  3, 10, -1, 13,  2, 11,  0,
  2,  2, 10,  2, 13,  0,
  3,  3, 10,  1, 12,  1, 13,  0,
  3,  2, 10,  1, 11,  1, 14,  0,
  2,  2, 10,  2, 11,  0,
  2,  1, 12, -4, 13,  0,
  2,  1, 10,  3, 13,  0,
  3,  2, 10,  1, 12,  2, 13,  1,
  3,  1, 12, -2, 13, -2, 11,  0,
  3,  1, 10,  1, 13,  2, 11,  0,
  3,  2, 10,  1, 12,  2, 11,  0,
  1,  4, 13,  0,
  3,  1, 10,  1, 12,  3, 13,  0,
  2,  2, 13,  2, 11,  0,
  4,  1, 10,  1, 12,  1, 13,  2, 11,  0,
  1,  4, 11,  0,
  2,  1, 12,  4, 13,  0,
  3,  1, 12,  2, 13,  2, 11,  0,
  3,  2, 10, -4, 13, -2, 11,  0,
  3,  6, 10, -2, 12, -1, 13,  0,
  2,  8, 10, -3, 13,  0,
  3,  6, 10, -1, 12, -1, 13,  0,
  3,  4, 10, -2, 12,  1, 13,  0,
  2,  6, 10, -1, 13,  0,
  3,  4, 10, -1, 12,  1, 13,  1,
  3,  6, 10,  1, 12, -1, 13,  0,
  4,  4, 10, -1, 12, -1, 13,  2, 11,  0,
  3,  2, 10, -2, 12,  3, 13,  0,
  2,  4, 10,  1, 13,  0,
  3,  4, 10, -1, 13,  2, 11,  0,
  3,  2, 10, -1, 12,  3, 13,  0,
  3,  4, 10,  1, 12,  1, 13,  0,
  4,  2, 10, -1, 12,  1, 13,  2, 11,  0,
  2,  2, 10,  3, 13,  0,
  3,  2, 10,  1, 13,  2, 11,  0,
  3,  2, 10, -1, 13,  4, 11,  0,
  3,  2, 10,  1, 12,  3, 13,  0,
  3,  1, 12, -3, 13, -2, 11,  0,
  3,  1, 10,  2, 13,  2, 11,  0,
  4,  2, 10,  1, 12,  1, 13,  2, 11,  0,
  1,  5, 13,  0,
  2,  3, 13,  2, 11,  0,
  2,  1, 13,  4, 11,  0,
  3,  1, 12,  3, 13,  2, 11,  0,
  2,  8, 10, -2, 13,  0,
  2,  6, 10, -1, 12,  0,
  1,  6, 10,  0,
  3,  6, 10, -2, 13,  2, 11,  0,
  3,  4, 10, -1, 12,  2, 13,  0,
  3,  4, 10, -1, 12,  2, 11,  0,
  2,  4, 10,  2, 13,  0,
  2,  4, 10,  2, 11,  0,
  3,  2, 10, -1, 12,  4, 13,  0,
  3,  4, 10,  1, 12,  2, 13,  0,
  4,  2, 10, -1, 12,  2, 13,  2, 11,  0,
  2,  2, 10,  4, 13,  0,
  3,  2, 10,  2, 13,  2, 11,  0,
  2,  2, 10,  4, 11,  0,
  1,  6, 13,  0,
  2,  4, 13,  2, 11,  0,
  2,  2, 13,  4, 11,  0,
  3,  6, 10, -1, 12,  1, 13,  0,
  2,  6, 10,  1, 13,  0,
  2,  4, 10,  3, 13,  0,
  3,  4, 10,  1, 13,  2, 11,  0,
  2,  2, 10,  5, 13,  0,
  3,  2, 10,  3, 13,  2, 11,  0,
 -1
};

static long btabr[] = {-1};
static long btabb[] = {-1};
static long btabl[] = {
        -3,        -4,
         4,     -1856,         0,      8043,
        -9,     -1082,
        -1,      -310,
        -1,      -522,
      -330,     -1449,      -853,      4656,
       -66,         7,
        -1,   9996928,
       -66,         6,
        23,       183,
         0,       173,
         0,       -56,
         0,        50,
         0,      -785,
         1,        51,
         0,       -60,
         1,     11843,         0,    -50754,
         0,      1834,         1,     -7910,
         0,    -48060,
         1,        56,
         0,     13141,        -1,    -56318,
         0,      2541,
        -1,      -649,
      -133,       778,
       -46,         8,
         1,   1665737,
       -47,         7,
         0,        65,
         0,        45,
         0,      -138,
         0,     -1005,
         0,     -2911,
         0,       -47,
         0,        96,
         0,      -394,
         2,        76,
         2,    -17302,         0,     74337,
         0,      -101,
         0,        58,
         0,      -171,
         0,       -77,
         0,     -1283,         0,      2686,
         0,       -55,
         0,        99,
         0,        55,
         0,       397,
         0,       540,
         0,       626,
        -1,     -5188,         0,     10857,
         0,      -216,
        -2,      -123,
         0,      6337,
         2,       224,
      -152,    -23472,       -29,    -74336,         0,    295775,
       -20,       149,
        -2,        84,
         9,       304,
         0,     -3051,
       -70,        -6,
       -57,        34,
         0,      -638,
         0,      -201,
       -73,         9,
         0,      -100,
      -101,        -8,
         0,       -57,
         0,      -207,
        -3,        80,
       -45,        45,
        -5,       102,
       -59,       -23,
        52,       201,
       -48,       233,      -220,        71,
         4,      2810,         0,   6236541,
       -61,       218,      -216,        67,
        51,       201,
       -59,       -23,
      -144,      -837,      -457,      3029,
       -45,        42,
       -15,        73,
        -6,      -169,
         0,       135,
       -64,        -7,
         0,    -16245,
         0,       -81,
       -74,       -10,
         0,       702,         0,     -3013,
         0,     -5889,
         1,       141,
        58,      9598,        12,     30443,         1,   -120946,
        -1,       -84,
        -2,     11246,        -1,    -48391,
         0,      1393,
         0,       200,
      -136,       -17,
         0,       558,
       -64,        -8,
         0,       -71,
         0,    317577,
       -28,       183,
         1,       219,
         0,       421,
         0,      -133,
       501,      -139,
         3,       354,
      -101,       -13,
        74,         7,
       144,       -84,
        59,        -2,
         1,        64,
     -2931,     12559,     -4641,      2638,      -303,     -2058,
       -13,      -100,      -123,       -79,
    -19214,      6084,      1494,     26993,     15213,    -82219,
        42,        52,        48,      -101,
       -53,        -4,
         4,        47,
        58,      -131,
        46,        14,
       -21,        -6,
     -1311,     -8791,     10198,     -4185,      2815,      5640,
       167,       422,      -229,        83,
      3140,        39,      1221,       120,        96,       -30,
        -1, 184612405,
       187,       416,      -226,        81,
     -1985,    -10083,      9983,     -4464,      2807,      5643,
       -21,        -9,
       113,      -367,
       120,       580,      -667,        27,
         8,        66,
       -56,        -6,
       337,        95,
       -87,      3303,
        -1,        65,
        68,      -374,
         0,      -574,
        15,       -94,
         0,       -53,
         0,     -1303,
         0,      -236,
       283,        36,
        -1,       -54,
       269,       -35,
         0,       -83,
         0,       -52,
         0,       730,         0,     -3129,
         0,       813,
         0,     -4299,
         1,        59,
        -6,      5130,         1,     16239,        -1,    -64603,
         0,       -80,
        91,        12,
         0,      -561,
       133,       -17,
         0,       250,
       -12,        71,
         0,    155664,
        82,       -11,
         0,       106,
         0,      -604,
         0,     21862,
        55,        -7,
         0,     -1514,         0,      6501,
         0,       906,
         0,       -68,
         0,       241,
         0,       366,
         0,        70,
         0,     -1382,         0,      5957,
         0,       113,
         0,       -51,
         0,       -55,
         0,       731,
         0,      -264,
         0,     65788,
         1,     -1504,         0,      3147,
         0,       217,
         0,     -4105,         0,     17658,
         1,        69,
         0,     -3518,
         0,     -1767,
       -43,     -7044,       -10,    -22304,         0,     88685,
         3,        91,
         0,      -485,
         0,       -57,
        -1,    333548,
       -24,       172,
        11,       544,         1,     -1132,
         0,       353,
         0,      -188,
         0,        53,
         0,        77,
       158,      -887,
        35,       131,
       -54,        13,
         0,   1994821,
       -53,        14,
        36,       125,
         2,        56,
         0,      -243,
         0,      -364,
        -2,      1916,         0,     -8227,
         0,     15700,        -1,    -67308,
         1,        66,
         0,    -53686,
         1,      3058,         1,    -13177,
         0,       -72,
         0,       -72,
         0,        61,
         0,     15812,
         0,       165,
         8,       -96,
       318,      1341,       803,     -4252,
        24,       193,
      1137,      -226,       310,       622,
       -56,        30,
        -3,  10101666,
       -56,        30,
      1096,      -225,       300,       600,
       -31,       409,
        -1,      -507,
         0,      -287,
         0,     -1869,         0,      8026,
         1,       544,        -1,     -1133,
         0,     27984,
         0,       -62,
         0,      -249,
         0,       187,
         0,     -1096,
         1,        53,
         2,     12388,         0,    -53107,
         0,      -322,
         0,       -94,
         0,     15157,
         0,      -582,
         0,      3291,
         0,       565,
         0,       106,
         0,       112,
         0,       306,
         0,       809,
         0,       130,
         0,      -961,         0,      4149,
         0,       174,
         0,      -105,
         0,      2196,
         0,        59,
         0,     36737,
        -1,     -1832,         0,      3835,
         0,      -139,
         0,     24138,
         0,      1325,
         1,        64,
         0,      -361,
         0,     -1162,
       -44,     -6320,       -10,    -20003,         0,     79588,
         2,        80,
         0,     -2059,
         0,      -304,
         0,     21460,
         0,      -166,
         0,       -87,
        89,      -493,
        32,       114,
        34,       510,         1,   1172616,
        31,       113,
        -1,        57,
         0,       214,
         0,      -656,
         0,      -646,
         0,      1850,         0,     -7931,
         0,     -6674,
         0,      2944,         0,    -12641,
         0,       916,
        45,      -255,
        16,        60,
        -1,    619116,
        16,        57,
         0,       -58,
         0,      1045,
         0,      -156,
       -15,        88,
         0,    -62964,
         0,      -126,
         0,      1490,         0,     -6387,
         0,       119,
         0,      1338,
         0,       -56,
         0,       204,
         0,       153,
         0,       940,
         0,       251,
         0,       312,
         0,       584,
         0,      -786,         0,      3388,
         0,       -52,
         0,      4733,
         0,       618,
         0,     29982,
         0,       101,
         0,      -174,
         0,     -2637,         0,     11345,
         0,      -284,
         0,      -524,
         0,      -121,
         0,      1464,
        11,       -60,
        -1,    151205,
         0,       139,
         0,     -2448,
         0,       -51,
         0,      -768,
         0,      -638,
         0,       552,         0,     -2370,
         0,        70,
         0,        64,
         0,        57,
         0,     39840,
         0,       104,
         0,    -10194,
         0,      -635,
         0,        69,
         0,       113,
         0,        67,
         0,        96,
         0,       367,
         0,       134,
         0,       596,
         0,        63,
         0,      1622,
         0,       483,
         0,        72,
         0,     11917,
         0,       -63,
         0,      1273,
         0,       -66,
         0,      -262,
         0,       -97,
         0,       103,
         0,     15196,
         0,     -1445,
         0,       -66,
         0,       -55,
         0,      -323,
         0,      2632,
         0,     -1179,
         0,        59,
         0,       -56,
         0,        78,
         0,        65,
         0,       422,
         0,       309,
         0,      2125,
         0,       -66,
         0,       124,
         0,       -57,
         0,      1379,
         0,      -304,
         0,       177,
         0,      -118,
         0,       146,
         0,       283,
         0,       119,
};
static CHAR bargs[] = {
  0,  1,
  3,  1, 10,  1, 12, -1, 11,  1,
  4,  2, 10,  2, 12, -1, 13, -1, 11,  0,
  5,  2, 10, -1, 13, -1, 11,  3,  2, -3,  3,  0,
  5,  2, 10, -1, 13, -1, 11,  2,  3, -2,  5,  0,
  2, -1, 13,  1, 14,  1,
  5, -1, 13,  1, 11,  4,  3, -8,  4,  3,  5,  0,
  2,  1, 13, -1, 11,  0,
  5,  1, 13, -1, 11,  4,  3, -8,  4,  3,  5,  0,
  5,  2, 10, -1, 13, -1, 11,  2,  3, -3,  5,  0,
  4,  1, 10,  1, 12, -2, 13,  1, 11,  0,
  4,  1, 13, -1, 11,  1,  2, -1,  3,  0,
  5,  2, 10, -1, 13, -1, 11,  2,  2, -2,  3,  0,
  3,  1, 10, -2, 13,  1, 11,  0,
  4,  1, 13, -1, 11,  1,  3, -1,  5,  0,
  4, -1, 13,  1, 11,  1,  2, -1,  3,  0,
  3,  1, 12,  1, 13, -1, 11,  1,
  4,  2, 10,  1, 12, -1, 13, -1, 11,  1,
  2,  1, 10, -1, 11,  0,
  4, -1, 13,  1, 11,  1,  3, -1,  5,  0,
  3,  1, 12, -1, 13,  1, 11,  1,
  3,  2, 10, -3, 13,  1, 11,  0,
  3,  2, 12,  1, 13, -1, 11,  0,
  3, -2, 10,  1, 13,  1, 14,  0,
  6, -2, 10,  1, 13,  1, 11,  4,  3, -8,  4,  3,  5,  0,
  3,  2, 10, -1, 13, -1, 11,  0,
  6,  2, 10, -1, 13, -1, 11,  4,  3, -8,  4,  3,  5,  0,
  4, -1, 13,  1, 11,  2,  3, -2,  5,  0,
  4, -1, 13,  1, 11,  3,  2, -3,  3,  0,
  3,  1, 10, -1, 12, -1, 11,  0,
  3,  2, 12, -1, 13,  1, 11,  0,
  3,  2, 10,  1, 13, -3, 11,  0,
  5, -2, 10,  1, 13,  1, 11,  1,  2, -1,  3,  0,
  4,  2, 10, -1, 12, -3, 13,  1, 11,  0,
  3,  3, 10, -2, 13, -1, 11,  0,
  5, -2, 10,  1, 13,  1, 11,  1,  3, -1,  5,  0,
  4,  2, 10, -1, 12, -1, 13, -1, 11,  1,
  2,  3, 10, -3, 11,  0,
  5, -2, 10,  1, 13,  1, 11,  2,  2, -2,  3,  0,
  4,  2, 10, -1, 12,  1, 13, -3, 11,  0,
  3,  4, 10, -3, 13, -1, 11,  0,
  4,  2, 10, -2, 12, -1, 13, -1, 11,  1,
  3,  4, 10, -1, 13, -3, 11,  0,
  4,  2, 10, -3, 12, -1, 13, -1, 11,  0,
  3,  4, 10, -1, 12, -3, 11,  0,
  3,  2, 10, -3, 12, -1, 11,  0,
  4,  4, 10, -1, 12, -2, 13, -1, 11,  0,
  2,  4, 10, -3, 11,  0,
  3,  2, 10, -2, 12, -1, 11,  1,
  4,  3, 10, -1, 12, -1, 13, -1, 11,  0,
  4, -2, 10,  1, 11,  2,  3, -2,  5,  0,
  3,  4, 10, -2, 13, -1, 11,  0,
  4, -2, 10,  1, 11,  2,  2, -2,  3,  0,
  3,  2, 10, -1, 12, -1, 11,  2,
  3, -2, 10,  1, 12,  1, 14,  0,
  4, -2, 10,  1, 11,  2,  3, -2,  4,  0,
  4, -2, 10,  1, 11,  1,  3, -1,  5,  0,
  3,  3, 10, -1, 13, -1, 11,  0,
  4, -2, 10,  1, 11,  3,  2, -4,  3,  0,
  4, -2, 10,  1, 11,  1,  3, -2,  5,  0,
  4,  2, 10, -1, 12, -2, 13,  1, 11,  0,
  4, -2, 10,  1, 11,  1,  2, -1,  3,  0,
  2, -1, 10,  1,  2,  0,
  3,  2, 10,  2, 13, -3, 11,  0,
  4, -2, 10,  1, 11,  2,  2, -3,  3,  0,
  3,  2, 12, -2, 13,  1, 11,  0,
  4,  1, 10, -1, 12,  1, 13, -1, 11,  0,
  3, -2, 10,  1, 11,  1,  5,  0,
  4,  2, 10, -1, 11,  1,  3, -2,  4,  0,
  3,  2, 10, -2, 11,  1, 14,  0,
  4, -2, 10,  1, 11,  8,  2,-13,  3,  0,
  5, -2, 10, -1, 13,  1, 11, 18,  2,-16,  3,  0,
  5,  2, 10, -1, 11,  4,  3, -8,  4,  3,  5,  1,
  2,  2, 10, -1, 11,  1,
  5, -2, 10,  1, 11,  4,  3, -8,  4,  3,  5,  1,
  5,  2, 10, -1, 13, -1, 11, 18,  2,-16,  3,  0,
  4,  2, 10, -1, 11,  8,  2,-13,  3,  0,
  2, -2, 10,  1, 14,  1,
  4, -2, 10,  1, 11,  1,  3, -2,  4,  0,
  3,  2, 10, -1, 11,  1,  5,  0,
  2,  2, 12, -1, 11,  0,
  4,  3, 10,  1, 12, -1, 13, -1, 11,  0,
  4,  2, 10, -1, 11,  2,  2, -3,  3,  0,
  3,  2, 10, -2, 13,  1, 11,  0,
  4,  2, 10, -1, 11,  1,  2, -1,  3,  0,
  3,  1, 10,  1,  2, -2,  3,  0,
  3,  1, 12, -2, 13,  1, 11,  1,
  3,  1, 10,  1, 13, -1, 11,  0,
  4,  2, 10, -1, 11,  1,  3, -1,  5,  0,
  3,  2, 10,  1, 12, -1, 11,  2,
  3, -2, 10, -1, 12,  1, 14,  0,
  2,  1, 12, -1, 11,  1,
  3,  1, 10, -1, 13,  1, 11,  0,
  4,  2, 10, -1, 11,  2,  2, -2,  3,  0,
  3,  1, 10,  2,  2, -3,  3,  0,
  4,  2, 10,  1, 12, -2, 13,  1, 11,  0,
  3, -1, 10,  1,  2, -2,  3,  0,
  3, -1, 11,  1,  2, -1,  3,  0,
  2,  2, 13, -1, 11,  0,
  2, -2, 13,  1, 14,  0,
  4,  2, 10, -1, 11,  2,  3, -2,  5,  0,
  4,  2, 10, -1, 11,  3,  2, -3,  3,  0,
  4,  2, 10,  2, 12, -2, 13, -1, 11,  0,
  3,  1, 10,  1,  3, -2,  5,  0,
  4,  1, 10,  1, 12,  1, 13, -1, 11,  0,
  3,  1, 10,  3,  2, -4,  3,  0,
  3,  1, 10,  1,  3, -1,  5,  0,
  3,  1, 10,  1,  3, -2,  6,  0,
  3,  1, 10,  2,  3, -2,  4,  0,
  4,  1, 10,  1, 12, -1, 13, -1, 11,  0,
  3,  2, 10,  2, 12, -1, 11,  2,
  4,  1, 10,  1,  3,  2,  5, -5,  6,  1,
  1,  1, 14,  2,
  3,  1, 10,  8,  2,-12,  3,  1,
  5, -2, 10,  1, 13, -1, 11, 20,  2,-21,  3,  0,
  5,  2, 10, -2, 13,  1, 11,  2,  3, -3,  5,  0,
  3,  1, 10,  1,  3,  1,  6,  0,
  4, -1, 13, -1, 11, 26,  2,-29,  3,  0,
  3, -1, 11,  8,  2,-13,  3,  0,
  4, -1, 13, -1, 11, 18,  2,-16,  3,  2,
  4, -1, 13,  1, 11, 10,  2, -3,  3,  1,
  1,  1, 11,  3,
  4, -1, 13, -1, 11, 10,  2, -3,  3,  1,
  4, -1, 13,  1, 11, 18,  2,-16,  3,  2,
  3,  1, 11,  8,  2,-13,  3,  0,
  2,  1, 10,  2,  4,  0,
  4,  2, 10, -1, 11,  5,  2, -6,  3,  1,
  5,  2, 10, -2, 13, -1, 11,  2,  3, -3,  5,  0,
  5, -2, 10,  1, 13,  1, 11, 20,  2,-21,  3,  0,
  3,  1, 10,  1,  3,  1,  5,  0,
  2, -2, 11,  1, 14,  0,
  5,  2, 10, -2, 13,  1, 11,  2,  3, -2,  5,  0,
  3,  1, 10,  5,  2, -7,  3,  0,
  4,  1, 10,  1, 12, -1, 13,  1, 11,  0,
  3,  1, 10,  2,  2, -2,  3,  0,
  4,  2, 10,  2, 12, -2, 13,  1, 11,  0,
  2,  2, 13, -3, 11,  0,
  4,  2, 10, -1, 11,  4,  2, -4,  3,  0,
  3,  1, 10,  4,  2, -5,  3,  0,
  3,  1, 10, -3, 13,  1, 11,  0,
  2,  1, 10,  1,  2,  0,
  3,  1, 11,  1,  2, -1,  3,  0,
  4,  2, 10, -1, 11,  3,  3, -3,  5,  0,
  3,  1, 12,  2, 13, -1, 11,  1,
  4,  2, 10,  1, 12, -2, 13, -1, 11,  0,
  3,  1, 10, -1, 13, -1, 11,  0,
  3,  1, 11,  1,  3, -1,  5,  0,
  2,  1, 12,  1, 11,  2,
  4,  2, 10, -1, 11,  5,  2, -5,  3,  0,
  3,  1, 10,  5,  2, -6,  3,  0,
  3,  2, 10,  1, 12, -3, 11,  0,
  3,  1, 10,  2,  2, -1,  3,  0,
  3,  2, 10, -4, 13,  1, 11,  0,
  3, -2, 10,  2, 13,  1, 14,  0,
  3,  2, 10, -2, 13, -1, 11,  0,
  3,  1, 10,  3,  2, -2,  3,  0,
  4,  1, 10, -1, 12, -1, 13, -1, 11,  0,
  2,  2, 12,  1, 11,  0,
  2,  2, 10, -3, 11,  0,
  3,  1, 10,  4,  2, -3,  3,  0,
  4,  2, 10, -1, 12, -2, 13, -1, 11,  1,
  3,  2, 10, -1, 12, -3, 11,  0,
  3,  4, 10, -4, 13, -1, 11,  0,
  4,  2, 10, -2, 12, -2, 13, -1, 11,  0,
  4,  4, 10, -2, 12, -1, 13, -1, 11,  0,
  3,  6, 10, -3, 13, -1, 11,  0,
  4,  4, 10, -1, 12, -1, 13, -1, 11,  1,
  4,  2, 10, -3, 12, -1, 13,  1, 11,  0,
  3,  5, 10, -2, 13, -1, 11,  0,
  3,  4, 10,  1, 13, -3, 11,  0,
  4,  2, 10, -2, 12,  1, 13, -1, 11,  0,
  3,  3, 10, -1, 12, -1, 11,  0,
  3,  4, 10, -1, 13, -1, 11,  0,
  4,  2, 10, -2, 12, -1, 13,  1, 11,  1,
  3,  4, 10, -3, 13,  1, 11,  0,
  4,  2, 10, -1, 12,  1, 13, -1, 11,  1,
  5, -2, 10,  1, 13, -1, 11,  2,  2, -2,  3,  0,
  2,  3, 10, -1, 11,  0,
  4,  4, 10,  1, 12, -1, 13, -1, 11,  0,
  4,  2, 10, -1, 12, -1, 13,  1, 11,  2,
  5, -2, 10,  1, 13, -1, 11,  1,  3, -1,  5,  0,
  3,  3, 10, -2, 13,  1, 11,  0,
  5, -2, 10,  1, 13, -1, 11,  1,  2, -1,  3,  0,
  3,  2, 10,  1, 13, -1, 11,  0,
  3, -2, 10, -1, 13,  1, 14,  0,
  3,  2, 12, -1, 13, -1, 11,  1,
  3,  3, 10,  1, 12, -1, 11,  0,
  3,  1, 10, -1, 12,  1, 11,  0,
  4, -1, 13, -1, 11,  3,  2, -3,  3,  0,
  4, -1, 13, -1, 11,  2,  3, -2,  5,  0,
  3,  2, 10, -1, 13,  1, 14,  0,
  4, -2, 10, -1, 11, 18,  2,-16,  3,  0,
  6,  2, 10, -1, 13,  1, 11,  4,  3, -8,  4,  3,  5,  0,
  3,  2, 10, -1, 13,  1, 11,  0,
  6, -2, 10,  1, 13, -1, 11,  4,  3, -8,  4,  3,  5,  0,
  5,  2, 10, -2, 13,  1, 11, 18,  2,-16,  3,  0,
  4, -2, 10,  1, 13, -2, 11,  1, 14,  0,
  3,  1, 12, -3, 13,  1, 11,  0,
  3,  1, 10,  2, 13, -1, 11,  0,
  4,  2, 10,  1, 12,  1, 13, -1, 11,  1,
  3,  1, 12, -1, 13, -1, 11,  1,
  4, -1, 13, -1, 11,  1,  3, -1,  5,  0,
  2,  1, 10,  1, 11,  0,
  4,  2, 10,  1, 12, -1, 13,  1, 11,  1,
  3,  1, 12,  1, 13, -3, 11,  0,
  4, -1, 13, -1, 11,  1,  2, -1,  3,  0,
  5,  2, 10, -1, 13,  1, 11,  2,  2, -2,  3,  0,
  2,  3, 13, -1, 11,  0,
  4,  1, 10,  1, 12, -2, 13, -1, 11,  0,
  4,  2, 10,  2, 12,  1, 13, -1, 11,  0,
  2,  1, 13,  1, 14,  1,
  5,  2, 10, -1, 13,  1, 11,  2,  3, -3,  5,  0,
  4, -2, 13, -1, 11, 18,  2,-16,  3,  1,
  5,  1, 13,  1, 11,  4,  3, -8,  4,  3,  5,  0,
  2,  1, 13,  1, 11,  0,
  5, -1, 13, -1, 11,  4,  3, -8,  4,  3,  5,  0,
  3,  1, 11, 18,  2,-16,  3,  1,
  3, -1, 13, -2, 11,  1, 14,  0,
  5,  2, 10, -1, 13,  1, 11,  2,  3, -2,  5,  0,
  5,  2, 10, -1, 13,  1, 11,  3,  2, -3,  3,  0,
  3,  1, 10,  1, 12,  1, 11,  1,
  4,  2, 10,  2, 12, -1, 13,  1, 11,  1,
  2,  1, 13, -3, 11,  0,
  4,  1, 13,  1, 11,  1,  2, -1,  3,  0,
  3,  1, 12,  3, 13, -1, 11,  0,
  4,  2, 10,  1, 12, -3, 13, -1, 11,  0,
  3,  1, 10, -2, 13, -1, 11,  0,
  4,  1, 13,  1, 11,  1,  3, -1,  5,  0,
  3,  1, 12,  1, 13,  1, 11,  1,
  2,  1, 10, -3, 11,  0,
  3,  1, 12, -1, 13,  3, 11,  0,
  3,  2, 10, -3, 13, -1, 11,  0,
  3,  2, 12,  1, 13,  1, 11,  0,
  3,  2, 10, -1, 13, -3, 11,  0,
  4,  2, 10, -1, 12, -3, 13, -1, 11,  0,
  4,  2, 10, -1, 12, -1, 13, -3, 11,  0,
  4,  6, 10, -1, 12, -2, 13, -1, 11,  0,
  3,  4, 10, -2, 12, -1, 11,  0,
  3,  6, 10, -2, 13, -1, 11,  0,
  4,  4, 10, -2, 12, -2, 13,  1, 11,  0,
  3,  4, 10, -1, 12, -1, 11,  1,
  3,  2, 10, -3, 12,  1, 11,  0,
  3,  5, 10, -1, 13, -1, 11,  0,
  4,  4, 10, -1, 12, -2, 13,  1, 11,  0,
  4,  2, 10, -2, 12,  2, 13, -1, 11,  0,
  2,  4, 10, -1, 11,  0,
  3,  2, 10, -2, 12,  1, 11,  1,
  4,  3, 10, -1, 12, -1, 13,  1, 11,  0,
  3,  4, 10, -2, 13,  1, 11,  0,
  4,  2, 10, -1, 12,  2, 13, -1, 11,  0,
  4, -2, 10, -1, 11,  2,  2, -2,  3,  0,
  3,  3, 10,  1, 13, -1, 11,  0,
  3,  4, 10,  1, 12, -1, 11,  0,
  3,  2, 10, -1, 12,  1, 11,  2,
  4, -2, 10, -1, 11,  1,  3, -1,  5,  0,
  3,  3, 10, -1, 13,  1, 11,  0,
  4,  4, 10,  1, 12, -2, 13,  1, 11,  0,
  3,  2, 10,  2, 13, -1, 11,  0,
  3,  2, 12, -2, 13, -1, 11,  0,
  4,  1, 10, -1, 12,  1, 13,  1, 11,  0,
  2,  2, 10,  1, 14,  0,
  5, -2, 10, -1, 13, -1, 11, 18,  2,-16,  3,  0,
  2,  2, 10,  1, 11,  1,
  5,  2, 10, -1, 13,  1, 11, 18,  2,-16,  3,  0,
  3, -2, 10, -2, 11,  1, 14,  0,
  4,  3, 10,  1, 12, -1, 13,  1, 11,  0,
  3,  2, 10, -2, 13,  3, 11,  0,
  4,  2, 10,  1, 12,  2, 13, -1, 11,  0,
  3,  1, 12, -2, 13, -1, 11,  1,
  3,  1, 10,  1, 13,  1, 11,  0,
  3,  2, 10,  1, 12,  1, 11,  1,
  2,  4, 13, -1, 11,  0,
  2,  2, 13,  1, 14,  0,
  4, -3, 13, -1, 11, 18,  2,-16,  3,  0,
  2,  2, 13,  1, 11,  0,
  4,  1, 13,  1, 11, 18,  2,-16,  3,  0,
  4,  2, 10,  1, 11,  2,  3, -2,  5,  0,
  4,  1, 10,  1, 12,  1, 13,  1, 11,  0,
  3,  2, 10,  2, 12,  1, 11,  0,
  2,  2, 11,  1, 14,  0,
  1,  3, 11,  0,
  3,  1, 10, -3, 13, -1, 11,  0,
  3,  1, 12,  2, 13,  1, 11,  1,
  2,  1, 12,  3, 11,  0,
  3,  2, 10, -4, 13, -1, 11,  0,
  3,  2, 12,  2, 13,  1, 11,  0,
  3,  2, 10, -2, 13, -3, 11,  0,
  4,  6, 10, -1, 12, -1, 13, -1, 11,  0,
  3,  6, 10, -1, 13, -1, 11,  0,
  4,  4, 10, -2, 12, -1, 13,  1, 11,  0,
  3,  6, 10, -3, 13,  1, 11,  0,
  4,  4, 10, -1, 12,  1, 13, -1, 11,  0,
  4,  4, 10, -1, 12, -1, 13,  1, 11,  1,
  3,  5, 10, -2, 13,  1, 11,  0,
  3,  4, 10,  1, 13, -1, 11,  0,
  4,  2, 10, -2, 12,  1, 13,  1, 11,  0,
  3,  4, 10, -1, 13,  1, 11,  0,
  4,  2, 10, -1, 12,  3, 13, -1, 11,  0,
  4,  4, 10,  1, 12,  1, 13, -1, 11,  0,
  4,  2, 10, -1, 12,  1, 13,  1, 11,  1,
  2,  3, 10,  1, 11,  0,
  4,  4, 10,  1, 12, -1, 13,  1, 11,  0,
  4,  2, 10, -1, 12, -1, 13,  3, 11,  0,
  3,  2, 10,  3, 13, -1, 11,  0,
  3,  2, 10,  1, 13,  1, 14,  0,
  3,  2, 10,  1, 13,  1, 11,  0,
  3,  3, 10,  1, 12,  1, 11,  0,
  3,  2, 10, -1, 13,  3, 11,  0,
  4,  2, 10,  1, 12,  3, 13, -1, 11,  0,
  3,  1, 12, -3, 13, -1, 11,  0,
  3,  1, 10,  2, 13,  1, 11,  0,
  4,  2, 10,  1, 12,  1, 13,  1, 11,  1,
  3,  1, 12, -1, 13, -3, 11,  0,
  2,  1, 10,  3, 11,  0,
  2,  5, 13, -1, 11,  0,
  2,  3, 13,  1, 11,  0,
  4,  1, 10,  1, 12,  2, 13,  1, 11,  0,
  2,  1, 13,  3, 11,  0,
  3,  1, 12,  3, 13,  1, 11,  0,
  3,  1, 12,  1, 13,  3, 11,  0,
  3,  2, 10, -5, 13, -1, 11,  0,
  3,  6, 10, -1, 12, -1, 11,  0,
  4,  6, 10, -1, 12, -2, 13,  1, 11,  0,
  2,  6, 10, -1, 11,  0,
  3,  4, 10, -2, 12,  1, 11,  0,
  3,  6, 10, -2, 13,  1, 11,  0,
  4,  4, 10, -1, 12,  2, 13, -1, 11,  0,
  3,  4, 10, -1, 12,  1, 11,  0,
  3,  4, 10,  2, 13, -1, 11,  0,
  4,  2, 10, -2, 12,  2, 13,  1, 11,  0,
  2,  4, 10,  1, 11,  0,
  3,  4, 10, -2, 13,  3, 11,  0,
  4,  2, 10, -1, 12,  2, 13,  1, 11,  0,
  3,  3, 10,  1, 13,  1, 11,  0,
  3,  4, 10,  1, 12,  1, 11,  0,
  3,  2, 10, -1, 12,  3, 11,  0,
  3,  2, 10,  4, 13, -1, 11,  0,
  3,  2, 10,  2, 13,  1, 11,  0,
  2,  2, 10,  3, 11,  0,
  3,  1, 12, -4, 13, -1, 11,  0,
  3,  1, 10,  3, 13,  1, 11,  0,
  4,  2, 10,  1, 12,  2, 13,  1, 11,  0,
  2,  4, 13,  1, 11,  0,
  2,  2, 13,  3, 11,  0,
  1,  5, 11,  0,
  3,  1, 12,  4, 13,  1, 11,  0,
  4,  6, 10, -1, 12, -1, 13,  1, 11,  0,
  3,  6, 10,  1, 13, -1, 11,  0,
  3,  6, 10, -1, 13,  1, 11,  0,
  4,  4, 10, -1, 12,  1, 13,  1, 11,  0,
  3,  4, 10,  1, 13,  1, 11,  0,
  3,  4, 10, -1, 13,  3, 11,  0,
  4,  2, 10, -1, 12,  3, 13,  1, 11,  0,
  4,  4, 10,  1, 12,  1, 13,  1, 11,  0,
  3,  2, 10,  3, 13,  1, 11,  0,
  3,  2, 10,  1, 13,  3, 11,  0,
  2,  5, 13,  1, 11,  0,
  2,  3, 13,  3, 11,  0,
  2,  6, 10,  1, 11,  0,
  3,  4, 10,  2, 13,  1, 11,  0,
  3,  2, 10,  4, 13,  1, 11,  0,
 -1
};
struct plantbl moonlr = {
  {  3, 26, 29, 23,  5, 10,  0,  0,  0,  8,  4,  4,  6,  2,  0,  0,  0,  0,},
 3,
 lrargs,
 lrtabl,
 lrtabb,
 lrtabr,
 2.5735686895300000e-03,
 3.6525000000000000e+06,
 1.0000000000000000e-04,
};

struct plantbl moonlat = {
  {  0, 26, 29,  8,  3,  5,  0,  0,  0,  6,  5,  3,  5,  1,  0,  0,  0,  0,},
 3,
 bargs,
 btabl,
 btabb,
 btabr,
 0.0000000000000000e+00,
 3.6525000000000000e+06,
 1.0000000000000000e-04,
};


/* Reduce arc seconds modulo 360 degrees
   answer in arc seconds  */
static double mods3600(double x)
{
  double y;
  y = x - 1296000. * floor( x/1296000.);
  return(y);
}


/* Time argument is Julian ephemeris date.  */

static void mean_elements (double JED)
{
  double x, T, T2;

  /* Time variables.  T is in Julian centuries.  */
  T = (JED - MOSHIER_J2000) / 36525.0;
  T2 = T*T;

  /* Mean longitudes of planets (Simon et al, 1994)
     .047" subtracted from constant term for offset to DE403 origin. */

  /* Mercury */
  x = mods3600( 538101628.6889819 * T + 908103.213 );
  x += (6.39e-6 * T
	 - 0.0192789) * T2;
  Args[0] = x;

  /* Venus */
  x = mods3600( 210664136.4335482 * T + 655127.236 );
  x += (-6.27e-6  * T
	 + 0.0059381) * T2;
  Args[1] = x;

  /* Earth  */
  x = mods3600( 129597742.283429 * T + 361679.198 );
  x += (-5.23e-6 * T
	 - 2.04411e-2 ) * T2;
  Ea_arcsec = x;
  Args[2] = x;

  /* Mars */
  x = mods3600(  68905077.493988 * T +  1279558.751 );
  x += (-1.043e-5 * T
	 + 0.0094264) * T2;
  Args[3] = x;

  /* Jupiter */
  x = mods3600( 10925660.377991 * T + 123665.420 );
  x += ((((-3.4e-10 * T
	    + 5.91e-8) * T
	   + 4.667e-6) * T
	  + 5.706e-5) * T
         - 3.060378e-1)*T2;
  Args[4] = x;

   /* Saturn */
  x = mods3600( 4399609.855372 * T + 180278.752 );
  x += (((( 8.3e-10 * T
	  - 1.452e-7) * T
	  - 1.1484e-5) * T
	   - 1.6618e-4) * T
	 + 7.561614E-1)*T2;
  Args[5] = x;

  /* Uranus */
  x = mods3600( 1542481.193933 * T + 1130597.971 )
       + (0.00002156*T - 0.0175083)*T2;
  Args[6] = x;

  /* Neptune */
  x = mods3600( 786550.320744 * T + 1095655.149 )
       + (-0.00000895*T + 0.0021103)*T2;
  Args[7] = x;

  /* Copied from cmoon.c, DE404 version.  */
  /* Mean elongation of moon = D */
  x = mods3600( 1.6029616009939659e+09 * T + 1.0722612202445078e+06 );
  x += (((((-3.207663637426e-013 * T
	       + 2.555243317839e-011) * T
	      + 2.560078201452e-009) * T
	     - 3.702060118571e-005) * T
            + 6.9492746836058421e-03) * T /* D, t^3 */
           - 6.7352202374457519e+00) * T2; /* D, t^2 */
  Args[9] = x;

  /* Mean distance of moon from its ascending node = F */
  x = mods3600( 1.7395272628437717e+09 * T + 3.3577951412884740e+05 );
  x += ((((( 4.474984866301e-013 * T
	       + 4.189032191814e-011) * T
	       - 2.790392351314e-009) * T
	      - 2.165750777942e-006) * T
	      - 7.5311878482337989e-04) * T /* F, t^3 */
	     - 1.3117809789650071e+01) * T2; /* F, t^2 */
  NF_arcsec = x;
  Args[10] = x;

/* Mean anomaly of sun = l' (J. Laskar) */
  x = mods3600(1.2959658102304320e+08 * T + 1.2871027407441526e+06);
  x += ((((((((
	       1.62e-20 * T
	       - 1.0390e-17 ) * T
	      - 3.83508e-15 ) * T
	     + 4.237343e-13 ) * T
	    + 8.8555011e-11 ) * T
	   - 4.77258489e-8 ) * T
	  - 1.1297037031e-5 ) * T
	 + 8.7473717367324703e-05) * T
	- 5.5281306421783094e-01) * T2;
  Args[11] = x;

  /* Mean anomaly of moon = l */
  x = mods3600( 1.7179159228846793e+09 * T + 4.8586817465825332e+05 );
  x += (((((-1.755312760154e-012) * T
		+ 3.452144225877e-011 * T
		- 2.506365935364e-008) * T
	       - 2.536291235258e-004) * T
              + 5.2099641302735818e-02) * T /* l, t^3 */
             + 3.1501359071894147e+01) * T2; /* l, t^2 */
  Args[12] = x;

  /* Mean longitude of moon, re mean ecliptic and equinox of date = L  */
  x = mods3600( 1.7325643720442266e+09 * T + 7.8593980921052420e+05);
  x += ((((( 7.200592540556e-014 * T
	     + 2.235210987108e-010) * T
	    - 1.024222633731e-008) * T
	   - 6.073960534117e-005) * T
	 + 6.9017248528380490e-03) * T /* L, t^3 */
	- 5.6550460027471399e+00) * T2; /* L, t^2 */
  LP_equinox = x;
  Args[13] = x;

  /* Precession of the equinox  */
 x = ((((((((( -8.66e-20*T - 4.759e-17)*T
           + 2.424e-15)*T
           + 1.3095e-12)*T
           + 1.7451e-10)*T
           - 1.8055e-8)*T
           - 0.0000235316)*T
           + 0.000076)*T
           + 1.105414)*T
           + 5028.791959)*T;
  /* Moon's longitude re fixed J2000 equinox.  */
 /*
   Args[13] -= x;
 */
   pA_precession = x;

  /*  OM = LP - NF; */

  /* Free librations.  */
  /*  LB 2.891725 years, psi amplitude 1.8" */
  Args[14] = mods3600( 4.48175409e7 * T + 8.060457e5 );

  /* 24.2 years */
  Args[15] = mods3600(  5.36486787e6 * T - 391702.8 );

#if 0
  /* 27.34907 days */
  Args[16] = mods3600( 1.7308227257e9 * T - 4.443583e5 );
#endif
  /* LA 74.7 years. */
Args[17] = mods3600( 1.73573e6 * T );
}


/* Prepare lookup table of sin and cos ( i*Lj )
 * for required multiple angles
 */
static int  sscc (int k, double arg, int n)
{
  double cu, su, cv, sv, s;
  int i;

  s = STR * arg;
  su = sin (s);
  cu = cos (s);
  ss[k][0] = su;		/* sin(L) */
  cc[k][0] = cu;		/* cos(L) */
  sv = 2.0 * su * cu;
  cv = cu * cu - su * su;
  ss[k][1] = sv;		/* sin(2L) */
  cc[k][1] = cv;
  for (i = 2; i < n; i++)
    {
      s = su * cv + cu * sv;
      cv = cu * cv - su * sv;
      sv = s;
      ss[k][i] = sv;		/* sin( i+1 L ) */
      cc[k][i] = cv;
    }
  return (0);
}

/* Generic program to accumulate sum of trigonometric series
   in two variables (e.g., longitude, radius)
   of the same list of arguments.  */
static int  g2plan (double J, struct plantbl *plan, double pobj[], int flag)
{
  int i, j, k, m, k1, ip, np, nt;
  /* On some systems such as Silicon Graphics, char is unsigned
     by default.  */
  CHAR *p;
  long *pl, *pr;
  double su, cu, sv, cv;
  double t, sl, sr;

  mean_elements (J);
  /* For librations, moon's longitude is sidereal.  */
  if (flag)
    Args[13] -= pA_precession;

  T = (J - MOSHIER_J2000) / plan->timescale;
  /* Calculate sin( i*MM ), etc. for needed multiple angles.  */
  for (i = 0; i < NARGS; i++)
    {
      if ((j = plan->max_harmonic[i]) > 0)
	{
	  sscc (i, Args[i], j);
	}
    }

  /* Point to start of table of arguments. */
  p = plan->arg_tbl;
  /* Point to tabulated cosine and sine amplitudes.  */
  pl = plan->lon_tbl;
  pr = plan->rad_tbl;
  sl = 0.0;
  sr = 0.0;

  for (;;)
    {
      /* argument of sine and cosine */
      /* Number of periodic arguments. */
      np = *p++;
      if (np < 0)
	break;
      if (np == 0)
	{			/* It is a polynomial term.  */
	  nt = *p++;
	  /* Longitude polynomial. */
	  cu = *pl++;
	  for (ip = 0; ip < nt; ip++)
	    {
	      cu = cu * T + *pl++;
	    }
	  /*	  sl +=  mods3600 (cu); */
	  sl += cu;
	  /* Radius polynomial. */
	  cu = *pr++;
	  for (ip = 0; ip < nt; ip++)
	    {
	      cu = cu * T + *pr++;
	    }
	  sr += cu;
	  continue;
	}
      k1 = 0;
      cv = 0.0;
      sv = 0.0;
      for (ip = 0; ip < np; ip++)
	{
	  /* What harmonic.  */
	  j = *p++;
	  /* Which planet.  */
	  m = *p++ - 1;
	  if (j)
	    {
	      k = abs (j);
	      k -= 1;
	      su = ss[m][k];	/* sin(k*angle) */
	      if (j < 0)
		su = -su;
	      cu = cc[m][k];
	      if (k1 == 0)
		{		/* set first angle */
		  sv = su;
		  cv = cu;
		  k1 = 1;
		}
	      else
		{		/* combine angles */
		  t = su * cv + cu * sv;
		  cv = cu * cv - su * sv;
		  sv = t;
		}
	    }
	}
      /* Highest power of T.  */
      nt = *p++;
      /* Longitude. */
      cu = *pl++;
      su = *pl++;
      for (ip = 0; ip < nt; ip++)
	{
	  cu = cu * T + *pl++;
	  su = su * T + *pl++;
	}
      sl += cu * cv + su * sv;
      /* Radius. */
      cu = *pr++;
      su = *pr++;
      for (ip = 0; ip < nt; ip++)
	{
	  cu = cu * T + *pr++;
	  su = su * T + *pr++;
	}
      sr += cu * cv + su * sv;
    }
  t = plan->trunclvl;
  pobj[0] = t * sl;
  pobj[2] = t * sr;
  return (0);
}



/* Generic program to accumulate sum of trigonometric series
   in one variable.  */

static double g1plan (double J, struct plantbl *plan)
{
  int i, j, k, m, k1, ip, np, nt;
  /* On some systems such as Silicon Graphics, char is unsigned
     by default.  */
  CHAR *p;
  long *pl;
  double su, cu, sv, cv;
  double t, sl;

  T = (J - MOSHIER_J2000) / plan->timescale;
  mean_elements (J);
  /* Calculate sin( i*MM ), etc. for needed multiple angles.  */
  for (i = 0; i < NARGS; i++)
    {
      if ((j = plan->max_harmonic[i]) > 0)
	{
	  sscc (i, Args[i], j);
	}
    }

  /* Point to start of table of arguments. */
  p = plan->arg_tbl;
  /* Point to tabulated cosine and sine amplitudes.  */
  pl = plan->lon_tbl;
  sl = 0.0;

  for (;;)
    {
      /* argument of sine and cosine */
      /* Number of periodic arguments. */
      np = *p++;
      if (np < 0)
	break;
      if (np == 0)
	{			/* It is a polynomial term.  */
	  nt = *p++;
	  cu = *pl++;
	  for (ip = 0; ip < nt; ip++)
	    {
	      cu = cu * T + *pl++;
	    }
	  /*	  sl +=  mods3600 (cu); */
	  sl += cu;
	  continue;
	}
      k1 = 0;
      cv = 0.0;
      sv = 0.0;
      for (ip = 0; ip < np; ip++)
	{
	  /* What harmonic.  */
	  j = *p++;
	  /* Which planet.  */
	  m = *p++ - 1;
	  if (j)
	    {
	      k = abs (j);
	      k -= 1;
	      su = ss[m][k];	/* sin(k*angle) */
	      if (j < 0)
		su = -su;
	      cu = cc[m][k];
	      if (k1 == 0)
		{		/* set first angle */
		  sv = su;
		  cv = cu;
		  k1 = 1;
		}
	      else
		{		/* combine angles */
		  t = su * cv + cu * sv;
		  cv = cu * cv - su * sv;
		  sv = t;
		}
	    }
	}
      /* Highest power of T.  */
      nt = *p++;
      /* Cosine and sine coefficients.  */
      cu = *pl++;
      su = *pl++;
      for (ip = 0; ip < nt; ip++)
	{
	  cu = cu * T + *pl++;
	  su = su * T + *pl++;
	}
      sl += cu * cv + su * sv;
    }
  return (plan->trunclvl * sl);
}


/* geocentric moon, mean ecliptic and equinox of date
 * J is Julian Epemeris Date
 * output in pobj[]:
 * pobj[0]:  l in rad
 * pobj[1]:  b in rad
 * pobj[2]:  r in au
 */
static int  gecmoon (double J, struct plantbl *lrtab, struct plantbl *lattab,  double pobj[])
{
  double x;

  g2plan (J, lrtab, pobj, 0);
  x = pobj[0];
  x += LP_equinox;
  if (x < -6.45e5)
    x += 1.296e6;
  if (x > 6.45e5)
    x -= 1.296e6;
  pobj[0] = STR * x;
  x = g1plan (J, lattab);
  pobj[1] = STR * x;
  pobj[2] = (STR * pobj[2] + 1.0) * lrtab->distance;
  return 0;
}

/*********** end stephen moshier's moon code ****************/

static void moon_fast (double mjd, double *lam, double *bet,
	double *hp, double *msp, double *mdp);

/* previous version (elwood):
 *
 * given the mjd, find the geocentric ecliptic longitude, lam, and latitude,
 *   bet, and horizontal parallax, hp for the moon. also return the sun's
 *   mean anomaly, *msp, and the moon's mean anomaly, *mdp.
 * N.B. series for long and lat are good to about 10 and 3 arcseconds. however,
 *   math errors cause up to 100 and 30 arcseconds error, even if use double.
 *   why?? suspect highly sensitive nature of difference used to get m1..6.
 * N.B. still need to correct for nutation. then for topocentric location
 *   further correct for parallax and refraction.
 */
static void moon_fast (double mjd, double *lam, double *bet,
	double *hp, double *msp, double *mdp)
{
	double t, t2;
	double ld;
	double ms;
	double md;
	double de;
	double f;
	double n;
	double a, sa, sn, b, sb, c, sc, e, e2, l, g, w1, w2;
	double m1, m2, m3, m4, m5, m6;

	t = mjd/36525.;
	t2 = t*t;

	m1 = mjd/27.32158213;
	m1 = 360.0*(m1-(long)m1);
	m2 = mjd/365.2596407;
	m2 = 360.0*(m2-(long)m2);
	m3 = mjd/27.55455094;
	m3 = 360.0*(m3-(long)m3);
	m4 = mjd/29.53058868;
	m4 = 360.0*(m4-(long)m4);
	m5 = mjd/27.21222039;
	m5 = 360.0*(m5-(long)m5);
	m6 = mjd/6798.363307;
	m6 = 360.0*(m6-(long)m6);

	ld = 270.434164+m1-(.001133-.0000019*t)*t2;
	ms = 358.475833+m2-(.00015+.0000033*t)*t2;
	md = 296.104608+m3+(.009192+.0000144*t)*t2;
	de = 350.737486+m4-(.001436-.0000019*t)*t2;
	f = 11.250889+m5-(.003211+.0000003*t)*t2;
	n = 259.183275-m6+(.002078+.000022*t)*t2;

	a = degrad(51.2+20.2*t);
	sa = sin(a);
	sn = sin(degrad(n));
	b = 346.56+(132.87-.0091731*t)*t;
	sb = .003964*sin(degrad(b));
	c = degrad(n+275.05-2.3*t);
	sc = sin(c);
	ld = ld+.000233*sa+sb+.001964*sn;
	ms = ms-.001778*sa;
	md = md+.000817*sa+sb+.002541*sn;
	f = f+sb-.024691*sn-.004328*sc;
	de = de+.002011*sa+sb+.001964*sn;
	e = 1-(.002495+7.52e-06*t)*t;
	e2 = e*e;

	ld = degrad(ld);
	ms = degrad(ms);
	n = degrad(n);
	de = degrad(de);
	f = degrad(f);
	md = degrad(md);

	l = 6.28875*sin(md)+1.27402*sin(2*de-md)+.658309*sin(2*de)+
	    .213616*sin(2*md)-e*.185596*sin(ms)-.114336*sin(2*f)+
	    .058793*sin(2*(de-md))+.057212*e*sin(2*de-ms-md)+
	    .05332*sin(2*de+md)+.045874*e*sin(2*de-ms)+.041024*e*sin(md-ms);
	l = l-.034718*sin(de)-e*.030465*sin(ms+md)+.015326*sin(2*(de-f))-
	    .012528*sin(2*f+md)-.01098*sin(2*f-md)+.010674*sin(4*de-md)+
	    .010034*sin(3*md)+.008548*sin(4*de-2*md)-e*.00791*sin(ms-md+2*de)-
	    e*.006783*sin(2*de+ms);
	l = l+.005162*sin(md-de)+e*.005*sin(ms+de)+.003862*sin(4*de)+
	    e*.004049*sin(md-ms+2*de)+.003996*sin(2*(md+de))+
	    .003665*sin(2*de-3*md)+e*.002695*sin(2*md-ms)+
	    .002602*sin(md-2*(f+de))+e*.002396*sin(2*(de-md)-ms)-
	    .002349*sin(md+de);
	l = l+e2*.002249*sin(2*(de-ms))-e*.002125*sin(2*md+ms)-
	    e2*.002079*sin(2*ms)+e2*.002059*sin(2*(de-ms)-md)-
	    .001773*sin(md+2*(de-f))-.001595*sin(2*(f+de))+
	    e*.00122*sin(4*de-ms-md)-.00111*sin(2*(md+f))+.000892*sin(md-3*de);
	l = l-e*.000811*sin(ms+md+2*de)+e*.000761*sin(4*de-ms-2*md)+
	     e2*.000704*sin(md-2*(ms+de))+e*.000693*sin(ms-2*(md-de))+
	     e*.000598*sin(2*(de-f)-ms)+.00055*sin(md+4*de)+.000538*sin(4*md)+
	     e*.000521*sin(4*de-ms)+.000486*sin(2*md-de);
	l = l+e2*.000717*sin(md-2*ms);
	*lam = ld+degrad(l);
	range (lam, 2*PI);

	g = 5.12819*sin(f)+.280606*sin(md+f)+.277693*sin(md-f)+
	    .173238*sin(2*de-f)+.055413*sin(2*de+f-md)+.046272*sin(2*de-f-md)+
	    .032573*sin(2*de+f)+.017198*sin(2*md+f)+.009267*sin(2*de+md-f)+
	    .008823*sin(2*md-f)+e*.008247*sin(2*de-ms-f);
	g = g+.004323*sin(2*(de-md)-f)+.0042*sin(2*de+f+md)+
	    e*.003372*sin(f-ms-2*de)+e*.002472*sin(2*de+f-ms-md)+
	    e*.002222*sin(2*de+f-ms)+e*.002072*sin(2*de-f-ms-md)+
	    e*.001877*sin(f-ms+md)+.001828*sin(4*de-f-md)-e*.001803*sin(f+ms)-
	    .00175*sin(3*f);
	g = g+e*.00157*sin(md-ms-f)-.001487*sin(f+de)-e*.001481*sin(f+ms+md)+
	     e*.001417*sin(f-ms-md)+e*.00135*sin(f-ms)+.00133*sin(f-de)+
	     .001106*sin(f+3*md)+.00102*sin(4*de-f)+.000833*sin(f+4*de-md)+
	     .000781*sin(md-3*f)+.00067*sin(f+4*de-2*md);
	g = g+.000606*sin(2*de-3*f)+.000597*sin(2*(de+md)-f)+
	    e*.000492*sin(2*de+md-ms-f)+.00045*sin(2*(md-de)-f)+
	    .000439*sin(3*md-f)+.000423*sin(f+2*(de+md))+
	    .000422*sin(2*de-f-3*md)-e*.000367*sin(ms+f+2*de-md)-
	    e*.000353*sin(ms+f+2*de)+.000331*sin(f+4*de);
	g = g+e*.000317*sin(2*de+f-ms+md)+e2*.000306*sin(2*(de-ms)-f)-
	    .000283*sin(md+3*f);
	w1 = .0004664*cos(n);
	w2 = .0000754*cos(c);
	*bet = degrad(g)*(1-w1-w2);

	*hp = .950724+.051818*cos(md)+.009531*cos(2*de-md)+.007843*cos(2*de)+
	      .002824*cos(2*md)+.000857*cos(2*de+md)+e*.000533*cos(2*de-ms)+
	      e*.000401*cos(2*de-md-ms)+e*.00032*cos(md-ms)-.000271*cos(de)-
	      e*.000264*cos(ms+md)-.000198*cos(2*f-md);
	*hp = *hp+.000173*cos(3*md)+.000167*cos(4*de-md)-e*.000111*cos(ms)+
	     .000103*cos(4*de-2*md)-.000084*cos(2*md-2*de)-
	     e*.000083*cos(2*de+ms)+.000079*cos(2*de+2*md)+.000072*cos(4*de)+
	     e*.000064*cos(2*de-ms+md)-e*.000063*cos(2*de+ms-md)+
	     e*.000041*cos(ms+de);
	*hp = *hp+e*.000035*cos(2*md-ms)-.000033*cos(3*md-2*de)-
	     .00003*cos(md+de)-.000029*cos(2*(f-de))-e*.000029*cos(2*md+ms)+
	     e2*.000026*cos(2*(de-ms))-.000023*cos(2*(f-de)+md)+
	     e*.000019*cos(4*de-ms-md);
	*hp = degrad(*hp);

	*msp = ms;
	*mdp = md;
}


#define EarthRadius 6378.16             /* Kilometers           */

/* moon() - front end rountine to get moon position; stern
 *
 * given the mjd, find the geocentric ecliptic longitude, lam, and latitude,
 *   bet, and geocentric distance, rho in a.u. for the moon.  also return
 *   the sun's mean anomaly, *msp, and the moon's mean anomaly, *mdp.
 *
 * now uses Stephen Moshier's expansion and code.
 *
 * TODO: - clarify lunar aberration for apparent places
 * 
 * still need to correct for nutation. then for topocentric location
 *   further correct for parallax and refraction.
 * NB:  Do NOT correct for aberration - the geocentric moon frame moves
 *	along with the earth.
 */

 void moon (double mjd, double *lam, double *bet, double *rho, double *msp, double *mdp)
{
	double pobj[3], dt;
	double hp;

	if (mjd >= MOSHIER_BEGIN && mjd <= MOSHIER_END) {
		/* retard for light time */
		moon_fast (mjd, lam, bet, &hp, msp, mdp);
		*rho = EarthRadius/AUKM/sin(hp);
		dt = *rho * 5.7755183e-3;       /* speed of light in a.u/day */
		gecmoon(mjd + MJD0 - dt, &moonlr, &moonlat, pobj);

		*lam = pobj[0];
		range (lam, 2*PI);
		*bet = pobj[1];
		*rho = pobj[2];
		*msp = STR * Args[11];	/* don't need range correction here */
		*mdp = STR * Args[12];
	} else {
		moon_fast (mjd, lam, bet, &hp, msp, mdp);
		*rho = EarthRadius/AUKM/sin(hp);

	}
}

#define NUT_SCALE	1e4
#define NUT_SERIES	106
#define NUT_MAXMUL	4
#define SECPERCIRC	(3600.*360.)

/* Delaunay arguments, in arc seconds; they differ slightly from ELP82B */
static double delaunay[5][4] = {
    {485866.733,  1717915922.633, 31.310,  0.064}, /* M', moon mean anom */
    {1287099.804, 129596581.224,  -0.577, -0.012}, /* M, sun mean anom */
    {335778.877,  1739527263.137, -13.257, 0.011}, /* F, moon arg lat */
    {1072261.307, 1602961601.328, -6.891,  0.019}, /* D, elong moon sun */
    {450160.280,  -6962890.539,   7.455,   0.008}, /* Om, moon l asc node */
};

/* multipliers for Delaunay arguments */
static short multarg[NUT_SERIES][5] = {
	/* bounds:  -2..3, -2..2, -2/0/2/4, -4..4, 0..2 */
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 2},
    {-2, 0, 2, 0, 1},
    {2, 0, -2, 0, 0},
    {-2, 0, 2, 0, 2},
    {1, -1, 0, -1, 0},
    {0, -2, 2, -2, 1},
    {2, 0, -2, 0, 1},
    {0, 0, 2, -2, 2},
    {0, 1, 0, 0, 0},
    {0, 1, 2, -2, 2},
    {0, -1, 2, -2, 2},
    {0, 0, 2, -2, 1},
    {2, 0, 0, -2, 0},
    {0, 0, 2, -2, 0},
    {0, 2, 0, 0, 0},
    {0, 1, 0, 0, 1},
    {0, 2, 2, -2, 2},
    {0, -1, 0, 0, 1},
    {-2, 0, 0, 2, 1},
    {0, -1, 2, -2, 1},
    {2, 0, 0, -2, 1},
    {0, 1, 2, -2, 1},
    {1, 0, 0, -1, 0},
    {2, 1, 0, -2, 0},
    {0, 0, -2, 2, 1},
    {0, 1, -2, 2, 0},
    {0, 1, 0, 0, 2},
    {-1, 0, 0, 1, 1},
    {0, 1, 2, -2, 0},
    {0, 0, 2, 0, 2},
    {1, 0, 0, 0, 0},
    {0, 0, 2, 0, 1},
    {1, 0, 2, 0, 2},
    {1, 0, 0, -2, 0},
    {-1, 0, 2, 0, 2},
    {0, 0, 0, 2, 0},
    {1, 0, 0, 0, 1},
    {-1, 0, 0, 0, 1},
    {-1, 0, 2, 2, 2},
    {1, 0, 2, 0, 1},
    {0, 0, 2, 2, 2},
    {2, 0, 0, 0, 0},
    {1, 0, 2, -2, 2},
    {2, 0, 2, 0, 2},
    {0, 0, 2, 0, 0},
    {-1, 0, 2, 0, 1},
    {-1, 0, 0, 2, 1},
    {1, 0, 0, -2, 1},
    {-1, 0, 2, 2, 1},
    {1, 1, 0, -2, 0},
    {0, 1, 2, 0, 2},
    {0, -1, 2, 0, 2},
    {1, 0, 2, 2, 2},
    {1, 0, 0, 2, 0},
    {2, 0, 2, -2, 2},
    {0, 0, 0, 2, 1},
    {0, 0, 2, 2, 1},
    {1, 0, 2, -2, 1},
    {0, 0, 0, -2, 1},
    {1, -1, 0, 0, 0},
    {2, 0, 2, 0, 1},
    {0, 1, 0, -2, 0},
    {1, 0, -2, 0, 0},
    {0, 0, 0, 1, 0},
    {1, 1, 0, 0, 0},
    {1, 0, 2, 0, 0},
    {1, -1, 2, 0, 2},
    {-1, -1, 2, 2, 2},
    {-2, 0, 0, 0, 1},
    {3, 0, 2, 0, 2},
    {0, -1, 2, 2, 2},
    {1, 1, 2, 0, 2},
    {-1, 0, 2, -2, 1},
    {2, 0, 0, 0, 1},
    {1, 0, 0, 0, 2},
    {3, 0, 0, 0, 0},
    {0, 0, 2, 1, 2},
    {-1, 0, 0, 0, 2},
    {1, 0, 0, -4, 0},
    {-2, 0, 2, 2, 2},
    {-1, 0, 2, 4, 2},
    {2, 0, 0, -4, 0},
    {1, 1, 2, -2, 2},
    {1, 0, 2, 2, 1},
    {-2, 0, 2, 4, 2},
    {-1, 0, 4, 0, 2},
    {1, -1, 0, -2, 0},
    {2, 0, 2, -2, 1},
    {2, 0, 2, 2, 2},
    {1, 0, 0, 2, 1},
    {0, 0, 4, -2, 2},
    {3, 0, 2, -2, 2},
    {1, 0, 2, -2, 0},
    {0, 1, 2, 0, 1},
    {-1, -1, 0, 2, 1},
    {0, 0, -2, 0, 1},
    {0, 0, 2, -1, 2},
    {0, 1, 0, 2, 0},
    {1, 0, -2, -2, 0},
    {0, -1, 2, 0, 1},
    {1, 1, 0, -2, 1},
    {1, 0, -2, 2, 0},
    {2, 0, 0, 2, 0},
    {0, 0, 2, 4, 2},
    {0, 1, 0, 1, 0}
};

/* amplitudes which  have secular terms; in 1/NUT_SCALE arc seconds
 * {index, constant dPSI, T/10 in dPSI, constant in dEPS, T/10 in dEPS}
 */
static long ampsecul[][5] = {
    {0  ,-171996 ,-1742 ,92025 ,89},
    {1  ,2062    ,2     ,-895  ,5},
    {8  ,-13187  ,-16   ,5736  ,-31},
    {9  ,1426    ,-34   ,54    ,-1},
    {10 ,-517    ,12    ,224   ,-6},
    {11 ,217     ,-5    ,-95   ,3},
    {12 ,129     ,1     ,-70   ,0},
    {15 ,17      ,-1    ,0     ,0},
    {17 ,-16     ,1     ,7     ,0},
    {30 ,-2274   ,-2    ,977   ,-5},
    {31 ,712     ,1     ,-7    ,0},
    {32 ,-386    ,-4    ,200   ,0},
    {33 ,-301    ,0     ,129   ,-1},
    {37 ,63      ,1     ,-33   ,0},
    {38 ,-58     ,-1    ,32    ,0},
    /* termination */  { -1, }
};

/* amplitudes which only have constant terms; same unit as above
 * {dPSI, dEPS}
 * indexes which are already in ampsecul[][] are zeroed
 */
static short ampconst[NUT_SERIES][2] = {
    {0,0},
    {0,0},
    {46,-24},
    {11,0},
    {-3,1},
    {-3,0},
    {-2,1},
    {1,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {48,1},
    {-22,0},
    {0,0},
    {-15,9},
    {0,0},
    {-12,6},
    {-6,3},
    {-5,3},
    {4,-2},
    {4,-2},
    {-4,0},
    {1,0},
    {1,0},
    {-1,0},
    {1,0},
    {1,0},
    {-1,0},
    {0,0},
    {0,0},
    {0,0},
    {0,0},
    {-158,-1},
    {123,-53},
    {63,-2},
    {0,0},
    {0,0},
    {-59,26},
    {-51,27},
    {-38,16},
    {29,-1},
    {29,-12},
    {-31,13},
    {26,-1},
    {21,-10},
    {16,-8},
    {-13,7},
    {-10,5},
    {-7,0},
    {7,-3},
    {-7,3},
    {-8,3},
    {6,0},
    {6,-3},
    {-6,3},
    {-7,3},
    {6,-3},
    {-5,3},
    {5,0},
    {-5,3},
    {-4,0},
    {4,0},
    {-4,0},
    {-3,0},
    {3,0},
    {-3,1},
    {-3,1},
    {-2,1},
    {-3,1},
    {-3,1},
    {2,-1},
    {-2,1},
    {2,-1},
    {-2,1},
    {2,0},
    {2,-1},
    {1,-1},
    {-1,0},
    {1,-1},
    {-2,1},
    {-1,0},
    {1,-1},
    {-1,1},
    {-1,1},
    {1,0},
    {1,0},
    {1,-1},
    {-1,0},
    {-1,0},
    {1,0},
    {1,0},
    {-1,0},
    {1,0},
    {1,0},
    {-1,0},
    {-1,0},
    {-1,0},
    {-1,0},
    {-1,0},
    {-1,0},
    {-1,0},
    {1,0},
    {-1,0},
    {1,0}
};

/* given the modified JD, mjd, find the nutation in obliquity, *deps, and
 * the nutation in longitude, *dpsi, each in radians.
 */
void nutation (double mjd, double *deps, double *dpsi)
/*double mjd;
double *deps;	 on input:  precision parameter in arc seconds 
double *dpsi;*/
{
	static double lastmjd = -10000, lastdeps, lastdpsi;
	double T, T2, T3, T10;			/* jul cent since J2000 */
	double prec;				/* series precis in arc sec */
	int i, isecul;				/* index in term table */
	static double delcache[5][2*NUT_MAXMUL+1];
			/* cache for multiples of delaunay args
			 * [M',M,F,D,Om][-min*x, .. , 0, .., max*x]
			 * make static to have unfilled fields cleared on init
			 */

	if (mjd == lastmjd) {
	    *deps = lastdeps;
	    *dpsi = lastdpsi;
	    return;
	}

	prec = 0.0;

#if 0	/* this is if deps should contain a precision value */
	prec =* deps;
	if (prec < 0.0 || prec > 1.0)	/* accept only sane value */
		prec = 1.0;
#endif

	/* augment for abundance of small terms */
	prec *= NUT_SCALE/10;

	T = (mjd - J2000)/36525.;
	T2 = T * T;
	T3 = T2 * T;
	T10 = T/10.;

	/* calculate delaunay args and place in cache */
	for (i = 0; i < 5; ++i) {
	    double x;
	    short j;

	    x = delaunay[i][0] +
		delaunay[i][1] * T +
		delaunay[i][2] * T2 +
		delaunay[i][3] * T3;

	    /* convert to radians */
	    x /= SECPERCIRC;
	    x -= floor(x);
	    x *= 2.*PI;

	    /* fill cache table */
	    for (j = 0; j <= 2*NUT_MAXMUL; ++j)
		delcache[i][j] = (j - NUT_MAXMUL) * x;
	}

	/* find dpsi and deps */
	lastdpsi = lastdeps = 0.;
	for (i = isecul = 0; i < NUT_SERIES ; ++i) {
	    double arg = 0., ampsin, ampcos;
	    short j;

	    if (ampconst[i][0] || ampconst[i][1]) {
		/* take non-secular terms from simple array */
		ampsin = ampconst[i][0];
		ampcos = ampconst[i][1];
	    } else {
		/* secular terms from different array */
		ampsin = ampsecul[isecul][1] + ampsecul[isecul][2] * T10;
		ampcos = ampsecul[isecul][3] + ampsecul[isecul][4] * T10;
		++isecul;
	    }

	    for (j = 0; j < 5; ++j)
		arg += delcache[j][NUT_MAXMUL + multarg[i][j]];

	    if (fabs(ampsin) >= prec)
		lastdpsi += ampsin * sin(arg);

	    if (fabs(ampcos) >= prec)
		lastdeps += ampcos * cos(arg);

	}

	/* convert to radians.
	 */
	lastdpsi = degrad(lastdpsi/3600./NUT_SCALE);
	lastdeps = degrad(lastdeps/3600./NUT_SCALE);

	lastmjd = mjd;
	*deps = lastdeps;
	*dpsi = lastdpsi;
}

/* given the modified JD, mjd, correct, IN PLACE, the right ascension *ra
 * and declination *dec (both in radians) for nutation.
 */
void nut_eq (double mjd, double *ra, double *dec)
{
	static double lastmjd = -10000;
	static double a[3][3];		/* rotation matrix */
	double xold, yold, zold, x, y, z;

	if (mjd != lastmjd) {
	    double epsilon, dpsi, deps;
	    double se, ce, sp, cp, sede, cede;

	    obliquity(mjd, &epsilon);
	    nutation(mjd, &deps, &dpsi);

	    /* the rotation matrix a applies the nutation correction to
	     * a vector of equatoreal coordinates Xeq to Xeq' by 3 subsequent
	     * rotations:  R1 - from equatoreal to ecliptic system by
	     * rotation of angle epsilon about x, R2 - rotate ecliptic
	     * system by -dpsi about its z, R3 - from ecliptic to equatoreal
	     * by rotation of angle -(epsilon + deps)
	     *
	     *	Xeq' = A * Xeq = R3 * R2 * R1 * Xeq
	     * 
	     *		[ 1       0          0    ]
	     * R1 =	[ 0   cos(eps)   sin(eps) ]
	     *		[ 0  - sin(eps)  cos(eps) ]
	     * 
	     *		[ cos(dpsi)  - sin(dpsi)  0 ]
	     * R2 =	[ sin(dpsi)   cos(dpsi)   0 ]
	     *		[      0           0      1 ]
	     * 
	     *		[ 1         0                 0         ]
	     * R3 =	[ 0  cos(eps + deps)  - sin(eps + deps) ]
	     *		[ 0  sin(eps + deps)   cos(eps + deps)  ]
	     * 
	     * for efficiency, here is a explicitely:
	     */
	    
	    se = sin(epsilon);
	    ce = cos(epsilon);
	    sp = sin(dpsi);
	    cp = cos(dpsi);
	    sede = sin(epsilon + deps);
	    cede = cos(epsilon + deps);

	    a[0][0] = cp;
	    a[0][1] = -sp*ce;
	    a[0][2] = -sp*se;

	    a[1][0] = cede*sp;
	    a[1][1] = cede*cp*ce+sede*se;
	    a[1][2] = cede*cp*se-sede*ce;

	    a[2][0] = sede*sp;
	    a[2][1] = sede*cp*ce-cede*se;
	    a[2][2] = sede*cp*se+cede*ce;

	    lastmjd = mjd;
	}

	sphcart(*ra, *dec, 1.0, &xold, &yold, &zold);
	x = a[0][0] * xold + a[0][1] * yold + a[0][2] * zold;
	y = a[1][0] * xold + a[1][1] * yold + a[1][2] * zold;
	z = a[2][0] * xold + a[2][1] * yold + a[2][2] * zold;
	cartsph(x, y, z, ra, dec, &zold);	/* radius should be 1.0 */
	if (*ra < 0.) *ra += 2.*PI;		/* make positive for display */
}

void ta_par (double tha, double tdec, double phi, double ht, double *rho, double *aha, double *adec)
{
	static double last_phi = 1000.0, last_ht = -1000.0, xobs, zobs;
	double x, y, z;	/* obj cartesian coord, in Earth radii */

	/* avoid calcs involving the same phi and ht */
	if (phi != last_phi || ht != last_ht) {
	    double cphi, sphi, robs, e2 = (2 - 1/298.257)/298.257;
	    cphi = cos(phi);
	    sphi = sin(phi);
	    robs = 1/sqrt(1 - e2 * sphi * sphi);

	    /* observer coordinates: x to meridian, y east, z north */
	    xobs = (robs + ht) * cphi;
	    zobs = (robs*(1-e2) + ht) * sphi;
	    last_phi  =  phi;
	    last_ht  =  ht;
	}

	sphcart(-tha, tdec, *rho, &x, &y, &z);
	cartsph(x - xobs, y, z - zobs, aha, adec, rho);
	*aha *= -1;
	range (aha, 2*PI);
}
#if 0 // planets
static void elongation (double lam, double bet, double lsn, double *el);
#endif
static void unrefractLT15 (double pr, double tr, double aa, double *ta);
static void unrefractGE15 (double pr, double tr, double aa, double *ta);

void unrefract (double pr, double tr, double aa, double *ta)
{
#define	LTLIM	14.5
#define	GELIM	15.5

	double aadeg = raddeg(aa);

	if (aadeg < LTLIM)
	    unrefractLT15 (pr, tr, aa, ta);
	else if (aadeg >= GELIM)
	    unrefractGE15 (pr, tr, aa, ta);
	else {
	    /* smooth blend -- important for inverse */
	    double taLT, taGE, p;

	    unrefractLT15 (pr, tr, aa, &taLT);
	    unrefractGE15 (pr, tr, aa, &taGE);
	    p = (aadeg - LTLIM)/(GELIM - LTLIM);
	    *ta = taLT + (taGE - taLT)*p;
	}
	    
}

static void unrefractGE15 (double pr, double tr, double aa, double *ta)
{
	double r;
	
	r = 7.888888e-5*pr/((273+tr)*tan(aa));
	*ta  =  aa - r;
}

static void unrefractLT15 (double pr, double tr, double aa, double *ta)
{
	double aadeg = raddeg(aa);
	double r, a, b;

	a = ((2e-5*aadeg+1.96e-2)*aadeg+1.594e-1)*pr;
	b = (273+tr)*((8.45e-2*aadeg+5.05e-1)*aadeg+1);
	r = degrad(a/b);

	*ta  =  aa - r;
}

/* correct the true altitude, ta, for refraction to the apparent altitude, aa,
 * each in radians, given the local atmospheric pressure, pr, in mbars, and
 * the temperature, tr, in degrees C.
 */
/* correct the true altitude, ta, for refraction to the apparent altitude, aa,
 * each in radians, given the local atmospheric pressure, pr, in mbars, and
 * the temperature, tr, in degrees C.
 */
void refract (double pr,double tr, double ta,double *aa)
{
#define	MAXRERR	degrad(0.1/3600.)	/* desired accuracy, rads */

	double d, t, t0, a;

	/* first guess of error is to go backwards.
	 * make use that we know delta-apparent is always < delta-true.
	 */
	unrefract (pr, tr, ta, &t);
	d = 0.8*(ta - t);
	t0 = t;
	a = ta;

	/* use secant method to discover a value that unrefracts to ta.
	 * max=7 ave=2.4 loops in hundreds of test cases.
	 */
	do {
	    a += d;
	    unrefract (pr, tr, a, &t);
	    d *= -(ta - t)/(t0 - t);
	    t0 = t;
	} while (fabs(ta-t) > MAXRERR);

	*aa = a;

#undef	MAXRERR
}

#define ABERR_CONST	(20.49552/3600./180.*PI)  /* aberr const in rad */
#define AB_ECL_EOD	0
#define AB_EQ_EOD	1

static void ab_aux (double mjd, double *x, double *y, double lsn,int mode);

/* apply aberration correction to ecliptical coordinates *lam and *bet
 * (in radians) for a given time mjd and handily supplied longitude of sun,
 * lsn (in radians)
 */

void ab_ecl (double mjd, double lsn, double *lam, double *bet)
{
	ab_aux(mjd, lam, bet, lsn, AB_ECL_EOD);
}

/* apply aberration correction to equatoreal coordinates *ra and *dec
 * (in radians) for a given time mjd and handily supplied longitude of sun,
 * lsn (in radians)
 */
void ab_eq (double mjd, double lsn, double *ra, double *dec)
{
	ab_aux(mjd, ra, dec, lsn, AB_EQ_EOD);
}

/* because the e-terms are secular, keep the real transformation for both
 * coordinate systems in here with the secular variables cached.
 * mode == AB_ECL_EOD:	x = lam, y = bet	(ecliptical)
 * mode == AB_EQ_EOD:	x = ra,  y = dec	(equatoreal)
 */
static void ab_aux (double mjd, double *x, double *y, double lsn, int mode)
{
	static double lastmjd = -10000;
	static double eexc;	/* earth orbit excentricity */
	static double leperi;	/* ... and longitude of perihelion */
	static char dirty = 1;	/* flag for cached trig terms */

	if (mjd != lastmjd) {
	    double T;		/* centuries since J2000 */

	    T = (mjd - J2000)/36525.;
	    eexc = 0.016708617 - (42.037e-6 + 0.1236e-6 * T) * T;
	    leperi = degrad(102.93735 + (0.71953 + 0.00046 * T) * T);
	    lastmjd = mjd;
	    dirty = 1;
	}

	switch (mode) {
	case AB_ECL_EOD:		/* ecliptical coords */
	    {
		double *lam = x, *bet = y;
		double dlsun, dlperi;

		dlsun = lsn - *lam;
		dlperi = leperi - *lam;

		/* valid only for *bet != +-PI/2 */
		*lam -= ABERR_CONST/cos(*bet) * (cos(dlsun) -
				eexc*cos(dlperi));
		*bet -= ABERR_CONST*sin(*bet) * (sin(dlsun) -
				eexc*sin(dlperi));
	    }
	    break;

	case AB_EQ_EOD:			/* equatoreal coords */
	    {
		double *ra = x, *dec = y;
		double sr, cr, sd, cd, sls, cls;/* trig values coords */
		static double cp, sp, ce, se;	/* .. and perihel/eclipic */
		double dra, ddec;		/* changes in ra and dec */

		if (dirty) {
		    double eps;

		    cp = cos(leperi);
		    sp = sin(leperi);
		    obliquity(mjd, &eps);
		    se = sin(eps);
		    ce = cos(eps);
		    dirty = 0;
		}

		sr = sin(*ra);
		cr = cos(*ra);
		sd = sin(*dec);
		cd = cos(*dec);
		sls = sin(lsn);
		cls = cos(lsn);

		dra = ABERR_CONST/cd * ( -(cr * cls * ce + sr * sls) +
			    eexc * (cr * cp * ce + sr * sp));

		ddec = se/ce * cd - sr * sd;	/* tmp use */
		ddec = ABERR_CONST * ( -(cls * ce * ddec + cr * sd * sls) +
			    eexc * (cp * ce * ddec + cr * sd * sp) );
		
		*ra += dra;
		range (ra, 2*PI);
		*dec += ddec;
	    }
	    break;

	default:
	    //printf ("ab_aux: bad mode: %d\n", mode);
	    //exit (1);
	    break;

	} /* switch (mode) */
}






static void ecleq_aux (int sw, double mjd, double x, double y,
    double *p, double *q);

#define	EQtoECL	1
#define	ECLtoEQ	(-1)


/* given the modified Julian date, mjd, and an equitorial ra and dec, each in
 * radians, find the corresponding geocentric ecliptic latitude, *lat, and
 * longititude, *lng, also each in radians.
 * correction for the effect on the angle of the obliquity due to nutation is
 * not included.
 */
void eq_ecl (double mjd, double ra, double dec, double *lat, double *lng)
{
	ecleq_aux (EQtoECL, mjd, ra, dec, lng, lat);
}

/* given the modified Julian date, mjd, and a geocentric ecliptic latitude,
 * *lat, and longititude, *lng, each in radians, find the corresponding
 * equitorial ra and dec, also each in radians.
 * correction for the effect on the angle of the obliquity due to nutation is
 * not included.
 */
void ecl_eq (double mjd, double lat, double lng, double *ra, double *dec)
{
	ecleq_aux (ECLtoEQ, mjd, lng, lat, ra, dec);
}

static void ecleq_aux (int sw, double mjd, double x, double y, double *p,double *q)
/*
int sw;		 +1 for eq to ecliptic, -1 for vv. 
double mjd;
double x, y;		 sw==1: x==ra, y==dec.  sw==-1: x==lng, y==lat. 
double *p, *q;		 sw==1: p==lng, q==lat. sw==-1: p==ra, q==dec. */
{
	static double lastmjd = -10000;	/* last mjd calculated */
	static double seps, ceps;	/* sin and cos of mean obliquity */
	double sx, cx, sy, cy, ty;

	if (mjd != lastmjd) {
	    double eps;
	    obliquity (mjd, &eps);		/* mean obliquity for date */
    	    seps = sin(eps);
	    ceps = cos(eps);
	    lastmjd = mjd;
	}

	sy = sin(y);
	cy = cos(y);				/* always non-negative */
        if (fabs(cy)<1e-20) cy = 1e-20;		/* insure > 0 */
        ty = sy/cy;
	cx = cos(x);
	sx = sin(x);
        *q = asin((sy*ceps)-(cy*seps*sx*sw));
        *p = atan(((sx*ceps)+(ty*seps*sw))/cx);
        if (cx<0) *p += PI;		/* account for atan quad ambiguity */
	range (p, 2*PI);
}

double geoc_lat (double phi);
static void aaha_aux (double lat, double x, double y, double *p, double *q);

/* given geographical latitude (n+, radians), lat, altitude (up+, radians),
 * alt, and azimuth (angle round to the east from north+, radians),
 * return hour angle (radians), ha, and declination (radians), dec.
 */
void aa_hadec (double lat, double alt,double  az, double *ha, double *dec)
{
	aaha_aux (lat, az, alt, ha, dec);
	if (*ha > PI)
	    *ha -= 2*PI;
}

/* given geographical (n+, radians), lat, hour angle (radians), ha, and
 * declination (radians), dec, return altitude (up+, radians), alt, and
 * azimuth (angle round to the east from north+, radians),
 */
void hadec_aa (double lat, double ha, double dec, double *alt, double *az)
{
	aaha_aux (lat, ha, dec, az, alt);
}

#ifdef NEED_GEOC
/* given a geographic (surface-normal) latitude, phi, return the geocentric
 * latitude, psi.
 */
double geoc_lat (double phi)
{
#define	MAXLAT	degrad(89.9999)	/* avoid tan() greater than this */
	return (fabs(phi)>MAXLAT ? phi : atan(tan(phi)/1.00674));
}
#endif

/* the actual formula is the same for both transformation directions so
 * do it here once for each way.
 * N.B. all arguments are in radians.
 */
static void aaha_aux (double lat, double x, double y, double *p, double *q)
/*double lat;
double x, y;
double *p, *q;*/
{
	static double last_lat = -3434, slat, clat;
	double cap, B;

	if (lat != last_lat) {
	    slat = sin(lat);
	    clat = cos(lat);
	    last_lat = lat;
	}

	solve_sphere (-x, PI/2-y, slat, clat, &cap, &B);
	*p = B;
	*q = PI/2 - acos(cap);
}

/* solve a spherical triangle:
 *           A
 *          /  \
 *         /    \
 *      c /      \ b
 *       /        \
 *      /          \
 *    B ____________ C
 *           a
 *
 * given A, b, c find B and a in range 0..B..2PI and 0..a..PI, respectively..
 * cap and Bp may be NULL if not interested in either one.
 * N.B. we pass in cos(c) and sin(c) because in many problems one of the sides
 *   remains constant for many values of A and b.
 */
void solve_sphere (double A, double b, double cc, double sc, double *cap, double *Bp)
{
	double cb = cos(b), sb = sin(b);
	double cA = cos(A);
	double ca;
	double B;

	ca = cb*cc + sb*sc*cA;
	if (ca >  1.0) ca =  1.0;
	if (ca < -1.0) ca = -1.0;
	if (cap)
	    *cap = ca;

	if (!Bp)
	    return;

	if (cc > .99999) {
	    /* as c approaches 0, B approaches pi - A */
	    B = PI - A;
	} else if (cc < -.99999) {
	    /* as c approaches PI, B approaches A */
	    B = A;
	} else {
	    /* compute cB and sB and remove common factor of sa from quotient.
	     * be careful where B causes atan to blow.
	     */
	    double sA = sin(A);
	    double x, y;

	    y = sA*sb*sc;
	    x = cb - ca*cc;
	
	    if (fabs(x) < 1e-5)
		B = y < 0 ? 3*PI/2 : PI/2;
	    else
		B = atan2 (y, x);
	}

	*Bp = B;
	range (Bp, 2*PI);
}


#define TABSTART 1620.0
#define TABEND 2004.0
#define TABSIZ 385

/* Note, Stephenson and Morrison's table starts at the year 1630.
 * The Chapronts' table does not agree with the Almanac prior to 1630.
 * The actual accuracy decreases rapidly prior to 1780.
 */
static short dt[TABSIZ] = {
    /* 1620.0 thru 1659.0 */
    12400, 11900, 11500, 11000, 10600, 10200, 9800, 9500, 9100, 8800,
    8500, 8200, 7900, 7700, 7400, 7200, 7000, 6700, 6500, 6300,
    6200, 6000, 5800, 5700, 5500, 5400, 5300, 5100, 5000, 4900,
    4800, 4700, 4600, 4500, 4400, 4300, 4200, 4100, 4000, 3800,
    /* 1660.0 thru 1699.0 */
    3700, 3600, 3500, 3400, 3300, 3200, 3100, 3000, 2800, 2700,
    2600, 2500, 2400, 2300, 2200, 2100, 2000, 1900, 1800, 1700,
    1600, 1500, 1400, 1400, 1300, 1200, 1200, 1100, 1100, 1000,
    1000, 1000, 900, 900, 900, 900, 900, 900, 900, 900,
    /* 1700.0 thru 1739.0 */
    900, 900, 900, 900, 900, 900, 900, 900, 1000, 1000,
    1000, 1000, 1000, 1000, 1000, 1000, 1000, 1100, 1100, 1100,
    1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100, 1100,
    1100, 1100, 1100, 1100, 1200, 1200, 1200, 1200, 1200, 1200,
    /* 1740.0 thru 1779.0 */
    1200, 1200, 1200, 1200, 1300, 1300, 1300, 1300, 1300, 1300,
    1300, 1400, 1400, 1400, 1400, 1400, 1400, 1400, 1500, 1500,
    1500, 1500, 1500, 1500, 1500, 1600, 1600, 1600, 1600, 1600,
    1600, 1600, 1600, 1600, 1600, 1700, 1700, 1700, 1700, 1700,
    /* 1780.0 thru 1799.0 */
    1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700, 1700,
    1700, 1700, 1600, 1600, 1600, 1600, 1500, 1500, 1400, 1400,
    /* 1800.0 thru 1819.0 */
    1370, 1340, 1310, 1290, 1270, 1260, 1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250, 1250, 1250, 1250, 1240, 1230, 1220,
    /* 1820.0 thru 1859.0 */
    1200, 1170, 1140, 1110, 1060, 1020, 960, 910, 860, 800,
    750, 700, 660, 630, 600, 580, 570, 560, 560, 560,
    570, 580, 590, 610, 620, 630, 650, 660, 680, 690,
    710, 720, 730, 740, 750, 760, 770, 770, 780, 780,
    /* 1860.0 thru 1899.0 */
    788, 782, 754, 697, 640, 602, 541, 410, 292, 182,
    161, 10, -102, -128, -269, -324, -364, -454, -471, -511,
    -540, -542, -520, -546, -546, -579, -563, -564, -580, -566,
    -587, -601, -619, -664, -644, -647, -609, -576, -466, -374,
    /* 1900.0 thru 1939.0 */
    -272, -154, -2, 124, 264, 386, 537, 614, 775, 913,
    1046, 1153, 1336, 1465, 1601, 1720, 1824, 1906, 2025, 2095,
    2116, 2225, 2241, 2303, 2349, 2362, 2386, 2449, 2434, 2408,
    2402, 2400, 2387, 2395, 2386, 2393, 2373, 2392, 2396, 2402,
    /* 1940.0 thru 1979.0 */
     2433, 2483, 2530, 2570, 2624, 2677, 2728, 2778, 2825, 2871,
     2915, 2957, 2997, 3036, 3072, 3107, 3135, 3168, 3218, 3268,
     3315, 3359, 3400, 3447, 3503, 3573, 3654, 3743, 3829, 3920,
     4018, 4117, 4223, 4337, 4449, 4548, 4646, 4752, 4853, 4959,
    /* 1980.0 thru 1995.0 */
     5054, 5138, 5217, 5296, 5379, 5434, 5487, 5532, 5582, 5630,
     5686, 5757, 5831, 5912, 5998, 6078,
    /* new USNO data (stern) */
     6163, 6230,
    /* new USNO extrapolation (stern), 2000.0 to 2004.0 */
     6296, 6420,
     6510, 6600, 6700, 6800, 6900  /* 7000, 7100, 7200, 7300, 7400, */

    /* Extrapolated values (USNO) (original Moshier)
     6183, 6280, 6377, 6475,
     6572, 6667, 6765, 6861, 6957
     */
};


/* calculate  DeltaT = ET - UT in seconds.  Describes the irregularities
 * of the Earth rotation rate in the ET time scale.
 */
double deltat(double mjd)
{
	double Y;
	double p, B;
	int d[6];
	int i, iy, k;
	static double ans;
	static double lastmjd = -10000;

	if (mjd == lastmjd) {
	    return(ans);
	}
	lastmjd = mjd;

	Y = 2000.0 + (mjd - J2000)/365.25;

	if( Y > TABEND ) {
	    /* linear interpolation from table end; stern */
	    B = Y - TABEND;
	    ans = dt[TABSIZ-1] + B * (dt[TABSIZ-1]  - dt[TABSIZ-2]);
	    ans *= 0.01;
	    return(ans);
	}

	if( Y < TABSTART ) {
	    if( Y >= 948.0 ) {
		/* Stephenson and Morrison, stated domain is 948 to 1600:
		 * 25.5(centuries from 1800)^2 - 1.9159(centuries from 1955)^2
		 */
		B = 0.01*(Y - 2000.0);
		ans = (23.58 * B + 100.3)*B + 101.6;
	    } else {
		/* Borkowski */
		B = 0.01*(Y - 2000.0)  +  3.75;
		ans = 35.0 * B * B  +  40.;
	    }
	    return(ans);
	}

	/* Besselian interpolation from tabulated values.
	 * See AA page K11.
	 */

	/* Index into the table.
	 */
	double a=Y;
	p = floor(a);
	iy =(int)( p - TABSTART);
	/* Zeroth order estimate is value at start of year
	 */
	ans = dt[iy];
	k = iy + 1;
	if( k >= TABSIZ )
	    goto done; /* No data, can't go on. */

	/* The fraction of tabulation interval
	 */
	p = Y - p;

	/* First order interpolated value
	 */
	ans += p*(dt[k] - dt[iy]);
	if( (iy-1 < 0) || (iy+2 >= TABSIZ) )
	    goto done; /* can't do second differences */

	/* Make table of first differences
	 */
	k = iy - 2;
	for( i=0; i<5; i++ ) {
	    if( (k < 0) || (k+1 >= TABSIZ) )
		d[i] = 0;
	    else d[i] = dt[k+1] - dt[k];
		k += 1;
	}

	/* Compute second differences
	 */
	for( i=0; i<4; i++ )
	    d[i] = d[i+1] - d[i];
	B = 0.25*p*(p-1.0);
	ans += B*(d[1] + d[2]);
	if( iy+2 >= TABSIZ )
	    goto done;

	/* Compute third differences
	 */
	for( i=0; i<3; i++ )
	    d[i] = d[i+1] - d[i];
	B = 2.0*B/3.0;
	ans += (p-0.5)*B*d[1];
	if( (iy-2 < 0) || (iy+3 > TABSIZ) )
	    goto done;

	/* Compute fourth differences
	 */
	for( i=0; i<2; i++ )
	    d[i] = d[i+1] - d[i];
	B = 0.125*B*(p+1.0)*(p-2.0);
	ans += B*(d[0] + d[1]);

	done:
	/* Astronomical Almanac table is corrected by adding the expression
	 *     -0.000091 (ndot + 26)(year-1955)^2  seconds
	 * to entries prior to 1955 (AA page K8), where ndot is the secular
	 * tidal term in the mean motion of the Moon.
	 *
	 * Entries after 1955 are referred to atomic time standards and
	 * are not affected by errors in Lunar or planetary theory.
	 */
	ans *= 0.01;
	if( Y < 1955.0 ) {
	    B = (Y - 1955.0);
	    ans += -0.000091 * (-25.8 + 26.0) * B * B;
	}
	return( ans );
}


/* given a Now at noon and a dt from noon, in hours, for a first approximation
 * to a rise or set event, refine the event by searching for when alt+dis = 0.
 * return 0: if find one within 12 hours of noon with np and op set to the
 *    better time and circumstances;
 * return -1: if error from obj_cir;
 * return -2: if converges but not today;
 * return -3: if does not converge at all (probably circumpolar or never up);
 */








