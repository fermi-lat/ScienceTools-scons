
#ifndef OrbitModel_JulianDate_H
#define OrbitModel_JulianDate_H

#include <stdio.h>
#include <math.h>
#include <string>

namespace astro {
/**
*  @class JulianDate
*
*  @brief store a Julian Date
*
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
    void getGregorianDate(int &An, int &Me, int &Gio, double &utc);
    std::string getGregorianDate(void);

    //! conversion constructor
    JulianDate(double jd):m_JD(jd){}

    //! allow to be treated as a double, for arithmetic for example
    operator double()const{return m_JD;}

    double seconds()const{ return m_JD* secondsPerDay;}

    enum{ secondsPerDay = 60*60*24 };
    
private:
    double m_JD;
};



} // astro
#endif
