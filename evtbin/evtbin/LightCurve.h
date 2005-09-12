/** \file LightCurve.h
    \brief Encapsulation of a Light curve, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_LightCurve_h
#define evtbin_LightCurve_h

#include <string>

#include "evtbin/DataProduct.h"
#include "evtbin/Hist1D.h"

namespace evtbin {

  class Binner;

  /** \class LightCurve
      \brief Encapsulation of a Light curve, with methods to read/write using tip.
  */
  class LightCurve : public DataProduct {
    public:
      /** \brief Create the light curve object.
          \param binner The binner used to create the histogram.
      */
      LightCurve(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
        const std::string & sc_table, const Binner & binner, const Gti & gti);

      virtual ~LightCurve() throw();

      /** \brief Write standard OGIP light curve file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

    private:
      Hist1D m_hist;
  };

}

#endif
