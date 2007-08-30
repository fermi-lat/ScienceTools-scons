/** @file Draw.h 
@brief declaration of the Draw wrapper class

$Header$
*/


#ifndef pointlike_Draw_h
#define pointlike_Draw_h

namespace map_tools {class PhotonMap;}
namespace astro { class SkyDir; }
#include <string>
#include <vector>
#include "embed_python/Module.h"


namespace pointlike {

    //! @class Draw
    //! @brief manage creating images to FITS files from a PhotonMap
    class Draw {
    public:

        //! Type of counts to include with the image
        typedef enum  
        {
            NONE = 0, ///< Don't include photon count layers.
            SIMPLE = 1,  ///< Simple counts for one level only.
            CHILDREN = 2, ///< Include counts of children of given pixel.
            WEIGHTED = 3 ///< Include counts of children of given pixel, weighted by level.
        } CountType;


        Draw(const map_tools::PhotonMap& map);

        //! create FITS image file using the data
        //! @param dir center
        //! @param dir file to write
        //! @param pixelsize in degrees
        //! @param fov  field of view (deg) if 180, use car
        //! @param proj projection: if not specified, use AIT for full sky, ZEA otherwsie

        void region(const astro::SkyDir& dir, std::string outputFile, double pixelsize, double fov);

        void sky(std::string outputfile, double pixelsize);

        void setCountType(CountType t){ m_countType =t;}
        void galactic(){m_galactic = true;}
        void equatorial(){m_galactic=false;}
        void projection(std::string p){m_proj = p;} ///< set the projection

    private:
        const map_tools::PhotonMap& m_map;
        bool m_galactic;    ///< galactic or equatorial
        std::string m_proj; ///< projection (CAR, AIT, etc.)
        CountType m_countType; 
    };












}// namespace pointline


#endif
