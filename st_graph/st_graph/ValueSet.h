#ifndef st_graph_ValueSet_h
#define st_graph_ValueSet_h

#include <vector>

namespace st_graph {

  class ValueSet {
    public:
      ValueSet(const std::vector<double> & values, const std::vector<double> & spreads);
  
      const std::vector<double> & getValues() const;

      const std::vector<double> & getSpreads() const;
  
    private:
      std::vector<double> m_values;
      std::vector<double> m_spreads;
  };

}

#endif
