/** \file Binner.h
    \brief Base class for all binners.
*/
#ifndef evtbin_Binner_h
#define evtbin_Binner_h

#include <string>

namespace evtbin {
  /** \class Binner
      \brief Base class for all binners.
  */
  class Binner {
    public:
      /** \brief Construct binner.
          \param name Name of quantity being binned. May be empty.
      */
      Binner(const std::string & name): m_name(name) {}

      virtual ~Binner() throw() {}

      /** \brief Return the bin number for the given value.
          \param value The value being binned.
      */
      virtual long computeIndex(double value) const = 0;

      /** \brief Return the number of bins currently defined.
      */
      virtual long getNumBins() const = 0;

      /** \brief Return the name of the quantity being binned.
      */
      const std::string & getName() const { return m_name; }

    private:
      std::string m_name;
  };

}

#endif
