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
#include "tip/FileSummary.h"
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
    const char * keys[] = { "TELESCOP", "DATE-OBS", "DATE-END", "OBJECT" };
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

  void LightCurve::updateKeywords(const std::string & file_name) const {
    // For convenience, make a local reference to tip's file service singleton.
    tip::IFileSvc & file_service = tip::IFileSvc::instance();

    // Get file summary, which lists all extensions in the file.
    tip::FileSummary summary;

    file_service.getFileSummary(file_name, summary);

    // Pointer to each extension in turn.
    tip::Extension * ext = 0;
    try {
      // Iterate over all extensions in the file.
      for (tip::FileSummary::const_iterator ext_itor = summary.begin(); ext_itor != summary.end(); ++ext_itor) {
        // Open extension.
        ext = file_service.editExtension(file_name, ext_itor->getExtId());

        // Retrieve the header.
        tip::Header & header = ext->getHeader();

        for (KeyValuePairCont_t::const_iterator key_itor = m_key_value_pairs.begin(); key_itor != m_key_value_pairs.end();
          ++key_itor) {

          // Need the keyword object in a couple places,
          tip::Keyword & keyword = header[key_itor->first];

          // Flag to determine whether to update the keyword.
          bool update_key = false;
          try {
            // See if keyword is already present by attempting to read it. If it is not present,
            // keyword.get(...) will throw an exception.
            std::string dummy;
            keyword.get(dummy);

            // No exception was thrown, so keyword must be present, so update it.
            update_key = true;
          } catch (...) {
            // Ignore errors. Keywords in the key value pair container may or may not be in any given
            // extension.
          }

          // If keyword is already present, update it with the value from the key-value pair.
          if (update_key) keyword.set(key_itor->second);
        }
        delete ext;
      }
    } catch (...) {
      // Make sure this clean up is done even if an exception is encountered.
      delete ext;
      throw;
    }
    
  }

  std::string LightCurve::formatDateKeyword(const time_t & time) const {
    // Standard date format defined by FITS standard.
    char string_time[] = "YYYY-MM-DDThh:mm:ss";

    // Format using ctime functions.
    struct tm * loc_time = localtime(&time);
    strftime(string_time, sizeof(string_time), "%Y-%m-%dT%H:%M:%S", loc_time);

    // Return formatted time string.
    return string_time;
  }

}
