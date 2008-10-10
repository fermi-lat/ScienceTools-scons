/** @file Quaternion.cxx
@brief implement class Quaternion

$Header$

*/

#include "astro/Quaternion.h"
#include "astro/SkyDir.h" // only for test

#include <stdexcept>

using namespace astro;
using namespace CLHEP;

Quaternion::Quaternion(const CLHEP::Hep3Vector& v, double s)
:m_v(v), m_s(s)
{
    // force identity
    if( fabs(m_s)>1.){
        m_s=1.; 
        m_v=Hep3Vector(0,0,0);
    }
}

Quaternion::Quaternion(const CLHEP::HepRotation& R)
: m_v(Hep3Vector(0,0,0))
, m_s(1)
{
    // code mostly from ROOT's TRotation::AngleAxis. 
    double cosa  = 0.5*(R.xx()+R.yy()+R.zz()-1);
    double cosa1 = 1-cosa;
    if (cosa1 >0 ){
        double x=0, y=0, z=0;
        if (R.xx() > cosa) x = sqrt((R.xx()-cosa)/cosa1);
        if (R.yy() > cosa) y = sqrt((R.yy()-cosa)/cosa1);
        if (R.zz() > cosa) z = sqrt((R.zz()-cosa)/cosa1);
        if (R.zy() < R.yz())  x = -x;
        if (R.xz() < R.zx())  y = -y;
        if (R.yx() < R.xy())  z = -z;
        m_s = sqrt(0.5*(1+cosa));
        m_v = Hep3Vector(x,y,z)*sqrt(0.5*cosa1);;
    }
}
Quaternion::Quaternion(const CLHEP::Hep3Vector& zhat, const CLHEP::Hep3Vector& xhat)
: m_v(Hep3Vector(0,0,0)), m_s(1)
{
    // note no check that they are unit vectors 
    double check( zhat.dot(xhat) ); // should be very small
    if( fabs(check) >5e-6){ // corresponds to 1 arc-sec
        std::cerr << "Quaternion ctor: fail 1 arc-sec orthogonality requirement, dot product = " << check << std::endl;
        throw std::invalid_argument("Quaternion ctor: fail orthogonality");
    }
    // correct orthogonality by adjusting x
    Hep3Vector xhatp( (xhat- check*zhat).unit());
    Hep3Vector yhat(zhat.cross(xhatp));
    // code mostly from ROOT's TRotation::AngleAxis. 
    double cosa  = 0.5*(xhat.x()+yhat.y()+zhat.z()-1);
    if( cosa < -1.) cosa=-1; if(cosa>1.) cosa=1.; // prevent sqrt errors
    double cosa1 = 1-cosa;
    if (cosa1 >0 ){
        double x=0, y=0, z=0;
        if (xhat.x() > cosa) x = sqrt((xhat.x()-cosa)/cosa1);
        if (yhat.y() > cosa) y = sqrt((yhat.y()-cosa)/cosa1);
        if (zhat.z() > cosa) z = sqrt((zhat.z()-cosa)/cosa1);
        if (zhat.y() > yhat.z())  x = -x;
        if (xhat.z() > zhat.x())  y = -y;
        if (yhat.x() > xhat.y())  z = -z;
        m_s = sqrt(0.5*(1+cosa));
        m_v = Hep3Vector(x,y,z)*sqrt(0.5*cosa1);;
    }
}

Quaternion Quaternion::operator* (const Quaternion & r) const
{
    Hep3Vector pv= m_v.cross(r.m_v) + m_s*r.m_v + m_v*r.m_s;
    double ps = m_s*r.m_s - m_v*r.m_v;

    return Quaternion(pv,ps);
}

Quaternion Quaternion::operator* (const CLHEP::Hep3Vector & vp) const
{
    Hep3Vector pv= m_v.cross(vp) + m_s*vp;
    double ps = -m_v*vp;

    return Quaternion(pv,ps);
}

Quaternion operator* (const CLHEP::Hep3Vector & v, const Quaternion & q)
{
    Hep3Vector pv= v.cross(q.vector())  + v*q.scalar();
    double ps =- v*q.vector();

    return Quaternion(pv,ps);
}

HepRotation Quaternion::rotation()const
{
    /// create rotation matrix
    double s(m_s), x(m_v.x()), y(m_v.y()), z(m_v.z());
    Hep3Vector 
        xcol(s*s+x*x-y*y-z*z, 2*(x*y+s*z),     2*(x*z-s*y) ),
        ycol(2*(x*y-s*z),     s*s+y*y-x*x-z*z, 2*(y*z+s*x) ) ,
        zcol(2*(x*z+s*y),     2*(z*y-s*x),     s*s+z*z-x*x-y*y);

    return HepRotation(xcol, ycol, zcol);
}

double Quaternion::norm()const
{
    return m_s*m_s + m_v.mag2();
}

CLHEP::Hep3Vector Quaternion::rotate(const CLHEP::Hep3Vector& t) const
{

#if 0 // inefficient code using formal definition: enable to verify
    Quaternion out = (*this) * t * conjugate();
    return out.vector();
#else   // derived from above
    return (2*m_s*m_s-1)*t + 2.*m_v*m_v.dot(t) +2.*m_s*m_v.cross(t); 

#endif
}

astro::Quaternion Quaternion::power(double t)const{
    if( t==0) return Quaternion();
    double a( m_s>1? 0 : acos(m_s) );
    return Quaternion( m_v.unit()*sin(a*t), cos(a*t) );
}

bool Quaternion::isNear(const Quaternion& other)const
{
    return m_v.isNear(other.vector()) && fabs(m_s-other.scalar())<1e-10;
}

Quaternion Quaternion::interpolate(const Quaternion& q1, double t)const
{
    if( t==0) return *this;
    if( m_v.dot(q1.vector()) <0) {
        // this is when the rotation angle approaches 180 degrees
        Quaternion q2(-q1.vector(), -q1.scalar());
        return (q2*(this->conjugate())).power(t) * (*this);
    }

    return (q1*(this->conjugate())).power(t) * (*this);
}


int Quaternion::test()
{
    using astro::SkyDir;
    int ret( 0 );

    double angle(-0.3);

    HepRotation R= HepRotationY(angle);

    Quaternion q(R); 

    Hep3Vector dir = q.vector().unit();
    double ameas = 2.*asin(q.vector().mag()); // should be absolute of angle
    if( fabs(ameas-fabs(angle))>1e-10 ) ret+=1;

    double norm = q.norm(); 

    // now test multiplication

    Quaternion qq(q*q);

    dir = qq.vector().unit();
    double angle2 = 2.*asin(qq.vector().mag());
    if( fabs(angle2-2*fabs(angle))>1e-10) ret+=1; // fail double angle test
    norm = qq.norm();
    if( fabs(norm-1.0)>1e-10) ret+=1; // fail normalization

    // multiply by vector on either side.
    Hep3Vector w(0,1,1); Quaternion q3(w*q), q4((q.conjugate()*w).conjugate());
    if( fabs(q3.norm()-q4.norm())>1e-10) ret+=1;

    // test rotation of a simple vector
    Hep3Vector test(1,1,0);
    Hep3Vector xrot = q.rotate(test), xrot2(R*test);

    // the following checks that the transformed vector is
    // the same length, and that the rotation is the same
    double check = fabs(xrot.mag2()-test.mag2());
    check += (xrot-xrot2).mag2();
    if( check>1e-10 ) ret+=1; // failed simple test

    HepRotation Rcheck= q.rotation();
    if( ! R.isNear(R) ) ret+=1;
   
    // check power
    Quaternion pcheck = (q.power(0.5)).power(2.0);
    if( ! pcheck.isNear(q)) ret +=1;

    // check interpolation
    Quaternion other(Hep3Vector(0,1,0), Hep3Vector(1,0,0) )
        , interp(q.interpolate(other, 1.0));
    if( ! interp.isNear(other) ) ret+=1;


    // check construction
    Hep3Vector zin(Hep3Vector(0,1,0)), xin(Hep3Vector(1,0,0) );
    Quaternion q9(zin, xin );
    Hep3Vector 
        xout( q9.rotate(Hep3Vector(1,0,0)) ), 
        zout( q9.rotate(Hep3Vector(0,0,1))  );
    if ( !xin.isNear(xout) ) ret+=1;
    if ( !zin.isNear(zout) ) ret+=1;


    // check apparent failure
    /*
root [10] t.Scan("start:ra_scz:dec_scz:ra_scx:dec_scx", "start==221622510")
************************************************************************
*    Row   *     start *    ra_scz *   dec_scz *    ra_scx *   dec_scx *
************************************************************************
*    26308 * 221622510 * 320.13729 * -62.60844 * 276.88589 * 20.676851 *
*************************************************************************/
    Quaternion q10(SkyDir( 320.13729, -62.60844 )(),SkyDir(276.88589, 20.676851)());
    if ( q10.scalar() > 1e-5 ) ++ret;
    return ret;

}
