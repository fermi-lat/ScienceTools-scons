/** \file ValueSet.h
    \brief Interface for ValueSet class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_ValueSet_h
#define st_graph_ValueSet_h

#include <vector>

namespace st_graph {

  /** \class ValueSet
      \brief Generic representation of a set of values with spreads. The values are interpreted as
             central data values. The spreads can be considered either widths, e.g. of histogram bins,
             or as error (uncertainties) in the values, depending on context.
  */
  class ValueSet {
    public:
      /** \brief Create an empty value set.
      */
      ValueSet();

      /** \brief Create an value set with the given data. The number of values must be >= the number of spreads.
                 Internally, the spreads will be padded with 0. if there are fewer spreads than values.
          \param values The data values.
          \param spreads The spreads in the data.
      */
      ValueSet(const std::vector<double> & values, const std::vector<double> & spreads);
  
      /// \brief Get the values.
      const std::vector<double> & getValues() const;

      /// \brief Get the spreads.
      const std::vector<double> & getSpreads() const;
  
      /// \brief Return the size of the values and spreads.
      std::vector<double>::size_type size() const;

      /// \brief Returns whether this value set is empty.
      bool empty() const;

    private:
      std::vector<double> m_values;
      std::vector<double> m_spreads;
  };

}

#endif
