/** \file TipEqualLogBinner.h
    \brief Binner which uses equal logarithmic intervals for its bin definitions.
*/
#ifndef evtbin_TipEqualLogBinner_h
#define evtbin_TipEqualLogBinner_h

#include <string>
#include <vector>

#include "evtbin/TipBinner.h"

/** \class TipEqualLogBinner
    \brief Binner which uses equal logarithmic intervals for its bin definitions.
*/
class TipEqualLogBinner : public TipBinner {
  public:
    /** \brief Constructor
        \param input_field The name of the field in the input table which will be binned.
        \param output_index The name of the field in the output table which holds the index of the bins.
        \param output_field The name of the field to be written to the output table.
        \param interval_begin The left-hand boundary of the interval.
        \param interval_end The right-hand boundary of the interval.
        \param num_bins The number of bins.
    */
    TipEqualLogBinner(const std::string & input_field, const std::string & output_field,
      double interval_begin, double interval_end, unsigned long num_bins);

    /** \brief Write the current histogram to the output table, starting at the
        given position.
        \param begin The first iterator position to begin writing.
    */
    virtual void writeHistogram(tip::Table * table) const;

    /** \brief Bin one table record into the histogram.
        \param record The record.
    */
    virtual void binRecord(const tip::Table::ConstRecord & record);

    /** \brief Determine which bin this record belongs to in the histogram.
        \param record The record.
    */
    virtual long computeIndex(const tip::Table::ConstRecord & record) const;

    /** \brief Return the number of bins currently defined.
    */
    virtual long getNumBins() const;

    /** \brief Return the name of the input field.
    */
    virtual const std::string & getInputField() const;

    /** \brief Return the name of the output field.
    */
    virtual const std::string & getOutputField() const;

  private:
    std::vector<double> m_histogram;
    std::vector<double> m_bins;
    std::string m_input_field;
    std::string m_output_index;
    std::string m_output_field;
    double m_interval_begin;
    double m_interval_end;
    double m_log_width;
    unsigned long m_num_bins;
};

#endif
