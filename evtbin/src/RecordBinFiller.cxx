/** \file RecordBinFiller.cxx
    \brief Functor which bins data from tip table records into Hist objects.
*/
#include "evtbin/RecordBinFiller.h"

namespace evtbin {

  RecordBinFiller::RecordBinFiller(Hist & hist): m_hist(hist), m_binners(hist.getBinners()), m_value(hist.getBinners().size()) {}

}
