#include <cmath>
#include <iostream>
#include <stdexcept>

#include "evtbin/TipEqualLogBinner.h"

TipEqualLogBinner::TipEqualLogBinner(const std::string & input_field, const std::string & output_field,
  double interval_begin, double interval_end, unsigned long num_bins):
  m_histogram(),
  m_bins(),
  m_input_field(input_field),
  m_output_index("CHANNEL"),
  m_output_field(output_field),
  m_interval_begin(interval_begin),
  m_interval_end(interval_end),
  m_log_width(),
  m_num_bins(num_bins) {
  // Confirm a positive number of bins:
  if (m_num_bins <= 0) throw std::runtime_error("interval begin is > interval end");

  // Resize the histogram, and initialize each member to 0:
  m_histogram.resize(m_num_bins, 0.);

  // Resize the bin vector, allowing room for the final boundary (interval_end).
  m_bins.resize(m_num_bins + 1);

  // Size of logarithmic bins:
  m_log_width = log(m_interval_end/m_interval_begin) / m_num_bins;

  // Compute bin vector:
  for (unsigned long index = 0; index < m_num_bins; ++index)
    m_bins[index] = m_interval_begin * exp(index * m_log_width);

  // Last bin boundary is the supplied interval end.
  m_bins[m_num_bins] = m_interval_end;
}

void TipEqualLogBinner::writeHistogram(tip::Table * out_table) const {
  // Change the size of the output table to hold the bins:
  out_table->setNumRecords(m_num_bins);

  // Get output iterator which points to the first record in the output table:
  tip::Table::Iterator table_itor = out_table->begin();

  // Iterate over this object's stored histogram:
  for (unsigned long ii = 0; ii < m_num_bins; ++ii, ++table_itor) {

    // Print the value for now:
    std::cout << ii + 1 << '\t' << m_histogram[ii] << std::endl;

    // Write the index into the output table:
    (*table_itor)[m_output_index].set(ii + 1);

    // Write the histogram bin to the selected field of the output file:
    (*table_itor)[m_output_field].set(m_histogram[ii]);
  }
}

void TipEqualLogBinner::binRecord(const tip::Table::ConstRecord & record) {
  long index = computeIndex(record);
  if (index >= 0) ++m_histogram[index];
}

long TipEqualLogBinner::computeIndex(const tip::Table::ConstRecord & record) const {
  double value;

  // Get the input value of the field we are binning:
  record[m_input_field].get(value);

  // Compute the bin number, first as a double.
  double bin_number = log(value / m_interval_begin) / m_log_width;

  // Make sure bin is in range of histogram, then add the count to the proper bin.
  if (bin_number >= 0. && bin_number < m_num_bins)
    return long(bin_number);

  return -1;
}

long TipEqualLogBinner::getNumBins() const { return m_num_bins; }

const std::string & TipEqualLogBinner::getInputField() const { return m_input_field; }

const std::string & TipEqualLogBinner::getOutputField() const { return m_output_field; }
