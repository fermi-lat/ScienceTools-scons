// $Header$

#include "astro/EarthOrbit.h"
#include "astro/EarthCoordinate.h"
#include "astro/SolarSystem.h"

#include <cmath>

namespace {
    
    double SecondsPerDay = 86400.;
    double R2D = 180/M_PI;
    double EarthFlat= (1/298.25);            /* Earth Flattening Coeff. */
    
    void range (double *s, double m)
    {
        while((*s)>=m) (*s)-=m;
        while((*s)<0) (*s)+=m;
    }
    
    
    inline double sqr(double x){return x*x;}
    
    astro::JulianDate JD_missionStart =astro::JulianDate(2001, 1, 1,0.0);
    astro::JulianDate JDStart =        astro::JulianDate(2005 ,7,18,0.0);
}

// static constants reflecting orbit parameters

namespace astro {
double EarthOrbit::s_altitude = 550.e3  ; //m
double EarthOrbit::s_incl = 28.5*M_PI/180; //radian 
double EarthOrbit::s_e = 0.; //eccentricity

double EarthOrbit::s_radius = 6378145.; //m



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EarthOrbit::EarthOrbit()
{
    initialize();
}

void EarthOrbit::initialize()
{
    double sini = sin(s_incl), cosi = cos(s_incl);
    static double J2=1.0822e-3;
    
    m_alt=(s_radius + s_altitude)/1000.; //altitude in km
    m_a = 1.0 + s_altitude/s_radius; 
    
    //THB double T = M_PI/2*pow(m_a,1.5)/sqrt(5.98e24*6.67e-11)*pow(s_radius,1.5)  ; 	
    
    double u = 3.9860044e14/pow(s_radius,3)  ; 
    double n = sqrt(u / pow(m_a,3));
    double n1 = n*(1. + 3.*J2/2.*sqrt(1. - s_e*s_e)/(m_a*m_a)/pow((1. - s_e*s_e),2)*(1.-1.5*sini*sini));
    m_dwdt = n1*3.*J2/2./(m_a*m_a)/pow((1. - s_e*s_e),2)*(2. - 2.5*sini*sini);
    
    m_dOmegadt = -n1*3.*J2/2./(m_a*m_a)/pow((1. - s_e*s_e),2)*cosi;
    m_dMdt = n1;
    // phases for launch start
    // this is really the elapsed time in seconds since the MissionStart
    //THB double StartSimDate = (JDStart-JD_missionStart) * SecondsPerDay;

    // these defaults should put the satellite at cape canaveral at t=mission start + 0 seconds.
    m_M0     = M_PI/2.;
    m_Omega0 = 4.8775;
    m_w0     = 0.0;
    
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Hep3Vector EarthOrbit::position(JulianDate JD) const
{
    double elapse = (JD - JDStart)*SecondsPerDay;
    
    double M=m_M0+m_dMdt*elapse;
    
    double Omega = m_Omega0+m_dOmegadt*elapse;
       
    double w = m_w0+m_dwdt*elapse;
    double Enew =Kepler(M,s_e); 
    
    
    Hep3Vector pos= Hep3Vector( cos(Enew)-s_e, sqrt(1.-sqr(s_e))*sin(Enew), 0 ).unit()*m_alt;
    pos.rotateZ(w).rotateX(s_incl).rotateZ(Omega);
    
    return pos;  
}

double EarthOrbit::Kepler(double MeanAnomaly,double Eccentricity)
{
    double E = MeanAnomaly;    // Initial guess to Eccentric Anomaly
    if( Eccentricity==0) return E; // THB: skip following
    double Error;
    double TrueAnomaly;
    
    do
    {
        Error = (E - Eccentricity*sin(E) - MeanAnomaly)
            / (1. - Eccentricity*cos(E));
        E -= Error;
    }
    while (fabs(Error) >= 0.000001);
    
    if (fabs(E-M_PI) < 0.000001)
        TrueAnomaly = M_PI;
    else
        TrueAnomaly = 2.*atan(sqrt((1.+Eccentricity)/(1.-Eccentricity))
        *tan(E/2.));
    if (TrueAnomaly < 0)
        TrueAnomaly += 2.*M_PI;
    
    return TrueAnomaly;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
JulianDate EarthOrbit::dateFromSeconds(double seconds)const{
    return JDStart+(seconds/SecondsPerDay);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double EarthOrbit::phase(JulianDate jd) const
{
    double elapse = (jd - JDStart)*SecondsPerDay;
    return m_M0+m_dMdt*elapse;
}

// Calculates the Shapiro delay due to gravitational relativistic effects on the 
// photon arrival times.
double EarthOrbit::calcShapiroDelay(JulianDate jd, const SkyDir &sourceDir) const
{
   SolarSystem::Body sunID = (SolarSystem::Body) 8;

   SolarSystem sun;
   SkyDir sunDir = sun.direction(sunID,jd);

   Hep3Vector rsun = sunDir.dir();
   Hep3Vector rsrc = sourceDir.dir();

   // Angle of source-sun-observer
   double costheta = - rsun.dot(rsrc) / ( rsun.mag() * rsrc.mag() );

   // m = G * Msun / c^3
   static double m = 4.9271e-6;
   return 2 * m * log(1+costheta);
}

// Takes a Julian Date and a source direction and computes the light travel time to the
// solar system barycenter.
double EarthOrbit::calcTravelTime(JulianDate jd, const SkyDir &sourceDir) const
{
   SolarSystem solsys;
   Hep3Vector barycenter;
   double correction = 0;
   double last_correction = 1e20; // impossible value to force loop to iterate more than once
   double eps = 1e-6;  // 1 microsecond precision
   bool precision_not_met = true;

   for(int i = 0; precision_not_met && i < 10; i++)
   {
      barycenter = solsys.getBarycenter(jd+correction/86400.);

      // Add location of satellite with respect to the earth to the barycenter vector;
      barycenter -= (EarthOrbit::position(jd+correction/86400.)/299792.458);

      Hep3Vector rsrc = sourceDir.dir();

      correction = -1. * barycenter.dot(rsrc) / (rsrc.mag() /** 299792458.*/);

      if(correction - last_correction < eps)
         precision_not_met = false;
      else
         last_correction = correction;
   }

   if(precision_not_met)
      std::cout << "Warning:  Desired precision not met in calculation of barycentric correction" << std::endl;

   return correction;
}


// Takes a Julian Date and returns the correction that is added to TT to get TDB.
// Does a linear interpolation to avoid recomputing 100+ sin functions each time
// the TDB-TT correction is needed.
double EarthOrbit::tdb_minus_tt(JulianDate jd) const
{
   static double tdb_tt;
   static double tdb_tt_dot;
   static long oldJD = 0;

   // If value has not been computed for this Julian date, then recompute.
   if( (int) floor(jd) != oldJD )
   {
      oldJD = (int) floor(jd);

      tdb_tt = ctatv(oldJD,0.0);
      tdb_tt_dot = ctatv(oldJD+1,0.0) - ctatv(oldJD, 0.0);
   }

   return (tdb_tt + (jd - (double) oldJD) * tdb_tt_dot);
}


/*  Taken from FTOOLS AXBARY
 *  This function computes the correction for TDB-TT.
 *  Modification by T. Hierath:  removed unused variable t31
 *----------------------------------------------------------------------
 *
 *     Routine name: CTATV
 *
 *     Programmer and Completion Date:
 *        Lloyd Rawley - S.T.X. - 07/28/89
 *        (Retyped by Masaki Mori - 04/19/96)
 *        (Converted to C for bary, optimized,
 *         corrected following Faithead & Bretagnon 1990, A&A 229, 240
 *         by Arnold Rots - 1997-11-20)
 *
 *     Function: Computes the cumulative relativistic time correction to
 *               earth-based clocks, TDB-TDT, for a given time. Routine
 *               furnished by the Bureau des Longitudes, modified by
 *               removal of terms much smaller than 0.1 microsecond.
 *
 *     Calling Sequence:  tdbdt = ctacv(jdno, fjdno)
 *        Argument   Type   I/O                Description
 *        --------   ----   ---  ----------------------------------------
 *        jdno      long     I   Julian day number of lookup
 *        fjdno     double   I   Fractional part of Julian day number
 *        tdbtdt    double   O   Time difference TDB-TDT (seconds)
 *
 *     Called by:  TTtoTDB
 *
 *     Calls:  none
 *
 *     COMMON use:  none
 *
 *     Significant Local Variables:
 *        Variable   Type   Ini. Val.        Description
 *        --------   ----   ---------  ----------------------------------
 *          t       double      -      Time since 2000.0 (millennia)
 *          tt      double      -      Square of T
 *
 *     Logical Units Used:  none
 *
 *     Method:
 *        Convert input time to millennia since 2000.0
 *        For each sinusoidal term of sufficient amplitude
 *           Compute value of term at input time
 *        End for
 *        Add together all terms and convert from microseconds to seconds
 *     End CTATV
 *
 *     Note for the retyped version:
 *        Results of this routine has been confirmed up to (1E-10)microsecond
 *        level compared with the calculation by the IBM machine: this seems
 *        to be within the 64-bit precision allowance, but still the original
 *        hardcopy should be kept as the right one. (M.M.)
 *
 *     Note for the C version: the accuracy is guaranteed to 100 ns.
 *
 *---------------------------------------------------------------------------*/

double EarthOrbit::ctatv(int long jdno, double fjdno) const
{

  double t, tt, t1, t2, t3, t4, t5, t24, t25, t29, t30;

      t = ((jdno-2451545) + fjdno)/(365250.0) ;
      tt = t*t ;

      t1  =       1656.674564 * sin(  6283.075943033*t + 6.240054195)
           +        22.417471 * sin(  5753.384970095*t + 4.296977442)
           +        13.839792 * sin( 12566.151886066*t + 6.196904410)
           +         4.770086 * sin(   529.690965095*t + 0.444401603)
           +         4.676740 * sin(  6069.776754553*t + 4.021195093)
           +         2.256707 * sin(   213.299095438*t + 5.543113262)
           +         1.694205 * sin(    -3.523118349*t + 5.025132748)
           +         1.554905 * sin( 77713.772618729*t + 5.198467090)
           +         1.276839 * sin(  7860.419392439*t + 5.988822341)
           +         1.193379 * sin(  5223.693919802*t + 3.649823730)
           +         1.115322 * sin(  3930.209696220*t + 1.422745069)
           +         0.794185 * sin( 11506.769769794*t + 2.322313077)
           +         0.600309 * sin(  1577.343542448*t + 2.678271909)
           +         0.496817 * sin(  6208.294251424*t + 5.696701824)
           +         0.486306 * sin(  5884.926846583*t + 0.520007179)
           +         0.468597 * sin(  6244.942814354*t + 5.866398759)
           +         0.447061 * sin(    26.298319800*t + 3.615796498)
           +         0.435206 * sin(  -398.149003408*t + 4.349338347)
           +         0.432392 * sin(    74.781598567*t + 2.435898309)
           +         0.375510 * sin(  5507.553238667*t + 4.103476804) ;

      t2  =          0.243085 * sin(  -775.522611324*t + 3.651837925)
           +         0.230685 * sin(  5856.477659115*t + 4.773852582)
           +         0.203747 * sin( 12036.460734888*t + 4.333987818)
           +         0.173435 * sin( 18849.227549974*t + 6.153743485)
           +         0.159080 * sin( 10977.078804699*t + 1.890075226)
           +         0.143935 * sin(  -796.298006816*t + 5.957517795)
           +         0.137927 * sin( 11790.629088659*t + 1.135934669)
           +         0.119979 * sin(    38.133035638*t + 4.551585768)
           +         0.118971 * sin(  5486.777843175*t + 1.914547226)
           +         0.116120 * sin(  1059.381930189*t + 0.873504123)
           +         0.101868 * sin( -5573.142801634*t + 5.984503847)
           +         0.098358 * sin(  2544.314419883*t + 0.092793886)
           +         0.080164 * sin(   206.185548437*t + 2.095377709)
           +         0.079645 * sin(  4694.002954708*t + 2.949233637)
           +         0.075019 * sin(  2942.463423292*t + 4.980931759)
           +         0.064397 * sin(  5746.271337896*t + 1.280308748)
           +         0.063814 * sin(  5760.498431898*t + 4.167901731)
           +         0.062617 * sin(    20.775395492*t + 2.654394814)
	   +         0.058844 * sin(   426.598190876*t + 4.839650148)
           +         0.054139 * sin( 17260.154654690*t + 3.411091093) ;

      t3  =          0.048373 * sin(   155.420399434*t + 2.251573730)
           +         0.048042 * sin(  2146.165416475*t + 1.495846011)
           +         0.046551 * sin(    -0.980321068*t + 0.921573539)
           +         0.042732 * sin(   632.783739313*t + 5.720622217)
           +         0.042560 * sin(161000.685737473*t + 1.270837679)
           +         0.042411 * sin(  6275.962302991*t + 2.869567043)
           +         0.040759 * sin( 12352.852604545*t + 3.981496998)
           +         0.040480 * sin( 15720.838784878*t + 2.546610123)
           +         0.040184 * sin(    -7.113547001*t + 3.565975565)
           +         0.036955 * sin(  3154.687084896*t + 5.071801441)
           +         0.036564 * sin(  5088.628839767*t + 3.324679049)
           +         0.036507 * sin(   801.820931124*t + 6.248866009)
           +         0.034867 * sin(   522.577418094*t + 5.210064075)
           +         0.033529 * sin(  9437.762934887*t + 2.404714239)
           +         0.033477 * sin(  6062.663207553*t + 4.144987272)
           +         0.032438 * sin(  6076.890301554*t + 0.749317412)
           +         0.032423 * sin(  8827.390269875*t + 5.541473556)
           +         0.030215 * sin(  7084.896781115*t + 3.389610345)
           +         0.029862 * sin( 12139.553509107*t + 1.770181024)
           +         0.029247 * sin(-71430.695617928*t + 4.183178762) ;

      t4  =          0.028244 * sin( -6286.598968340*t + 5.069663519)
	   +         0.027567 * sin(  6279.552731642*t + 5.040846034)
           +         0.025196 * sin(  1748.016413067*t + 2.901883301)
           +         0.024816 * sin( -1194.447010225*t + 1.087136918)
           +         0.022567 * sin(  6133.512652857*t + 3.307984806)
           +         0.022509 * sin( 10447.387839604*t + 1.460726241)
           +         0.021691 * sin( 14143.495242431*t + 5.952658009)
           +         0.020937 * sin(  8429.241266467*t + 0.652303414)
           +         0.020322 * sin(   419.484643875*t + 3.735430632)
           +         0.017673 * sin(  6812.766815086*t + 3.186129845)
           +         0.017806 * sin(    73.297125859*t + 3.475975097)
           +         0.016155 * sin( 10213.285546211*t + 1.331103168)
           +         0.015974 * sin( -2352.866153772*t + 6.145309371)
           +         0.015949 * sin(  -220.412642439*t + 4.005298270)
           +         0.015078 * sin( 19651.048481098*t + 3.969480770)
           +         0.014751 * sin(  1349.867409659*t + 4.308933301)
           +         0.014318 * sin( 16730.463689596*t + 3.016058075)
           +         0.014223 * sin( 17789.845619785*t + 2.104551349)
           +         0.013671 * sin(  -536.804512095*t + 5.971672571)
           +         0.012462 * sin(   103.092774219*t + 1.737438797) ;

      t5  =          0.012420 * sin(  4690.479836359*t + 4.734090399)
           +         0.011942 * sin(  8031.092263058*t + 2.053414715)
           +         0.011847 * sin(  5643.178563677*t + 5.489005403)
           +         0.011707 * sin( -4705.732307544*t + 2.654125618)
           +         0.011622 * sin(  5120.601145584*t + 4.863931876)
           +         0.010962 * sin(     3.590428652*t + 2.196567739)
           +         0.010825 * sin(   553.569402842*t + 0.842715011)
           +         0.010396 * sin(   951.718406251*t + 5.717799605)
           +         0.010453 * sin(  5863.591206116*t + 1.913704550)
           +         0.010099 * sin(   283.859318865*t + 1.942176992)
           +         0.009858 * sin(  6309.374169791*t + 1.061816410)
           +         0.009963 * sin(   149.563197135*t + 4.870690598)
           +         0.009370 * sin(149854.400135205*t + 0.673880395) ;

      t24 = t * (  102.156724 * sin(  6283.075849991*t + 4.249032005)
           +         1.706807 * sin( 12566.151699983*t + 4.205904248)
           +         0.269668 * sin(   213.299095438*t + 3.400290479)
           +         0.265919 * sin(   529.690965095*t + 5.836047367)
           +         0.210568 * sin(    -3.523118349*t + 6.262738348)
           +         0.077996 * sin(  5223.693919802*t + 4.670344204) ) ;

      t25 = t * (    0.059146 * sin(    26.298319800*t + 1.083044735)
           +         0.054764 * sin(  1577.343542448*t + 4.534800170)
           +         0.034420 * sin(  -398.149003408*t + 5.980077351)
           +         0.033595 * sin(  5507.553238667*t + 5.980162321)
           +         0.032088 * sin( 18849.227549974*t + 4.162913471)
           +         0.029198 * sin(  5856.477659115*t + 0.623811863)
           +         0.027764 * sin(   155.420399434*t + 3.745318113)
           +         0.025190 * sin(  5746.271337896*t + 2.980330535)
           +         0.024976 * sin(  5760.498431898*t + 2.467913690)
	   +         0.022997 * sin(  -796.298006816*t + 1.174411803)
           +         0.021774 * sin(   206.185548437*t + 3.854787540)
           +         0.017925 * sin(  -775.522611324*t + 1.092065955)
           +         0.013794 * sin(   426.598190876*t + 2.699831988)
           +         0.013276 * sin(  6062.663207553*t + 5.845801920)
           +         0.012869 * sin(  6076.890301554*t + 5.333425680)
           +         0.012152 * sin(  1059.381930189*t + 6.222874454)
           +         0.011774 * sin( 12036.460734888*t + 2.292832062)
           +         0.011081 * sin(    -7.113547001*t + 5.154724984)
           +         0.010143 * sin(  4694.002954708*t + 4.044013795)
           +         0.010084 * sin(   522.577418094*t + 0.749320262)
           +         0.009357 * sin(  5486.777843175*t + 3.416081409) ) ;

      t29 = tt * (   0.370115 * sin(                     4.712388980)
           +         4.322990 * sin(  6283.075849991*t + 2.642893748)
           +         0.122605 * sin( 12566.151699983*t + 2.438140634)
           +         0.019476 * sin(   213.299095438*t + 1.642186981)
           +         0.016916 * sin(   529.690965095*t + 4.510959344)
           +         0.013374 * sin(    -3.523118349*t + 1.502210314) ) ;

      t30 = t * tt * 0.143388 * sin( 6283.075849991*t + 1.131453581) ;

      return (t1+t2+t3+t4+t5+t24+t25+t29+t30) * 1.0e-6 ;
}


}
