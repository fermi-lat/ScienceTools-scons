/** @file SkyProj.h
@brief declaration of the class SkyProj

$Header$
=======
*/

#ifndef astro_SkyProj_H
#define astro_SkyProj_H

// EAC, introduce a base class for projections
#include "astro/ProjBase.h"

// Include files
#include <utility> // for pair
#include <string>

// forward declaration
struct wcsprm;
namespace tip { class Header;}

namespace astro {


    /** @class SkyProj
    * @brief Map Projection wrapper class for WCSLIB
    * @author T. Hierath
    *
    This class acts as a wrapper for 
    <a href=http://www.atnf.csiro.au/people/mcalabre/>WCSLIB</a>, 
    a library written for handling transformations
    between celestial and projection coordinate systems. 
    See <a href="../skyproj.html"> this document</a> for example images.

    For celestial coordinates, SkyProj does input and output in degrees with the latitude in the 
    range [-90,90] and longitudes in the range [0,360).  

    Implementation note: The wcslib function wcsset and all references 
    to it were changed to wcsset2.  This was the least intrusive way to resolve a naming 
    conflict with a Windows function of the same name.  So any future upgrades to new versions 
    of WCSLIB should incorporate this change.

    */

    class SkyProj : public ProjBase
    {
    public:
        ///Constructors

        /** @brief Constructor specified by FITS parameters
        @param projName String containing three char code
        Valid codes are:
        @verbatim
        AZP: zenithal/azimuthal perspective 
        SZP: slant zenithal perspective 
        TAN: gnomonic 
        STG: stereographic 
        SIN: orthographic/synthesis 
        ARC: zenithal/azimuthal equidistant 
        ZPN: zenithal/azimuthal polynomial 
        ZEA: zenithal/azimuthal equal area 
        AIR: Airy 
        CYP: cylindrical perspective
        CEA: cylindrical equal area 
        CAR: Plate carree 
        MER: Mercator 
        SFL: Sanson-Flamsteed 
        PAR: parabolic 
        MOL: Mollweide 
        AIT: Hammer-Aitoff 
        COP: conic perspective 
        COE: conic equal area
        COD: conic equidistant
        COO: conic orthomorphic
        BON: Bonne
        PCO: polyconic
        TSC: tangential spherical cube
        CSC: COBE quadrilateralized spherical cube
        QSC: quadrilateralized spherical cube
        HPX: HEALPix projection (this is NOT the single indexed healpix)
        @endverbatim
        @param crpix corresponds to the FITS keyword CRPIXi (coordinate reference point)
        @param crval corresponds to the FITS keyword CRVALi (coordinate value at reference point)
        @param cdelt corresponds to the FITS keyword CDELTi
        @param crota2 [default 0] corresponds to the FITS keyword CROTA2
        @param galactic if coords are to be interpreted as galactic
        **/
        SkyProj(const std::string &projName, 
            double* crpix, double* crval, double* cdelt, double crota2=0, bool galactic=false);

        /** @brief Alternate constructor with 2 additional parameters
        @param crpix corresponds to the FITS keyword CRPIXi (coordinate reference point)
        @param crval corresponds to the FITS keyword CRVALi (coordinate value at reference point)
        @param cdelt corresponds to the FITS keyword CDELTi
        @param crota2 [default 0] corresponds to the FITS keyword CROTA2
        @param galactic if coords are to be interpreted as galactic
        @param lonpole corresponds to the FITS keyword LONPOLE (native coordinates of celestial pole)
        @param latpole corresponds to the FITS keyword LATPOLE 

        */
        SkyProj(const std::string &projName, 
            double* crpix, double* crval, double* cdelt, double lonpole, double latpole,
            double crota2=0, bool galactic=false);

        /** @brief Constructor that reads wcs information from
            a FITS image extension
            @param fitsFile The FITS filename.
            @param extension The HDU extension name. If a null string,
            then the primary HDU is used.
        */
       SkyProj(const std::string & fitsFile, const std::string & extension="");

        /** @brief Constructor that uses wcslib's function wcspih to extract the required 
                   information from the fits header.  Also, this constructor does not call
                   the init member function of SkyProj.
            @param fitsFile string containing the name of the fits file to obtain header info from
            @param relax integer which determines what keywords are accepted
                    0: Recognize only FITS keywords defined by the
                       published WCS standard.
                    1: Admit all recognized informal extensions of the
                       WCS standard.
            @param ctrl integer used by wcspih for error reporting
                    0: Do not report any rejected header cards.
                    1: Produce a one-line message stating the number
                       of WCS cards rejected (nreject).
                    2: Report each rejected card and the reason why it
                       was rejected.
                    3: As above, but also report all non-WCS cards
                       that were discarded, and the number of
                       coordinate representations (nwcs) found.
        */
        SkyProj(const std::string &fitsFile, int relax, int ctrl=0);

        // Destructor
        virtual ~SkyProj();
        /// copy constructor
        SkyProj(const SkyProj& proj);
        /// assignment
        const SkyProj& operator=(const SkyProj& rhs);
	
	// Clone operator
	virtual ProjBase* clone() const { return new SkyProj(*this); }

        /** @brief tranform form world  to pixels with the given coordinates
        @param s1 ra or l, in degrees
        @param s2 dec or b, in degrees
        @return pair(x,y) in pixel coordinates
        */
        virtual std::pair<double,double> sph2pix(double s1, double s2)const;

        /** @brief Convert from one projection to another
        @param x1 projected equivalent to ra or l, in degrees
        @param x2 projected equivalent dec or b, in degrees
        @param projection used to deproject these coordinates
        @return pair(x,y) in new pixel coordinates
        */
	std::pair<double,double> pix2pix(double x1, double x2, const ProjBase& otherProjection)const {
	  std::pair<double,double> s = otherProjection.pix2sph(x1,x2);
	  return sph2pix(s.first,s.second);	  
	}

        /** @brief Does the inverse projection
        @param x1 projected equivalent to ra or l, in degrees
        @param x2 projected equivalent dec or b, in degrees
         @return pair(x,y) in spherical coordinates
       */
        virtual std::pair<double,double> pix2sph(double x1, double x2) const;
        
        /** @brief returns the range 
        @param xvar varies x if true, varies y if false
        @param x1 x or y coordinate to find y or x range respectively
        */
        virtual std::pair<double,double> range(double x1, bool xvar);
        
        /** @brief returns 0 if point (x1,x2) is in range */
        virtual int testpix2sph(double x1, double x2)const;

        /** @brief set appropriate keywords in the FITS header

        */
        virtual void setKeywords(tip::Header& header);

    private:

        /** @brief called by constructor to initialize the projection */
        void init(const std::string &projName, 
            double* crpix, double* crval, double* cdelt, 
            double lonpole, double latpole, double crota2, bool galactic);
        
        
        /* Structure defined in WCSLIB wcs.h.  This contains all
        projection information. */
        wcsprm* m_wcs;

        /*@brief determines if bounding rectangle exists
           @param xvar varies x if true, varies y if false
           @param x1 x or y coordinate to find y or x range respectively */
        bool hasRange(double x1, bool xvar);
        
        /*@brief finds bounding rectangle if it exists
        * @param crpix wcs pixel definition*/
        void findBound(double* crpix);

        /*used to determine bounding rectangle if it exists
         0-xmax, 1-xmin, 2-ymax, 3-ymin, 4-x finite, 5-y finite */
        double limit[6];
        static const int max=1600;
        double tol;

        // allocate a local array to hold the wcslib: must be at least as large
        static const size_t sizeof_wcslib = 2000;
        char  m_wcs_struct[sizeof_wcslib];

        // Number of coordinate representations found by wcspih
        int m_nwcs; 

        /* Boolean used by destructor so that it can call the appropriate function to
           deallocate memory. */
        bool m_wcspih_used;
    };

} // namespace astro
#endif    // astro_SKYPROJ_H


