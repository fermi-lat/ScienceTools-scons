/**
 * @file ExposureHyperCube.cxx
 *
 * $Header$
 */

#include "image/Image.h"

#include "map_tools/ExposureHyperCube.h"

using namespace map_tools;

ExposureHyperCube::ExposureHyperCube( const Exposure& exp, 
                                      std::string outfile) : m_image(0)
{
   std::vector<long> naxes(3);
   naxes[0]= Exposure::Index::ra_factor;
   naxes[1]= Exposure::Index::dec_factor;
   naxes[2] =Exposure::Index::cosfactor;
   
   m_image = new FloatImg("hypercube", outfile, naxes);
   dynamic_cast<FloatImg *>(m_image)->data() = exp.data(); 
   
   setKey("TELESCOP", "GLAST");
   setKey("INSTRUME", "SIMULATION");
   
   setKey("DATE-OBS", "");
   setKey("DATE-END", "");
   setKey("EQUINOX", 2000.0);
   
   setKey("CTYPE1", "RA--CAR");
   setKey("CRPIX1",  0.5*(1+naxes[0]));
   setKey("CRVAL1",  0);
   setKey("CDELT1",  -1.);
   setKey("CUNIT1",  "deg");
   
   setKey("CTYPE2",  "DEC--CAR");
   setKey("CRPIX2",  0.5*(1+naxes[1]));
   setKey("CRVAL2",  0.);
   setKey("CDELT2",  1.0);
   setKey("CUNIT2",  "deg");

   setKey("CTYPE3",  "SQRT(1-COSTHETA)");
   setKey("CRPIX3",   0.5*(1+naxes[2]) );
   setKey("CRVAL3",  0.);
   setKey("CDELT3",  1./naxes[2] );
   setKey("CUNIT3",  "");
   
   setKey("BUNIT", "s");

   setKey("CROTA2",  0);
   // record total exposure as a key
   setKey("TOTAL", exp.total());
   
}

void ExposureHyperCube::setKey(std::string name, double value, 
                               std::string unit, std::string comment)
{
   m_image->addAttribute(DoubleAttr(name, value, unit, comment)); 
}

void ExposureHyperCube::setKey(std::string name, std::string value,
                               std::string unit, std::string comment)
{
   m_image->addAttribute(StringAttr(name, value,unit,comment)); 
}

void ExposureHyperCube::save(std::string outfile){

   dynamic_cast<FloatImg*>(m_image)->saveElement(outfile);
   m_image;
   m_image=0;
}

ExposureHyperCube::~ExposureHyperCube(){
   if(m_image!=0) save();
}
