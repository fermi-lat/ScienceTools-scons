/** @file Quaternion.h
@brief declare class Quaternion

@author T. Burnett <tburnett@u.washington.edu>
$Header$

*/

#ifndef astro_Quaternion_h
#define astro_Quaternion_h

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
#include <iostream>

namespace astro {
    /** @class Quaternion

    Manage quaternion objects, implementing multiplication and rotation of vectors.
    */
    class Quaternion {
    public:

        /** default ctor: identity transformation 
        */
        Quaternion():m_v(0,0,0),m_s(1){}

        /** @brief ctor from vector and scalar
        note that the normalization is not checked!

        */
        Quaternion(const CLHEP::Hep3Vector& v, double s);

        /** ctor from x and z directions of rotated object

        */
        Quaternion(const CLHEP::Hep3Vector& zhat, const CLHEP::Hep3Vector& xhat);


        /** ctor from a rotation matrix
        */
        explicit Quaternion(const CLHEP::HepRotation& R);

        ~Quaternion(){};

        /// is it approximately identical?
        bool isNear(const Quaternion& other)const;


        /** 
        Quaternion multiplication is performed thusly:

        (v,s)(v’,s’) = (vÄv’ + sv’ + vs’, ss’ – v·v’) where Ä 
        is the vector cross-product and · is the vector dot-product. 
        Multiplication is associative but not commutative.
        */
        Quaternion operator* (const Quaternion & r) const;

        /** multiply a vector, Q*v -> Q' */
        Quaternion operator* (const CLHEP::Hep3Vector & r) const;

        /** multiply by a vector v*Q -> Q/ */
        friend Quaternion operator* (const CLHEP::Hep3Vector & rx, const Quaternion & r);

        /** access to vector part
        */
        const CLHEP::Hep3Vector& vector()const{return m_v;}

        /** access to scalar part
        */
        double scalar()const { return m_s;}

        /** return the normalization, which should be 1 for rotations
        */
        double norm()const;

        /** return conjugate quaternioin
        */
        Quaternion conjugate()const{ return Quaternion(-m_v,m_s);}

        /** rotate a vector
        */
        CLHEP::Hep3Vector rotate(const CLHEP::Hep3Vector& v) const;

        /** return equivalent rotation matrix
        */
        CLHEP::HepRotation rotation()const;

        /** return to the given power
        */
        Quaternion power(double t)const;

        /** SLERP interpolation
        @param q1 the guy to interpolate to
        @param t  when t==1, should evaluate to q1; when  0, returns this,
        and smoothly inbetween.
        */
        Quaternion interpolate(const Quaternion& q1, double t)const;

        static int test();

    private:
        CLHEP::Hep3Vector m_v;
        double m_s;
    };

}


#endif
