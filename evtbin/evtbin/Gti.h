/** \file Gti.h
    \brief Encapsulation of the concept of a GTI. May be constructed from a GTI extension.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef evtbin_Gti_h
#define evtbin_Gti_h

#include <string>
#include <utility>
#include <vector>

namespace evtbin {

  /** \class Gti
      \brief Encapsulation of the concept of a GTI. May be constructed from a GTI extension.
  */
  class Gti {
    public:
      typedef std::pair<double, double> Interval_t;
      typedef std::vector<Interval_t> IntervalCont_t;
      typedef IntervalCont_t::iterator Iterator;
      typedef IntervalCont_t::const_iterator ConstIterator;

      /// \brief Construct a GTI object which contains no intervals.
      Gti();

      /** \brief Construct a GTI object, reading its intervals from the given file.
          \param file_name The input file.
          \param ext_name The extension in the file containing the GTI information.
      */
      Gti(const std::string & file_name, const std::string & ext_name = std::string("GTI"));

      /** \brief Compute fraction of time interval bounded by tstart and tstop which falls
          within one or more of the Good time intervals.
          \param tstart Interval start time.
          \param tstop Interval start time.
          \param gti_pos Iterator pointing to the first good time interval which might contain
          part of the given interval (a hint on which GTI to use for increased efficiency).
      */
      double getFraction(double tstart, double tstop, ConstIterator & gti_pos) const;

      /** \brief From this Gti and a second, produce a third Gti which is the intersection of
          the two inputs.
      */
      Gti operator &(const Gti & gti) const;

      /** \brief Compare two sets of time intervals. If they differ in any way, this returns true.
          \param gti The Gti object with which this one will be compared.
      */
      bool operator !=(const Gti & gti) const;

      /// Return iterator pointing to first time interval in GTI.
      Iterator begin();

      /// Return iterator pointing to one past last time interval in GTI.
      Iterator end();

      /// Return const iterator pointing to first time interval in GTI.
      ConstIterator begin() const;

      /// Return const iterator pointing to one past last time interval in GTI.
      ConstIterator end() const;

      /** \brief Add an interval to this GTI object.
          \param tstart The start time of the interval.
          \param tstop The start time of the interval.
      */
      void insertInterval(double tstart, double tstop);

      /// \brief Return the number of intervals in this GTI container.
      int getNumIntervals() const;

      /** \brief Set the number of intervals in this GTI container.
          \param num_intv The new size of the GTI container.
      */
      void setNumIntervals(int num_intv);

      /// \brief Compute and return the ontime (total of all intervals).
      double computeOntime() const;

    private:
      IntervalCont_t m_intervals;
  };

}

#endif
