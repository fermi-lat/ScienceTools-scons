/** \file Hist1D.cxx
    \brief One dimensional histogram.
*/
#include <stdexcept>

#include "evtbin/Binner.h"
#include "evtbin/Hist1D.h"

namespace evtbin {

  Hist1D::Hist1D(const Binner & binner): m_data(binner.getNumBins(), 0.) {
    // Save binner:
    m_binners.resize(1, binner.clone());
  }

  Hist1D::~Hist1D() throw() {}

  void Hist1D::fillBin(const std::vector<double> & value, double weight) {
    fillBin(value[0], weight);
  }

  void Hist1D::fillBin(double value, double weight) {
    // Use the binner to determine the index for the data:
    long index = m_binners[0]->computeIndex(value);

    // Make sure index is valid:
    if (0 <= index) {
      // Grow the container to accomodate this value, if necessary.
      if (Cont_t::size_type(index) >= m_data.size()) m_data.resize(index + 1);

      // Increment the appropriate bin:
      m_data[index] += weight;
    }
  }

}
