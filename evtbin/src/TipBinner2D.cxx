#include <cmath>
#include <iostream>
#include <stdexcept>

#include "evtbin/TipBinner2D.h"

TipBinner2D::TipBinner2D(TipBinner * binner1, TipBinner * binner2):
  m_histogram(),
  m_binner1(binner1),
  m_binner2(binner2) {

  // Set first dimension of histogram:
  m_histogram.resize(m_binner1->getNumBins());

  // Set second dimension of each row in the second dimension:
  for (Hist_t::iterator itor = m_histogram.begin(); itor != m_histogram.end(); ++itor) {
    // itor acts like pointer to vector<double>
    itor->resize(m_binner2->getNumBins());
  }
}

TipBinner2D::~TipBinner2D() {}

void TipBinner2D::fillBins(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
  // Iterate over the given range:
  for (tip::Table::ConstIterator itor = begin; itor != end; ++itor) {
    // Bin a single record (row):
    binRecord(*itor);
  }
}

void TipBinner2D::writeHistogram(tip::Table * out_table) const {
  // Change the size of the output table to hold the bins:
  out_table->setNumRecords(m_histogram.size());

  // Get output iterator which points to the first record in the output table:
  tip::Table::Iterator table_itor = out_table->begin();

  // Start with the beginning of the first interval:
  // TODO: This should be the midpoint, not the beginning of the interval!
//  double current_interval = m_interval_begin;
  
  // Write the channel too:
  long channel = 0;

  // Iterate over this object's stored histogram:
  for (std::vector<double>::const_iterator hist_itor = m_histogram[0].begin(); hist_itor != m_histogram[0].end(); ++hist_itor, ++table_itor) {

    // Print the value for now:
    std::cout << *hist_itor << std::endl;

    // Write the interval into the output table:
//    (*table_itor)[m_input_field].set(current_interval);

    // Go on to the next interval:
//    current_interval += m_interval_width;

    // Write the channel:
    (*table_itor)["CHANNEL"].set(++channel);

    // Write the histogram bin to the selected field of the output file:
    (*table_itor)[m_binner2->getOutputField()].set(*hist_itor);
  }
}

void TipBinner2D::binRecord(const tip::Table::ConstRecord & record) {
  long x_index = m_binner1->computeIndex(record);
  long y_index = m_binner2->computeIndex(record);
  if (x_index >= 0 && y_index >= 0) ++m_histogram[x_index][y_index];
}
