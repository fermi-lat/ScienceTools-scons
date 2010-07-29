/** \file Binner.h
    \brief Base class for all binners.
	$Header$
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
          Interval(): m_begin(0.), m_end(0.) {}

          Interval(double begin, double end): m_begin(begin), m_end(end) {}

          /** \brief Compute and return the midpoint of the interval.
          */
          double midpoint() const { return (m_begin + m_end) / 2.; }

          double width() const { return (m_end - m_begin); }

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

      /** \brief Create copy of this object.
      */
      virtual Binner * clone() const = 0;

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

  inline bool operator < (const double & value, const Binner::Interval & interval) { return value < interval.begin(); }

  inline bool operator < (const Binner::Interval & interval, const double & value) { return interval.end() < value; }

  // this case is not well defined, but seems to be needed: arbitrary compare centers
  // however, only vc90 complains
#ifdef WIN32  
  inline bool operator < (const Binner::Interval & left_interval,const  Binner::Interval & right_interval) {
	  return left_interval.midpoint() < right_interval.midpoint(); }
#endif
}

#endif
