/** 
 * @file FitsImage.h
 * @brief Declaration of FitsImage class
 * @authors J. Chiang
 *
 * $Header$
 *
 */

#ifndef st_facilities_FitsImage_h
#define st_facilities_FitsImage_h

#include <string>
#include <vector>

namespace astro {
   class SkyProj;
}

namespace tip {
   class Header;
}

namespace st_facilities {

/** 
 * @class FitsImage
 *
 * @brief A class for accessing FITS image data.
 *
 * @author J. Chiang
 *    
 * $Header$
 *
 */

class FitsImage {
    
public:

   FitsImage() : m_proj(0) {}

   FitsImage(const std::string & fitsfile, 
             const std::string & extension="");

   FitsImage(const FitsImage &);

   virtual ~FitsImage();

   FitsImage & operator=(const FitsImage &);

   /// A vector of the image axes dimensions
   virtual void getAxisDims(std::vector<int> & axisDims) const;

   /// The names (CTYPEs) of the image axes
   virtual void getAxisNames(std::vector<std::string> & axisNames) const;

   /// This method computes arrays of longitude and latitude obtained
   /// by traversing the image plane by column number then row.
   virtual void getCelestialArrays(std::vector<double> & lonArray,
                                   std::vector<double> & latArray) const;
   
   /// Get the pixel values.  They will be indexed by column, row,
   /// then plane, i.e., indx = i + j*NAXIS1 + k*NAXIS1*NAXIS2.  Note
   /// that each image plane is indexed starting at the lower left
   /// (South-East) corner.
   virtual void getImageData(std::vector<double> & imageData) const;

   /// This returns the pixel solid angles.  Use of this method assumes
   /// that m_axis[0] represents a longitudinal coordinate and that
   /// m_axis[1] represents a latitudinal coordinate.  The pixel values
   /// will be indexed by column then row, indx = i + j*NAXIS1.
   virtual void getSolidAngles(std::vector<double> & solidAngles) const;

   /// @brief Factory method to create an astro::SkyProj object.
   /// @param fitsFile FITS file containing the WCS projection information.
   /// @param extension The name of the relevent FITS image extension.
   static astro::SkyProj * skyProjCreate(const std::string & fitsFile,
                                         const std::string & extension="");

protected:

   /// Get a vector filled with axis abscissa points for the naxis-th
   /// coordinate.
   virtual void getAxisVector(unsigned int naxis, 
                              std::vector<double> & axisVector) const;

/** 
 * @class AxisParams
 * @brief Nested n-tuple class to represent FITS image axis information
 */
   class AxisParams {
   public:
      AxisParams() {}
      ~AxisParams() {}
      int size;
      float refVal;
      float step;
      float refPixel;
      std::string axisType;
      std::string comment;
      bool logScale;

      /// Returns a vector of abscissa values based on the axis parameters.
      void computeAxisVector(std::vector<double> &axisVector);
   };

   void read_fits_image();

   /// FITS file name.
   std::string m_filename;

   /// FITS extension name
   std::string m_extension;

   /// Descriptions for each image axis.
   std::vector<AxisParams> m_axes;

   /// Vectors of abscissa values for each axis.
   std::vector< std::vector<double> > m_axisVectors;

   /// The FITS image data
   std::vector<float> m_image;

   /// The file-specific projection information.
   astro::SkyProj * m_proj;

   void setProjection(const tip::Header & header);

};

} // namespace st_facilities

#endif // st_facilities_FitsImage.h
