/** \file SingleSpec.h
    \brief Encapsulation of a single spectrum, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_SingleSpec_h
#define evtbin_SingleSpec_h

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
      */
      SingleSpec(const Binner & binner);

      virtual ~SingleSpec() throw();

      /** \brief Write standard OGIP PHA1 file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

      /** \brief Write ebounds extension.
          \param out_file The output file name.
          \param binner The binner used to write the output.
      */
      virtual void writeEbounds(const std::string & out_file, const Binner * binner) const;

    private:
      Hist1D m_hist;
  };

}

#endif
