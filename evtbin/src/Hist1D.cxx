/** \file Hist1D.cxx
    \brief One dimensional histogram.
*/
#include <stdexcept>

#include "evtbin/Binner.h"
#include "evtbin/Hist1D.h"

namespace evtbin {

  Hist1D::Hist1D(const Binner * binner): m_data() {
    // Check for valid input:
    if (0 == binner) throw std::logic_error("Hist1D cannot be constructed with a null binner pointer.");

    // Set initial size of data array:
    m_data.resize(binner->getNumBins(), 0);

    // Save binner:
    m_binners.resize(1, binner);
  }

  Hist1D::~Hist1D() throw() {}

  void Hist1D::fillBin(const std::vector<double> & value) {
    fillBin(value[0]);
  }

  void Hist1D::fillBin(double value) {
    // Use the binner to determine the index for the data:
    long index = m_binners[0]->computeIndex(value);

    // Make sure index is valid:
    if (0 <= index) {
      // Increment the appropriate bin:
      ++m_data[index];
    }
  }

}
