/** \file FourierAnalysis.h
    \brief Declaration of FourierAnalysis class.
    \authors Masaharu Hirayama, GSSC
             James Peachey, HEASARC/GSSC
*/
#ifndef periodSearch_FourierAnalysis_h
#define periodSearch_FourierAnalysis_h

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include "periodSearch/PeriodSearch.h"

/** \class FourierAnalysis
*/
class FourierAnalysis : public periodSearch::PeriodSearch {
  public:
    /** \brief Construct a FourierAnalysis.
        \param t_start Time lower boundary.
        \param t_stop Time upper boundary.
        \param width Width of one time bin in one data subset to be transformed.
        \param num_bins The number of bins used in each FFT. Depending on the specific test, this
                        may be an upper limit on the actual number of bins used.
        \param num_events Hint giving the anticipated number of events to be filled.
    */
    FourierAnalysis(double t_start, double t_stop, double width, size_type num_bins, int num_events = 0);

    /** \brief Fill given time into the internal storage of this object.
        \param evt_time The time of the event.
    */
    virtual void fill(double evt_time);

    /** \brief Compute Fourier powers at trial frequencies with Discrete Fast Fourier Transform algorighm.
    */
    virtual const std::vector<double> & computeStats();

    /** \brief Return the number of independent trials for this search method.
    */
    virtual size_type numIndepTrials(double min_freq = -1., double max_freq = -1.) const;

    /** \brief Compute the chance probability for the given parameters. Return pair with lower, upper limit.
        \param stat The value of the statistic.
    */
    virtual std::pair<double, double> chanceProbOneTrial(double stat) const; 

    /** \brief Return a description of this search.
    */
    virtual std::string getDescription() const;

    /** \brief Get a reference to an internal statistic viewer for an object of this class.
        \param min_freq The minimum frequency to view.
        \param max_freq The maximum frequency to view.
    */
    virtual StatisticViewer & getViewer(double min_freq = -1., double max_freq = -1.);

  private:
    typedef std::multimap<size_type, size_type> index_map_type;
    index_map_type m_index;
    double m_t_start;
    double m_t_stop;
    double m_width;
    double m_fourier_res;
    size_type m_num_segments;
    size_type m_num_bins;
};

#endif
