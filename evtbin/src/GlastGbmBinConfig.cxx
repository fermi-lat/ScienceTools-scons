#include "GlastGbmBinConfig.h"

#include "evtbin/Gti.h"
#include "evtbin/OrderedBinner.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {

  void GlastGbmBinConfig::load() {
    static GlastGbmBinConfig s_prototype;
    s_config_cont.insert(std::make_pair("GLAST::GBM", &s_prototype));
  }

  GlastGbmBinConfig * GlastGbmBinConfig::clone() const {
    return new GlastGbmBinConfig(*this);
  }

  void GlastGbmBinConfig::energyParPrompt(st_app::AppParGroup &) const {
    // Do nothing. For GBM data energy bins are taken from ebounds, and spectral binner bins
    // in channel space.
  }
  
  Binner * GlastGbmBinConfig::createEnergyBinner(const st_app::AppParGroup & par_group) const {
    // Binning GBM data in energy is really making a histogram of the already
    // binned data using the bin numbers to set the bin intervals.

    // Open ebounds extension.
    std::auto_ptr<const tip::Table> ebounds(tip::IFileSvc::instance().readTable(par_group["evfile"], "EBOUNDS"));

    // Create a container of appropriate size for the intervals. 
    OrderedBinner::IntervalCont_t intervals(ebounds->getNumRecords());

    // Fill the intervals using the contents of the channel column.
    OrderedBinner::IntervalCont_t::iterator interval_itor = intervals.begin();
    for (tip::Table::ConstIterator itor = ebounds->begin(); itor != ebounds->end(); ++itor, ++interval_itor) {
      double channel = (*itor)["CHANNEL"].get();

      *interval_itor = Binner::Interval(channel - .5, channel + .5);
    }

    // Binning will occur in "PHA" space.
    return new OrderedBinner(intervals, "PHA");
  }
  
  Binner * GlastGbmBinConfig::createEbounds(const st_app::AppParGroup & par_group) const {
    // Ebounds are read from the ebounds extension.

    // Open ebounds extension.
    std::auto_ptr<const tip::Table> ebounds(tip::IFileSvc::instance().readTable(par_group["evfile"], "EBOUNDS"));

    // Create the intervals. 
    OrderedBinner::IntervalCont_t intervals(ebounds->getNumRecords());

    // Fill the intervals using the contents of the emin/emax column.
    OrderedBinner::IntervalCont_t::iterator interval_itor = intervals.begin();
    for (tip::Table::ConstIterator itor = ebounds->begin(); itor != ebounds->end(); ++itor, ++interval_itor) {
      double e_min = (*itor)["E_MIN"].get();
      double e_max = (*itor)["E_MAX"].get();

      *interval_itor = Binner::Interval(e_min, e_max);
    }

    return new OrderedBinner(intervals);

  }

  Gti * GlastGbmBinConfig::createGti(const st_app::AppParGroup & par_group) const {
    Gti * gti = new Gti;

    // Open events extension.
    std::auto_ptr<const tip::Table> table(tip::IFileSvc::instance().readTable(par_group["evfile"], par_group["evtable"]));
    tip::Table::ConstIterator start_itor = table->begin();
    if (table->end() != start_itor) {
      double tstart = (*start_itor)["TIME"].get();
      tip::Table::ConstIterator stop_itor = table->end();
      --stop_itor;
      double tstop = (*stop_itor)["TIME"].get();
      gti->insertInterval(tstart, tstop);
    }

    return gti;
  }
}
