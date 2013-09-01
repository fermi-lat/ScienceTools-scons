/** \file TipBinner.cxx
    \brief Implmentation of base class TipBinner.
*/
#include "evtbin/TipBinner.h"

TipBinner::~TipBinner() {}

void TipBinner::fillBins(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
  // Iterate over the given range:
  for (tip::Table::ConstIterator itor = begin; itor != end; ++itor) {
    // Bin a single record (row):
    binRecord(*itor);
  }
}
