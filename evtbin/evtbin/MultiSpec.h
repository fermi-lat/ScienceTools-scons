/** \file MultiSpec.h
    \brief Encapsulation of a group of spectra, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_MultiSpec_h
#define evtbin_MultiSpec_h

#include "evtbin/DataProduct.h"
#include "evtbin/Hist2D.h"

namespace evtbin {

  class Binner;

  /** \class MultiSpec
      \brief Encapsulation of a group of spectra, with methods to read/write using tip.
  */
  class MultiSpec : public DataProduct {
    public:
      /** \brief Create the spectra object, using the given bins.
          \param time_binner The binner used to bin the time dimension.
          \param energy_binner The binner used to create the energy dimension.
      */
      MultiSpec(const Binner & time_binner, const Binner & energy_binner);

      virtual ~MultiSpec() throw();

      /** \brief Write standard OGIP PHA2 file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

    private:
      Hist2D m_hist;
  };

}

#endif
