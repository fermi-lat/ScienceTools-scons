/** @file Quaternion.h
@brief declare class Quaternion

$Header$

*/

#ifndef astro_Quaternion_h
#define astro_Quaternion_h

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"


namespace astro {
/** @class Quaternion

*/
    class Quaternion {
    public:

        /** ctor from vector and scalar

        */
        Quaternion(const CLHEP::Hep3Vector& v, double s=0)
            :m_v(v), m_s(s)
        {}

        /** ctor from x and z directions of rotated object

        */
        Quaternion(const CLHEP::Hep3Vector& xhat, const CLHEP::Hep3Vector& zhat);

      //  Quaternion(const Quaternion& other);

        /** ctor from a rotation matrix

        */
        explicit Quaternion(const CLHEP::HepRotation& R);

        ~Quaternion(){};
    
 /**
Quaternion multiplication is performed thusly:

(v,s)(v’,s’) = (vÄv’ + sv’ + vs’, ss’ – v·v’) where Ä is the vector cross-product and · is the vector dot-product. 

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

        static int test();

    private:
        CLHEP::Hep3Vector m_v;
        double m_s;
    };

}


#endif
