/** \file Hist2D.h
    \brief Two dimensional histogram.
*/

#include "evtbin/Binner.h"
#include "evtbin/Hist2D.h"

namespace evtbin {

  Hist2D::Hist2D(const Binner * binner1, const Binner * binner2): m_data(), m_binner1(binner1), m_binner2(binner2) {
    // Verify input:
    if (0 == m_binner1 || 0 == m_binner2) throw std::logic_error("Hist2D cannot be constructed with a null binner pointer.");

    // Set initial size of data array:
    m_data.resize(m_binner1->getNumBins());
    for (Cont_t::iterator itor = m_data.begin(); itor != m_data.end(); ++itor) {
      itor->resize(m_binner2->getNumBins(), 0);
    }
  }

  Hist2D::~Hist2D() throw() {}

  void Hist2D::fillBin(double value1, double value2) {
    // Use the binners to determine the indices for the data:
    long index1 = m_binner1->computeIndex(value1);
    long index2 = m_binner2->computeIndex(value2);

    // Make sure indices are valid:
    if (0 <= index1 && 0 <= index2) {
      // Increment the appropriate bin:
      ++m_data[index1][index2];
    }
  }

}
