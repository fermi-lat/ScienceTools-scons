
#ifndef OrbitModel_JulianDate_H
#define OrbitModel_JulianDate_H

namespace astro {
/**
*  @class JulianDate
*
*
*  @brief store a Julian Date
*  Julian dates (abbreviated JD) are a continuous 
*   count of days and fractions since noon Universal Time on January 1, 4713 BCE 
*    (on the Julian calendar).
*  @author Gino Tosti (primary)
*  @author Toby Burnett (convert to a class)
*  <hr>$Id$ 
*/
class JulianDate {
public:

    /** Initialize from:
    * @param An year
    * @param Me month
    * @param Gio day
    * @param utc hours
    */
    JulianDate(int An,int Me,int Gio,double utc);

    //! conversion constructor
    JulianDate(double jd):m_JD(jd){}

    //! allow to be treated as a double, for arithmetic for example
    operator double()const{return m_JD;}

    double seconds()const{ return m_JD* secondsPerDay;}

    enum{ secondsPerDay = 60*60*24 };
    
private:
    double m_JD;
};

inline JulianDate::JulianDate(int An,int Me,int Gio,double utc)
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
}
} // astro
#endif