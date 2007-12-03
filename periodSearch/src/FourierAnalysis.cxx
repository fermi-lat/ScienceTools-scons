/** \file FourierAnalysis.cxx
    \brief Implementation of FourierAnalysis class.
    \authors Masaharu Hirayama, GSSC
             James Peachey, HEASARC/GSSC
*/

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "ChiSquaredProb.h"
#include "FourierAnalysis.h"
#include "StatisticViewer.h"

#include "fftw/fftw3.h"

using namespace periodSearch;

  FourierAnalysis::FourierAnalysis(double t_start, double t_stop, double width, PeriodSearch::size_type num_bins, int /* num_events */):
    PeriodSearch(num_bins / 2 + 1), m_index(), m_t_start(t_start), m_t_stop(t_stop),
    m_width(width), m_fourier_res(0.), m_num_segments(0) , m_num_bins(num_bins) {
    if (t_start > t_stop) throw std::runtime_error("FourierAnalysis: start time is > stop time");
    // m_index.reserve(num_events);

    // Set up frequency array.
    m_fourier_res = 1. / (m_width * m_num_bins);
    for (size_t ii = 0; ii < m_freq.size(); ++ii) {
      m_freq[ii] = ii * m_fourier_res;
    }
  }

  void FourierAnalysis::fill(double evt_time) {
    if (m_t_start <= evt_time && evt_time <= m_t_stop) {
      // Compute index as if we had one huge array with all the segments.
      size_type global_idx = size_type(std::floor((evt_time - m_t_start) / m_width) + .5);
      // Determine segment for this event.
      size_type segment_idx = global_idx / m_num_bins;
      // Determine bin within the segment for this event.
      size_type bin_idx = global_idx % m_num_bins;

      // Keep track of the last segment seen.
      m_num_segments = std::max(m_num_segments, segment_idx + 1);

      // Track the segment and bin index in the m_index member.
      m_index.insert(std::make_pair(segment_idx, bin_idx));
    }
  }

  const std::vector<double> & FourierAnalysis::computeStats() {
    double * in = 0;
    fftw_complex * out = 0;
    fftw_plan p = 0;
    size_t num_cpx_elements = m_freq.size();
    size_t num_dbl_elements = 2 * num_cpx_elements;

    // Allocate array for fftw input/output.
    in = (double *) fftw_malloc(sizeof(double) * num_dbl_elements);
    if (0 == in) throw std::runtime_error("FourierAnalysis could not allocate array");

    // Perform transform in place.
    out = (fftw_complex *) in;

    // Set up to perform fftw-style transform.
    // p = fftw_plan_dft_1d(m_num_bins, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    p = fftw_plan_dft_r2c_1d(m_num_bins, in, out, FFTW_ESTIMATE);

    // Iterate over segments.
    for (size_t seg_idx = 0; seg_idx < m_num_segments; ++seg_idx) {
      // Initialize array to be all zero.
      std::memset(in, '\0', sizeof(double) * num_dbl_elements);

      // Populate the real portion of the input array.
      double num_events = 0.;
      for (index_map_type::iterator itor = m_index.lower_bound(seg_idx); itor != m_index.upper_bound(seg_idx); ++itor) {
        // Increment the number of counts observed in this bin.
        ++in[itor->second];
        // Increment the total number of events observed in this segment.
        ++num_events;
      }

      // Shift origin by the average number of events per bin.
      double events_per_bin = num_events / m_num_bins;
      for (size_t ii = 0; ii < m_num_bins; ++ii) {
        //in[ii][0] -= events_per_bin;
        in[ii] -= events_per_bin;
      }

      // Do the transformation.
      fftw_execute(p);

      // Pack results into the array holding the power density.
      for (size_t ii = 0; ii < num_cpx_elements; ++ii) {
        const double & real = out[ii][0];
        const double & imag = out[ii][1];
        m_spec[ii] += (real * real + imag * imag) * 2. / num_events;
      }
    }

    fftw_destroy_plan(p);
    fftw_free(in);

    return m_spec;
  }

  PeriodSearch::size_type FourierAnalysis::numIndepTrials(double min_freq, double max_freq) const {
    std::pair<size_type, size_type> indices = getRangeIndex(min_freq, max_freq);
    size_type num_indep_trials = indices.second >= indices.first ? indices.second - indices.first : 0;
    return num_indep_trials;
  }

  std::pair<double,double> FourierAnalysis::chanceProbOneTrial(double stat) const {
    periodSearch::ChiSquaredProb prob(2 * m_num_segments);
    return prob(stat);
  }

  std::string FourierAnalysis::getDescription() const {
    std::ostringstream os;
    os << "Search Type: Fourier Analysis\n"
       << "Fourier Resolution: " << m_fourier_res << " Hz\n"
       << "Sampling Frequency: " << m_fourier_res << " Hz\n"
       << "Data Binning: " << m_num_segments << " segments with " << m_num_bins << " time bins in each segment\n"
       << "Probability Distribution: Chi Squared with " << 2 * m_num_segments << " degrees of freedom";
    return os.str();
  }

  StatisticViewer FourierAnalysis::getViewer(bool copy_data, double min_freq, double max_freq) const {
    // Let the base class create a viewer.
    StatisticViewer viewer = PeriodSearch::getViewer(copy_data, min_freq, max_freq);

    // Add/modify plot title.
    viewer.setTitle("Fourier Analysis: Power Spectrum");

    // Return the viewer.
    return viewer;
  }
