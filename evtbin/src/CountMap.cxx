/** \file CountMap.cxx
    \brief Encapsulation of a count map, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "astro/SkyDir.h"
#include "evtbin/LinearBinner.h"
#include "evtbin/Hist2D.h"
#include "evtbin/CountMap.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

static const double pi = 3.14159265358979323846;

namespace evtbin {

  CountMap::CountMap(double ref_ra, double ref_dec, const std::string & proj, unsigned long num_x_pix, unsigned long num_y_pix,
    double pix_scale, double axis_rot, bool use_lb, const std::string & ra_field, const std::string & dec_field): DataProduct(),
    m_hist(
      LinearBinner(pix_scale * ref_ra - num_x_pix / 2., pix_scale * ref_ra + num_x_pix / 2., 1., ra_field), 
      LinearBinner(pix_scale * ref_dec - num_y_pix / 2., pix_scale * ref_dec + num_y_pix / 2., 1., dec_field)
    ) {
    m_hist_ptr = &m_hist;

    astro::SkyDir::ProjType type;
    if (0 == proj.compare("AIT")) type = astro::SkyDir::AIT;
    else if (0 == proj.compare("ARC")) type = astro::SkyDir::ARC;
    else if (0 == proj.compare("BAD")) type = astro::SkyDir::BAD;
    else if (0 == proj.compare("CAR")) type = astro::SkyDir::CAR;
    else if (0 == proj.compare("GLS")) type = astro::SkyDir::GLS;
    else if (0 == proj.compare("MER")) type = astro::SkyDir::MER;
    else if (0 == proj.compare("NCP")) type = astro::SkyDir::NCP;
    else if (0 == proj.compare("SIN")) type = astro::SkyDir::SIN;
    else if (0 == proj.compare("STG")) type = astro::SkyDir::STG;
    else if (0 == proj.compare("TAN")) type = astro::SkyDir::TAN;
    else throw std::runtime_error(std::string("CountMap::CountMap cannot handle projection type ") + proj);

    // Set up the projection. The minus sign in the X-scale is because RA is backwards.
    astro::SkyDir::setProjection(ref_ra * pi / 180., ref_dec * pi / 180., type, ref_ra * pix_scale,
      ref_dec * pix_scale, -pix_scale, pix_scale, axis_rot * pi / 180., use_lb);
  }

  CountMap::~CountMap() throw() {}

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
      std::pair<double, double> coord = astro::SkyDir(ra, dec).project();

      // Bin the value.
      m_hist.fillBin(coord.first, coord.second);
    }
  }

  void CountMap::writeOutput(const std::string & creator, const std::string & out_file) const {
    // Standard file creation from base class.
    createFile(creator, out_file, m_data_dir + "LatCountMapTemplate");

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
  }

}
