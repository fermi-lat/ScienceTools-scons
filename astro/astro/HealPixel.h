/** @file HealPixel.h
@brief Define the HealPixel class 
$Header$
*/

#ifndef astro_HealPixel_h
#define astro_HealPixel_h

#include "astro/SkyDir.h"
#include <vector>

namespace astro {
/**@class HealPixel
    @brief represent a Healpix pixel, assuming nested indexing

    This allows for a mixture of different nest levels (note that nside is forced to 
    be a power of 2) with a class variable to describe the coordinate system to use

    The sorting, defined by the operator<, follows the pixel indexing, with outer pixels preceding.

    @author T. Burnett <tburnett@u.washington.edu>
    */
    class HealPixel{
    public:
        ///@brief construct a pixel from the index and level (nside = 2**level).
        HealPixel(long index, int level);

        ///@brief create a HealPixel from a direction, and a level (nside=2**level)
        HealPixel(const astro::SkyDir& dir, int level);

        ///@brief behave like a skydir object, in center of pixel
        operator astro::SkyDir()const;
        astro::SkyDir operator()()const{ return *this;}

        long index()const{return m_index;} ///< the pixel index

        int nside()const{return m_level<1? 1 : 1<< m_level;} ///< the Healpix parameter nside

        int level()const{return m_level;} ///< the level, where nside=2**level

        /// sort operator
        bool operator<(const HealPixel& other)const;

        static bool test(); // should be true


        std::vector<HealPixel> neighbors() const;

        /// set the coordinate system for all pixels
        static void setCoordinateSystem(astro::SkyDir::CoordSystem sys){
            s_coordsys=sys;
        }

        /// use the same coordinate system for all these objects
       static astro::SkyDir::CoordSystem s_coordsys;

    private:
        long m_index; ///< the Healpix nested index
        int m_level;  ///< nesting level: nside is 2**level
    };

}
#endif

