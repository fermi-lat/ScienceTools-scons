/** \file DataProduct.cxx
    \brief Base class for encapsulations of specific data products, with methods to read/write them using tip.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>

#include "evtbin/DataProduct.h"
#include "evtbin/Hist.h"
#include "evtbin/RecordBinFiller.h"
#include "tip/Extension.h"
#include "tip/FileSummary.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  DataProduct::DataProduct(): m_key_value_pairs(), m_known_keys(), m_data_dir() {
    // Find the directory containing templates.
    const char * top_dir = getenv("EVTBINROOT");
    if (0 != top_dir) m_data_dir = std::string(top_dir) + "/data/";

    // Make a list of known keywords. These can be harvested from the input events extension
    // and used to update the output file(s).
    const char * keys[] = { "TELESCOP", "INSTRUME", "DATE", "DATE-OBS", "DATE-END", "OBJECT", "TIMESYS", "MJDREF",
      "EQUNINOX", "RADECSYS", "EXPOSURE", "ONTIME" };
    m_known_keys.insert(m_known_keys.end(), keys, keys + sizeof(keys) / sizeof(const char *));
  }

  DataProduct::~DataProduct() throw() {}

  void DataProduct::binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
    if (0 == m_hist_ptr) throw std::logic_error("DataProduct::binInput cannot bin a NULL histogram");
    // Fill histogram.
    std::for_each(begin, end, RecordBinFiller(*m_hist_ptr));
  }

  void DataProduct::createFile(const std::string & creator, const std::string & out_file, const std::string & fits_template) const {
    // Create light curve file using template from the data directory.
    tip::IFileSvc::instance().createFile(out_file, fits_template);

    // Add CREATOR keyword to the hash of keywords.
    m_key_value_pairs["CREATOR"] = creator;

    // Update newly created file with keywords which were harvested from input data.
    updateKeywords(out_file);
  }
  
  void DataProduct::harvestKeywords(const tip::Header & header) {
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

  void DataProduct::updateKeywords(const std::string & file_name) const {
    // For convenience, make a local reference to tip's file service singleton.
    tip::IFileSvc & file_service = tip::IFileSvc::instance();

    // Get file summary, which lists all extensions in the file.
    tip::FileSummary summary;

    file_service.getFileSummary(file_name, summary);

    // If DATE keyword is present, set it to the current date/time.
    KeyValuePairCont_t::iterator date_itor = m_key_value_pairs.find("DATE");
    if (m_key_value_pairs.end() != date_itor)
     date_itor->second = formatDateKeyword(time(0));

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

  std::string DataProduct::formatDateKeyword(const time_t & time) const {
    // Standard date format defined by FITS standard.
    char string_time[] = "YYYY-MM-DDThh:mm:ss";

    // Format using ctime functions.
    struct tm * loc_time = localtime(&time);
    strftime(string_time, sizeof(string_time), "%Y-%m-%dT%H:%M:%S", loc_time);

    // Return formatted time string.
    return string_time;
  }

}
