/** \file TipEqualLinearBinner.h
    \brief Binner which uses equal linear intervals for its bin definitions.
*/
#ifndef evtbin_TipEqualLinearBinner_h
#define evtbin_TipEqualLinearBinner_h

#include <string>
#include <vector>

#include "evtbin/TipBinner.h"

/** \class TipEqualLinearBinner
    \brief Binner which uses equal linear intervals for its bin definitions.
*/
class TipEqualLinearBinner : public TipBinner {
  public:
    /** \brief Constructor
        \param input_field The name of the field in the input table.
        \param output_field The name of the field to be written to the output table.
        \param interval_begin The left-hand boundary of the interval.
        \param interval_end The right-hand boundary of the interval.
        \param interval_width The width of each interval (bin).
    */
    TipEqualLinearBinner(const std::string & input_field, const std::string & output_field,
      double interval_begin, double interval_end, double interval_width);

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
    std::string m_input_field;
    std::string m_output_field;
    double m_interval_begin;
    double m_interval_end;
    double m_interval_width;
};

#endif
