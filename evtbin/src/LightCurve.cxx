/** \file LightCurve.cxx
    \brief Encapsulation of a Light curve, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/LightCurve.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  LightCurve::LightCurve(const Binner & binner): DataProduct(), m_hist(binner) { m_hist_ptr = &m_hist; }

  LightCurve::~LightCurve() throw() {}

  void LightCurve::writeOutput(const std::string & creator, const std::string & out_file) const {
    // Create light curve file using template from the data directory.
    tip::IFileSvc::instance().createFile(out_file, m_data_dir + "LatLightCurveTemplate");

    // Add CREATOR keyword to the hash of keywords.
    m_key_value_pairs["CREATOR"] = creator;

    // Set DATE keyword to the current time.
    m_key_value_pairs["DATE"] = formatDateKeyword(time(0));

    // Update newly created file with keywords which were harvested from input data.
    updateKeywords(out_file);

    // Open RATE extension of output light curve file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "RATE"));

    // The binner from the histogram will be used below.
    const Binner * binner = m_hist.getBinners().at(0);

    // Resize table: number of records in light curve must == the number of bins in the binner.
    output_table->setNumRecords(binner->getNumBins());

    // Need output table iterator.
    tip::Table::Iterator table_itor = output_table->begin();

    // Iterate over bin number and output table iterator, writing fields in order.
    for (long index = 0; index != binner->getNumBins(); ++index, ++table_itor) {
      // Midpoint time of each bin, from the binner.
      (*table_itor)["TIME"].set(binner->getInterval(index).midpoint());

      // Width of each bin, from the binner.
      (*table_itor)["TIMEDEL"].set(binner->getBinWidth(index));

      // Number of counts in each bin, from the histogram.
      (*table_itor)["COUNTS"].set(m_hist[index]);
    }
  }

}
