/**
 * @file DataCube.h
 * @brief N-dimensional hypercube of numeric values, encapsulating the
 * astroroot-derived Image class.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef map_tools_DataCube_h
#define map_tools_DataCube_h

#include <sstream>

#include "facilities/Util.h"

#include "image/Fits_IO.h"
#include "image/Image.h"

namespace map_tools {

/**
 * @class DataCube
 *
 * @brief A base class for an N-dimensional cube of numeric data.  It
 * provides "header" attribute access through its Image<T>* data
 * member.
 *
 * @author J. Chiang
 *
 * $Header$
 */

template <typename T>
class DataCube {

public:

   DataCube() : m_image(0) {}

   /// Create a new DataCube "in memory".
   DataCube(const std::vector<long> & naxes, const std::string & extName) 
      : m_naxes(naxes), m_extName(extName), m_filename("") {
      m_image = new Image<T>(extName, naxes);
   }

   /// Create a new DataCube with a given filename.
   DataCube(const std::vector<long> & naxes, const std::string & extName, 
            std::string filename, VirtualIO::FMode mode=VirtualIO::ReadWrite) 
      : m_naxes(naxes), m_extName(extName),m_filename(filename) {
      facilities::Util::expandEnvVar(&filename);
      m_image = new Image<T>(extName, filename, naxes);
      m_image->setFileAccess(mode);
   }

   /// Create a new DataCube, reading in the data from a FITS file.
   DataCube(const std::string &filename, const std::string &extName) 
      : m_extName(extName), m_filename("") {
      Image<T> * original_image 
         = dynamic_cast<Image<T> *>(Fits_IO::read(filename, extName));
      int n;
      original_image->getValue("NAXIS", n);
      for (int i=0; i < n; i++) {
         std::ostringstream keyword;
         keyword << "NAXIS" << i + 1;
         int naxis;
         original_image->getValue(keyword.str(), naxis);
         m_naxes.push_back(naxis);
      }
// The Image<T> classes do not have proper clone methods or copy
// constructors, so we must copy the data by hand:
      m_image = new Image<T>(extName, m_naxes);
      m_image->data() = original_image->data();
      for (Header::const_iterator it = original_image->begin();
           it != original_image->end(); it++) {
         m_image->push_back((*it)->clone());
      }
      delete original_image;
   }

   virtual ~DataCube() {
//      if (m_image != 0) save();
      delete m_image;
   }
   
   template<typename KeyType>
   void setKey(const std::string & keyname, const KeyType & value,
               const std::string & unit="", const std::string & comment="") {
      m_image->addAttribute(Attr<KeyType>(keyname, value, unit, comment));
   }

   template<typename KeyType>
   void getKey(const std::string & keyname, KeyType & value) {
      m_image->getValue(keyname, value);
   }

//    template<typename KeyType>
//    void getKey(const::string & keyname, KeyType & value, std::string & unit,
//                std::string & comment);
   
   void save(std::string filename="") {
      facilities::Util::expandEnvVar(&filename);
      m_image->saveElement(filename);
   }

   void getKeywordNames(std::vector<std::string> & names) const {
      m_image->getAttributeNames(names);
   }

   const std::vector<T> & data() const {return m_image->data();}
   std::vector<T> & data() {return m_image->data();}

private:

   Image<T> * m_image;

   std::vector<long> m_naxes;
   
   std::string m_extName;
   std::string m_filename;

// All of the classes higher up in the Image<T> hierarchy lack proper
// copy constructors, despite having pointers as data members, so we
// must disable these methods:

   DataCube(const DataCube<T> &rhs) {
//       m_image = rhs.m_image->clone();
//       m_naxes = rhs.m_naxes;
//       m_filename = rhs.m_filename;
   }

   DataCube & operator=(const DataCube<T> &rhs) {
//       return *this(rhs);
   }






};

} // namespace map_tools

#endif // map_tools_DataCube_h
