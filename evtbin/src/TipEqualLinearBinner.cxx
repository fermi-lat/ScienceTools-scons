#include <cmath>
#include <iostream>
#include <stdexcept>

#include "evtbin/TipEqualLinearBinner.h"

TipEqualLinearBinner::TipEqualLinearBinner(const std::string & input_field, const std::string & output_field,
  double interval_begin, double interval_end, double interval_width):
  m_histogram(),
  m_input_field(input_field),
  m_output_field(output_field),
  m_interval_begin(interval_begin),
  m_interval_end(interval_end),
  m_interval_width(interval_width) {
  // Make sure there is enough room:
  // JP: todo: what is ceil in C++?
  long num_bins = long(ceil( (m_interval_end - m_interval_begin)/ m_interval_width));

  // Confirm a positive number of bins:
  if (num_bins <= 0) throw std::runtime_error("interval begin is > interval end");

  // Resize the vector, and initialize each member to 0:
  m_histogram.resize(num_bins, 0.);
}

void TipEqualLinearBinner::writeHistogram(tip::Table * out_table) const {
  // Change the size of the output table to hold the bins:
  out_table->setNumRecords(m_histogram.size());

  // Get output iterator which points to the first record in the output table:
  tip::Table::Iterator table_itor = out_table->begin();

  // Start with the beginning of the first interval:
  // TODO: This should be the midpoint, not the beginning of the interval!
  double current_interval = m_interval_begin;
  
  // Iterate over this object's stored histogram:
  for (std::vector<double>::const_iterator hist_itor = m_histogram.begin(); hist_itor != m_histogram.end(); ++hist_itor, ++table_itor) {

    // Print the value for now:
    std::cout << *hist_itor << std::endl;

    // Write the interval into the output table:
    (*table_itor)[m_input_field].set(current_interval);

    // Go on to the next interval:
    current_interval += m_interval_width;

    // Write the histogram bin to the selected field of the output file:
    (*table_itor)[m_output_field].set(*hist_itor);
  }
}

void TipEqualLinearBinner::binRecord(const tip::Table::ConstRecord & record) {
  long index = computeIndex(record);
  if (index >= 0) ++m_histogram[index];
}

long TipEqualLinearBinner::computeIndex(const tip::Table::ConstRecord & record) const {
  double value;

  // Get the input value of the field we are binning:
  record[m_input_field].get(value);

  // Exclude values outside of [interval_begin, interval_end)
  if (value >= m_interval_begin && value < m_interval_end)
    return long((value - m_interval_begin) / m_interval_width);

  return -1;
}

long TipEqualLinearBinner::getNumBins() const { return m_histogram.size(); }

const std::string & TipEqualLinearBinner::getInputField() const { return m_input_field; }

const std::string & TipEqualLinearBinner::getOutputField() const { return m_output_field; }
