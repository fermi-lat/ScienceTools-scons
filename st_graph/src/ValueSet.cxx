#include <stdexcept>

#include "st_graph/ValueSet.h"

namespace st_graph {

  ValueSet::ValueSet(const std::vector<double> & values, const std::vector<double> & spreads): m_values(values),
    m_spreads(spreads) {
    // Make sure by the end of the constructor, m_values and m_spreads have the same number of items.
    if (m_values.size() < m_spreads.size()) {
      m_spreads.resize(m_values.size());
      // This is probably a mistake:
      throw std::logic_error("st_graph::ValueSet constructor was passed fewer values than spreads");
    } else if (m_values.size() > m_spreads.size()) {
      // This is probably OK: maybe there are no errors for this set of values.
      m_spreads.resize(m_values.size(), 0.);
    }
  }

  const std::vector<double> & ValueSet::getValues() const { return m_values; }

  const std::vector<double> & ValueSet::getSpreads() const { return m_spreads; }
  
}
