/** @file JulianDate.cxx
    @brief JulianDate implementation 

    $Header$
*/
#include "astro/JulianDate.h"

#include <stdio.h>
#include <cmath>

namespace {
  // getGregorianDate
   // Adapted from DAYCNV in the astronomy IDL library
   // http://idlastro.gsfc.nasa.gov/ftp/pro/astro/daycnv.pro
   //
   // Appears to give better than .00004 second consistency with JulianDate() over time 
   // interval 2002 to 2020.

    void gregDate(double JD, int &An, int &Me, int &Gio, double &utc)
   {
      double jd;
      double frac, hr;
      int yr, mn, day, l, n;

      jd = int(JD);       // Truncate to integral day
      frac = JD - jd + 0.5;    // Fractional part of calendar day

      if(frac >= 1.0)      // Is it really the next calendar day?
      {
         frac = frac - 1.0;
         jd = jd + 1.0;
      }

      hr = frac*24.0;
      l = int(jd + 68569);
      n = 4*l / 146097l;
      l = l - (146097*n + 3l) / 4;
      yr = 4000*(l+1) / 1461001;
      l = l - 1461*yr / 4 + 31;        // 1461 = 365.25 * 4
      mn = 80*l / 2447;
      day = l - 2447*mn / 80;
      l = mn/11;
      mn = mn + 2 - 12*l;
      yr = 100*(n-49) + yr + l;
      An = yr;   
      Me = mn;   
      Gio = day;  
      utc = hr;
   }

   // leap seconds applied after these JD times
   astro::JulianDate leaptime[]={
       astro::JulianDate(2006,1,1,1./3600.), 
       astro::JulianDate(2009,1,1,1./3600.)};
}

namespace astro{

   JulianDate::JulianDate(int An,int Me,int Gio,double utc)
   {

      if (Me > 2);
      else {
         An = An - 1;
         Me = Me + 12;
      }
      int A = (An / 100); 
      int B = 2 - A + (A / 4);
      long int C = (long int)(365.25 * An); 
      if (An < 0) C = C - 1;
      int D = (int)(30.6001 * (Me + 1));
      m_JD = B + C + D + Gio + 1720994.5+ utc / 24.;
        
      double leap(0);
      // add in leap seconds
      if (m_JD>leaptime[0]) leap+= 1./secondsPerDay; 
      if (m_JD>leaptime[1]) leap+= 1./secondsPerDay; 
       m_JD += leap;
   }

   void JulianDate::getGregorianDate(int &An, int &Me, int &Gio, double &utc) const
   {
       double JD(m_JD);

       // correct for leap seconds here
       if( m_JD>leaptime[0])  JD-=1./secondsPerDay;
       if( m_JD>leaptime[1])  JD-=1./secondsPerDay;

       gregDate(JD, An, Me, Gio,utc);
   }

   std::string JulianDate::getGregorianDate(void) const
   {
      int year, month, day, hour, minute, second, sec_deci;
      double utc;
      char buffer[256];

      getGregorianDate(year,month,day,utc);

      hour = int(floor(utc));
      minute = int(floor(60*(utc-hour)));
      second = int(floor(60*(60*(utc-hour) - minute)));
      sec_deci = int(floor((60*(60*(utc-hour) - minute) - floor(60*(60*(utc-hour) - minute)))*10000));

      sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%04d", year, month, day, hour, minute, second, sec_deci);   

      return std::string(buffer);
   }

}
