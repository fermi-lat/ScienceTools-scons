/** \file DataProduct.cxx
    \brief Base class for encapsulations of specific data products, with methods to read/write them using tip.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/DataProduct.h"
#include "evtbin/Hist.h"
#include "evtbin/RecordBinFiller.h"
#include "tip/Extension.h"
#include "tip/FileSummary.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  DataProduct::DataProduct(const std::string & event_file): m_key_value_pairs(), m_known_keys(), m_data_dir(),
    m_event_file(event_file), m_gti(), m_hist_ptr(0) {
    // Find the directory containing templates.
    const char * top_dir = getenv("EVTBINROOT");
    if (0 != top_dir) m_data_dir = std::string(top_dir) + "/data/";

    // Make a list of known keywords. These can be harvested from the input events extension
    // and used to update the output file(s).
    const char * keys[] = { "TELESCOP", "INSTRUME", "DATE", "DATE-OBS", "DATE-END", "OBJECT", "TIMESYS", "MJDREF",
      "EQUNINOX", "RADECSYS", "EXPOSURE", "ONTIME", "TSTART", "TSTOP" };
    m_known_keys.insert(m_known_keys.end(), keys, keys + sizeof(keys) / sizeof(const char *));

    // Read GTI.
    readGti(event_file);
  }

  DataProduct::~DataProduct() throw() {}

  void DataProduct::binInput() {
    using namespace tip;
    std::auto_ptr<const Table> events(IFileSvc::instance().readTable(m_event_file, "EVENTS"));

    binInput(events->begin(), events->end());
  }

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

  void DataProduct::readGti(const std::string & in_file) {
    std::auto_ptr<const tip::Table> gti_table(tip::IFileSvc::instance().readTable(in_file, "GTI"));

    // Clear the table.
    m_gti.setNumIntervals(gti_table->getNumRecords());

    // Start at beginning of the data.
    Gti::Iterator itor = m_gti.begin();

    // Start at beginning of the table.
    for (tip::Table::ConstIterator table_itor = gti_table->begin(); table_itor != gti_table->end(); ++table_itor, ++itor) {
      itor->first = (*table_itor)["START"].get();
      itor->second = (*table_itor)["STOP"].get();
    }
  }

  void DataProduct::writeGti(const std::string & out_file) const {
    std::auto_ptr<tip::Table> gti_table(tip::IFileSvc::instance().editTable(out_file, "GTI"));

    // Resize Gti extension to match gti data.
    gti_table->setNumRecords(m_gti.getNumIntervals());

    // Start at beginning of the data.
    Gti::ConstIterator itor = m_gti.begin();

    // Write the gti structure to the table.
    for (tip::Table::Iterator table_itor = gti_table->begin(); table_itor != gti_table->end(); ++table_itor, ++itor) {
      (*table_itor)["START"].set(itor->first);
      (*table_itor)["STOP"].set(itor->second);
    }
  }

  const Gti & DataProduct::getGti() const { return m_gti; }

  bool DataProduct::adjustGti(const Binner * binner) {
    // Get number of bins.
    long num_bins = binner->getNumBins();

    // Create a fake GTI-like object.
    Gti fake_gti;

    // Convert bins from binner into the new Gti.
    for (long ii = 0; ii < num_bins; ++ii) {
      // Get the binner interval.
      const Binner::Interval & interval = binner->getInterval(ii);

      // Add the same interval to the Gti.
      fake_gti.insertInterval(interval.begin(), interval.end());
    }

    // Find overlap between the original Gti and the fake one we just constructed.
    fake_gti = m_gti & fake_gti;

    // If this changed the gti at all, return true.
    if (fake_gti != m_gti) {
      m_gti = fake_gti;
      return true;
    }
    return false;
  }

  void DataProduct::writeEbounds(const std::string & out_file, const Binner * binner) const {
    // Open EBOUNDS extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "EBOUNDS"));

    // Resize table: number of records in output file must == the number of bins in the binner.
    output_table->setNumRecords(binner->getNumBins());

    // Need output table iterator.
    tip::Table::Iterator table_itor = output_table->begin();

    // Iterate over bin number and output table iterator, writing fields in order.
    for (long index = 0; index != binner->getNumBins(); ++index, ++table_itor) {
      // From the binner, get the interval.
      Binner::Interval interval = binner->getInterval(index);

      // Write channel number.
      (*table_itor)["CHANNEL"].set(index + 1);

      // Write beginning/ending value of interval into E_MIN/E_MAX, converting from MeV to keV.
      (*table_itor)["E_MIN"].set(1000. * interval.begin());
      (*table_itor)["E_MAX"].set(1000. * interval.end());
    }
  }

  void DataProduct::harvestKeywords(const std::string & file_name, const std::string & ext_name) {
    std::auto_ptr<const tip::Table> table(tip::IFileSvc::instance().readTable(file_name, ext_name));
    harvestKeywords(table->getHeader());
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

  void DataProduct::adjustTimeKeywords(const std::string & sc_file, const Binner * binner) {
    std::stringstream ss;
    ss.precision(24);
    if (0 != binner) {
      // Update TSTART and TSTOP keywords to reflect the new boundaries of the binning interval,
      // only if they are more restrictive than the original TSTART/TSTOP.
      if (m_key_value_pairs["TSTART"].empty()) {
        // TSTART keyword was blank, so set it.
        ss << binner->getInterval(0).begin() << std::endl;
        ss >> m_key_value_pairs["TSTART"];
      } else {
        ss << m_key_value_pairs["TSTART"] << std::endl;
        double tstart;
        ss >> tstart;
        if (binner->getInterval(0).begin() > tstart) {
          // Binner has later start time, so change TSTART to match it.
          ss << binner->getInterval(0).begin() << std::endl;
          ss >> m_key_value_pairs["TSTART"];
        }
      }

      if (m_key_value_pairs["TSTOP"].empty()) {
        // TSTOP keyword was blank, so set it.
        ss << binner->getInterval(binner->getNumBins() - 1).end() << std::endl;
        ss >> m_key_value_pairs["TSTOP"];
      } else {
        ss << m_key_value_pairs["TSTOP"] << std::endl;
        double tstop;
        ss >> tstop;
        if (binner->getInterval(binner->getNumBins() - 1).end() < tstop) {
          // Binner has earlier stop time, so change TSTOP to match it.
          ss << binner->getInterval(binner->getNumBins() - 1).end() << std::endl;
          ss >> m_key_value_pairs["TSTOP"];
        }
      }
    }

    // Compute the EXPOSURE keyword.
    ss << computeExposure(sc_file) << std::endl;
    ss >> m_key_value_pairs["EXPOSURE"];

    // Compute the ONTIME keyword.
    ss << m_gti.computeOntime() << std::endl;
    ss >> m_key_value_pairs["ONTIME"];
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

  double DataProduct::computeExposure(const std::string & sc_file) const {
    // Open the spacecraft data table.
    std::auto_ptr<const tip::Table> sc_table(tip::IFileSvc::instance().readTable(sc_file, "Ext1"));

    // If no rows in the table, issue a warning and then return 0.
    if (0 == sc_table->getNumRecords()) {
      std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data file is empty!" << std::endl;
      return 0.;
    }

    // Start with no exposure.
    double exposure = 0.0;

    // Start from beginning of first interval in the GTI.
    Gti::ConstIterator gti_pos = m_gti.begin();

    // In the spacecraft data table, start from the first entry.
    tip::Table::ConstIterator itor = sc_table->begin();

    // Check first entry in the table for validity.
    if ((*itor)["START"].get() > m_gti.begin()->first) 
      std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data commences after start of first GTI" << std::endl;

    // Iterate through the spacecraft data.
    for (; itor != sc_table->end(); ++itor) {
      double start = (*itor)["START"].get();
      double stop = (*itor)["STOP"].get();

      // Compute the total fraction of this time which overlaps one or more intervals in the GTI extension.
      double fract = m_gti.getFraction(start, stop, gti_pos);

      // Use this fraction to prorate the livetime before adding it to the total exposure time.
      exposure += fract * (*itor)["LIVETIME"].get();
    }

    // Go back to last position in the table, and check it for validity.
    --itor;
    if ((*itor)["STOP"].get() < (m_gti.end() - 1)->second)
      std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data ceases before end of last GTI" << std::endl;
    
    return exposure;
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
