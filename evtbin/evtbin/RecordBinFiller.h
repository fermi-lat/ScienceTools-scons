/** \file RecordBinFiller.h
    \brief Functor which bins data from tip table records into Hist objects.
*/
#ifndef evtbin_RecordBinFiller_h
#define evtbin_RecordBinFiller_h

#include <vector>

#include "evtbin/Binner.h"
#include "evtbin/Hist.h"

#include "tip/Table.h"

namespace evtbin {
  /** \class RecordBinFiller
      \brief Functor which bins data from tip table records into Hist objects.
  */
  class RecordBinFiller {
    public:
      /** \brief Construct a RecordBinFiller which refers to the given Hist object.
          \param hist The Hist object.
      */
      RecordBinFiller(Hist & hist);

      /** \brief Perform the binning.
          \param record The tip table record containing the values to be binned.
      */
      void operator () (const tip::Table::ConstRecord & record) {
        // Iterate over the binners and populate the value:
        for (std::vector<const Binner *>::size_type ii = 0; ii < m_binners.size(); ++ii) {
          // Look up each binner's name in the table's record, and put it in the local value container:
          m_value[ii] = record[m_binners[ii]->getName()].get();
        }

        // Pass the constructed value to the histogram using the generic fillBin from the histogram base class:
        m_hist.fillBin(m_value);
      }

    private:
      Hist & m_hist;
      const Hist::BinnerCont_t & m_binners;
      std::vector<double> m_value;
  };

}

#endif
