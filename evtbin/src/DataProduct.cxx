/** \file DataProduct.cxx
    \brief Base class for encapsulations of specific data products, with methods to read/write them using tip.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <sstream>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/DataProduct.h"
#include "evtbin/Hist.h"
#include "evtbin/Hist1D.h"
#include "evtbin/Hist2D.h"
#include "evtbin/RecordBinFiller.h"
#include "st_facilities/Env.h"
#include "st_facilities/FileSys.h"
#include "tip/Extension.h"
#include "tip/FileSummary.h"
#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/KeyRecord.h"
#include "tip/Table.h"

namespace {

  // Internal utility class to make it easy to sort/track spacecraft files.
  class SpacecraftTable {
    public:
      SpacecraftTable(const std::string & sc_file, const std::string & sc_table): m_sc_file(sc_file), m_sc_table(sc_table),
        m_first_start(0.), m_last_stop(0.), m_num_rec(0) {
        std::auto_ptr<const tip::Table> table(tip::IFileSvc::instance().readTable(sc_file, sc_table));

        m_num_rec = table->getNumRecords();

        if (0 != m_num_rec) {
          // Track the time range spanned by this file, from first start time...
          tip::Table::ConstIterator itor = table->begin();
          m_first_start = (*itor)["START"].get();

          // ... to last stop time.
          itor = table->end();
          --itor;
          m_last_stop = (*itor)["STOP"].get();
        }

      }

      operator tip::Index_t() const { return getNumRecords(); }

      tip::Index_t getNumRecords() const { return m_num_rec; }

      bool operator <(const SpacecraftTable & table) const {
        return m_first_start != table.m_first_start ? (m_first_start < table.m_first_start) : (m_last_stop < table.m_last_stop);
      }

      bool connects(const SpacecraftTable & table) const { return m_last_stop == table.m_first_start; }

      const tip::Table * openTable() const { return tip::IFileSvc::instance().readTable(m_sc_file, m_sc_table); }

    private:
      std::string m_sc_file;
      std::string m_sc_table;
      double m_first_start;
      double m_last_stop;
      tip::Index_t m_num_rec;
  };

}

namespace evtbin {

  DataProduct::DataProduct(const std::string & event_file, const std::string & event_table, const Gti & gti):
    m_key_value_pairs(), m_known_keys(), m_dss_keys(), m_event_file_cont(), m_data_dir(), m_event_file(event_file),
    m_event_table(event_table), m_gti(gti), m_hist_ptr(0) {
    using namespace st_facilities;

    // Find the directory containing templates.
    m_data_dir = st_facilities::Env::getDataDir("evtbin");

    // Make a list of known keywords. These can be harvested from the input events extension
    // and used to update the output file(s).
    const char * keys[] = { "TELESCOP", "INSTRUME", "DATE", "DATE-OBS", "DATE-END", "OBJECT", "TIMESYS", "MJDREF",
      "EQUNINOX", "RADECSYS", "EXPOSURE", "ONTIME", "TSTART", "TSTOP", "OBSERVER" };
    m_known_keys.insert(m_known_keys.end(), keys, keys + sizeof(keys) / sizeof(const char *));

    // Get container of file names from the supplied input file.
    FileSys::FileNameCont file_cont = FileSys::expandFileList(event_file);

    // Make space for the input file names.
    m_event_file_cont.resize(file_cont.size());

    // Copy input to output.
    FileNameCont_t::size_type index = 0;
    for (FileSys::FileNameCont::iterator itor = file_cont.begin(); itor != file_cont.end(); ++itor, ++index) {
      m_event_file_cont[index] = *itor;
    }
  }

  DataProduct::~DataProduct() throw() {}

  void DataProduct::binInput() {
    using namespace tip;
    for (FileNameCont_t::iterator itor = m_event_file_cont.begin(); itor != m_event_file_cont.end(); ++itor) {
      std::auto_ptr<const Table> events(IFileSvc::instance().readTable(*itor, m_event_table));
      binInput(events->begin(), events->end());
    }
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
    updateKeyValue("CREATOR", creator, "Software and version creating file");

    // Update newly created file with keywords which were harvested from input data.
    updateKeywords(out_file);
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

  const Hist1D & DataProduct::getHist1D() const {
    const Hist1D * hist = dynamic_cast<const Hist1D *>(m_hist_ptr);
    if (0 == hist) throw std::logic_error("DataProduct::getHist1D: not a 1 dimensional histogram");
    return *hist;
  }

  const Hist2D & DataProduct::getHist2D() const {
    const Hist2D * hist = dynamic_cast<const Hist2D *>(m_hist_ptr);
    if (0 == hist) throw std::logic_error("DataProduct::getHist2D: not a 2 dimensional histogram");
    return *hist;
  }

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

  void DataProduct::harvestKeywords(const FileNameCont_t & file_name_cont, const std::string & ext_name) {
    for (FileNameCont_t::const_iterator itor = file_name_cont.begin(); itor != file_name_cont.end(); ++itor) {
      harvestKeywords(*itor, ext_name);
    }
  }

  void DataProduct::harvestKeywords(const std::string & file_name, const std::string & ext_name) {
    std::auto_ptr<const tip::Extension> ext(tip::IFileSvc::instance().readExtension(file_name, ext_name));
    harvestKeywords(ext->getHeader());
  }

  void DataProduct::harvestKeywords(const tip::Header & header) {
    // See if any DSS keywords are present.
    int num_dss_keys = 0;
    try {
      header["NDSKEYS"].get(num_dss_keys);
      m_known_keys.push_back("NDSKEYS");
    } catch (...) {
    }

    // Add all DSS keywords to container of known keys.
    for (int idx = 0; idx < num_dss_keys; ++idx) {
      // Get the number of this sub-sequence of DSS keywords.
      std::ostringstream os;

      // DSS keywords are numbered starting with 1.
      os << idx + 1;

      // Harvest this sub-sequence of DSS keywords.
      std::list<std::string> key_name;
      key_name.push_back("DSTYP" + os.str());
      key_name.push_back("DSUNI" + os.str());
      key_name.push_back("DSVAL" + os.str());
      key_name.push_back("DSREF" + os.str());
      for (std::list<std::string>::iterator itor = key_name.begin(); itor != key_name.end(); ++itor) {
        // Do not add keywords more than once.
        if (m_known_keys.end() == std::find(m_known_keys.begin(), m_known_keys.end(), *itor)) {
          m_dss_keys.push_back(*itor);
          m_known_keys.push_back(*itor);
        }
      }
    }

    // Iterate over keywords which are known to be useful in this case.
    for (KeyCont_t::const_iterator itor = m_known_keys.begin(); itor != m_known_keys.end(); ++itor) {
      try {
        // Read each key record as a whole.
        tip::KeyRecord record;
        header[*itor].getRecord(record);

        // This record was found, so save it in the container of records.
        m_key_value_pairs[*itor] = record;
      } catch (...) {
        // Ignore errors. Keywords are obtained on a best effort basis, but missing them shouldn't
        // cause the software to fail.
      }
    }
  }

  void DataProduct::adjustTimeKeywords(const std::string & sc_file, const std::string & sc_table, const Binner * binner) {
    std::stringstream ss;
    ss.precision(24);
    if (0 != binner) {
      // Get the start of the valid time range from the start of the first bin of the binner.
      double new_tstart = binner->getInterval(0).begin();
      // Find the current value of TSTART, if it is defined.
      KeyValuePairCont_t::iterator found = m_key_value_pairs.find("TSTART");
      if (m_key_value_pairs.end() != found && !found->second.empty()) {
        // Fetch current TSTART value.
        double tstart;
        found->second.getValue(tstart);

        // Use current TSTART or tstart defined by the binner, whichever is later.
        new_tstart = (tstart > new_tstart) ? tstart : new_tstart;
      }
      updateKeyValue("TSTART", new_tstart);

      // Get the stop of the valid time range from the stop of the last bin of the binner.
      double new_tstop = binner->getInterval(binner->getNumBins() - 1).end();
      found = m_key_value_pairs.find("TSTOP");
      if (m_key_value_pairs.end() != found && !found->second.empty()) {
        // Fetch current TSTOP value.
        double tstop;
        found->second.getValue(tstop);

        // Use current TSTOP or tstop defined by the binner, whichever is earlier.
        new_tstop = (tstop < new_tstop) ? tstop : new_tstop;
      }
      updateKeyValue("TSTOP", new_tstop);
    }

    // Compute the EXPOSURE keyword.
    updateKeyValue("EXPOSURE", computeExposure(sc_file, sc_table), "Integration time (in seconds) for the PHA data");

    // Compute the ONTIME keyword.
    updateKeyValue("ONTIME", m_gti.computeOntime(), "Sum of all Good Time Intervals");
  }

  void DataProduct::updateKeywords(const std::string & file_name) const {
    // For convenience, make a local reference to tip's file service singleton.
    tip::IFileSvc & file_service = tip::IFileSvc::instance();

    // Get file summary, which lists all extensions in the file.
    tip::FileSummary summary;

    file_service.getFileSummary(file_name, summary);

    // Update DATE keyword
    updateKeyValue("DATE", formatDateKeyword(time(0)));

    // Find position of last / or \ in file name.
    std::string::size_type last_slash = file_name.find_last_of("/\\");
    // If no slash found, just use whole file name, otherwise assume the file name starts after the /.
    last_slash = std::string::npos == last_slash ? 0 : last_slash + 1;

    // Add FILENAME keyword, set to the file-only portion of the file name.
    updateKeyValue("FILENAME", file_name.substr(last_slash));

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
          if (update_key) keyword.setRecord(key_itor->second);
        }
        delete ext;
      }
    } catch (...) {
      // Make sure this clean up is done even if an exception is encountered.
      delete ext;
      throw;
    }

  }

  void DataProduct::writeDssKeywords(tip::Header & header) const {
    // Iterate over all DSS keywords.
    for (std::list<std::string>::const_iterator dss_itor = m_dss_keys.begin(); dss_itor != m_dss_keys.end(); ++dss_itor) {

      // Look up keywords in dictionary.
      KeyValuePairCont_t::const_iterator itor = m_key_value_pairs.find(*dss_itor);

      // If keyword was found, write it.
      if (m_key_value_pairs.end() != itor) header[*dss_itor].setRecord(itor->second);
    }
  }

  double DataProduct::computeExposure(const std::string & sc_file, const std::string & sc_table) const {
    using namespace st_facilities;

    // If Gti is empty, return 0. exposure.
    if (0 == m_gti.getNumIntervals()) return 0.;

    // If no spacecraft file is available, return the total ontime.
    if (sc_file.empty()) return m_gti.computeOntime();

    // Get container of file names from the supplied input file.
    FileSys::FileNameCont file_name_cont = FileSys::expandFileList(sc_file);

    // Get container of spacecraft files.
    std::vector<SpacecraftTable> table_cont;

    // Fill container of spacecraft files, summing the total number of records at the same time.
    tip::Index_t total_num_rec = 0;
    for (FileSys::FileNameCont::iterator itor = file_name_cont.begin(); itor != file_name_cont.end(); ++itor) {
      table_cont.push_back(SpacecraftTable(*itor, sc_table));
      total_num_rec += table_cont.back().getNumRecords();
    }

    // Sort them into ascending order.
    std::sort(table_cont.begin(), table_cont.end());

    // Start with no exposure.
    double exposure = 0.0;

    // If no rows in the table(s), issue a warning and then return 0.
    if (0 == total_num_rec) {
      std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data file(s) contain no pointings!" << std::endl;
      return exposure;
    }

    // Start from beginning of first interval in the GTI.
    Gti::ConstIterator gti_pos = m_gti.begin();

    // Iterate over spacecraft files.
    for (std::vector<SpacecraftTable>::iterator table_itor = table_cont.begin(); table_itor != table_cont.end(); ++table_itor) {
      std::auto_ptr<const tip::Table> table(table_itor->openTable());

      // In each spacecraft data table, start from the first entry.
      tip::Table::ConstIterator itor = table->begin();

      // Check first entry in the table for validity.
// TODO: move this test outside the loop.
//      if ((*itor)["START"].get() > m_gti.begin()->first) 
//        std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data commences after start of first GTI" << std::endl;

      // Iterate through the spacecraft data.
      for (; itor != table->end(); ++itor) {
        double start = (*itor)["START"].get();
        double stop = (*itor)["STOP"].get();

        // Compute the total fraction of this time which overlaps one or more intervals in the GTI extension.
        double fract = m_gti.getFraction(start, stop, gti_pos);

        // Use this fraction to prorate the livetime before adding it to the total exposure time.
        exposure += fract * (*itor)["LIVETIME"].get();
      }
    }

    // Go back to last position in the table, and check it for validity.
// TODO: make this test work again:
//    --itor;
//    Gti::ConstIterator last = m_gti.end();
//    --last;
//    if ((*itor)["STOP"].get() < last->second)
//      std::clog << "WARNING: DataProduct::computeExposure: Spacecraft data ceases before end of last GTI" << std::endl;
    
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
