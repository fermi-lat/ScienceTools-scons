/** \file FitsPrimProps.cxx

    \brief Templated utility class holding type-specific information to simplify calling cfitsio functions
    from templated functions. This class is not part of the API and should not be of interest to clients.

    \author James Peachey, HEASARC
*/

#include <string>

#include "fitsio.h"

#include "FitsPrimProps.h"

namespace table {

  // Currently these codes are not defined for complex and double complex cfitsio types.
  template <> int FitsPrimProps<std::string>::dataTypeCode() { return TSTRING; }
  template <> int FitsPrimProps<bool>::dataTypeCode() { return TLOGICAL; }
  template <> int FitsPrimProps<double>::dataTypeCode() { return TDOUBLE; }
  template <> int FitsPrimProps<float>::dataTypeCode() { return TFLOAT; }
  template <> int FitsPrimProps<char>::dataTypeCode() { return TBYTE; }
  template <> int FitsPrimProps<signed char>::dataTypeCode() { return TBYTE; }
  template <> int FitsPrimProps<short>::dataTypeCode() { return TSHORT; }
  template <> int FitsPrimProps<int>::dataTypeCode() { return TINT; }
  template <> int FitsPrimProps<long>::dataTypeCode() { return TLONG; }
  template <> int FitsPrimProps<unsigned char>::dataTypeCode() { return TBYTE; }
  template <> int FitsPrimProps<unsigned short>::dataTypeCode() { return TUSHORT; }
  template <> int FitsPrimProps<unsigned int>::dataTypeCode() { return TUINT; }
  template <> int FitsPrimProps<unsigned long>::dataTypeCode() { return TULONG; }

}
