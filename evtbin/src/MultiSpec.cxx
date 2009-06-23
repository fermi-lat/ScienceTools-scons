/** \file MultiSpec.cxx
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <string>

#include "evtbin/Binner.h"
#include "evtbin/MultiSpec.h"
#include "evtbin/SingleSpec.h"

#include "st_facilities/Env.h"

#include "facilities/commonUtilities.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  MultiSpec::MultiSpec(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
    const std::string & sc_table, const Binner & time_binner, const Binner & energy_binner, const Binner & ebounds,
    const Gti & gti): DataProduct(event_file, event_table, gti), m_sc_file(sc_file), m_sc_table(sc_table),
    m_hist(time_binner, energy_binner), m_ebounds(ebounds.clone()) { m_hist_ptr = &m_hist;

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
    adjustTimeKeywords(sc_file, sc_table, &time_binner);
  }

  MultiSpec::~MultiSpec() throw() { delete m_ebounds; }

  void MultiSpec::writeOutput(const std::string & creator, const std::string & out_file) const {
    const Binner * energy_binner = m_hist.getBinners().at(1);

    // Add DETCHANS, which is just the number of bins in the energy binner.
    updateKeyValue("DETCHANS", energy_binner->getNumBins(), "Total number of detector channels available.");

    // Standard file creation from base class.
    createFile(creator, out_file, facilities::commonUtilities::joinPath(m_data_dir, "LatBinnedTemplate"));

    // Open SPECTRUM extension of output PHA1 file. Use an auto_ptr so that the table object
    // will for sure be deleted, even if an exception is thrown.
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SPECTRUM"));

    // Write DSS keywords to preserve cut information.
    writeDssKeywords(output_table->getHeader());

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
    (*table_itor)["STAT_ERR"].setNumElements(num_energy_bins);
    
    // Resize table: number of records in output file must == the number of bins in the time binner.
    output_table->setNumRecords(num_time_bins);

    long * channel = new long[num_energy_bins];
    double * staterr = new double[num_energy_bins];
    for (long index = 0; index != num_energy_bins; ++index) channel[index] = index + 1;

    // Iterate over bin number and output table iterator, writing fields in order.
    double total_counts=0;
    double total_counts2=0;
    double total_error_channel=0;
    double total_error_channel2=0;
    for (long index = 0; index != num_time_bins; ++index, ++table_itor) {
      // Get interval of this time bin.
      const Binner::Interval & time_int = time_binner->getInterval(index);

      // Calculate STAT_ERR for current time bin.
      for (long index2 = 0; index2 != num_energy_bins; ++index2) staterr[index2] = calcStatErr(m_hist[index][index2]);

      // Record time binning information.
      (*table_itor)["TSTART"].set(time_int.begin());
      (*table_itor)["TELAPSE"].set(time_int.width());

      // Number the spectra.
      (*table_itor)["SPEC_NUM"].set(index + 1);

      // Channel of each bin.
      (*table_itor)["CHANNEL"].set(channel, channel + num_energy_bins, 0);

      // Number of counts in each bin, from the histogram.
      (*table_itor)["COUNTS"].set(&(m_hist[index][0]), &(m_hist[index][num_energy_bins]), 0);
            
      // Keep a running total of binned counts for current spectrum.
      for (long index2 = 0; index2 != num_energy_bins; ++index2) {
	if (index2 <=126){
	  total_counts+=m_hist[index][index2];
	}else{
	  total_error_channel+=m_hist[index][index2];
	}
      }
      // And the total for all spectrums.
      total_counts2+=total_counts;
      total_error_channel2+=total_error_channel;

      //Statistical Error
      (*table_itor)["STAT_ERR"].set(staterr, staterr + num_energy_bins, 0);

      // Create a Gti object which contains just the current time interval.
      Gti gti;
      gti.insertInterval(time_int.begin(), time_int.end());

      // Merge this with the overall input Gti.
      gti = gti & m_gti;

      // Create an object which represents just this single spectrum, but with the reduced Gti.
      SingleSpec spec(m_event_file, m_event_table, m_sc_file, m_sc_table, *energy_binner, *m_ebounds, gti);

      // Use the single spectrum to compute exposure for the current spectrum, in a manner similar
      // to keywords for a single spectrum.
      (*table_itor)["EXPOSURE"].set(spec.computeExposure(m_sc_file, m_sc_table));

      // Then check for GBM deadtime for each spectrum.
      // We do this here rather than using the gbmExposure method since it writes to a table and not
      // to a header keyword.
      KeyValuePairCont_t::iterator found2 = m_key_value_pairs.find("EVT_DEAD");
      KeyValuePairCont_t::iterator found3 = m_key_value_pairs.find("EVTDEDHI");
      if (m_key_value_pairs.end() != found2 && !found2->second.empty()) {
	double deadtime;
	found2->second.getValue(deadtime);
	double evtdedhi;
	found3->second.getValue(evtdedhi);
	double gbm_exposure=(gti.computeOntime())-(total_counts*deadtime)-(total_error_channel*evtdedhi);
	(*table_itor)["EXPOSURE"].set(gbm_exposure);
      }
      total_counts=0;
      total_error_channel=0;
    }

    delete [] channel;
    delete [] staterr;

    // Write the EBOUNDS extension.
    writeEbounds(out_file, m_ebounds);

    // Check for and if needed make gbm specific correction for deadtime.
    // This is for the total exposure in the headers.
    gbmExposure(total_counts2, total_error_channel2, out_file);

    // Write GTI extension.
    writeGti(out_file);
  }

}
