/** \file Sequence.h
    \brief Implemention of ISequence and its templated subclasses for handling various types of sequences
           of values.
*/
#ifndef st_graph_Sequence_h
#define st_graph_Sequence_h

#include <cctype>
#include <iterator>
#include <string>
#include <vector>

namespace st_graph {

  /** \class ISequence
      \brief Abstract interface representing the idea of a sequence of values with spreads (error bars, bin widths etc.),
             with methods which access the sequence properties, i.e. values, lower/upper bounds, etc. The specific
             manner in which the sequence elements are converted to the sequence properties is determined in subclasses.
  */
  class ISequence {
    public:
      typedef unsigned long size_type;
      /** \brief Construct an ISequence with the given number of points.
          \param num_points The number of points in the sequence.
      */
      ISequence(size_type num_points): m_num_points(num_points) {}

      virtual ~ISequence() {}

      /** \brief Fill the output container with the values of the sequence.
          \param val The output container.
      */
      virtual void getValues(std::vector<double> & val) const = 0;

      /** \brief Fill the output containers with the upper and lower bounds of each element in the sequence.
          \param lower The lower bounds of the sequence elements.
          \param upper The upper bounds of the sequence elements.
      */
      virtual void getIntervals(std::vector<double> & lower, std::vector<double> & upper) const = 0;

      /** \brief Fill the output containers with the upper and lower spreads of each element in the sequence.
          \param lower The lower spreads of the sequence elements.
          \param upper The upper spreads of the sequence elements.
      */
      virtual void getSpreads(std::vector<double> & lower, std::vector<double> & upper) const = 0;

      /** \brief Return the number of elements in the sequence.
      */
      size_type size() const { return m_num_points; }

    private:
      size_type m_num_points;
  };

  /** \class ScalarSequence
      \brief An ISequence in which the individual sequence elements are given by a range of single iterators.
  */
  template <typename Itor_t>
  class ScalarSequence : public ISequence {
    public:
      /** \brief Create a ScalarSequence spanning the given range.
          \param begin Iterator pointing to the first element in the sequence.
          \param end Iterator pointing to one position past the last element in the sequence.
      */
      ScalarSequence(const Itor_t & begin, const Itor_t & end): ISequence(std::distance(begin, end)), m_begin(begin), m_end(end) {}

      /** \brief Compute the value at a given position in the input sequence.
          \param itor Position at which to compute the value.
      */
      virtual double value(const Itor_t & itor) const = 0;

      /** \brief Compute the lower bound at a given position in the input sequence.
          \param itor Position at which to compute the lower bound.
      */
      virtual double lowerBound(const Itor_t & itor) const = 0;

      /** \brief Compute the upper bound at a given position in the input sequence.
          \param itor Position at which to compute the upper bound.
      */
      virtual double upperBound(const Itor_t & itor) const = 0;

      /** \brief Compute the lower spread at a given position in the input sequence.
          \param itor Position at which to compute the lower spread.
      */
      virtual double lowerSpread(const Itor_t & itor) const = 0;

      /** \brief Compute the upper spread at a given position in the input sequence.
          \param itor Position at which to compute the upper spread.
      */
      virtual double upperSpread(const Itor_t & itor) const = 0;

      /** \brief Compute the total spread (sum of upper and lower spreads) at a given position in the input sequence.
          \param itor Position at which to compute the total spread.
      */
      virtual double width(const Itor_t & itor) const = 0;

      /** \brief Fill the output container with the values of the sequence.
          \param val The output container.
      */
      virtual void getValues(std::vector<double> & val) const;

      /** \brief Fill the output containers with the upper and lower bounds of each element in the sequence.
          \param lower The lower bounds of the sequence elements.
          \param upper The upper bounds of the sequence elements.
      */
      virtual void getIntervals(std::vector<double> & lower, std::vector<double> & upper) const;

      /** \brief Fill the output containers with the upper and lower spreads of each element in the sequence.
          \param lower The lower spreads of the sequence elements.
          \param upper The upper spreads of the sequence elements.
      */
      virtual void getSpreads(std::vector<double> & lower, std::vector<double> & upper) const;

    protected:
      virtual double nextElement(const Itor_t & itor) const;
      virtual double prevElement(const Itor_t & itor) const;

      const Itor_t m_begin;
      const Itor_t m_end;
      size_type m_size;
  };

  template <typename Itor_t>
  inline void ScalarSequence<Itor_t>::getValues(std::vector<double> & val) const {
    using namespace std;
    val.resize(size());
    vector<double>::iterator val_itor = val.begin();
    for (Itor_t in_itor = m_begin; in_itor != m_end; ++in_itor, ++val_itor) {
      *val_itor = value(in_itor);
    }
  }

  template <typename Itor_t>
  inline void ScalarSequence<Itor_t>::getIntervals(std::vector<double> & lower, std::vector<double> & upper) const {
    using namespace std;
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    vector<double>::iterator low_itor = lower.begin();
    vector<double>::iterator high_itor = upper.begin();
    for (Itor_t in_itor = m_begin; in_itor != m_end; ++in_itor, ++low_itor, ++high_itor) {
      *low_itor = lowerBound(in_itor);
      *high_itor = upperBound(in_itor);
    }
  }

  template <typename Itor_t>
  inline void ScalarSequence<Itor_t>::getSpreads(std::vector<double> & lower, std::vector<double> & upper) const {
    using namespace std;
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    vector<double>::iterator low_itor = lower.begin();
    vector<double>::iterator high_itor = upper.begin();
    for (Itor_t in_itor = m_begin; in_itor != m_end; ++in_itor, ++low_itor, ++high_itor) {
      *low_itor = lowerSpread(in_itor);
      *high_itor = upperSpread(in_itor);
    }
  }

  template <typename Itor_t>
  inline double ScalarSequence<Itor_t>::nextElement(const Itor_t & itor) const {
    Itor_t next(itor + 1);
    if (m_end != next) return *next;
    else if (m_begin != itor) {
      // Extrapolate to next element using distance to the previous element.
      Itor_t prev(itor - 1);
      return *itor + (*itor - *prev);
    }
    return *itor;
  }

  template <typename Itor_t>
  inline double ScalarSequence<Itor_t>::prevElement(const Itor_t & itor) const {
    if (m_begin != itor) return *(itor - 1);
    else if (m_end != itor) {
      // Extrapolate to previous element using distance to the next element.
      Itor_t next(itor + 1);
      if (m_end != next) return *itor + *itor - *next; // *itor - (*next - *itor)
    }
    return *itor;
  }

  /** \class PointSequence
      \brief A ScalarSequence in which the iterators are assumed to represent perfectly sharp points,
             with no spreads.
  */
  template <typename Itor_t>
  class PointSequence : public ScalarSequence<Itor_t> {
    public:
      /** \brief Create a PointSequence spanning the given range of iterators. The iterators
                 represent points which are perfectly defined, with no spreads. The upper and
                 lower bounds are the same as the values, and all apreads are assumed to be 0.
          \param begin Iterator pointing to the first element in the sequence.
          \param end Iterator pointing to one position past the last element in the sequence.
      */
      PointSequence(const Itor_t & begin, const Itor_t & end): ScalarSequence<Itor_t>(begin, end) {}

      virtual double value(const Itor_t & itor) const { return *itor; }

      virtual double lowerBound(const Itor_t & itor) const { return *itor; }

      virtual double upperBound(const Itor_t & itor) const { return *itor; }

      virtual double lowerSpread(const Itor_t &) const { return 0.; }

      virtual double upperSpread(const Itor_t &) const { return 0.; }

      virtual double width(const Itor_t &) const { return 0.; }
  };

  /** \class ValueSequence
      \brief A ScalarSequence in which the iterators are assumed to represent values, with the spreads
             and bounds for each element determined from adjacent elements.
  */
  template <typename Itor_t>
  class ValueSequence : public ScalarSequence<Itor_t> {
    public:
      /** \brief Create a ValueSequence spanning the given range of iterators. The iterators
                 point to the values of the sequence. The lower/upper bound of each element is taken
                 to be the midpoint between the current value and the previous/next value. At the
                 beginning/end of the sequence, the previous/next values are extrapolated.
          \param begin Iterator pointing to the first element in the sequence.
          \param end Iterator pointing to one position past the last element in the sequence.
      */
      ValueSequence(const Itor_t & begin, const Itor_t & end): ScalarSequence<Itor_t>(begin, end) {}

      virtual double value(const Itor_t & itor) const { return *itor; }

      virtual double lowerBound(const Itor_t & itor) const { return *itor - lowerSpread(itor); }

      virtual double upperBound(const Itor_t & itor) const { return *itor + upperSpread(itor); }

      virtual double lowerSpread(const Itor_t & itor) const { return .5 * (*itor - prevElement(itor)); }

      virtual double upperSpread(const Itor_t & itor) const { return .5 * (nextElement(itor) - *itor); }

      virtual double width(const Itor_t & itor) const { return .5 * (nextElement(itor) - prevElement(itor)); }
  };

  /** \class LowerBoundsSequence
      \brief A ScalarSequence in which the iterators are assumed to represent the lower bounds of each element,
             with the spreads, values and upper bound for each element determined from adjacent elements.
  */
  template <typename Itor_t>
  class LowerBoundSequence : public ScalarSequence<Itor_t> {
    public:
      /** \brief Create a LowerBoundSequence spanning the given range of iterators. The iterators
                 point to the lower bounds of the sequence. The upper bound of each element is taken
                 to be the next lower bound in the sequence. At the end of the sequence, the next lower bound
                 is extrapolated.
          \param begin Iterator pointing to the first element in the sequence.
          \param end Iterator pointing to one position past the last element in the sequence.
      */
      LowerBoundSequence(const Itor_t & begin, const Itor_t & end): ScalarSequence<Itor_t>(begin, end) {}

      virtual double value(const Itor_t & itor) const { return .5 * (*itor + nextElement(itor)); }

      virtual double lowerBound(const Itor_t & itor) const { return *itor; }

      virtual double upperBound(const Itor_t & itor) const { return nextElement(itor); }

      virtual double lowerSpread(const Itor_t & itor) const { return .5 * width(itor); }

      virtual double upperSpread(const Itor_t & itor) const { return .5 * width(itor); }

      virtual double width(const Itor_t & itor) const { return nextElement(itor) - *itor; }
  };

  /** \class ValueSpreadSequence
      \brief An ISequence in which several distinct iterators represent the values and spreads of each element.
  */
  template <typename Itor_t>
  class ValueSpreadSequence : public ISequence {
    public:
      /** \brief Create a ValueSpreadSequence from a range of values and symmetric spreads.
                 The lower/upper bound is found by subtracting/adding the spread of each element
                 from/to the corresponding value.
          \param value_begin Iterator pointing to the first value in the sequence.
          \param value_end Iterator pointing to one position past the last value in the sequence.
          \param spread_begin Iterator pointing to the first spread.
      */
      ValueSpreadSequence(const Itor_t & value_begin, const Itor_t & value_end, const Itor_t & spread_begin):
        ISequence(std::distance(value_begin, value_end)), m_value_begin(value_begin), m_value_end(value_end),
        m_low_spread_begin(spread_begin), m_high_spread_begin(spread_begin) {}

      /** \brief Create a ValueSpreadSequence from a range of values and asymmetric spreads.
                 The lower/upper bound is found by subtracting/adding the lower/upper spread of
                 each element from/to the value.
          \param value_begin Iterator pointing to the first value in the sequence.
          \param value_end Iterator pointing to one position past the last value in the sequence.
          \param low_spread_begin Iterator pointing to the first lower spread.
          \param high_spread_begin Iterator pointing to the first upper spread.
      */
      ValueSpreadSequence(const Itor_t & value_begin, const Itor_t & value_end, const Itor_t & low_spread_begin,
        const Itor_t & high_spread_begin): ISequence(std::distance(value_begin, value_end)), m_value_begin(value_begin),
        m_value_end(value_end), m_low_spread_begin(low_spread_begin), m_high_spread_begin(high_spread_begin) {}

      /** \brief Fill the output container with the values of the sequence.
          \param val The output container.
      */
      virtual void getValues(std::vector<double> & val) const;

      /** \brief Fill the output containers with the upper and lower bounds of each element in the sequence.
          \param lower The lower bounds of the sequence elements.
          \param upper The upper bounds of the sequence elements.
      */
      virtual void getIntervals(std::vector<double> & lower, std::vector<double> & upper) const;

      /** \brief Fill the output containers with the upper and lower spreads of each element in the sequence.
          \param lower The lower spreads of the sequence elements.
          \param upper The upper spreads of the sequence elements.
      */
      virtual void getSpreads(std::vector<double> & lower, std::vector<double> & upper) const;

    private:
      Itor_t m_value_begin;
      Itor_t m_value_end;
      Itor_t m_low_spread_begin;
      Itor_t m_high_spread_begin;
  };

  template <typename Itor_t>
  void ValueSpreadSequence<Itor_t>::getValues(std::vector<double> & val) const {
    val.resize(size());
    std::vector<double>::iterator out_val = val.begin();
    for (Itor_t in_val = m_value_begin; in_val != m_value_end; ++in_val, ++out_val) {
      *out_val = *in_val;
    }
  }

  template <typename Itor_t>
  void ValueSpreadSequence<Itor_t>::getIntervals(std::vector<double> & lower, std::vector<double> & upper) const {
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    std::vector<double>::iterator out_low = lower.begin();
    std::vector<double>::iterator out_high = upper.begin();
    Itor_t in_low = m_low_spread_begin;
    Itor_t in_high = m_high_spread_begin;
    for (Itor_t in_val = m_value_begin; in_val != m_value_end; ++in_val, ++in_low, ++in_high, ++out_low, ++out_high) {
      *out_low = *in_val - *in_low;
      *out_high = *in_val + *in_high;
    }
  }

  template <typename Itor_t>
  void ValueSpreadSequence<Itor_t>::getSpreads(std::vector<double> & lower, std::vector<double> & upper) const {
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    std::vector<double>::iterator out_low = lower.begin();
    std::vector<double>::iterator out_high = upper.begin();
    Itor_t in_low = m_low_spread_begin;
    Itor_t in_high = m_high_spread_begin;
    for (Itor_t in_val = m_value_begin; in_val != m_value_end; ++in_val, ++in_low, ++in_high, ++out_low, ++out_high) {
      *out_low = *in_low;
      *out_high = *in_high;
    }
  }

  /** \class IntervalSequence
      \brief An ISequence in which two distinct iterators represent the lower and upper bounds of each element.
  */
  template <typename Itor_t>
  class IntervalSequence : public ISequence {
    public:
      /** \brief Create a IntervalSequence from a range of upper/lower bounds. Values are computed from the
                 midpoints of each interval. Spreads are computed from one half the difference
                 between upper and lower bounds.
          \param lower_begin Iterator pointing to the first lower bound in the sequence.
          \param lower_end Iterator pointing to one position past the last lower bound in the sequence.
          \param upper_begin Iterator pointing to the first upper bound in the sequence.
      */
      IntervalSequence(const Itor_t & lower_begin, const Itor_t & lower_end, const Itor_t & upper_begin):
        ISequence(std::distance(lower_begin, lower_end)), m_low_begin(lower_begin), m_low_end(lower_end),
        m_high_begin(upper_begin) {}

      /** \brief Fill the output container with the values of the sequence.
          \param val The output container.
      */
      virtual void getValues(std::vector<double> & val) const;

      /** \brief Fill the output containers with the upper and lower bounds of each element in the sequence.
          \param lower The lower bounds of the sequence elements.
          \param upper The upper bounds of the sequence elements.
      */
      virtual void getIntervals(std::vector<double> & lower, std::vector<double> & upper) const;

      /** \brief Fill the output containers with the upper and lower spreads of each element in the sequence.
          \param lower The lower spreads of the sequence elements.
          \param upper The upper spreads of the sequence elements.
      */
      virtual void getSpreads(std::vector<double> & lower, std::vector<double> & upper) const;

    private:
      Itor_t m_low_begin;
      Itor_t m_low_end;
      Itor_t m_high_begin;
  };

  template <typename Itor_t>
  void IntervalSequence<Itor_t>::getValues(std::vector<double> & val) const {
    val.resize(size());
    std::vector<double>::iterator out_val = val.begin();
    Itor_t in_high = m_high_begin;
    for (Itor_t in_low = m_low_begin; in_low != m_low_end; ++in_low, ++in_high, ++out_val) {
      *out_val = .5 * (*in_low + *in_high);
    }
  }

  template <typename Itor_t>
  void IntervalSequence<Itor_t>::getIntervals(std::vector<double> & lower, std::vector<double> & upper) const {
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    std::vector<double>::iterator out_low = lower.begin();
    std::vector<double>::iterator out_high = upper.begin();
    Itor_t in_high = m_high_begin;
    for (Itor_t in_low = m_low_begin; in_low != m_low_end; ++in_low, ++in_high, ++out_low, ++out_high) {
      *out_low = *in_low;
      *out_high = *in_high;
    }
  }

  template <typename Itor_t>
  void IntervalSequence<Itor_t>::getSpreads(std::vector<double> & lower, std::vector<double> & upper) const {
    size_type seq_size = size();
    lower.resize(seq_size);
    upper.resize(seq_size);
    std::vector<double>::iterator out_low = lower.begin();
    std::vector<double>::iterator out_high = upper.begin();
    Itor_t in_high = m_high_begin;
    for (Itor_t in_low = m_low_begin; in_low != m_low_end; ++in_low, ++in_high, ++out_low, ++out_high) {
      double spread = .5 * (*in_high - *in_low);
      *out_low = spread;
      *out_high = spread;
    }
  }

}

#endif
