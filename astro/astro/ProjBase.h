/** @file ProjBase.h
@brief declaration of the class ProjBase

$Header$
=======
*/

#ifndef astro_ProjBase_H
#define astro_ProjBase_H


// Include files
#include <utility> // for pair
#include <string>

// forward declaration
struct wcsprm;
namespace tip { class Header;}

#define ASTRO_D2R 0.017453292519943295
#define ASTRO_R2D 57.29577951308232
#define ASTRO_PIOVER2 1.5707963267948966
#define ASTRO_4PI 12.566370614359172

namespace astro {

    // these have to live somewhere
    inline double degToRad(double deg) { return deg*ASTRO_D2R; }
    inline double radToDeg(double rad) { return rad*ASTRO_R2D; }
    inline double latToTheta_Deg(double lat) { return 90. - lat; }
    inline double thetaToLat_Deg(double theta) { return 90. - theta; }
    inline double latToTheta_Rad(double lat) { return ASTRO_PIOVER2 - lat; }
    inline double thetaToLat_Rad(double theta) { return ASTRO_PIOVER2 - theta; }


    /** @class ProjBase
    * @brief Base class for both WCS and Healpix based projection/pixelizations
    * @author E. Charles from T. Hierath's SkyProj
    *
    See sub-classes SkyProj and HealpixProj for specific details

    For celestial coordinates, ProjBase does input and output in degrees with the latitude in the 
    range [-90,90] and longitudes in the range [0,360).  

    */

    class ProjBase {
    public:
      
      typedef enum  { 
	//! WCS=based projections
	WCS = 0,
	//! HEALPix-pixelization based projection 
	HEALPIX = 1,
	//! Just in base we messed up
	UNKNOWN = 2
      } Method;
      
      /*
	@param projName String containing three char code
	@endverbatim
	@param galactic if coords are to be interpreted as galactic
      */
      ProjBase(const std::string &projName, bool galactic=false, Method method= WCS)
	:m_projName(projName),m_galactic(galactic),m_method(method){;};
	
      /* @brief Default c'tor */
      ProjBase(bool galactic=false,Method method=WCS)
	:m_projName(),m_galactic(galactic),m_method(method){;}

      // Destructor
      virtual ~ProjBase() {;};
      
      // copy constructor
      ProjBase(const ProjBase& other)
	:m_projName(other.m_projName),m_galactic(other.m_galactic),m_method(other.m_method){;};

      // Clone operator
      virtual ProjBase* clone() const = 0;

      /* @brief tranform form world  to pixels with the given coordinates
	 @param s1 ra or l, in degrees
	 @param s2 dec or b, in degrees
	 @return pair(x,y) in pixel coordinates
      */
      virtual std::pair<double,double> sph2pix(double s1, double s2)const = 0;
      
      /* @brief Convert from one projection to another
	 @param x1 projected equivalent to ra or l, in degrees
	 @param x2 projected equivalent dec or b, in degrees
	 @param projection used to deproject these coordinates
	 @return pair(x,y) in new pixel coordinates
      */
      virtual std::pair<double,double> pix2pix(double x1, double x2, const ProjBase& otherProjection) const = 0;

      /* @brief Does the inverse projection
	 @param x1 projected equivalent to ra or l, in degrees
	 @param x2 projected equivalent dec or b, in degrees
         @return pair(x,y) in spherical coordinates
      */
      virtual std::pair<double,double> pix2sph(double x1, double x2) const = 0;

      /* @brief is this galactic? */
      inline bool isGalactic() const { return m_galactic; }

      /* @brief return the name of the projection */
      inline std::string projType() const {return m_projName;} 

      /* @brief return the method of the projection */
      inline Method method() const { return m_method; }

        
      /* @brief returns the range 
	 @param xvar varies x if true, varies y if false
	 @param x1 x or y coordinate to find y or x range respectively
      */
      virtual std::pair<double,double> range(double x1, bool xvar) = 0;
        
      /* @brief returns 0 if point (x1,x2) is in range */
      virtual int testpix2sph(double x1, double x2) const = 0;

      /* @brief set appropriate keywords in the FITS header */
      virtual void setKeywords(tip::Header& header) = 0;

    protected:

      inline void initBase(const std::string& projName, 
			   bool isGalactic,
			   Method method) {
	m_projName = projName;
	m_galactic = isGalactic;
	m_method = method;
      }

    private:
      
      std::string m_projName;        
      bool m_galactic;
      Method m_method;

    };

} // namespace astro
#endif    // astro_SKYPROJ_H


