/** \file Hist2D.h
    \brief Two dimensional histogram.
*/
#include <stdexcept>

#include "evtbin/Binner.h"
#include "evtbin/Hist2D.h"

namespace evtbin {

  Hist2D::Hist2D(const Binner & binner1, const Binner & binner2): m_data() {
    // Set initial size of data array:
    m_data.resize(binner1.getNumBins());
    for (Cont_t::iterator itor = m_data.begin(); itor != m_data.end(); ++itor) {
      itor->resize(binner2.getNumBins(), 0);
    }

    // Save binners:
    m_binners.resize(2);
    m_binners[0] = binner1.clone();
    m_binners[1] = binner2.clone();
  }

  Hist2D::~Hist2D() throw() {}

  void Hist2D::fillBin(const std::vector<double> & value, double weight) {
    fillBin(value[0], value[1], weight);
  }

  void Hist2D::fillBin(double value1, double value2, double weight) {
    // Use the binners to determine the indices for the data:
    long index1 = m_binners[0]->computeIndex(value1);
    long index2 = m_binners[1]->computeIndex(value2);

    // Make sure indices are valid:
    if (0 <= index1 && 0 <= index2) {
      // Grow the container to accomodate this value, if necessary.
      if (Cont_t::size_type(index1) >= m_data.size()) m_data.resize(index1 + 1);
      if (Cont_t::size_type(index2) >= m_data[index1].size()) m_data[index1].resize(index2 + 1);

      // Increment the appropriate bin:
      m_data[index1][index2] += weight;
    }
  }

  void Hist2D::getImage(std::vector<float> & image) const {
    if (!m_data.empty()) {
      // Get the sizes of the 2 dimensions from the binners.
      Cont_t::size_type size0 = m_binners[0]->getNumBins();
      Cont_t::size_type size1 = m_binners[1]->getNumBins();

      // Resize the output image accordingly.
      image.resize(size0 * size1, 0.);
      for (Cont_t::size_type index0 = 0; index0 != size0; ++index0) {
        for (Cont_t::size_type index1 = 0; index1 != size1; ++index1) {
          image[index0 + index1 * size0] = m_data[index0][index1];
        }
      }

    } else {
      image.clear();
    }
  }
}
