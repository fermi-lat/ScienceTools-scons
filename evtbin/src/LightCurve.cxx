/** \file LightCurve.cxx
    \brief Encapsulation of a Light curve, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <string>

#include "evtbin/Hist1D.h"
#include "evtbin/LightCurve.h"
#include "evtbin/RecordBinFiller.h"
#include "tip/Extension.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  LightCurve::LightCurve(const Binner & binner): m_key_value_pairs(), m_known_keys(), m_data_dir(), m_hist(binner) {
    // Find the directory containing templates.
    const char * top_dir = getenv("EVTBINROOT");
    if (0 != top_dir) m_data_dir = std::string(top_dir) + "/data/";

    // Make a list of known keywords. These can be harvested from the input events extension
    // and used to update the output file(s).
    const char * keys[] = { "TELESCOP", "DATE-OBS" };
    m_known_keys.insert(m_known_keys.end(), keys, keys + sizeof(keys) / sizeof(const char *));
  }

  LightCurve::~LightCurve() throw() {}

  void LightCurve::binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
    // Fill histogram.
    std::for_each(begin, end, RecordBinFiller(m_hist));
  }

  void LightCurve::writeOutput(const std::string & creator, const std::string & out_file) const {
    // Create light curve file using template from the data directory.
    tip::IFileSvc::instance().createFile(out_file, m_data_dir + "LatLightCurveTemplate");

    // Open RATE extension of output light curve file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "RATE"));

    // Update some keywords.
    tip::Header & out_header = output_table->getHeader();

    // Explicitly set CREATOR keyword.
    out_header["CREATOR"].set(creator);

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

  void LightCurve::harvestKeywords(const tip::Header & header) {
    // Iterate over keywords which are known to be useful in this case.
    for (KeyCont_t::const_iterator itor = m_known_keys.begin(); itor != m_known_keys.end(); ++itor) {
      try {
        // Read each keyword as a string.
        std::string value;
        header[*itor].get(value);

        // Write each keyword name-value pair into the container of pairs.
        m_key_value_pairs[*itor] = value;

      } catch (...) {
        // Ignore errors. Keywords are obtained on a best effort basis, but missing them shouldn't
        // cause the software to fail.
      }
    }
  }
}
