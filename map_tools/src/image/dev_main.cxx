#include "Header.h"
#include "IOElement.h"
#include "Fits_IO.h"
#include "Image.h"
#include <iostream>
#include <numeric>


#if 0
  template <typename T>
  class Ref {
    public:
      /** \brief Construct a Ref object which refers to the given Cell object.
          \param cell The referent Cell object.
      */
      Ref(Attr<T> & cell): m_cell(&cell) {}

      /** \brief Assignment from Cell. This changes which Cell object the Ref refers to.
          \param cell The new referent Cell object.
      */
      Ref & operator =(Attr<T> & cell) { m_cell = &cell; }

      /** \brief Assignment from templated parameter type. This will write the assigned value into the
          cell to which this object refers.
          \param data The source value for the assignment.
      */
      Ref & operator =(T data) { /* m_cell->write(data); */ return *this; }

      /** \brief Retrieve the current templated parameter data value of this object.
      */
      operator T () const { T data; m_cell->read(data); return data; }

    private:
      Attr<T> & m_cell;
  };
#endif

int main()
{
#if 0 
    IOElement h;
    h.addAttribute(Attr<int>("Int1",99));
    h.addAttribute(Attr<double>("Double2", 99.));
    h.push_back(new Attr<std::string>("String", "this is a string"));

    for(Header::const_iterator it=h.begin(); it!=h.end(); ++it) {
        const BaseAttr& a = **it;
        std::cout << a.name() << "=" << a << std::endl; 
    }
#endif
    try{

    std::cout << "Reading an image file " << std::endl;
    Image<float> & f = *dynamic_cast<Image<float > *>(Fits_IO::read("/glast/DC1/data/DC1_image.fits", "", 1));
    
    int isize = f.pixelCount();
    std::cout << "\tpixels: " << isize << std::endl;


    std::cout << "\tkeyword, value" << std:: endl;
    for(Header::const_iterator it=f.begin(); it!=f.end(); ++it) {
        const BaseAttr& a = *(it->second);
        std::cout << "\t\t" << a.name() << "\t =" << a << std::endl; 
    }

    std::cout << "writing an image file..." << std::endl;
    std::string outfile("!/glast/DC1/data/DC1_image_copy.fits");

    FloatImg image("output",  f.getAxisSize() );
//    std::copy(f.begin(), f.end(), std::insert_iterator<Header>(image));
    for (Header::const_iterator it=f.begin(); it!=f.end(); ++it) {
       image.addAttribute(*(it->second));
    }
    std::cout << "Keywords in new element"<< std::endl;
    for(Header::const_iterator it=image.begin(); it!=image.end(); ++it) {
        const BaseAttr& a = *(it->second);
        std::cout << "\t\t" << a.name() << "\t =" << a << std::endl; 
    }
    const std::vector<float>& image_data = f.data();
    int len = image_data.size();
    float total = std::accumulate(image_data.begin(), image_data.end(), 0.0);

    image.data() = f.data(); // copy the image

    float test = image_data[isize/2];
    image.saveElement(outfile);

    }catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "written OK" << std::endl;
  
    return 0;
}
