/** \file SingleSpec.cxx
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/SingleSpec.h"

#include "st_facilities/Env.h"

#include "facilities/commonUtilities.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  SingleSpec::SingleSpec(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
    const std::string & sc_table, const Binner & binner, const Binner & ebounds, const Gti & gti):
    DataProduct(event_file, event_table, gti), m_hist(binner), m_ebounds(ebounds.clone()) {
    m_hist_ptr = &m_hist;

    // Collect any/all needed keywords from the primary extension.
    harvestKeywords(m_event_file_cont);

    // Collect any/all needed keywords from the ebounds extension.
    // But do not fail if ebounds isn't there.  This is for GBM headers.
    try {
      harvestKeywords(m_event_file_cont, "EBOUNDS");
    } catch (...){}

    // Collect any/all needed keywords from the events extension.
    harvestKeywords(m_event_file_cont, m_event_table);

    // Update tstart/tstop etc.
    adjustTimeKeywords(sc_file, sc_table);
  }

  SingleSpec::~SingleSpec() throw() { delete m_ebounds; }

  void SingleSpec::writeOutput(const std::string & creator, const std::string & out_file) const {
    // The binner from the histogram will be used below.
    const Binner * binner = m_hist.getBinners().at(0);

    // Add DETCHANS, which is just the number of bins in the binner.
    updateKeyValue("DETCHANS", binner->getNumBins(), "Total number of detector channels available.");

    // Standard file creation from base class.
    createFile(creator, out_file, facilities::commonUtilities::joinPath(m_data_dir, "LatSingleBinnedTemplate"));

    // Open SPECTRUM extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SPECTRUM"));

    // Write DSS keywords to preserve cut information.
    writeDssKeywords(output_table->getHeader());

    // Resize table: number of records in output file must == the number of bins in the binner.
    output_table->setNumRecords(binner->getNumBins());

    // Need output table iterator.
    tip::Table::Iterator table_itor = output_table->begin();

    // Iterate over bin number and output table iterator, writing fields in order.
    double total_counts=0;
    double total_error_channel=0;
    for (long index = 0; index != binner->getNumBins(); ++index, ++table_itor) {
      // Channel of each bin.
      (*table_itor)["CHANNEL"].set(index + 1);

      // Number of counts in each bin, from the histogram.
      (*table_itor)["COUNTS"].set(m_hist[index]);

      //Keep a running total of binned counts.
      if (index <= 126){
	total_counts+=m_hist[index];
      }else{
	total_error_channel+=m_hist[index];
      }

      //Statistical Error
      (*table_itor)["STAT_ERR"].set(calcStatErr(m_hist[index]));
    }

    // Write the EBOUNDS extension.
    writeEbounds(out_file, m_ebounds);

    //Check for and if needed make gbm specific correction for deadtime.
    gbmExposure(total_counts, total_error_channel, out_file);

    // Write GTI extension.
    writeGti(out_file);
  }

}
