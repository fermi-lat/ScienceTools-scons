/** \file Hist.cxx
    \brief Base class for histogram abstractions.
*/
#include "evtbin/Binner.h"
#include "evtbin/Hist.h"

namespace evtbin {

  Hist::~Hist() throw() {
    for (BinnerCont_t::reverse_iterator itor = m_binners.rbegin(); itor != m_binners.rend(); ++itor)
      delete *itor;
  }

  const Hist::BinnerCont_t & Hist::getBinners() const { return m_binners; }

}
