// $Header$
#include <cmath>

#include "astro/EarthCoordinate.h"

namespace {
    
    double EarthFlat= (1/298.25);            /* Earth Flattening Coeff. */
    double J2000= astro::JulianDate(2000,1,1,12); // 2451545.0;
 
    inline double sqr(double x){return x*x;}
}

// static constants 
namespace astro {
double EarthCoordinate::s_EarthRadius = 6378145.; //m


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EarthCoordinate::EarthCoordinate(Hep3Vector pos, JulianDate jd)
{
    m_lat = M_PI/2- pos.theta();
    double GMST0 = GetGMST(jd);
    m_lon = GetGMST(jd)*M_PI/180 - pos.phi();
    m_lon = fmod(m_lon, 2*M_PI); if(m_lon>M_PI) m_lon -= 2*M_PI;

    // oblateness correction to obtain geodedic latitude 
    m_lat=(atan(tan(m_lat))/(sqr(1.-EarthFlat)) );

    // this is also such a correction: the number 0.00669454 is the geodetic eccentricity squared?
    // see http://www.cage.curtin.edu.au/~will/gra64_05.pdf
    // or http://www.colorado.edu/geography/gcraft/notes/datum/gif/ellipse.gif
    m_altitude=sqrt(sqr(pos.x())+sqr(pos.y()))/cos(m_lat)
        -s_EarthRadius / (1000.*sqrt(1.-sqr(0.00669454*sin(m_lat))));

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EarthCoordinate::EarthCoordinate(double latDeg, double lonDeg, double alt)
: m_lat(latDeg*M_PI/180), m_lon(lonDeg*M_PI/180), m_altitude(alt)
{}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double  EarthCoordinate::GetGMST(JulianDate jd)
{
    double J_D=jd;
    double M, Ora_Un_Dec=modf(J_D-0.5,&M)*24;  J_D-=Ora_Un_Dec/24;
    double T = (J_D - J2000) / 36525.;
    double T1 = (24110.54841 + 8640184.812866 * T + 0.0093103 * T * T)/86400.0;
    double Tempo_Siderale_0 = modf(T1,&M) * 24.;
    double Tempo_Siderale_Ora = Tempo_Siderale_0 + Ora_Un_Dec * 1.00273790935;
    if (Tempo_Siderale_Ora < 0.) Tempo_Siderale_Ora = Tempo_Siderale_Ora + 24.;
    if (Tempo_Siderale_Ora >= 24.) Tempo_Siderale_Ora = Tempo_Siderale_Ora - 24.;
    return Tempo_Siderale_Ora*15.;  
}  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool EarthCoordinate::insideSAA()const
{
    double perim[7][2] = {{-88.,-30.},{-88.,-12.},{-55.,-0.1},{-32.,-0.1},{-7,-12},
    {50.,-25},{50.,-30.}};
    double longmin=-88;
    double longmax=50.;
    double latmin=-30;
    double latmax=-0.1;
    double diffx[7],diffy[7],diffd[7];
    double lon=longitude(), lat=latitude();
    if((lon>=longmin)&&(lon<=longmax)&&(lat>=latmin)&&(lat<=latmax)){
        for (int i=0;i<6;i++){
            diffx[i]=perim[i+1][0]-perim[i][0];
            diffy[i]=perim[i+1][1]-perim[i][1];
            //cout<<i<<"  "<<diffx[i]<<endl;
        }
        diffx[6]=perim[0][0]-perim[6][0];
        diffy[6]=perim[0][1]-perim[6][1];
        
        for (i=0;i<7;i++){
            diffd[i]=sqrt(sqr(diffx[i])+sqr(diffy[i]));
        }
        double xnew[7],ynew[7];
        int inside[7];
        int xtemp[7],txtemp=0,tins=0;
        for (i=0;i<7;i++){
            xnew[i]=((lon-perim[i][0])*diffx[i]+(lat-perim[i][1])*diffy[i])/diffd[i];
            ynew[i]=(lat-perim[i][1])*diffx[i]-(lon-perim[i][0])*diffy[i];
            xtemp[i]=((xnew[i] >=0.)&&(xnew[i]<=diffd[i]));
            inside[i]=(xtemp[i] == 1) && (ynew[i]<0.);
            txtemp+=xtemp[i];
            tins+=inside[i];
            //cout<<diffd[i]<<endl;
        }
        bool insaa=(txtemp==tins)&&(txtemp >0);
        
        return insaa;
    }
    else
        return false;
}

} // namespace astro
