/** 
 * @file FitsImage.cxx
 * @brief Implementation of FitsImage member functions
 * @authors J. Chiang
 *
 * $Header$
 *
 */

#include <cmath>

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"

#include "astro/SkyProj.h"

#include "st_facilities/FitsImage.h"

namespace st_facilities {

FitsImage::FitsImage(const std::string & fitsfile,
                     const std::string & extension) 
   : m_filename(fitsfile), m_extension(extension), m_proj(0) {
   read_fits_image();
   m_proj = skyProjCreate(fitsfile, extension);
   for (unsigned int i = 0; i < m_axes.size(); i++) {
      std::vector<double> axisVector;
      m_axes[i].computeAxisVector(axisVector);
      m_axisVectors.push_back(axisVector);
   }
}

FitsImage::FitsImage(const FitsImage & rhs) 
   : m_filename(rhs.m_filename), m_extension(rhs.m_extension) {
   m_axes = rhs.m_axes;
   m_axisVectors = rhs.m_axisVectors;
   m_image = rhs.m_image;
   m_proj = new astro::SkyProj(*(rhs.m_proj));
}

FitsImage & FitsImage::operator=(const FitsImage & rhs) {
   if (this != &rhs) {
      m_filename = rhs.m_filename;
      m_extension = rhs.m_extension;
      m_axes = rhs.m_axes;
      m_axisVectors = rhs.m_axisVectors;
      m_image = rhs.m_image;
      delete m_proj;
      m_proj = new astro::SkyProj(*(rhs.m_proj));
   }
   return *this;
}

FitsImage::~FitsImage() {
   delete m_proj;
}

void FitsImage::getAxisDims(std::vector<int> &axisDims) const {
   axisDims.clear();
   for (unsigned int i = 0; i < m_axes.size(); i++) {
      axisDims.push_back(m_axes[i].size);
   }
}

void FitsImage::getAxisNames(std::vector<std::string> &axisNames) const {
   axisNames.clear();
   for (unsigned int i = 0; i < m_axes.size(); i++) {
      axisNames.push_back(m_axes[i].axisType);
   }
}

void FitsImage::getAxisVector(unsigned int naxis,
                              std::vector<double> &axisVector) const {
   if (naxis >= m_axes.size()) {
      std::ostringstream message;
      message << "FitsImage::getAxisVector: Invalid axis number " << naxis;
      throw std::invalid_argument(message.str());
   }
   axisVector = m_axisVectors[naxis];
}

void FitsImage::getCelestialArrays(std::vector<double> & lonArray,
                                   std::vector<double> & latArray) const {
   int npix = m_axes[0].size*m_axes[1].size;
   lonArray.clear();
   lonArray.reserve(npix);
   latArray.clear();
   latArray.reserve(npix);

// NB: wcslib starts indexing pixel arrays with 1, not 0.
   for (int j = 1; j < m_axes[1].size + 1; j++) {
      for (int i = 1; i < m_axes[0].size + 1; i++) {
         std::pair<double, double> dir = m_proj->pix2sph(i, j);
         lonArray.push_back(dir.first);
         latArray.push_back(dir.second);
      }
   }
}
         
void FitsImage::getSolidAngles(std::vector<double> &solidAngles) const {
// This solid angle calculation *assumes* that m_axes[0] is a
// longitudinal coordinate and that m_axes[1] is a latitudinal one.
// The axis units are assumed to be degrees, while the solid angles
// are returned as steradians.

   int npix = m_axes[0].size*m_axes[1].size;
   solidAngles.clear();
   solidAngles.reserve(npix);

// NB: wcslib starts indexing pixel arrays with 1, not 0.
   for (int j = 1; j < m_axes[1].size + 1; j++) {
      for (int i = 1; i < m_axes[0].size + 1; i++) {
         std::pair<double, double> center = m_proj->pix2sph(i, j);
         std::pair<double, double> lower = m_proj->pix2sph(i, j-1);
         std::pair<double, double> upper = m_proj->pix2sph(i, j+1);
         double thetamin = (lower.second + center.second)/2.*M_PI/180.;
         double thetamax = (upper.second + center.second)/2.*M_PI/180.;

         lower = m_proj->pix2sph(i-1, j);
         upper = m_proj->pix2sph(i+1, j);
         double dphi = (upper.first - lower.first)/2.;
         solidAngles.push_back(std::fabs(dphi*M_PI/180.*(sin(thetamax) - 
                                                         sin(thetamin))));
      }
   }
}

void FitsImage::getImageData(std::vector<double> &imageData) const {
   imageData.resize(m_image.size());
   for (unsigned int i = 0; i < m_image.size(); i++) {
      imageData.at(i) = m_image.at(i);
   }
}

void FitsImage::
AxisParams::computeAxisVector(std::vector<double> &axisVector) {
   axisVector.clear();
   axisVector.reserve(size);
   for (int i = 0; i < size; i++) {
      double value = step*(i - refPixel + 1) + refVal;
      if (logScale) {
         value = exp(value);
      }
      axisVector.push_back(value);
   }
}

void FitsImage::read_fits_image() {
   std::auto_ptr<const tip::Image>
      image(tip::IFileSvc::instance().readImage(m_filename, m_extension));

   image->get(m_image);

   std::vector<tip::PixOrd_t> dims = image->getImageDimensions();

   if (!(dims.size() == 2 || dims.size() == 3)) {
      std::ostringstream errorMessage;
      errorMessage << "FitsImage::read_fits_image: \n"
                   << "FITS file " << m_filename << " "
                   << "does not have the expected number of dimensions.\n"
                   << "It has NAXES = " << dims.size() << "\n";
      throw std::runtime_error(errorMessage.str());
   }

// Handle case where there are three dimensions but just one image plane.
   if (dims.size() == 3 && dims.back() == 1) {
      m_axes.resize(2);
   } else {
      m_axes.resize(dims.size());
   }

   char * crval[] = {"CRVAL1", "CRVAL2", "CRVAL3"};
   char * cdelt[] = {"CDELT1", "CDELT2", "CDELT3"};
   char * crpix[] = {"CRPIX1", "CRPIX2", "CRPIX3"};
   char * ctype[] = {"CTYPE1", "CTYPE2", "CTYPE3"};

   const tip::Header & header = image->getHeader();
   for (unsigned int i = 0; i < dims.size(); i++) {
      m_axes.at(i).size = static_cast<int>(dims.at(i));
      header[crval[i]].get(m_axes.at(i).refVal);
      header[cdelt[i]].get(m_axes.at(i).step);
      header[crpix[i]].get(m_axes.at(i).refPixel);
      header[ctype[i]].get(m_axes.at(i).axisType);
// Check for logarithmic scaling.
      int offset = m_axes.at(i).axisType.substr(0).find("log_");
      if (offset == 0) {
         m_axes[i].logScale = true;
      } else {
         m_axes[i].logScale = false;
      }
   }
}

astro::SkyProj * FitsImage::skyProjCreate(const std::string & fitsFile,
                                          const std::string & extension) {
   const tip::Image * image = 
      tip::IFileSvc::instance().readImage(fitsFile, extension);

   const tip::Header & header = image->getHeader();

   bool galactic;
   std::string ctype;
   header["CTYPE1"].get(ctype);
   if (ctype.substr(0, 2) == "RA") {
      galactic = false;
   } else if (ctype.substr(0, 4) == "GLON") {
      galactic = true;
   } else {
      throw std::runtime_error("Unrecognized coordinate system in " 
                               + fitsFile + "[" + extension + "]");
   }
   
   std::string trans("");
   if (ctype.size() > 7) {
      trans = ctype.substr(ctype.size() - 3, 3);
   }
   
   double crpix[2], crval[2], cdelt[2];
   header["CRPIX1"].get(crpix[0]);
   header["CRVAL1"].get(crval[0]);
   header["CDELT1"].get(cdelt[0]);

   header["CRPIX2"].get(crpix[1]);
   header["CRVAL2"].get(crval[1]);
   header["CDELT2"].get(cdelt[1]);

   double crota2(0);
   try {
      header["CROTA2"].get(crota2);
   } catch (...) {
   }

   delete image;

   return new astro::SkyProj(trans, crpix, crval, cdelt, crota2, galactic);
}

} // namespace st_facilities
