/** \file SingleSpec.h
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_SingleSpec_h
#define evtbin_SingleSpec_h

#include <string>

#include "evtbin/DataProduct.h"
#include "evtbin/Hist1D.h"

namespace evtbin {

  class Binner;

  /** \class SingleSpec
      \brief Encapsulation of a single spectrum, with methods to read/write using tip.
  */
  class SingleSpec : public DataProduct {
    public:
      /** \brief Create the spectrum object.
          \param binner The binner used to create the histogram.
          \param ebounds The binner which describes the energy intervals associated with the defined bins.
      */
      SingleSpec(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
        const Binner & binner, const Binner & ebounds, const Gti & gti);

      virtual ~SingleSpec() throw();

      /** \brief Write standard OGIP PHA1 file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

    private:
      Hist1D m_hist;
      Binner * m_ebounds;
  };

}

#endif
