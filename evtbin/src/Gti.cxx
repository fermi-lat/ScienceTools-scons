/** \file Gti.cxx
    \brief Implementation of encapsulation of the concept of a GTI. May be constructed from a GTI extension.
    \author James Peachey, HEASARC/GSSC
*/
#include <iostream>
#include <memory>

#include "evtbin/Gti.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

namespace evtbin {
  Gti::Gti(): m_intervals() {}

  Gti::Gti(const std::string & file_name, const std::string & ext_name): m_intervals() {
    // Open GTI extension.
    std::auto_ptr<const tip::Table> gti_table(tip::IFileSvc::instance().readTable(file_name, ext_name));

    // Resize the interval container.
    m_intervals.resize(gti_table->getNumRecords());

    // Start with first interval.
    IntervalCont_t::iterator int_itor = m_intervals.begin();

    // Fill container with intervals from the extension.
    for (tip::Table::ConstIterator itor = gti_table->begin(); itor != gti_table->end(); ++itor, ++int_itor) {
      double start = (*itor)["START"].get();
      double stop = (*itor)["STOP"].get();
      *int_itor = Interval_t(start, stop);
    }
  }

  double Gti::getFraction(double tstart, double tstop, ConstIterator & gti_pos) const {
    double fraction = 0.;

    for (; gti_pos != m_intervals.end(); ++gti_pos) { 
      // Check if this interval ends before GTI starts and return 0. fraction if it does.
      if (tstop <= gti_pos->first) break;

      // Check if this interval is completely contained in the GTI and return 1 if it does.
      if (tstart >= gti_pos->first && tstop <= gti_pos->second) {
        if (tstop == gti_pos->second) ++gti_pos;
        fraction = 1.;
        break;
      }

      // Check if there is some overlap and add that overlap.
      if (tstart < gti_pos->second) {
        double start = tstart > gti_pos->first ? tstart : gti_pos->first;
        double stop = tstop < gti_pos->second ? tstop : gti_pos->second;
        fraction += (stop - start) / (tstop - tstart);
      }

      // Check if this GTI still has some part which might overlap some future interval.
      // If it does, break to avoid incrementing the GTI iterator.
      if (tstop < gti_pos->second) break;
    }

    return fraction;
  }

  Gti Gti::operator &(const Gti & gti) const {
    Gti new_gti;

    ConstIterator it1 = m_intervals.begin();
    ConstIterator it2 = gti.m_intervals.begin();

    // Iterate until either set of intervals is finished.
    while(it1 != m_intervals.end() && it2 != gti.m_intervals.end()) {
      // See if interval 1 comes before interval 2.
      if (it1->second <= it2->first) ++it1;
      // See if interval 2 comes before interval 1.
      else if (it2->second <= it1->first) ++it2;
      else {
        // They overlap, so find latest start time.
        double start = it1->first > it2->first ? it1->first : it2->first;

        // And earliest stop time.
        double stop = it1->second < it2->second ? it1->second: it2->second;

        // Check whether this interval can simply augment the last new interval.
        if (!new_gti.m_intervals.empty() && new_gti.m_intervals.back().second >= start) {
          // Modify last new interval to include the current stop time.
          new_gti.m_intervals.back().second = stop;
        } else {
          // Add a new interval from the current start & stop.
          new_gti.insertInterval(start, stop);
        }

        // Skip to the next interval in the series for whichever interval ends earliest.
        if (it1->second < it2->second) ++it1; else ++it2;
      }
    }

    return new_gti;
  }

  bool Gti::operator !=(const Gti & gti) const { return m_intervals != gti.m_intervals; }

  Gti::Iterator Gti::begin() { return m_intervals.begin(); }

  Gti::Iterator Gti::end() { return m_intervals.end(); }

  Gti::ConstIterator Gti::begin() const { return m_intervals.begin(); }

  Gti::ConstIterator Gti::end() const { return m_intervals.end(); }

  void Gti::insertInterval(double tstart, double tstop) {
    m_intervals.push_back(Interval_t(tstart, tstop));
  }

  int Gti::getNumIntervals() const { return m_intervals.size(); }

  void Gti::setNumIntervals(int num_intv) { m_intervals.resize(num_intv); }

  double Gti::computeOntime() const {
    double on_time = 0.;
    for (IntervalCont_t::const_iterator itor = m_intervals.begin(); itor != m_intervals.end(); ++itor)
      on_time += itor->second - itor->first;
    return on_time;
  }

}
