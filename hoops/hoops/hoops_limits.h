/******************************************************************************
 *   File name: hoops_limits.h                                                *
 *                                                                            *
 * Description: Encapsulation of machine-dependent numeric limits,            *
 *     needed because of variations in the implementations of <limits>.       *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOP_LIMITS_H
#define HOOP_LIMITS_H

////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  // Enumerated codes to describe all primitive types. These are combined
  // using binary logic to facilitate more compact logic during conversions.
  enum PrimTypeCode_e {
    BOOL_TYPE       = 1 << 0,
    CHAR_TYPE       = 1 << 1,
    SHORT_TYPE      = 1 << 2,
    INT_TYPE        = 1 << 3,
    LONG_TYPE       = 1 << 4,
    SIGNED_TYPE     = 1 << 5,
    UNSIGNED_TYPE   = 1 << 6,
    FP_TYPE         = 1 << 7,
    EXTENDED_TYPE   = 1 << 8,
    POINTER_TYPE    = 1 << 9,
    P_BOOL          = BOOL_TYPE | UNSIGNED_TYPE,
    P_CHAR          = CHAR_TYPE,
    P_SCHAR         = CHAR_TYPE | SIGNED_TYPE,
    P_SHORT         = SHORT_TYPE | SIGNED_TYPE,
    P_INT           = INT_TYPE | SIGNED_TYPE,
    P_LONG          = LONG_TYPE | SIGNED_TYPE,
    P_UCHAR         = CHAR_TYPE | UNSIGNED_TYPE,
    P_USHORT        = SHORT_TYPE | UNSIGNED_TYPE,
    P_UINT          = INT_TYPE | UNSIGNED_TYPE,
    P_ULONG         = LONG_TYPE | UNSIGNED_TYPE,
    P_WCHAR         = CHAR_TYPE | EXTENDED_TYPE,
    P_FLOAT         = FP_TYPE | SHORT_TYPE,
    P_DOUBLE        = FP_TYPE | LONG_TYPE,
    P_LONGDOUBLE    = FP_TYPE | EXTENDED_TYPE,
    P_BOOL_P        = BOOL_TYPE | POINTER_TYPE,
    P_CHAR_P        = CHAR_TYPE | POINTER_TYPE,
    P_SCHAR_P       = CHAR_TYPE | SIGNED_TYPE | POINTER_TYPE,
    P_SHORT_P       = SHORT_TYPE | SIGNED_TYPE | POINTER_TYPE,
    P_INT_P         = INT_TYPE | SIGNED_TYPE | POINTER_TYPE,
    P_LONG_P        = LONG_TYPE | SIGNED_TYPE | POINTER_TYPE,
    P_UCHAR_P       = CHAR_TYPE | UNSIGNED_TYPE | POINTER_TYPE,
    P_USHORT_P      = SHORT_TYPE | UNSIGNED_TYPE | POINTER_TYPE,
    P_UINT_P        = INT_TYPE | UNSIGNED_TYPE | POINTER_TYPE,
    P_ULONG_P       = LONG_TYPE | UNSIGNED_TYPE | POINTER_TYPE,
    P_WCHAR_P       = CHAR_TYPE | EXTENDED_TYPE | POINTER_TYPE,
    P_FLOAT_P       = FP_TYPE | SHORT_TYPE | POINTER_TYPE,
    P_DOUBLE_P      = FP_TYPE | LONG_TYPE | POINTER_TYPE,
    P_LONGDOUBLE_P  = FP_TYPE | EXTENDED_TYPE | POINTER_TYPE,
    P_VOID_P        = POINTER_TYPE,
    P_UNKNOWN       = 0,
    P_LONGLONG      = SIGNED_TYPE | EXTENDED_TYPE,
    P_LONGLONG_P    = SIGNED_TYPE | EXTENDED_TYPE | POINTER_TYPE
  };
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  // Template class Lim, which contains the necessary information to
  // assess compatibility of different numeric types. Note that T
  // must be a fundamental primitive type: bool, char, etc.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class Lim {
    public:
      inline static bool is_smaller_than(PrimTypeCode_e typecode) throw () {
        switch (typecode) {
          case P_BOOL: return sizeof(T) < sizeof(bool); break;
          case P_CHAR: return sizeof(T) < sizeof(char); break;
          case P_SCHAR: return sizeof(T) < sizeof(signed char); break;
          case P_SHORT: return sizeof(T) < sizeof(short); break;
          case P_INT: return sizeof(T) < sizeof(int); break;
          case P_LONG: return sizeof(T) < sizeof(long); break;
          case P_UCHAR: return sizeof(T) < sizeof(unsigned char); break;
          case P_USHORT: return sizeof(T) < sizeof(unsigned short); break;
          case P_UINT: return sizeof(T) < sizeof(unsigned int); break;
          case P_ULONG: return sizeof(T) < sizeof(unsigned long); break;
          case P_FLOAT: return sizeof(T) < sizeof(float); break;
          case P_DOUBLE: return sizeof(T) < sizeof(double); break;
          case P_LONGDOUBLE: return sizeof(T) < sizeof(long double); break;
          default: break;
        }
        return false;
      }

      // Note that maybe_smaller_than is defined for all types,
      // but it is only valid when applied to integral types.
      static bool maybe_smaller_than(PrimTypeCode_e typecode) throw ()
        { return false; }
      static const PrimTypeCode_e code;

      // Redefinitions based on the ISO standard numeric_limits class
      // This wrapping is done because of variations in the implementations
      // of numeric_limits, as well as differences in template handling
      // on different systems. In addition, min for floating point types
      // is -max, unlike the numeric_limits meaning of min which is the
      // absolute value of the smallest possible floating value.
      static const int digits10;
      static T const epsilon;
      static const bool is_integer;
      static const bool is_signed;
      static const T round_error;
      static const T max;
      static const T min;
  };

  // Template class Lim specializations.
  //////////////////////////////////////////////////////////////////////////////

  // Type bool is considered smaller than all other types, regardless
  // of how the actual sizes compare.
  template <>
  inline bool Lim<bool>::is_smaller_than(PrimTypeCode_e typecode) throw ()
    { return P_BOOL != typecode; }

  // Type bool is considered smaller than all other types, regardless
  // of how the actual sizes compare.
  template <>
  inline bool Lim<bool>::maybe_smaller_than(PrimTypeCode_e typecode) throw ()
    { return P_BOOL != typecode; }

  template <>
  inline bool Lim<char>::maybe_smaller_than(PrimTypeCode_e typecode) throw ()
    { return !((BOOL_TYPE | CHAR_TYPE) & typecode); }

  template <>
  inline bool Lim<signed char>::maybe_smaller_than(PrimTypeCode_e typecode)
      throw ()
    { return !((BOOL_TYPE | CHAR_TYPE) & typecode); }

  template <>
  inline bool Lim<short>::maybe_smaller_than(PrimTypeCode_e typecode) throw ()
    { return (INT_TYPE | LONG_TYPE) & typecode; }

  template <>
  inline bool Lim<int>::maybe_smaller_than(PrimTypeCode_e typecode) throw ()
    { return LONG_TYPE & typecode; }

  template <>
  inline bool Lim<unsigned char>::maybe_smaller_than(PrimTypeCode_e typecode)
      throw ()
    { return !((BOOL_TYPE | CHAR_TYPE) & typecode); }

  template <>
  inline bool Lim<unsigned short>::maybe_smaller_than(PrimTypeCode_e typecode)
      throw ()
    { return (INT_TYPE | LONG_TYPE) & typecode; }

  template <>
  inline bool Lim<unsigned int>::maybe_smaller_than(PrimTypeCode_e typecode)
      throw ()
    { return LONG_TYPE & typecode; }

#ifdef FOO
  // Lim<T>::code static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const PrimTypeCode_e Lim<bool>::code;
  template <>
  const PrimTypeCode_e Lim<char>::code;
  template <>
  const PrimTypeCode_e Lim<signed char>::code;
  template <>
  const PrimTypeCode_e Lim<short>::code;
  template <>
  const PrimTypeCode_e Lim<int>::code;
  template <>
  const PrimTypeCode_e Lim<long>::code;
  template <>
  const PrimTypeCode_e Lim<unsigned char>::code;
  template <>
  const PrimTypeCode_e Lim<unsigned short>::code;
  template <>
  const PrimTypeCode_e Lim<unsigned int>::code;
  template <>
  const PrimTypeCode_e Lim<unsigned long>::code;
  template <>
  const PrimTypeCode_e Lim<float>::code;
  template <>
  const PrimTypeCode_e Lim<double>::code;
  template <>
  const PrimTypeCode_e Lim<long double>::code;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::digits10 static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const int Lim<bool>::digits10;
  template <>
  const int Lim<char>::digits10;
  template <>
  const int Lim<signed char>::digits10;
  template <>
  const int Lim<short>::digits10;
  template <>
  const int Lim<int>::digits10;
  template <>
  const int Lim<long>::digits10;
  template <>
  const int Lim<unsigned char>::digits10;
  template <>
  const int Lim<unsigned short>::digits10;
  template <>
  const int Lim<unsigned int>::digits10;
  template <>
  const int Lim<unsigned long>::digits10;
  template <>
  const int Lim<float>::digits10;
  template <>
  const int Lim<double>::digits10;
  template <>
  const int Lim<long double>::digits10;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::epsilon static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::epsilon;
  template <>
  const char Lim<char>::epsilon;
  template <>
  const signed char Lim<signed char>::epsilon;
  template <>
  const short Lim<short>::epsilon;
  template <>
  const int Lim<int>::epsilon;
  template <>
  const long Lim<long>::epsilon;
  template <>
  const unsigned char Lim<unsigned char>::epsilon;
  template <>
  const unsigned short Lim<unsigned short>::epsilon;
  template <>
  const unsigned int Lim<unsigned int>::epsilon;
  template <>
  const unsigned long Lim<unsigned long>::epsilon;
  template <>
  const float Lim<float>::epsilon;
  template <>
  const double Lim<double>::epsilon;
  template <>
  const long double Lim<long double>::epsilon;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::is_integer static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::is_integer;
  template <>
  const bool Lim<char>::is_integer;
  template <>
  const bool Lim<signed char>::is_integer;
  template <>
  const bool Lim<short>::is_integer;
  template <>
  const bool Lim<int>::is_integer;
  template <>
  const bool Lim<long>::is_integer;
  template <>
  const bool Lim<unsigned char>::is_integer;
  template <>
  const bool Lim<unsigned short>::is_integer;
  template <>
  const bool Lim<unsigned int>::is_integer;
  template <>
  const bool Lim<unsigned long>::is_integer;
  template <>
  const bool Lim<float>::is_integer;
  template <>
  const bool Lim<double>::is_integer;
  template <>
  const bool Lim<long double>::is_integer;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::is_signed static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::is_signed;
  template <>
  const bool Lim<char>::is_signed;
  template <>
  const bool Lim<signed char>::is_signed;
  template <>
  const bool Lim<short>::is_signed;
  template <>
  const bool Lim<int>::is_signed;
  template <>
  const bool Lim<long>::is_signed;
  template <>
  const bool Lim<unsigned char>::is_signed;
  template <>
  const bool Lim<unsigned short>::is_signed;
  template <>
  const bool Lim<unsigned int>::is_signed;
  template <>
  const bool Lim<unsigned long>::is_signed;
  template <>
  const bool Lim<float>::is_signed;
  template <>
  const bool Lim<double>::is_signed;
  template <>
  const bool Lim<long double>::is_signed;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::round_error static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::round_error;
  template <>
  const char Lim<char>::round_error;
  template <>
  const signed char Lim<signed char>::round_error;
  template <>
  const short Lim<short>::round_error;
  template <>
  const int Lim<int>::round_error;
  template <>
  const long Lim<long>::round_error;
  template <>
  const unsigned char Lim<unsigned char>::round_error;
  template <>
  const unsigned short Lim<unsigned short>::round_error;
  template <>
  const unsigned int Lim<unsigned int>::round_error;
  template <>
  const unsigned long Lim<unsigned long>::round_error;
  template <>
  const float Lim<float>::round_error;
  template <>
  const double Lim<double>::round_error;
  template <>
  const long double Lim<long double>::round_error;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::max static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::max;
  template <>
  const char Lim<char>::max;
  template <>
  const signed char Lim<signed char>::max;
  template <>
  const short Lim<short>::max;
  template <>
  const int Lim<int>::max;
  template <>
  const long Lim<long>::max;
  template <>
  const unsigned char Lim<unsigned char>::max;
  template <>
  const unsigned short Lim<unsigned short>::max;
  template <>
  const unsigned int Lim<unsigned int>::max;
  template <>
  const unsigned long Lim<unsigned long>::max;
  template <>
  const float Lim<float>::max;
  template <>
  const double Lim<double>::max;
  template <>
  const long double Lim<long double>::max;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::min static declarations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::min;
  template <>
  const char Lim<char>::min;
  template <>
  const signed char Lim<signed char>::min;
  template <>
  const short Lim<short>::min;
  template <>
  const int Lim<int>::min;
  template <>
  const long Lim<long>::min;
  template <>
  const unsigned char Lim<unsigned char>::min;
  template <>
  const unsigned short Lim<unsigned short>::min;
  template <>
  const unsigned int Lim<unsigned int>::min;
  template <>
  const unsigned long Lim<unsigned long>::min;
  // Note: for all floating points, min is -max, _not_ min in the
  // sense of numeric_limits::min().
  template <>
  const float Lim<float>::min;
  template <>
  const double Lim<double>::min;
  template <>
  const long double Lim<long double>::min;
  //////////////////////////////////////////////////////////////////////////////

  // Support of all the above for long long.
  //////////////////////////////////////////////////////////////////////////////
#ifdef HAVE_LONGLONG
  // Specializations of all static members of Lim for type long long.
  template <>
  const PrimTypeCode_e Lim<long long>::code;

  template <>
  const long long Lim<long long>::epsilon;

  template <>
  const bool Lim<long long>::is_signed;

  template <>
  const bool Lim<long long>::is_integer;

  template <>
  const long long Lim<long long>::round_error;

  template <>
  const long long Lim<long long>::max;

  template <>
  const long long Lim<long long>::min;
#endif
#endif
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable forward declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

}
#endif

/******************************************************************************
 * $Log$
 * Revision 1.4  2003/12/02 14:39:43  peachey
 * To support compilers which do not have limits, such as g++ 2.95.x,
 * add round_error field to Lim class. This allows the test code to
 * be compiled without the limits header.
 *
 * Revision 1.3  2003/11/26 18:50:03  peachey
 * Merging changes made to SLAC repository into Goddard repository.
 *
 * Revision 1.2  2003/06/18 18:16:52  peachey
 * Original purpose of this file is now met by hoops_numeric_limits.h.
 * This file now contains only items related to Lim class.
 *
 * Revision 1.1.1.1  2003/11/04 01:48:30  jchiang
 * First import
 *
 * Revision 1.1  2003/05/15 03:55:47  peachey
 * Replacement for <limits> for systems/compilers which don't yet have <limits>.
 *
 ******************************************************************************/
