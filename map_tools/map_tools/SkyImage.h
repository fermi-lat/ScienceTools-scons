/** @file SkyImage.h

    @brief declare  the class SkyImage

*/

#ifndef TOOL_SKYIMAGE_H
#define TOOL_SKYIMAGE_H

class MapParameters;
class BaseImage;

namespace astro { class SkyDir; }
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
    SkyImage(const MapParameters& pars);
    /**
        @brief add a count to the map, using current SkyDir projection
        @param dir A SkyDir object
        @param delta incremental value (default 1 if not present)
        @param layer for multi-layer app. 0 (default) means the first layer
    */
    void addPoint(const astro::SkyDir& dir, double delta=1.0, int layer=0);


    ~SkyImage();

    /** @class SkyImage::Requester 
        @brief virtual base class for requesting data to fill a given pixel
       
    */
    class Requester {
    public:
        //! @param coordinates of the center of a bin
        virtual float operator()(const astro::SkyDir& bincenter)const=0;
    };

    /**
    @brief loop over all internal bins, request the intensity from a functor
    */
    void fill( Requester& req);

    /** brief clear the image, putting nulls around a AIT map
    */
    void clear();

    //! @brief return the sum of all pixel values in the image
    double total()const{return m_total;}

    
private:
    int m_naxis1, m_naxis2, m_naxis3;
    double m_total;
    //! pointer to the BaseImage abstract class, which is cast in the implementation.
    BaseImage* m_image;
    unsigned int m_pixelCount;
};


#endif
