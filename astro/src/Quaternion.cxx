/** @file Quaternion.cxx
@brief implement class Quaternion

$Header$

*/

#include "astro/Quaternion.h"

using namespace astro;
using namespace CLHEP;

Quaternion::Quaternion(const CLHEP::HepRotation& R)
: m_v(Hep3Vector(0,0,0))
, m_s(1)
{
    // code mostly from ROOT's TRotation::AngleAxis. rversed sign of rot axis
    double cosa  = 0.5*(R.xx()+R.yy()+R.zz()-1);
    double cosa1 = 1-cosa;
    if (cosa1 >0 ){
        double x=0, y=0, z=0;
        if (R.xx() > cosa) x = sqrt((R.xx()-cosa)/cosa1);
        if (R.yy() > cosa) y = sqrt((R.yy()-cosa)/cosa1);
        if (R.zz() > cosa) z = sqrt((R.zz()-cosa)/cosa1);
        if (R.zy() > R.yz())  x = -x;
        if (R.xz() > R.zx())  y = -y;
        if (R.yx() > R.xy())  z = -z;
        m_s = sqrt(0.5*(1+cosa));
        m_v = Hep3Vector(x,y,z)*sqrt(0.5*cosa1);;
    }
}
Quaternion::Quaternion(const CLHEP::Hep3Vector& xhat, const CLHEP::Hep3Vector& zhat)
: m_v(Hep3Vector(0,0,0)), m_s(1)
{
    // note no check that they are unit vectors and orthogonal, beware
    Hep3Vector yhat(zhat.cross(xhat));
    // code mostly from ROOT's TRotation::AngleAxis. rversed sign of rot axis
    double cosa  = 0.5*(xhat.x()+yhat.y()+zhat.z()-1);
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


Quaternion astro::operator* (const CLHEP::Hep3Vector & v, const Quaternion & q)
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
        xcol(s*s+x*x-y*y-z*z, 2*(x*y-s*z),     2*(x*z+s*y) ),
        ycol(2*(x*y+s*z),     s*s+y*y-x*x-z*z, 2*(y*z-s*x) ) ,
        zcol(2*(x*z-s*y),     2*(z*y+s*x),     s*s+z*z-x*x-y*y);

    return HepRotation(xcol, ycol, zcol);
}

double Quaternion::norm()const
{
    return m_s*m_s + m_v.mag2();
}

CLHEP::Hep3Vector Quaternion::rotate(const CLHEP::Hep3Vector& t) const
{

#if 0 // inefficient code using formal definition: enable to verify
    Quaternion qv(v), qc(-m_v, m_s), q1 = t*(*this);
    Quaternion out = qc * q1;
    return out.vector();
#else   // derived from above
    return (2*m_s*m_s-1)*t + 2.*m_v*m_v.dot(t) -2.*m_s*m_v.cross(t); 

#endif
}

int Quaternion::test()
{
    int ret( 0 );

    double angle(0.1);

    HepRotation R= HepRotationY(angle);

    Quaternion q(R); 

    Hep3Vector dir = q.vector().unit();
    double ameas = 2.*asin(q.vector().mag()); // should be absolute of angle

    double norm = q.norm(); 

    // now test multiplication

    Quaternion qq(q*q);

    dir = qq.vector().unit();
    double angle2 = 2.*asin(qq.vector().mag());
    if( fabs(angle2-2*angle)>1e-10) ret=1; // fail angle test
    norm = qq.norm();
    if( fabs(norm-1.0)>1e-10) ret=1; // fail normalization

    // test rotation of a simple vector
    Hep3Vector test(1,1,0);
    Hep3Vector xrot = q.rotate(test), xrot2(R*test);

    // the following checks that the transformed vector is
    // the same length, and that the rotation is the same
    double check = fabs(xrot.mag2()-test.mag2());
    check += (xrot-xrot2).mag2();
    if( check>1e-10 ) ret=1; // failed simple test

    HepRotation Rcheck= q.rotation();
    bool nearcheck( R.isNear(R));

    return ret;

}