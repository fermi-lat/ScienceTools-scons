/** \file TipBinner2D.h
    \brief Binner which uses equal linear intervals for its bin definitions.
*/
#ifndef evtbin_TipBinner2D_h
#define evtbin_TipBinner2D_h

#include <string>
#include <vector>

#include "evtbin/TipBinner.h"
#include "tip/Table.h"

/** \class TipBinner2D
    \brief Binner which uses equal linear intervals for its bin definitions.
*/
class TipBinner2D {
  public:
    typedef std::vector<std::vector<double> > Hist_t;

    /** \brief Constructor
        \param input_field The name of the field in the input table.
    */
    TipBinner2D(TipBinner * binner1, TipBinner * binner2);

    /** \brief Destructor
    */
    virtual ~TipBinner2D();

    /** \brief Fill histogram with values from a range of iterators.
        \param begin The beginning table iterator to fill.
        \param end The ending iterator (one past the last) to fill.
    */
    void fillBins(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

    /** \brief Write the current histogram to the output table, starting at the
        given position.
        \param begin The first iterator position to begin writing.
    */
    virtual void writeHistogram(tip::Table * table) const;

    /** \brief Bin one table record into the histogram.
        \param record The record.
    */
    virtual void binRecord(const tip::Table::ConstRecord & record);

  private:
    Hist_t m_histogram;
    TipBinner * m_binner1;
    TipBinner * m_binner2;
};

#endif
