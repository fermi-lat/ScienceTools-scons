/** @file Healpix.h
@brief Define the Healpix class, and nested classes Healpix::Iterator and Healpix::Pixel 

@author B. Lesnick (based on information from http://www.eso.org/science/healpix/) 

$Header$
*/

#ifndef astro_Healpix_h
#define astro_Healpix_h

#include "astro/SkyDir.h"
#include "astro/SkyFunction.h"

namespace astro{
/**
@class Healpix
@brief Encapsulate the healpix C routines as distributed by WMAP, 
and present the pixels as an STL collection. 

Nested classes:
- Pixel to represent an individual pixel--can be created from a direction (pixelization)
- Iterator to treat as a container of Pixels.
- Integrand, is provided to facilitate numeric integrals
over the sphere. A simple example using it is:
@verbatim
    int nside = 20; 
    Healpix h(nside);
    const astro::SkyFunction& fun ; // from somewhere
    double integral = std::accumulate(h.begin(), h.end(), 0., Healpix::Integrand(fun));
@endverbatim
Or, the member function integrate() can be used:
@verbatim
    double integral = Healpix(nside).integrate(fun);
@endverbatim

Reference: http://www.eso.org/science/healpix/

Note: Healpix stands for "Hierarchical, Equal Area, and iso-Latitude Pixelisation of the sphere".
*/

class Healpix {
public:

    typedef enum
    {
        RING = 0,
        NESTED = 1
    } Ordering; 

    /**@brief specify configuration
    @param nside Number of divisions of the side of 
    each of the 12 base pixels.Indicates resolution.  Min = 1.  Max = ?? 
    @param ord  Whether pixels are ordered along equi-latitudinal rings or hierarchiclly.
    @param coordsys equatorial (ra,dec) or galactic (l,b)

    */
    Healpix(long nside=2, Ordering ord = NESTED, 
        astro::SkyDir::CoordSystem coordsys = astro::SkyDir::EQUATORIAL);

    ///@brief the number of sides per dodecahedron
    long nside()const{return m_nside; }
    ///@brief the number of pixels
    long npix()const{return 12*m_nside*m_nside;}

    ///@brief the number of pixels, as the size.
    size_t size()const{return 12*m_nside*m_nside;}

    ///@brief the area per pixel
    double pixelArea()const{return 4*M_PI/npix();}
    ///@brief the value of the ordering parameter: either NESTED or RING
    Ordering ord()const{return m_ord;}
    bool nested()const{return m_ord==NESTED;}
    astro::SkyDir::CoordSystem coordsys()const{return m_coordsys;}
    bool galactic()const{return m_coordsys==astro::SkyDir::GALACTIC; }

    /**@class Pixel
    @brief represent a Healpix pixel

    */
    class Pixel{
    public:
        ///@brief construct a pixel from the index
        Pixel(long index, const Healpix& hp)
            :  m_index(index) , m_healpix(&hp)
        {}
        ///@brief create a Pixel from a direction
        Pixel(const astro::SkyDir& dir, const Healpix& hp);
        double area()const{return m_healpix->pixelArea();}
        ///@brief behave like a skydir object, in center of pixel
        operator astro::SkyDir()const;
        astro::SkyDir operator()()const{ return *this;}
        long index()const{return m_index;}

    private:
        long m_index;
        const Healpix* m_healpix;
    };

    ///@brief return the pixel corresponding to the given direction
    Pixel pixel(const astro::SkyDir& dir)const{ return Pixel(dir, *this);}

    /** @class Iterator
    @brief forward iterator of effective container of pixels
    */
    class Iterator
    {
    public:
        Iterator(long index, const Healpix& hp)
            :  m_index(index) , m_healpix(hp) 
        {}    

        ///@brief pre-increment operator
        Iterator & operator ++ () { ++m_index; return *this; }   
        ///@brief dereference operator
        Pixel operator * ()const{return Healpix::Pixel(m_index,m_healpix);}  
        ///@brief behave like the index for comparison
        operator long () const { return m_index;}  
    private:
        long m_index;
        const Healpix& m_healpix;
    };

    typedef Iterator const_iterator;

    ///@brief dereferences to first Pixel (index 0)
    Iterator begin () const  { return Iterator(0, *this);}

    ///@brief correspond to last+1 index
    Iterator end () const    { return Iterator(npix(), *this); }

    /** @class Integrand
    @brief Functor that can be used with std::accumulate to 
    perform numeric integration.
    */
    class Integrand     {  
    public:
        /** brief ctor saves reference to a SkyFunction    */
        Integrand(const astro::SkyFunction& f): m_f(f){}
        /** brief function called by accumulate    */
        double operator()( double result, const Healpix::Pixel& node)const{
            return result+ node.area()*m_f(node);  }
        const astro::SkyFunction& m_f;
    };

    ///@brief do the integral
    double integrate(const astro::SkyFunction& f)const;

    // direct access to healpix routines
    void pix2ang(long index, double &theta, double &phi)const;
    void ang2pix(double theta, double phi, long &index)const;
private:

    long m_nside; ///< number of sides per dodecahedron
    Ordering m_ord; ///< ordering
    astro::SkyDir::CoordSystem m_coordsys;///< how to define SkyDir

};

} // namespace astro
#endif /* astro_Healpix_h */
