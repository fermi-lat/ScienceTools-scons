/** \file MultiSpec.cxx
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/MultiSpec.h"

#include "st_facilities/Env.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  MultiSpec::MultiSpec(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
    const Binner & time_binner, const Binner & energy_binner, const Binner & ebounds, const Gti & gti):
    DataProduct(event_file, event_table, gti), m_hist(time_binner, energy_binner), m_ebounds(ebounds.clone()) {
    m_hist_ptr = &m_hist;

    // Collect any/all needed keywords from the primary extension.
    harvestKeywords(m_event_file);

    // Collect any/all needed keywords from the events extension.
    harvestKeywords(m_event_file, m_event_table);

    // Update tstart/tstop etc.
    adjustTimeKeywords(sc_file, &time_binner);
  }

  MultiSpec::~MultiSpec() throw() { delete m_ebounds; }

  void MultiSpec::writeOutput(const std::string & creator, const std::string & out_file) const {
    const Binner * energy_binner = m_hist.getBinners().at(1);

    // Add DETCHANS, which is just the number of bins in the energy binner.
    updateKeyValue("DETCHANS", energy_binner->getNumBins(), "Total number of detector channels available.");

    // Standard file creation from base class.
    createFile(creator, out_file, st_facilities::Env::appendFileName(m_data_dir, "LatBinnedTemplate"));

    // Open SPECTRUM extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SPECTRUM"));

    // Get number of bins in each dimension.
    const Binner * time_binner = m_hist.getBinners().at(0);
    long num_time_bins = time_binner->getNumBins();
    long num_energy_bins = m_hist.getBinners().at(1)->getNumBins();

    // Tweak the dimensionality of output table.
    // Need output table iterator.
    tip::Table::Iterator table_itor = output_table->begin();

    // Resize counts field: number of elements must be the same as the number of bins in the energy binner.
    (*table_itor)["CHANNEL"].setNumElements(num_energy_bins);
    (*table_itor)["COUNTS"].setNumElements(num_energy_bins);
    
    // Resize table: number of records in output file must == the number of bins in the time binner.
    output_table->setNumRecords(num_time_bins);

    long * channel = new long[num_energy_bins];
    for (long index = 0; index != num_energy_bins; ++index) channel[index] = index + 1;

    // Iterate over bin number and output table iterator, writing fields in order.
    for (long index = 0; index != num_time_bins; ++index, ++table_itor) {
      // Get interval of this time bin.
      const Binner::Interval & time_int = time_binner->getInterval(index);

      // Record time binning information.
      (*table_itor)["TSTART"].set(time_int.begin());
      (*table_itor)["TELAPSE"].set(time_int.width());

      // Number the spectra.
      (*table_itor)["SPEC_NUM"].set(index + 1);

      // Channel of each bin.
      (*table_itor)["CHANNEL"].set(channel, channel + num_energy_bins, 0);

      // Number of counts in each bin, from the histogram.
      (*table_itor)["COUNTS"].set(&(m_hist[index][0]), &(m_hist[index][num_energy_bins]), 0);
    }

    delete [] channel;

    // Write the EBOUNDS extension.
    writeEbounds(out_file, m_ebounds);

    // Write GTI extension.
    writeGti(out_file);
  }

}
