/** @file HealpixArray.h
@brief Define the HealpixArray template class

@author T. Burnett

$Header$
*/

#ifndef astro_HealpixArray_h
#define astro_HealpixArray_h

#include "astro/Healpix.h"

#include <vector>

namespace astro {
/** @class HealpixArray<class C>
    @brief Associate a vector of type C with the the Healpix 
    sky tesselization scheme

    It extends a std::vector of the given type, overriding the 
    operator[] with a SkyDir.
*/

template< class C>
class HealpixArray : public std::vector<C> {
public:

    //! ctor: must pass in a Healpix object to define the binning
    HealpixArray(Healpix hp)
        :m_hp(hp)
    { resize(hp.size());
    }

    //! default ctor, need to set the healpix
    HealpixArray():m_hp(1){resize(m_hp.size());}

    //! return the direction associated with an iterator
    astro::SkyDir dir(const_iterator it){
        astro::Healpix::Pixel px(it-begin(), m_hp);
        return px();
    }

    //! @brief access a content object by direction, for modificaion
    C& operator[](const astro::SkyDir& dir){
        astro::Healpix::Pixel pix = m_hp.pixel(dir);
        return at(pix.index());
    }

    //! brief access a content object in read-only mode
    const C& operator[](const astro::SkyDir& dir)const{
        astro::Healpix::Pixel pix = m_hp.pixel(dir);
        return at(pix.index());
    }
    //! access the Healpix configuration object
    Healpix healpix()const{return m_hp;}
    void setHealpix(astro::Healpix hp){m_hp=hp; resize(hp.size());}

private:
    astro::Healpix m_hp;
};

} // namespace astro
#endif
