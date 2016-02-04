/** @file HealpixRegion.cxx
@brief implementation of the class HealpixProj

$Header$
*/

// Include files
#include "healpix/HealpixRegion.h"

#include <stdexcept>
#include <cstring>
#include <cstdio>
#include "tip/Header.h"
#include "facilities/Util.h"


namespace healpix {
  
  int HealpixRegion::parseString(const std::string& st,
				RegionType& regionType, 
				std::vector<float>& pars) {
    const std::string delims(",()");
    std::vector<std::string> tokens;
    facilities::Util::stringTokenize(st,delims,tokens);
    pars.clear();

    if ( tokens.size() == 0 ) {
      std::cerr << "HealpixRegion::parseString input string tokenize failed: " << st << std::endl;
      throw std::runtime_error("HealpixRegion::parseString failed.");
      return -1;
    }
    if ( tokens[0] == "DISK" ){
      if ( tokens.size() < 4 ) {
	std::cerr << "HealpixRegion::parseString input string expects 4 tokens for DISK, got : " << tokens.size() << std::endl
		  << ".  st=" << st << std::endl;
	throw std::runtime_error("HealpixRegion::parseString failed.");
	return -1;
      }
      regionType = DISK;
      pars.push_back( facilities::Util::stringToDouble( tokens[1] ) );
      pars.push_back( facilities::Util::stringToDouble( tokens[2] ) );
      pars.push_back( facilities::Util::stringToDouble( tokens[3] ) );
    } else if ( tokens[0] == "DISK_INC"){
      if ( tokens.size() < 5 ) {
	std::cerr << "HealpixRegion::parseString input string expects 5 tokens for DISK_INC, got : " << tokens.size() << std::endl
		  << ".  st=" << st << std::endl;
	throw std::runtime_error("HealpixRegion::parseString failed.");
	return -1;
      }
      regionType = DISK_INC;
      pars.push_back( facilities::Util::stringToDouble( tokens[1] ) );
      pars.push_back( facilities::Util::stringToDouble( tokens[2] ) );
      pars.push_back( facilities::Util::stringToDouble( tokens[3] ) );
      pars.push_back( facilities::Util::stringToInt( tokens[4] ) );
    } else {
      regionType = UNKNOWN_REGION;
      std::cerr << "HealpixRegion::parseString did not recognize region type : " << tokens[0] << std::endl
		<< ".  st=" << st << std::endl;
      throw std::runtime_error("HealpixRegion::parseString failed.");
      return -1;
    }
    return 0;
  }
  
  int HealpixRegion::buildString(RegionType regionType, 
				const std::vector<float>& pars,
				std::string& st) {
    st.clear();
    char buf[255];
    switch (regionType) {
    case DISK:
      sprintf(buf,"DISK(%f,%f,%f)",pars[0],pars[1],pars[2]);
      st += buf;
      break;
    case DISK_INC:
      sprintf(buf,"DISK_INC(%f,%f,%f,%i)",pars[0],pars[1],pars[2],pars[3]);
      st += buf;
      break;
    case UNKNOWN_REGION:
    default:
      return -1;
    }
  }
  
  void HealpixRegion::query_disk(const Healpix_Base& healpix,
				const std::vector<float>& pars,
				std::vector<int>& pixelIndices) {

    static const double degToRad = 0.017453292519943295;
    double phi = degToRad*pars[0];
    double theta = degToRad*(90.-pars[1]);
    double rad =  degToRad*pars[2];
    pointing ptg(theta,phi);
    rangeset<int> pixset;    
    healpix.query_disc(ptg,rad,pixset);
    pixset.toVector(pixelIndices);        
  }
  
  void HealpixRegion::query_disk_inclusive(const Healpix_Base& healpix,
					  const std::vector<float>& pars,
					  std::vector<int>& pixelIndices) {
    static const double degToRad = 0.017453292519943295; 
    double phi = degToRad*pars[0];
    double theta = degToRad*(90.-pars[1]);
    double rad =  degToRad*pars[2];
    int factor = int(pars[3]);
    pointing ptg(theta,phi);
    rangeset<int> pixset;    
    healpix.query_disc_inclusive(ptg,rad,pixset,factor);
    pixset.toVector(pixelIndices);        
  }
  

  HealpixRegion::HealpixRegion(const std::string& st)
    :m_string(st),
     m_regionType(UNKNOWN_REGION),
     m_params(){
    parseString(m_string,m_regionType,m_params);
  }
  
  HealpixRegion::HealpixRegion(RegionType regionType, 
			      const std::vector<float>& pars)
    :m_string(),
     m_regionType(regionType),
     m_params(pars){
    buildString(m_regionType,m_params,m_string);
  }

  HealpixRegion::HealpixRegion(const HealpixRegion& other)
    :m_string(other.m_string),
     m_regionType(other.m_regionType),
     m_params(other.m_params){
  }

  void HealpixRegion::getPixels(const Healpix_Base& healpix,
			       std::vector<int>& pixelIndices) {
    bool ok(true);
        
    switch (m_regionType) {
    case DISK:
      query_disk(healpix,m_params,pixelIndices);
      break;
    case DISK_INC:
      query_disk_inclusive(healpix,m_params,pixelIndices);
      break;
    case UNKNOWN_REGION:
    default:
      ok = false;
    }
    if ( !ok ) {
      
    }
    return;
  }

  void HealpixRegion::setKeywords(tip::Header& header) {
    header["HPXREGION"].set(m_string);
  }


  void HealpixRegion::getRefDir(double& c1, double &c2) {
    switch (m_regionType) {
    case DISK:
    case DISK_INC:
      c1 = m_params[0];
      c2 = m_params[1];
      return;
    case UNKNOWN_REGION:
    default:
      c1 = 0.;
      c2 = 0.;
      break;
    }
    throw std::runtime_error("HealpixRegion::getRefDir, unknown region type.");
    return;
  }

  void HealpixRegion::getRegionSize(double& r) {
    switch (m_regionType) {
    case DISK:
    case DISK_INC:
      r = m_params[2];
      return;
    case UNKNOWN_REGION:
    default:
      r = 180.;
      break;
    }
    throw std::runtime_error("HealpixRegion::getRegionSize, unknown region type.");
    return;
  }


}
