/** @file SkyImage.h

    @brief declare  the class SkyImage
    @author Toby Burnett <tburnett@u.washington.edu>
    $Header$

*/

#ifndef MAP_TOOLS_SKYIMAGE_H
#define MAP_TOOLS_SKYIMAGE_H

#include "astro/SkyFunction.h"

#include <string>
#include <vector>
class BaseImage;

namespace astro { class SkyDir; }

namespace map_tools {
class MapParameters;

/**
    @class SkyImage
    @brief define an image for export to a FITS image

*/
class SkyImage 
{
public:
    /** @brief constructor set up the map

    @param pars an object containing all the necessary parameters
    */
    SkyImage(const map_tools::MapParameters& pars);

    /** @brief load an image from a file.
        @param filename name of the file, only FITS for now
        @param extension Name of an extension: if blank, assume primary

    */
    SkyImage(const std::string& filename, const std::string& extension="");

    /**
        @brief add a count to the map, using current SkyDir projection
        @param dir A SkyDir object
        @param delta incremental value (default 1 if not present)
        @param layer for multi-layer app. 0 (default) means the first layer 
    */
    void addPoint(const astro::SkyDir& dir, double delta=1.0, unsigned int layer=0);

 
     /** @brief direct access to the pixel at the given direction and current layer
    */
    float & operator[](const astro::SkyDir&  pixel);
    const float & operator[](const astro::SkyDir&  pixel)const;

    ~SkyImage();

    //! set default layer, return previous 
    unsigned int setLayer(unsigned int newlayer);
 
    /**
    @brief loop over all internal bins, request the intensity from a functor derived
    from SkyFunction
    @param req a functor that returns a double for a SkyDir
    @param layer layer number to fill [default 0]
    */
    void fill( const astro::SkyFunction& req, unsigned int layer=0);

    /** brief clear the image, putting nulls around a AIT map
    */
    void clear();

    //! @brief return the sum of all pixel values in the image
    double total()const{return m_total;}

    /** @brief get value of the pixel at given skydir location
        @param pos position in the sky
        @param layer number
        @return value of the pixel corresponding to the given direction
    */
    double pixelValue(const astro::SkyDir& pos, unsigned int layer=0)const;
    
    /** @brief  set a list of the neighbor values
    @param pos position in the sky
    @param nlist list of neighbor values to set
    */
    void getNeighbors(const astro::SkyDir& pos, std::vector<double>& neighbors)const ;

private:
    //! sizes of the respective axes.
    int   m_naxis1, m_naxis2, m_naxis3;
    double m_total;
    //! pointer to the BaseImage abstract class, which is cast in the implementation.
    BaseImage* m_image;
    unsigned int m_pixelCount;
    bool m_save; 
    unsigned int m_layer;

    
};
} //namesace map_tools

#endif
