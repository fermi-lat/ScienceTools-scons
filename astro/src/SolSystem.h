// SolSystem.h: interface for the SolSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOLSYSTEM_H__8DAA36A1_3615_11D4_AA49_005004B7281D__INCLUDED_)
#define AFX_SOLSYSTEM_H__8DAA36A1_3615_11D4_AA49_005004B7281D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#ifndef PI
#define PI 3.141592654
#endif
#define	degrad(x)	((x)*PI/180.)
#define	raddeg(x)	((x)*180./PI)
#define	hrdeg(x)	((x)*15.)
#define	deghr(x)	((x)/15.)
#define	hrrad(x)	degrad(hrdeg(x))
#define	radhr(x)	deghr(raddeg(x))
double mjd_hr(double jd);

#define	MERCURY	0
#define	VENUS	1
#define	MARS	2
#define	JUPITER	3
#define	SATURN	4
#define	URANUS	5
#define	NEPTUNE	6
#define	PLUTO	7
#define	SUN		8
#define MOON	9

typedef struct Riset{
    int Flags;	/* info about what has been computed and any
					* special conditions; see flags, below.
				*/
    double RiseTu;	/* mjd time of rise today */
    double RiseAz;	/* azimuth of rise, rads E of N */
    double TranTu;	/* mjd time of transit today */
    double TranAlt;	/* altitude of transit, rads up from horizon */
    double SetTu;	/* mjd time of set today */
    double SetAz;	/* azimuth of set, rads E of N */
} RISESET;

/* RiseSet flags */
#define	RS_NORISE	0x0001	/* object does not rise as such today */
#define	RS_NOSET	0x0002	/* object does not set as such today */
#define	RS_NOTRANS	0x0004	/* object does not transit as such today */
#define	RS_CIRCUMPOLAR	0x0010	/* object stays up all day today */
#define	RS_NEVERUP	0x0020	/* object never up at all today */
#define	RS_ERROR	0x1000	/* can't figure out anything! */
#define	RS_RISERR	(0x0100|RS_ERROR) /* error computing rise */
#define	RS_SETERR	(0x0200|RS_ERROR) /* error computing set */
#define	RS_TRANSERR	(0x0400|RS_ERROR) /* error computing transit */

class SolSystem  
{
public:

	double RiseTu;	/* mjd time of rise today */
    double RiseAz;	/* azimuth of rise, rads E of N */
    double TranTu;	/* mjd time of transit today */
    double TranAlt;	/* altitude of transit, rads up from horizon */
    double SetTu;	/* mjd time of set today */
    double SetAz;	/* azimuth of set, rads E of N */

    double Ra;		/* geo/topo app/mean ra, rads */		\
    double Dec;		/* geo/topo app/mean dec, rads */		\
	double HRa,DDec;
    double GRa;		/* geo apparent ra, rads */			\
    double GDec;	/* geo apparent dec, rads */			\
    double Az;		/* azimuth, >0 e of n, rads */			\
    double Alt;		/* altitude above topocentric horizon, rads */	\
    double Elong;	/* angular sep btwen obj and sun, >0 E, degs */	\
    double Size;	/* angular size, arc secs */			\
    double Mag;		/* visual magnitude * MAGSCALE */
	double Sdist;	/* dist from object to sun, au */		\
    double Edist;	/* dist from object to earth, au */		\
    double Hlong;	/* heliocentric longitude, rads */		\
    double Hlat;	/* heliocentric latitude, rads */		\
    double Phase;	/* phase, % */
	
	SolSystem();
	virtual ~SolSystem();

	void		RiseSet();
	void		RiseSet(double dis);
	void		SetLocation(double lati, double loni,double eleva);
	void		CalculatePos(double jd,int obo);
	void		CalculatePos(double jd);
	void		SetJD(double jd);
	void		SetObj(int nob);
	int			ephi_sun();
	void		ephi_planet();
	int			ephi_moon();

private:
	int Obj;
	//RISESET rp;
	int Flags;
	int LOCSET;
	double lng, lat,elev, dis, GG;
	void elongation (double lam, double bet, double lsn, double *el);
	void deflect (double mjd1, double lpd, double psi, double lsn, double rsn, double rho, double *ra,double *dec);
	int find_transit (double mjd, double dt, double dis, double *alt, double *mjdt);
	int find_0alt (double mjd, double dt, double dis, double *az, double *mjdt);
	void ephi_pos(double mdj,int obj,double bet, double lam, double *rho, double *ra1, double *dec1);
	void ephi_riset ();

};

#endif // !defined(AFX_SOLSYSTEM_H__8DAA36A1_3615_11D4_AA49_005004B7281D__INCLUDED_)
