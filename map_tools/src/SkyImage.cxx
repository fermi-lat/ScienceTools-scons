/** @file SkyImage.cxx

@brief implement the class SkyImage
$Header$
*/

#include "hoops/hoops_group.h"

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "astro/SkyDir.h"
#include "astro/SkyFunction.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include <cctype>
#include <stdexcept>
#include <sstream>
#include <errno.h> // to test result of std::remove()

namespace {
    static unsigned long lnan[2]={0xffffffff, 0x7fffffff};
    static double& dnan = *( double* )lnan;
}
using namespace map_tools;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const astro::SkyDir& center,  
                   const std::string& outputFile, 
                   double pixel_size, double fov, int layers, 
                   const std::string& ptype,
                   bool galactic)
: m_naxis3(layers)  
, m_image(0)
, m_save(true)
, m_layer(0)
{

    if( fov>90) {
        std::string types[]={"" ,"CAR","AIT","ZEA"};
        int xsize[] =       {360, 360,  325,  230}; 
        int ysize[] =       {180, 180,  162,  230}; 
        for( unsigned int i = 0; i< sizeof(types)/sizeof(std::string); ++i){
            if( ptype == types[i]) {
                m_naxis1 = static_cast<int>(xsize[i]/pixel_size);
                m_naxis2 = static_cast<int>(ysize[i]/pixel_size);
                break;
            }
        }

        if( m_naxis1==0) {
            throw std::invalid_argument("SkyImage::SkyImage -- projection type " 
                +ptype +" does not have default image size");
        }
    }else{

        m_naxis1=m_naxis2 = static_cast<int>(fov/pixel_size + 0.5);
    }

    double crval[2] = { galactic?center.l():center.ra(),galactic? center.b(): center.dec()};
    double cdelt[2] = { -pixel_size, pixel_size };
    double crpix[2] = { (m_naxis1+1)/2.0, (m_naxis2+1)/2.0};

    m_wcs = new astro::SkyProj(ptype, crpix, crval, cdelt, 0., galactic);
    this->setupImage(outputFile);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const map_tools::MapParameters& pars)
: m_naxis1(pars.npix())
, m_naxis2(pars.npixY())
, m_naxis3(pars.npixZ()) // for future expansion
, m_total(0)
, m_image(0)
, m_imageData()
, m_save(true)
, m_layer(0)
, m_wcs(0)
{
    using namespace astro;

    // see if there is an input count map
    std::string cm_file = pars.getValue<std::string>("cmfile");
    std::string uc_cm_file = cm_file;
    for ( std::string::iterator itor = uc_cm_file.begin(); itor != uc_cm_file.end(); ++itor) *itor = toupper(*itor);
        
    if ( "NONE" != uc_cm_file){
        static double s_Mev_per_keV = .001;
        // read the count map to get the image dimensions from it
        loadImage(cm_file, "", true);
        // read energies associated with layers from ebounds extension.
        std::auto_ptr<const tip::Table> ebounds(tip::IFileSvc::instance().readTable(cm_file, "EBOUNDS"));
        m_energy.resize(ebounds->getNumRecords());
        std::vector<double>::iterator out_itor = m_energy.begin();
        for (tip::Table::ConstIterator in_itor = ebounds->begin(); in_itor != ebounds->end(); ++in_itor, ++out_itor) {
          *out_itor = (*in_itor)["E_MIN"].get() * s_Mev_per_keV;
        }
    }else{
        std::string ptype(pars.projType());
        double pixelsize = pars["pixelsize"];

        if( m_naxis1==0){
            // special code to determine all-sky limits based on scale factor and transformation
            std::string types[]={"" ,"CAR","AIT","ZEA"};
            int xsize[] =       {360, 360,  325,  230}; 
            int ysize[] =       {180, 180,  162,  230}; 
            for( unsigned int i = 0; i< sizeof(types)/sizeof(std::string); ++i){
                if( ptype == types[i]) {
                    m_naxis1 = static_cast<int>(xsize[i]/pixelsize);
                    m_naxis2 = static_cast<int>(ysize[i]/pixelsize);
                    break;
                }
            }
            if( m_naxis1==0) {
                throw std::invalid_argument("SkyImage::SkyImage -- projection type " 
                    +ptype +" does not have default image size");
            }
        }
        if( m_naxis2==0) m_naxis2=m_naxis1; // default square image
        bool galactic = pars.uselb();

        /// arrays describing transformation: assume reference in the center
        double          //lon            lat
            crval[2]={ pars.xref(),      pars.yref()},
            crpix[2]={ (m_naxis1+1)/2.0, (m_naxis2+1)/2.0},
            cdelt[2]={ -pixelsize,       pixelsize },
            crota2=pars.rot();
        m_wcs = new astro::SkyProj( pars.projType(), crpix, crval, cdelt, crota2, galactic);
  
        double energy = pars["emin"], eratio = pars["eratio"];
        m_energy.resize(m_naxis3);
        for ( int ii = 0; ii != m_naxis3; ++ii, energy *= eratio){
            m_energy[ii] = energy;
        }
    }
    setupImage(pars.outputFile(),  pars.clobber());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Note: this constructor was stolen on 2/7/2006 by James Peachey from
// SkyImage::SkyImage(const map_tools::MapParameters&) and modified to use
// ScienceTools-compliant parameters for the map geometry. This constructor
// is thus redundant to the one above, but represents a migration path toward
// making map_tools more like the other ScienceTools.
// TODO: migrate all tools in map_tools to use this constructor, then
// remove Parameters and MapParameters classes and the constructor above.
SkyImage::SkyImage(const hoops::IParGroup& pars)
: m_naxis1(pars["numxpix"])
, m_naxis2(pars["numypix"])
, m_naxis3(pars["layers"])
, m_total(0)
, m_image(0)
, m_imageData()
, m_save(true)
, m_layer(0)
, m_wcs(0)
{
    using namespace astro;

    // see if there is an input count map
    std::string cm_file = pars["cmfile"];
    std::string uc_cm_file = cm_file;
    for ( std::string::iterator itor = uc_cm_file.begin(); itor != uc_cm_file.end(); ++itor) *itor = toupper(*itor);
        
    if ( "NONE" != uc_cm_file){
        static double s_Mev_per_keV = .001;
        // read the count map to get the image dimensions from it
        loadImage(cm_file, "", true);
        // read energies associated with layers from ebounds extension.
        std::auto_ptr<const tip::Table> ebounds(tip::IFileSvc::instance().readTable(cm_file, "EBOUNDS"));
        m_energy.resize(ebounds->getNumRecords());
        std::vector<double>::iterator out_itor = m_energy.begin();
        for (tip::Table::ConstIterator in_itor = ebounds->begin(); in_itor != ebounds->end(); ++in_itor, ++out_itor) {
          *out_itor = (*in_itor)["E_MIN"].get() * s_Mev_per_keV;
        }
    }else{
        std::string ptype = pars["proj"];
        double pixelsize = pars["pixscale"];

        if( m_naxis1==0){
            // special code to determine all-sky limits based on scale factor and transformation
            std::string types[]={"" ,"CAR","AIT","ZEA"};
            int xsize[] =       {360, 360,  325,  230}; 
            int ysize[] =       {180, 180,  162,  230}; 
            for( unsigned int i = 0; i< sizeof(types)/sizeof(std::string); ++i){
                if( ptype == types[i]) {
                    m_naxis1 = static_cast<int>(xsize[i]/pixelsize);
                    m_naxis2 = static_cast<int>(ysize[i]/pixelsize);
                    break;
                }
            }
            if( m_naxis1==0) {
                throw std::invalid_argument("SkyImage::SkyImage -- projection type " 
                    +ptype +" does not have default image size");
            }
        }
        if( m_naxis2==0) m_naxis2=m_naxis1; // default square image
        std::string coord_sys = pars["coordsys"];
        for (std::string::iterator itor = coord_sys.begin(); itor != coord_sys.end(); ++itor) *itor = toupper(*itor);
        bool galactic = (coord_sys == "GAL");

        /// arrays describing transformation: assume reference in the center
        double          //lon            lat
            crval[2]={ pars["xref"],      pars["yref"]},
            crpix[2]={ (m_naxis1+1)/2.0, (m_naxis2+1)/2.0},
            cdelt[2]={ -pixelsize,       pixelsize },
            crota2=pars["axisrot"];
        m_wcs = new astro::SkyProj( pars["proj"], crpix, crval, cdelt, crota2, galactic);
  
        double energy = pars["emin"], eratio = pars["eratio"];
        m_energy.resize(m_naxis3);
        for ( int ii = 0; ii != m_naxis3; ++ii, energy *= eratio){
            m_energy[ii] = energy;
        }
    }
    setupImage(pars["outfile"],  pars["clobber"]);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::setupImage(const std::string& outputFile,  bool clobber)
{
    std::string extension("skyimage"); // maybe a parameter?

    if( clobber ){
        int rc = std::remove(outputFile.c_str());
        if( rc==-1 && errno ==EACCES ) throw std::runtime_error(
            std::string("SkyImage: cannot remove file "+outputFile)
            );
    }

    // setup the image: it needs an axis dimension array and the file name to write to
    std::vector<long> naxes(3);
    naxes[0]=m_naxis1;
    naxes[1]=m_naxis2;
    naxes[2]=m_naxis3;

    // now add an image to the file
    tip::IFileSvc::instance().appendImage(outputFile, extension, naxes);
    m_image = tip::IFileSvc::instance().editImage(outputFile, extension);

    m_pixelCount = m_naxis1*m_naxis2*m_naxis3;
    m_imageData.resize(m_pixelCount);

    m_wcs->setKeywords(m_image->getHeader());
}
void SkyImage::loadImage(const std::string& fits_file, const std::string& extension, bool dim_only)
{
    tip::Image* m_image = const_cast<tip::Image*>(tip::IFileSvc::instance().readImage(fits_file, extension));
    tip::Header& header = m_image->getHeader();

    // standard ordering for ra, dec, cos(theta).
    header["NAXIS1"].get(m_naxis1);
    header["NAXIS2"].get(m_naxis2);
    header["NAXIS3"].get(m_naxis3);
    m_pixelCount = m_naxis1*m_naxis2*m_naxis3;

    m_wcs = new astro::SkyProj(fits_file,1);

    // finally, read in the image if client desires
    if (dim_only) m_imageData.resize(m_pixelCount, 0.);
    else m_image->get(m_imageData);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const std::string& fits_file, const std::string& extension)
:  m_save(false)
, m_layer(0)
, m_wcs(0)
{
    loadImage(fits_file, extension);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int SkyImage::setLayer(unsigned int newlayer)
{
    checkLayer(newlayer);
    unsigned int t = m_layer;
    m_layer = newlayer;
    return t;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::addPoint(const astro::SkyDir& dir, double delta, unsigned int layer)
{
    std::pair<double,double> p= dir.project(*m_wcs);
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(  k< m_pixelCount){
        m_imageData[k] += delta;
        m_total += delta;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::checkLayer(unsigned int layer)const
{
    if( layer >= (unsigned int)m_naxis3){
        std::stringstream errmsg;
        errmsg << "SkyImage: requested layer " << layer 
            << " not compatible with axis3: " << m_naxis3 << std::endl;
        throw std::out_of_range(errmsg.str());
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::fill(const astro::SkyFunction& req, unsigned int layer)
{
    checkLayer(layer);
    m_total=m_count=m_sumsq=0;
    m_min=1e10;m_max=-1e10;
    int offset = m_naxis1* m_naxis2 * layer;
    for( size_t k = 0; k< (unsigned int)(m_naxis1)*(m_naxis2); ++k){
        // 2/7/2006 JP commented out the following line to silence compiler warning.
        // size_t kk = k%(unsigned int)(m_naxis1* m_naxis2);
        // determine the bin center (pixel coords start at (1,1) in center of lower left
        double 
            x = static_cast<int>(k%m_naxis1)+1.0, 
            y = static_cast<int>(k/m_naxis1)+1.0;
        if( m_wcs->testpix2sph(x,y)==0) {
            astro::SkyDir dir(x,y, *m_wcs);
            double t= req(dir);
            m_imageData[k+offset] = t;
            m_total += t;
            ++m_count;
            m_sumsq += t*t;
            m_min = t<m_min? t:m_min;
            m_max = t>m_max? t:m_max;
        }else{
            // not valid (off the edge, perhaps)
            m_imageData[k+offset]=dnan; 
        }
    }
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::clear()
{
    size_t s = m_imageData.size();
    for( size_t k = 0; k< s; ++k){
        // 2/7/2006 JP changed the following line to silence compiler warning.
        // m_imageData[k]=NULL; 
        m_imageData[k]=0; 
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::~SkyImage()
{
    if( m_save) {
        m_image->set(m_imageData);
    }
    delete m_image; 
    delete m_wcs;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double SkyImage::pixelValue(const astro::SkyDir& pos,unsigned  int layer)const
{
    checkLayer(layer); 
    unsigned int k = pixel_index(pos,layer);
    return m_imageData[k];        
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float &  SkyImage::operator[](const astro::SkyDir&  pixel)
{
    unsigned int k = pixel_index(pixel);
    return m_imageData[k];        

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const float &  SkyImage::operator[](const astro::SkyDir&  pixel)const
{
    unsigned int k = pixel_index(pixel);
    return m_imageData[k];        

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double SkyImage::operator()(const astro::SkyDir& s)const
{
    return pixelValue(s, m_layer);        
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::getNeighbors(const astro::SkyDir& pos, std::vector<double>&neighbors)const
{
    int layer = 0; ///@todo: get neighbors on a different layer
    std::pair<double,double> p= pos.project(*m_wcs);
    if( p.first<0) p.first += m_naxis1;
    if(p.second<0) p.second += m_naxis2;
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(i+1<(unsigned int)m_naxis1)neighbors.push_back(m_imageData[k+1]); 
    if(i>0) neighbors.push_back(m_imageData[k-1]);
    if(j+1<(unsigned int)m_naxis2)neighbors.push_back(m_imageData[k+m_naxis1]);
    if(j>0)neighbors.push_back(m_imageData[k-m_naxis1]);

}

// internal routine to convert a SkyDir to a pixel index
unsigned int SkyImage::pixel_index(const astro::SkyDir& pos, int layer) const
{
    // if not specified, use the data member
    if( layer<0 ) layer = m_layer;

    // project using wcslib interface, then adjust to be positive
    std::pair<double,double> p= pos.project(*m_wcs);
    if( p.first<0) p.first += m_naxis1;
    if(p.second<0) p.second += m_naxis2;
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if( k >= m_pixelCount ) {
        throw std::range_error("SkyImage::pixel_index -- outside image hyper cube");
    }
    return k;
}


