
#include <string>
#include <cmath>
#include <iostream>
#include <fstream> 

#include "astroutil.h"
#include "SolSystem.h"

#define comparison
#ifdef comparison // for comparison
#include "astro/JulianDate.h"
#include "astro/EarthOrbit.h"
#include "astro/SolarSystem.h"
#include "astro/EarthCoordinate.h"
#include <cassert>
#endif
#define zenmax	105.F
#define Ao	13000.F // cm^2 maximum area at 0 angle incidence

main(){
    
    using namespace std;
    //source coordinates
    
    double ra=0;
    double altitude = 550.e3  ; //m
    double incl = 28.5; // deg
    double e = 0. ;
    double rock = 35.; 
    double delt = 30.; 
    double delthmin = 5.;
    double delthmax = 10.;
    double ddec=2.0;
    long Norb=0;
    
    const double TliveEff=0.90;
    //Time
    const double MissionStartTime =GetJD(2005,1,1,0.0);
    double JDStart=GetJD(2005.,7,18,0.0);
    double StartSimDate=(GetJD(2005.,7,18,0.0)-MissionStartTime)*SecondsPerDay;
    double GMST0=GetGMST(GetJD(2005.,7,18,0.0));
    double EndSimDate=(GetJD(2005,7,19,0.0)-MissionStartTime)*SecondsPerDay;
    double SimDurationTime =(EndSimDate-StartSimDate);
    
    
    double sini = sin(incl*D2R);
    double cosi = cos(incl*D2R);
    
    
    double Rearth = R_TERRA  ; 
    double alt=(Rearth + altitude)/1000.;
    double a = (Rearth + altitude)/Rearth; 
    
    double T = PI2*pow(a,1.5)/sqrt(5.98e24*6.67e-11)*pow(Rearth,1.5)  ; 	
    
    double u = 3.9860044e14/pow(Rearth,3)  ; 
    double n = sqrt(u / pow(a,3));
    double n1 = n*(1. + 3.*J2/2.*sqrt(1. - e*e)/(a*a)/pow((1. - e*e),2)*(1.-1.5*sini*sini));
    double dwdt = n1*3.*J2/2./(a*a)/pow((1. - e*e),2)*(2. - 2.5*sini*sini);
    
    double dOmegadt = -n1*3.*J2/2./(a*a)/pow((1. - e*e),2)*cosi;
    
    double dMdt = n1;
    
    //double duration = 2.*PI/fabs(dOmegadt); 
    double duration = SimDurationTime;
    
    long Nsteps = (long)(duration/delt+0.5) + 1L;
    
    double Tlive=0.;
    
    // Initialize the elapsed time
    double elapse = 0.;
    
    // Convergence tolerance for determining the eccentric anomaly below
    int IsSAA = 0; 
    
    double M0=dMdt*StartSimDate;
    range(&M0,6.28);
    double Omega0 = dOmegadt*StartSimDate;
    double w0 = dwdt*StartSimDate;
    double M,Omega,w;
    double StartDate, EndDate;
    //Nsteps=4;
    ofstream out("orbit2.out");
    SolSystem Sun, Moon;
    Sun.SetLocation(0.,0.,0.);
    Moon.SetLocation(0.,0.,0.);
    double SunRa;//
    double SunDec;//
    double MoonRa;//
    double MoonDec;//
    double longitude;
    double latitude;
    double LATraZ;
    double LATDecZ;
    double LATraX;
    double LATDecX;
    double SCPos[3];
    double SCLat,SCLon,SCAlt;
    double RaZ,DecZ;

#ifdef comparison
    astro::EarthOrbit orbit; 
#endif

    for (long ti = 0;ti<(Nsteps-1);ti++){
        if( ti%100 ==0) cout << ti << endl;
        elapse = elapse + delt;
        StartDate=StartSimDate+elapse;
        JDStart+=(delt/86400.);
        GMST0=GetGMST(JDStart);
        EndDate=StartDate+delt;
        
        Sun.SetObj(SUN);
        Sun.CalculatePos(JDStart);
        Moon.SetObj(MOON);
        Moon.CalculatePos(JDStart);

        SunRa=Sun.GRa;
        SunDec=Sun.GDec;
        MoonRa=Moon.GRa;
        MoonDec=Moon.DDec;
        
        M=M0+dMdt*elapse;
        range(&M,6.28);
        Omega = Omega0+dOmegadt*elapse;
        range(&Omega,6.28);
        w = w0+dwdt*elapse;
        double Enew;
        
        Enew=Kepler(M,e);
        
        double pp[3],location[3],p[3][3],ppr[3],pointdir[3],zenitdir[3];
        double a1[3] = {a*(cos(Enew) - e),a*sqrt(1.- e*e) * sin(Enew),0};
        calc_unit_vector(a1, pp);
        double cosOmega = cos(Omega)  ; 
        double sinOmega = sin(Omega);
        double cosw = cos(w) ;         
        double sinw = sin(w);
        
        p[0][0] = cosw*cosOmega - sinw*sinOmega*cosi;
        p[0][1] = cosw*sinOmega + sinw*cosOmega*cosi;
        p[0][2] = sinw*sini;
        p[1][0] = -sinw*cosOmega - cosw*sinOmega*cosi;
        p[1][1] = -sinw*sinOmega + cosw*cosOmega*cosi;
        p[1][2] = cosw*sini;
        p[2][0] = sinOmega*sini;
        p[2][1] = -cosOmega*sini;
        p[2][2] = cosi;
        
        vector_matrix_multiply(pp,p,location);
        memcpy(zenitdir,location,3*sizeof(double));
        SCPos[0]= zenitdir[0]*alt;
        SCPos[1]= zenitdir[1]*alt;
        SCPos[2]= zenitdir[2]*alt;
        
        longitude=atan2(location[1],location[0])*R2D;
        RaZ=longitude;
        if(RaZ<0.)RaZ+=360;
        //longitude = (longitude +(0.25068447/60.*elapse*SidSolar) +(GMST0+elapse*SidSolar*15./3600.); //verificare 1.00273790935
        longitude=(GMST0-RaZ);
        if(longitude>180.)longitude-=360.;
        if(longitude<-180.)longitude+=360;
        
        latitude=asin(location[2]);
        DecZ=latitude*R2D;
        latitude=(atan(tan(latitude))/((1.-EarthFlat)*(1.-EarthFlat)) );
        
        SCAlt=sqrt(SCPos[0]*SCPos[0]+SCPos[1]*SCPos[1])/cos(latitude)-Rearth/(1000.*sqrt(1.-0.00669454*0.00669454*sin(latitude)*sin(latitude)));
        SCLat=latitude*R2D;
        Tlive=delt*TliveEff;
        IsSAA=InsideSAA(longitude,latitude);
        SCLon=longitude;
        int temp = (int)(elapse/T); 
        //range(&longitude,360.);
   
#ifdef comparison
        // temporary to compare with copy
        Hep3Vector pos = orbit.position(astro::JulianDate(JDStart));
        Hep3Vector posdiff(pos.x()-SCPos[0], pos.y()-SCPos[1], pos.z()-SCPos[2]);
        double absdiff=posdiff.mag();

        astro::EarthCoordinate epos(pos, JDStart);
        double lat=epos.latitude(), lon=epos.longitude(), alt=epos.altitude();

        astro::SkyDir sun = astro::SolarSystem(astro::SolarSystem::Sun, JDStart );
        astro::SkyDir moon = astro::SolarSystem(astro::SolarSystem::Moon, JDStart );

        double sun_ra = sun.ra(), sun_dec= sun.dec();
        double moon_ra = moon.ra(), moon_dec = moon.dec();

        bool test =  absdiff< 0.050 && fabs(lat-SCLat) < 0.2 && fabs(lon-SCLon)<0.2 ;
        if( !test){
            cout << "failed: elapse=" << elapse << endl;
        }
#endif
        GetRockMat(pp,rock,temp,ppr);
        vector_matrix_multiply(ppr,p,pointdir);
        
        LATraZ=atan2(pointdir[1],pointdir[0])*R2D;
        LATraZ = LATraZ; 
        range(&LATraZ,360.);
        LATDecZ=asin(pointdir[2])*R2D;
        
        LATDecX=LATDecZ; //+0.25068447/60.*elapse* +GMST0;
        LATraX=LATraZ-90;
        range(&LATraX,360.);
        out.flags(ios::fixed);
        
        out<<StartDate <<'\t';
        out<<EndDate <<'\t';
        out<< SCPos[0]<<'\t';
        out<< SCPos[1]<<'\t';
        out<< SCPos[2]<<'\t';
        out<<LATraZ<<'\t';
        out<<LATDecZ<<'\t';
        out<<LATraX<<'\t';
        out<<LATDecX<<'\t';
        out<<RaZ<<"\t";
        out<<DecZ <<'\t';
        out<<"1"<<'\t';
        out<<Tlive <<'\t';
        out<<IsSAA<<'\t';
        out<<SCLon <<'\t';
        out<<SCLat <<'\t';
        out<<SCAlt <<'\t';
        out<< SunRa <<"\t"<<SunDec  <<'\t';
        out<<MoonRa <<"\t"<<MoonDec   <<endl;
        
                                }//time loop
                                
                                out.close();
                                return 0;
}




//Contents Units
;
/*out<<"1 Start (Mission Elapsed Time) s: "<<StartDate <<endl;cout<<"2 End (Mission Elapsed Time) s :"<<EndDate <<endl;
out<<"3 position of S/C at start of interval (x,inertial coordinates) km "<<  zenitdir[0]*alt/1000. <<endl;
out<<"3 position of S/C at start of interval (y,inertial coordinates) km "<<  zenitdir[1]*alt/1000.<<endl;
out<<"3 position of S/C at start of interval (z,inertial coordinates) km "<<  zenitdir[2]*alt/1000.<<endl;
//cout<<"4 viewing direction at start (LAT +z axis), 2 angles deg"<< <<endl;
//cout<<"5 orientation at start (LAT +x axis), 2 angles deg"<< <<endl;
out<<"6 zenith direction at start, 2 angles deg "<<" "<<longitude<<" "<<latitude <<endl;
out<<"7 LAT operation mode dimensionless: "<<"1"<<endl;
out<<"8 livetime s:"<<Tlive <<endl;
out<<"9 SAA flag dimensionless :"<<saa_count<<endl;
out<<"10 S/C longitude deg :"<<longitude <<endl;
out<<"11 S/C latitude deg :"<<latitude <<endl;
out<<"12 S/C altitude km :"<<alt/1000. <<endl;
out<<"13 direction of the sun, 2 angles Deg :"<< SunRa <<" "<<SunDec  <<endl;
out<<"14 direction of the moon, 2 angles deg: "<<MoonRa <<" "<<MoonDec   <<endl;*/

