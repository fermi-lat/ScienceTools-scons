/** \file TipBinner.h
    \brief Base class for different binners which operate on Tip objects.
*/

#include "tip/Table.h"

/** \class TipBinner
    \brief Base class for different binners which operate on Tip objects.
*/
class TipBinner {
  public:
    virtual ~TipBinner();

    /** \brief Fill histogram with values from a range of iterators.
        \param begin The beginning table iterator to fill.
        \param end The ending iterator (one past the last) to fill.
    */
    void fillBins(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

    /** \brief Write the current histogram to the output table, starting at the
        given position.
        \param begin The first iterator position.
    */
    virtual void writeHistogram(tip::Table * table) const = 0;

    /** \brief Bin one table record into the histogram.
        \param record The record.
    */
    virtual void binRecord(const tip::Table::ConstRecord & record) = 0;
};
