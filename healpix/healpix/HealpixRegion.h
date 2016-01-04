/** @file SkyProj.h
@brief declaration of the class SkyProj

$Header$
=======
*/

#ifndef healpix_HealpixRegion_H
#define healpix_HealpixRegion_H

// Include files
#include <string>
#include <vector>

#include "healpix_base.h"
#include "tip/Header.h"

namespace healpix {


  /** @class HealpixRegion
   * @brief Region selection definition
   * @author E. Charles 
   *   
  */

  class HealpixRegion {
  
  public:
    typedef enum  { 
      UNKNOWN_REGION=0, // 
      DISK=1,           // Pixels whose centers lie within a disk
      DISK_INC=2        // Pixels which overlap a disk
    } RegionType;


    static int parseString(const std::string& st,
			   RegionType& regionType, 
			   std::vector<float>& pars);

    static int buildString(RegionType regionType, 
			   const std::vector<float>& pars,
			   std::string& st);

    static void query_disk(const Healpix_Base& healpix,
			   const std::vector<float>& pars,			   
			   std::vector<int>& pixelIndices);

    static void query_disk_inclusive(const Healpix_Base& healpix,
				     const std::vector<float>& pars,
				     std::vector<int>& pixelIndices);

    ///Constructors
    
    /* @brief Constructor specified by string
       @param theString is parsed to get the region parameters
    */
    HealpixRegion(const std::string& st);
    
    /* @brief Constructor specified a set of parameters
       @param regionType is an enum specifying the region type
       @param parameters are the number of parameters
    */
    HealpixRegion(RegionType regionType, const std::vector<float>& pars);
     
    // Destructor
    ~HealpixRegion(){};

    /// copy constructor
    HealpixRegion(const HealpixRegion& other);
    
    /// access to the string representation
    inline const std::string& asString() const { return m_string; }
      
    /// region type
    inline RegionType regionType() const { return m_regionType; }

    /// float
    inline const std::vector<float>& parameters() const { return m_params; }

    /// fill the pixel list for a particular healpix representatoin
    void getPixels(const Healpix_Base& healpix,
		   std::vector<int>& pixelIndices);

    /// set appropriate keywords in the FITS header
    void setKeywords(tip::Header& header);

    /// get the reference direction
    void getRefDir(double& c1, double &c2);

    /// get the "size" of the region
    void getRegionSize(double& r);


  private:    

    std::string m_string;
    RegionType m_regionType;
    std::vector<float> m_params;

  };
  
} // namespace astro
#endif    // astro_HealpixProj_H


