/** \file CountMap.cxx
    \brief Encapsulation of a count map, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "astro/SkyDir.h"
#include "astro/SkyProj.h"

#include "evtbin/LinearBinner.h"
#include "evtbin/Hist2D.h"
#include "evtbin/CountMap.h"

#include "st_facilities/Env.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

static const double pi = 3.14159265358979323846;

namespace evtbin {

  CountMap::CountMap(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
    const std::string & sc_table, double ref_ra, double ref_dec, const std::string & proj,
    unsigned long num_x_pix, unsigned long num_y_pix, double pix_scale, double axis_rot, bool use_lb,
    const std::string & ra_field, const std::string & dec_field, const Gti & gti):
    DataProduct(event_file, event_table, gti), m_hist(
      //LinearBinner(- (long)(num_x_pix) / 2., num_x_pix / 2., 1., ra_field),
      //LinearBinner(- (long)(num_y_pix) / 2., num_y_pix / 2., 1., dec_field)
      LinearBinner(0.5, num_x_pix + 0.5, 1., ra_field),
      LinearBinner(0.5, num_y_pix + 0.5, 1., dec_field)
    ), m_proj_name(proj), m_crpix(), m_crval(), m_cdelt(), m_axis_rot(axis_rot), m_proj(0), m_use_lb(use_lb) {
    m_hist_ptr = &m_hist;

    m_crpix[0] = (num_x_pix + 1.) / 2.;
    m_crpix[1] = (num_y_pix + 1.) / 2.;
//    m_crpix[0] = 0;
//    m_crpix[1] = 0;
    m_crval[0] = ref_ra;
    m_crval[1] = ref_dec;
    m_cdelt[0] = -pix_scale;
    m_cdelt[1] = pix_scale;

    m_proj = new astro::SkyProj(proj, m_crpix, m_crval, m_cdelt, m_axis_rot, m_use_lb);
    // Set up the projection. The minus sign in the X-scale is because RA is backwards.
    //astro::SkyDir::setProjection(ref_ra * pi / 180., ref_dec * pi / 180., type, ref_ra * pix_scale,
    //  ref_dec * pix_scale, -pix_scale, pix_scale, axis_rot * pi / 180., m_use_lb);

    // Collect any/all needed keywords from the primary extension.
    harvestKeywords(m_event_file);

    // Collect any/all needed keywords from the events extension.
    harvestKeywords(m_event_file, m_event_table);

    // Correct time keywords.
    adjustTimeKeywords(sc_file, sc_table);
  }

  CountMap::~CountMap() throw() { delete m_proj; }

  void CountMap::binInput() {
    DataProduct::binInput();
  }

  void CountMap::binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
    // Get binners for the two dimensions.
    const Hist::BinnerCont_t & binners = m_hist.getBinners();

    // From each binner, get the name of its field, interpreted as ra and dec.
    std::string ra_field = binners[0]->getName();
    std::string dec_field = binners[1]->getName();

    // Fill histogram, converting each RA/DEC to Sky X/Y on the fly:
    for (tip::Table::ConstIterator itor = begin; itor != end; ++itor) {
      // Extract the ra and dec from each record.
      double ra = (*itor)[ra_field].get();
      double dec = (*itor)[dec_field].get();

      // Convert to sky coordinates.
      std::pair<double, double> coord = astro::SkyDir(ra, dec).project(*m_proj);

      // Bin the value.
      m_hist.fillBin(coord.first, coord.second);
    }
  }

  void CountMap::writeOutput(const std::string & creator, const std::string & out_file) const {
    // Standard file creation from base class.
    createFile(creator, out_file, st_facilities::Env::appendFileName(m_data_dir, "LatCountMapTemplate"));

    // Open Count map extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Image> output_image(tip::IFileSvc::instance().editImage(out_file, ""));

    // Get dimensions of image.
    typedef std::vector<tip::PixOrd_t> DimCont_t;
    DimCont_t dims = output_image->getImageDimensions();

    // Make sure image is two dimensional.
    DimCont_t::size_type num_dims = dims.size();
    if (2 != num_dims) throw std::runtime_error("CountMap::writeOutput cannot write a count map to an image which is not 2D");

    // Get the binners.
    const Hist::BinnerCont_t & binners = m_hist.getBinners();

    // Compute settings for CTYPE keywords.
    std::string ctype1;
    std::string ctype2;

    // First 4 characters of CTYPEn describe coordinates, with trailing dashes.
    if (m_use_lb) {
      ctype1 = "GLON";
      ctype2 = "GLAT";
    } else {
      ctype1 = "RA--";
      ctype2 = "DEC-";
    }

    // Last 4 characters of CTYPEn describe projection, with leading dashes.
    std::ostringstream os;
    os.fill('-');
    os.width(4);
    os << std::right << m_proj_name;
    ctype1 += os.str();
    ctype2 += os.str();

    // Write c* keywords
    tip::Header & header = output_image->getHeader();
    header["CRPIX1"].set(m_crpix[0]);
    header["CRPIX2"].set(m_crpix[1]);
    header["CRVAL1"].set(m_crval[0]);
    header["CRVAL2"].set(m_crval[1]);
    header["CDELT1"].set(m_cdelt[0]);
    header["CDELT2"].set(m_cdelt[1]);
    header["CROTA2"].set(m_axis_rot);
    header["CTYPE1"].set(ctype1);
    header["CTYPE2"].set(ctype2);

    // Write DSS keywords to preserve cut information.
    writeDssKeywords(header);

    // Resize image dimensions to conform to the binner dimensions.
    for (DimCont_t::size_type index = 0; index != num_dims; ++index) {
      dims[index] = binners.at(index)->getNumBins();
    }

    // Set size of image.
    output_image->setImageDimensions(dims);

    // Copy bins into image.
    for (tip::PixOrd_t x_index = 0; x_index != dims[0]; ++x_index) {
      for (tip::PixOrd_t y_index = 0; y_index != dims[1]; ++y_index) {
        output_image->setPixel(x_index, y_index, m_hist[x_index][y_index]);
      }
    }

    // Write the GTI extension.
    writeGti(out_file);
  }

}
