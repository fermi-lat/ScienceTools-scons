/** \file CountMap.h
    \brief Encapsulation of a count map, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_CountMap_h
#define evtbin_CountMap_h

#include "evtbin/DataProduct.h"
#include "evtbin/Hist2D.h"

namespace evtbin {

  class Binner;

  /** \class CountMap
      \brief Encapsulation of a count map, with methods to read/write using tip.
  */
  class CountMap : public DataProduct {
    public:
      /** \brief Create the count map object.
      */
      CountMap(double ref_ra, double ref_dec, const std::string & proj, unsigned long num_x_pix, unsigned long num_y_pix,
        double pix_scale, double axis_rot, bool use_lb, const std::string & ra_field, const std::string & dec_field);

      virtual ~CountMap() throw();

      /** \brief Bin input from tip table.
          \param begin Table iterator pointing to the first record to be binned.
          \param end Table iterator pointing to one past the last record to be binned.
      */
      virtual void binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

      /** \brief Write count map file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

    private:
      Hist2D m_hist;
  };

}

#endif
