/** \file SingleSpec.cxx
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <sstream>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/SingleSpec.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  SingleSpec::SingleSpec(const std::string & event_file, const std::string & sc_file, const Binner & binner):
    DataProduct(event_file), m_hist(binner) {
    m_hist_ptr = &m_hist;

    harvestKeywords(event_file, "EVENTS");

    // Update tstart/tstop etc.
    adjustTimeKeywords(sc_file);
  }

  SingleSpec::~SingleSpec() throw() {}

  void SingleSpec::writeOutput(const std::string & creator, const std::string & out_file) const {
    // The binner from the histogram will be used below.
    const Binner * binner = m_hist.getBinners().at(0);

    // Add DETCHANS, which is just the number of bins in the binner.
    std::ostringstream os;
    os << binner->getNumBins();
    m_key_value_pairs["DETCHANS"] = os.str();

//    m_key_value_pairs["ONTIME"] = 1000.;

    // Standard file creation from base class.
    createFile(creator, out_file, m_data_dir + "LatSingleBinnedTemplate");

    // Open SPECTRUM extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SPECTRUM"));

    // Resize table: number of records in output file must == the number of bins in the binner.
    output_table->setNumRecords(binner->getNumBins());

    // Need output table iterator.
    tip::Table::Iterator table_itor = output_table->begin();

    // Iterate over bin number and output table iterator, writing fields in order.
    for (long index = 0; index != binner->getNumBins(); ++index, ++table_itor) {
      // Channel of each bin.
      (*table_itor)["CHANNEL"].set(index + 1);

      // Number of counts in each bin, from the histogram.
      (*table_itor)["COUNTS"].set(m_hist[index]);
    }

    // Write the EBOUNDS extension.
    writeEbounds(out_file, binner);

    // Write GTI extension.
    writeGti(out_file);
  }

}
