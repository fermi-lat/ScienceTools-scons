/** \file Binner.h
    \brief Base class for all binners.
*/
#ifndef evtbin_Binner_h
#define evtbin_Binner_h

#include <string>

namespace evtbin {
  /** \class Binner
      \brief Base class for all binners.
  */
  class Binner {
    public:
      class Interval {
        public:
          Interval(double begin, double end): m_begin(begin), m_end(end) {}

          /** \brief Compute and return the midpoint of the interval.
          */
          double getMidpoint() const { return (m_begin + m_end) / 2.; }

          double begin() const { return m_begin; }

          double end() const { return m_end; }

        private:
          double m_begin;
          double m_end;
      };

      /** \brief Construct binner.
          \param name Name of quantity being binned. May be empty.
      */
      Binner(const std::string & name): m_name(name) {}

      virtual ~Binner() throw() {}

      /** \brief Return the bin number for the given value.
          \param value The value being binned.
      */
      virtual long computeIndex(double value) const = 0;

      /** \brief Return the number of bins currently defined.
      */
      virtual long getNumBins() const = 0;

      /** \brief Return the interval spanned by the given bin.
          \param index The index indicating the bin number.
      */
      virtual Interval getInterval(long index) const = 0;

      /** \brief Compute and return the bin width of the given bin.
          \param index The index of the bin.
      */
      double getBinWidth(long index) const {
        Interval i = getInterval(index);
        return i.end() - i.begin();
      }

      /** \brief Return the name of the quantity being binned.
      */
      const std::string & getName() const { return m_name; }

    private:
      std::string m_name;
  };

}

#endif
