/** \file Hist3D.h
    \brief Three dimensional histogram.
*/
#include <stdexcept>

#include "evtbin/Binner.h"
#include "evtbin/Hist3D.h"

namespace evtbin {

  Hist3D::Hist3D(const Binner & binner1, const Binner & binner2, const Binner & binner3): m_data() {
    // Set initial size of data array:
    m_data.resize(binner1.getNumBins());
    for (Cont_t::iterator itor1 = m_data.begin(); itor1 != m_data.end(); ++itor1) {
      itor1->resize(binner2.getNumBins());
      for (Cont_t::value_type::iterator itor2 = itor1->begin(); itor2 != itor1->end(); ++itor2) {
        itor2->resize(binner3.getNumBins(), 0.);
      }
    }

    // Save binners:
    m_binners.resize(3);
    m_binners[0] = binner1.clone();
    m_binners[1] = binner2.clone();
    m_binners[2] = binner3.clone();
  }

  Hist3D::~Hist3D() throw() {}

  void Hist3D::fillBin(const std::vector<double> & value, double weight) {
    fillBin(value[0], value[1], value[2], weight);
  }

  void Hist3D::fillBin(double value1, double value2, double value3, double weight) {
    // Use the binners to determine the indices for the data:
    long index1 = m_binners[0]->computeIndex(value1);
    long index2 = m_binners[1]->computeIndex(value2);
    long index3 = m_binners[2]->computeIndex(value3);

    // Make sure indices are valid:
    if (0 <= index1 && 0 <= index2 && 0 <= index3) {
      // Grow the container to accomodate this value, if necessary.
      if (Cont_t::size_type(index1) >= m_data.size()) m_data.resize(index1 + 1);
      if (Cont_t::size_type(index2) >= m_data[index1].size()) m_data[index1].resize(index2 + 1);
      if (Cont_t::size_type(index3) >= m_data[index1][index2].size()) m_data[index1][index2].resize(index3 + 1); 


      // Increment the appropriate bin:
      m_data[index1][index2][index3] += weight;
    }
  }

  void Hist3D::getImage(std::vector<float> & image) const {
    if (!m_data.empty()) {
      // Get the sizes of the 3 dimensions from the binners.
      Cont_t::size_type size0 = m_binners[0]->getNumBins();
      Cont_t::size_type size1 = m_binners[1]->getNumBins();
      Cont_t::size_type size2 = m_binners[2]->getNumBins();

      // Resize the output image accordingly.
      image.resize(size0 * size1 * size2, 0.);
      for (Cont_t::size_type index0 = 0; index0 != size0; ++index0) {
        for (Cont_t::size_type index1 = 0; index1 != size1; ++index1) {
          for (Cont_t::size_type index2 = 0; index2 != size2; ++index2) {
            image[index0 + index1 * size0 + index2 * size0 * size1] = m_data[index0][index1][index2];
          }
        }
      }

    } else {
      image.clear();
    }
  }
}
