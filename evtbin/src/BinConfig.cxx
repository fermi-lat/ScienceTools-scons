/** \file BinConfig.cxx
    \brief Implementation of helper class which uses standard sets of parameters to configure binners for standard applications.
    \author James Peachey, HEASARC
*/
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>

#include "evtbin/BinConfig.h"
#include "evtbin/ConstSnBinner.h"
#include "evtbin/LinearBinner.h"
#include "evtbin/LogBinner.h"
#include "evtbin/OrderedBinner.h"

// Interactive parameter file access from st_app.
#include "st_app/AppParGroup.h"

// File access from tip.
#include "tip/IFileSvc.h"
// Table access from tip.
#include "tip/Table.h"

namespace evtbin {

  void BinConfig::energyParPrompt(st_app::AppParGroup & par_group) {
    parPrompt(par_group, "energybinalg", "energyfield", "emin", "emax", "deltaenergy", "enumbins", "energybinfile");
  }

  void BinConfig::spatialParPrompt(st_app::AppParGroup & par_group) {
    par_group.Prompt("numxpix");
    par_group.Prompt("numypix");
    par_group.Prompt("pixscale");
    par_group.Prompt("xref");
    par_group.Prompt("yref");
    par_group.Prompt("rafield");
    par_group.Prompt("decfield");
    par_group.Prompt("axisrot");
    par_group.Prompt("proj");
    par_group.Prompt("uselb");
  }

  void BinConfig::timeParPrompt(st_app::AppParGroup & par_group) {
    parPrompt(par_group, "timebinalg", "timefield", "tstart", "tstop", "deltatime", "ntimebins", "timebinfile");
  }

  Binner * BinConfig::createEnergyBinner(const st_app::AppParGroup & par_group) const {
    return createBinner(par_group, "energybinalg", "energyfield", "emin", "emax", "deltaenergy", "enumbins", "energybinfile",
      "ENERGYBINS", "E_MIN", "E_MAX");
  }

  Binner * BinConfig::createTimeBinner(const st_app::AppParGroup & par_group) const {
    return createBinner(par_group, "timebinalg", "timefield", "tstart", "tstop", "deltatime", "ntimebins", "timebinfile",
      "TIMEBINS", "START", "STOP");
  }

  void BinConfig::parPrompt(st_app::AppParGroup & par_group, const std::string & alg, const std::string & in_field,
    const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size, const std::string & num_bins,
    const std::string & bin_file) {
    // Determine the time binning algorithm.
    par_group.Prompt(alg);
    par_group.Prompt(in_field);

    // Get the type of bin specification.
    std::string bin_type = par_group[alg].Value();

    // Make all upper case for case-insensitive comparisons.
    for (std::string::iterator itor = bin_type.begin(); itor != bin_type.end(); ++itor) *itor = toupper(*itor);

    if (bin_type == "LIN") {
      // Get remaining parameters needed for linearly uniform interval binner.
      par_group.Prompt(bin_begin);
      par_group.Prompt(bin_end);
      par_group.Prompt(bin_size);
    } else if (bin_type == "LOG") {
      // Get remaining parameters needed for logarithmically uniform interval binner.
      par_group.Prompt(bin_begin);
      par_group.Prompt(bin_end);
      par_group.Prompt(num_bins);
    } else if (bin_type == "FILE") {
      // Get remaining parameters needed for user defined bins from a bin file.
      par_group.Prompt(bin_file);
    } else throw std::runtime_error(std::string("Unknown binning algorithm ") + par_group[alg].Value());
  }

  Binner * BinConfig::createBinner(const st_app::AppParGroup & par_group, const std::string & alg,
    const std::string & in_field, const std::string & bin_begin, const std::string & bin_end, const std::string & bin_size,
    const std::string & num_bins, const std::string & bin_file, const std::string & bin_ext, const std::string & start_field,
    const std::string & stop_field, const std::string & sn_ratio, const std::string & lc_emin, const std::string & lc_emax) const {
    using namespace evtbin;

    Binner * binner = 0;

    // Get the type of bin specification.
    std::string bin_type = par_group[alg].Value();

    // Make all upper case for case-insensitive comparisons.
    for (std::string::iterator itor = bin_type.begin(); itor != bin_type.end(); ++itor) *itor = toupper(*itor);

    if (bin_type == "LIN") {
      binner = new LinearBinner(par_group[bin_begin], par_group[bin_end], par_group[bin_size], par_group[in_field]);
    } else if (bin_type == "LOG") {
      binner = new LogBinner(par_group[bin_begin], par_group[bin_end], par_group[num_bins], par_group[in_field]);
    } else if (bin_type == "FILE") {
      // Create interval container for user defined bin intervals.
      OrderedBinner::IntervalCont_t intervals;

      std::string bin_ext_uc = bin_ext;

      // Make all upper case for case-insensitive comparisons.
      for (std::string::iterator itor = bin_ext_uc.begin(); itor != bin_ext_uc.end(); ++itor) *itor = toupper(*itor);

      // Open the data file.
      std::auto_ptr<const tip::Table> table(tip::IFileSvc::instance().readTable(par_group[bin_file], bin_ext));

// TODO Refactor this!
      // Temporary hack pending a complete handling of units in tip.
      double factor = 1.;

      // For the moment, energy bins are in keV, period, so in the case of an energy binner, convert keV to MeV.
      if (bin_ext_uc == "EBOUNDS" || bin_ext_uc == "ENERGYBINS") factor = 1.e-3;

      // Iterate over the file, saving the relevant values into the interval array.
      for (tip::Table::ConstIterator itor = table->begin(); itor != table->end(); ++itor) {
        intervals.push_back(Binner::Interval(factor * (*itor)[start_field].get(), factor * (*itor)[stop_field].get()));
      }

      // Create binner from these intervals.
      binner = new OrderedBinner(intervals, par_group[in_field]);

    } else if (bin_type == "SN") {
      binner = new ConstSnBinner(par_group[bin_begin], par_group[bin_end], par_group[sn_ratio], par_group[lc_emin],
        par_group[lc_emax]);
    } else throw std::runtime_error(std::string("Unknown binning algorithm ") + par_group[alg].Value());

    return binner;
  }

}
