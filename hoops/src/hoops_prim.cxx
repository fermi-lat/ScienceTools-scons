/******************************************************************************
 *   File name:                                                               *
 *                                                                            *
 * Description:                                                               *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Header files.
////////////////////////////////////////////////////////////////////////////////
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#ifdef HAVE_LIMITS
#include <limits>
#else
#include "hoops/hoops_limits.h"
#endif
#include <string>
#include "hoops/hoops_exception.h"
#include "hoops/hoops_prim.h"
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

#ifdef HAVE_LIMITS
  using std::numeric_limits;
#endif

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
  // Static function declarations.
  //////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
  static int strcasecmp(const char *s1, const char *s2);
#endif
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions.
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
      static const T max;
      static const T min;
  };
  //////////////////////////////////////////////////////////////////////////////

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
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::code static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const PrimTypeCode_e Lim<bool>::code = P_BOOL;
  template <>
  const PrimTypeCode_e Lim<char>::code = P_CHAR;
  template <>
  const PrimTypeCode_e Lim<signed char>::code = P_SCHAR;
  template <>
  const PrimTypeCode_e Lim<short>::code = P_SHORT;
  template <>
  const PrimTypeCode_e Lim<int>::code = P_INT;
  template <>
  const PrimTypeCode_e Lim<long>::code = P_LONG;
  template <>
  const PrimTypeCode_e Lim<unsigned char>::code = P_UCHAR;
  template <>
  const PrimTypeCode_e Lim<unsigned short>::code = P_USHORT;
  template <>
  const PrimTypeCode_e Lim<unsigned int>::code = P_UINT;
  template <>
  const PrimTypeCode_e Lim<unsigned long>::code = P_ULONG;
  template <>
  const PrimTypeCode_e Lim<float>::code = P_FLOAT;
  template <>
  const PrimTypeCode_e Lim<double>::code = P_DOUBLE;
  template <>
  const PrimTypeCode_e Lim<long double>::code = P_LONGDOUBLE;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::digits10 static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const int Lim<bool>::digits10 = numeric_limits<bool>::digits10;
  template <>
  const int Lim<char>::digits10 = numeric_limits<char>::digits10;
  template <>
  const int Lim<signed char>::digits10 =
      numeric_limits<signed char>::digits10;
  template <>
  const int Lim<short>::digits10 = numeric_limits<short>::digits10;
  template <>
  const int Lim<int>::digits10 = numeric_limits<int>::digits10;
  template <>
  const int Lim<long>::digits10 = numeric_limits<long>::digits10;
  template <>
  const int Lim<unsigned char>::digits10 =
      numeric_limits<unsigned char>::digits10;
  template <>
  const int Lim<unsigned short>::digits10 =
      numeric_limits<unsigned short>::digits10;
  template <>
  const int Lim<unsigned int>::digits10 =
      numeric_limits<unsigned int>::digits10;
  template <>
  const int Lim<unsigned long>::digits10 =
      numeric_limits<unsigned long>::digits10;
  template <>
  const int Lim<float>::digits10 = numeric_limits<float>::digits10;
  template <>
  const int Lim<double>::digits10 = numeric_limits<double>::digits10;
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const int Lim<long double>::digits10 =
      numeric_limits<double>::digits10;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::epsilon static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::epsilon = numeric_limits<bool>::epsilon();
  template <>
  const char Lim<char>::epsilon = numeric_limits<char>::epsilon();
  template <>
  const signed char Lim<signed char>::epsilon =
      numeric_limits<signed char>::epsilon();
  template <>
  const short Lim<short>::epsilon = numeric_limits<short>::epsilon();
  template <>
  const int Lim<int>::epsilon = numeric_limits<int>::epsilon();
  template <>
  const long Lim<long>::epsilon = numeric_limits<long>::epsilon();
  template <>
  const unsigned char Lim<unsigned char>::epsilon =
      numeric_limits<unsigned char>::epsilon();
  template <>
  const unsigned short Lim<unsigned short>::epsilon =
      numeric_limits<unsigned short>::epsilon();
  template <>
  const unsigned int Lim<unsigned int>::epsilon =
      numeric_limits<unsigned int>::epsilon();
  template <>
  const unsigned long Lim<unsigned long>::epsilon =
      numeric_limits<unsigned long>::epsilon();
  template <>
  const float Lim<float>::epsilon = numeric_limits<float>::epsilon();
  template <>
  const double Lim<double>::epsilon = numeric_limits<double>::epsilon();
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const long double Lim<long double>::epsilon =
      numeric_limits<double>::epsilon();
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::is_integer static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::is_integer = numeric_limits<bool>::is_integer;
  template <>
  const bool Lim<char>::is_integer = numeric_limits<char>::is_integer;
  template <>
  const bool Lim<signed char>::is_integer =
      numeric_limits<signed char>::is_integer;
  template <>
  const bool Lim<short>::is_integer = numeric_limits<short>::is_integer;
  template <>
  const bool Lim<int>::is_integer = numeric_limits<int>::is_integer;
  template <>
  const bool Lim<long>::is_integer = numeric_limits<long>::is_integer;
  template <>
  const bool Lim<unsigned char>::is_integer =
      numeric_limits<unsigned char>::is_integer;
  template <>
  const bool Lim<unsigned short>::is_integer =
      numeric_limits<unsigned short>::is_integer;
  template <>
  const bool Lim<unsigned int>::is_integer =
      numeric_limits<unsigned int>::is_integer;
  template <>
  const bool Lim<unsigned long>::is_integer =
      numeric_limits<unsigned long>::is_integer;
  template <>
  const bool Lim<float>::is_integer = numeric_limits<float>::is_integer;
  template <>
  const bool Lim<double>::is_integer = numeric_limits<double>::is_integer;
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const bool Lim<long double>::is_integer =
      numeric_limits<double>::is_integer;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::is_signed static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::is_signed = numeric_limits<bool>::is_signed;
  template <>
  const bool Lim<char>::is_signed = numeric_limits<char>::is_signed;
  template <>
  const bool Lim<signed char>::is_signed =
      numeric_limits<signed char>::is_signed;
  template <>
  const bool Lim<short>::is_signed = numeric_limits<short>::is_signed;
  template <>
  const bool Lim<int>::is_signed = numeric_limits<int>::is_signed;
  template <>
  const bool Lim<long>::is_signed = numeric_limits<long>::is_signed;
  template <>
  const bool Lim<unsigned char>::is_signed =
      numeric_limits<unsigned char>::is_signed;
  template <>
  const bool Lim<unsigned short>::is_signed =
      numeric_limits<unsigned short>::is_signed;
  template <>
  const bool Lim<unsigned int>::is_signed =
      numeric_limits<unsigned int>::is_signed;
  template <>
  const bool Lim<unsigned long>::is_signed =
      numeric_limits<unsigned long>::is_signed;
  template <>
  const bool Lim<float>::is_signed = numeric_limits<float>::is_signed;
  template <>
  const bool Lim<double>::is_signed = numeric_limits<double>::is_signed;
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const bool Lim<long double>::is_signed =
      numeric_limits<double>::is_signed;
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::max static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::max = numeric_limits<bool>::max();
  template <>
  const char Lim<char>::max = numeric_limits<char>::max();
  template <>
  const signed char Lim<signed char>::max =
      numeric_limits<signed char>::max();
  template <>
  const short Lim<short>::max = numeric_limits<short>::max();
  template <>
  const int Lim<int>::max = numeric_limits<int>::max();
  template <>
  const long Lim<long>::max = numeric_limits<long>::max();
  template <>
  const unsigned char Lim<unsigned char>::max =
      numeric_limits<unsigned char>::max();
  template <>
  const unsigned short Lim<unsigned short>::max =
      numeric_limits<unsigned short>::max();
  template <>
  const unsigned int Lim<unsigned int>::max =
      numeric_limits<unsigned int>::max();
  template <>
  const unsigned long Lim<unsigned long>::max =
      numeric_limits<unsigned long>::max();
  template <>
  const float Lim<float>::max = numeric_limits<float>::max();
  template <>
  const double Lim<double>::max = numeric_limits<double>::max();
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const long double Lim<long double>::max =
      numeric_limits<double>::max();
  //////////////////////////////////////////////////////////////////////////////

  // Lim<T>::min static initializations.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  const bool Lim<bool>::min = numeric_limits<bool>::min();
  template <>
  const char Lim<char>::min = numeric_limits<char>::min();
  template <>
  const signed char Lim<signed char>::min =
      numeric_limits<signed char>::min();
  template <>
  const short Lim<short>::min = numeric_limits<short>::min();
  template <>
  const int Lim<int>::min = numeric_limits<int>::min();
  template <>
  const long Lim<long>::min = numeric_limits<long>::min();
  template <>
  const unsigned char Lim<unsigned char>::min =
      numeric_limits<unsigned char>::min();
  template <>
  const unsigned short Lim<unsigned short>::min =
      numeric_limits<unsigned short>::min();
  template <>
  const unsigned int Lim<unsigned int>::min =
      numeric_limits<unsigned int>::min();
  template <>
  const unsigned long Lim<unsigned long>::min =
      numeric_limits<unsigned long>::min();
  // Note: for all floating points, min is -max, _not_ min in the
  // sense of numeric_limits::min().
  template <>
  const float Lim<float>::min = -numeric_limits<float>::max();
  template <>
  const double Lim<double>::min = -numeric_limits<double>::max();
  // On *expletive* Solaris, numeric_limits<long double> causes the 
  // compiler to exit with assertion failures.
  template <>
  const long double Lim<long double>::min =
      -numeric_limits<double>::max();
  //////////////////////////////////////////////////////////////////////////////

  // Support of all the above for long long.
  //////////////////////////////////////////////////////////////////////////////
#ifdef HAVE_LONGLONG
  // Utility to determine maximum value for type long long.
  // On Solaris, numeric_limits does not contain meaningful
  // specializations for long long, so the maximum value is
  // computed.
  static long long LongLongMax() {
    long long max = numeric_limits<long long>::max();
    if (0 == max) {
      long long bitmax = 1;

      // Find highest positive power of 2.
      while (0 < bitmax << 1) bitmax <<= 1;

      // Fill lower bits, always testing for overflow.
      while (0 < bitmax) {
        if (0 < max | bitmax) max |= bitmax;
        bitmax >>= 1;
      }
    }
    return max;
  }

  // Utility to determine minimum value for type long long.
  // On Solaris, numeric_limits does not contain meaningful
  // specializations for long long, so the minimum value is
  // computed using LongLongMax, and then taking the 2s complement.
  static long long LongLongMin() {
    static long long min = 0;
    if (0 == min) {
      min = numeric_limits<long long>::min();
      if (0 == min) {
        // Assume 2s complement
        min = LongLongMax() + 1;
      }
    }
    return min;
  }

  // Specializations of all static members of Lim for type long long.
  template <>
  const PrimTypeCode_e Lim<long long>::code = P_LONGLONG;

  template <>
  const long long Lim<long long>::epsilon =
      numeric_limits<long long>::epsilon();

  template <>
  const bool Lim<long long>::is_signed = numeric_limits<long>::is_signed;

  template <>
  const bool Lim<long long>::is_integer = numeric_limits<long>::is_integer;

  template <>
  const long long Lim<long long>::max = LongLongMax();

  template <>
  const long long Lim<long long>::min = LongLongMin();
#endif

  // Template class Prim. Any Prim class can be constructed from
  // any primitive type, though exceptions can be thrown if the
  // types have some sort of conversion problem!
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class Prim: public IPrim {
    public:
      // Constructors.
      Prim() throw(): mData() {}
      Prim(const Prim & p) throw (): mData(p.mData) {}
      Prim(const IPrim & p) throw (Hexception): mData() { p.To(mData); }
      Prim(const bool & p) throw (Hexception): mData() { From(p); }
      Prim(const char & p) throw (Hexception): mData() { From(p); }
      Prim(const signed char & p) throw (Hexception): mData() { From(p); }
      Prim(const short & p) throw (Hexception): mData() { From(p); }
      Prim(const int & p) throw (Hexception): mData() { From(p); }
      Prim(const long & p) throw (Hexception): mData() { From(p); }
      Prim(const unsigned char & p) throw (Hexception): mData() { From(p); }
      Prim(const unsigned short & p) throw (Hexception): mData() { From(p); }
      Prim(const unsigned int & p) throw (Hexception): mData() { From(p); }
      Prim(const unsigned long & p) throw (Hexception): mData() { From(p); }
      Prim(const float & p) throw (Hexception): mData() { From(p); }
      Prim(const double & p) throw (Hexception): mData() { From(p); }
      Prim(const long double & p) throw (Hexception): mData() { From(p); }
      Prim(const char * p) throw (Hexception): mData() { From(p); }
      Prim(const std::string & p) throw (Hexception): mData() { From(p); }

      // Destructor.
      virtual ~Prim() throw() {}

      // Assignments.
      virtual Prim & operator =(const Prim & p) throw (Hexception)
        { mData = p.mData; return *this; }

      virtual Prim &operator =(const IPrim & p) throw (Hexception)
        { From(p); return *this; }

      virtual void From(const IPrim & p) throw (Hexception)
        { if (&p != this) p.To(mData); }

      virtual void From(const bool & p) throw (Hexception)
        { ConvertFrom((unsigned long) p, Lim<bool>::code); }

      virtual void From(const char & p) throw (Hexception) {
        if (Lim<char>::is_signed)
          ConvertFrom((long) p, Lim<char>::code);
        else
          ConvertFrom((unsigned long) p, Lim<char>::code);
      }

      virtual void From(const signed char & p) throw (Hexception)
        { ConvertFrom((long) p, Lim<signed char>::code); }

      virtual void From(const short & p) throw (Hexception)
        { ConvertFrom((long) p, Lim<short>::code); }

      virtual void From(const int & p) throw (Hexception)
        { ConvertFrom((long) p, Lim<int>::code); }

      virtual void From(const long & p) throw (Hexception)
        { ConvertFrom((long) p, Lim<long>::code); }

      virtual void From(const unsigned char & p) throw (Hexception)
        { ConvertFrom((unsigned long) p, Lim<unsigned char>::code); }

      virtual void From(const unsigned short & p) throw (Hexception)
        { ConvertFrom((unsigned long) p, Lim<unsigned short>::code); }

      virtual void From(const unsigned int & p) throw (Hexception)
        { ConvertFrom((unsigned long) p, Lim<unsigned int>::code); }

      virtual void From(const unsigned long & p) throw (Hexception)
        { ConvertFrom((unsigned long) p, Lim<unsigned long>::code); }

      virtual void From(const float & p) throw (Hexception)
        { ConvertFrom((long double) p, Lim<float>::code); }

      virtual void From(const double & p) throw (Hexception)
        { ConvertFrom((long double) p, Lim<double>::code); }

      virtual void From(const long double & p) throw (Hexception)
        { ConvertFrom((long double) p, Lim<long double>::code); }

      virtual void From(const char * p) throw (Hexception)
        { ConvertFrom(p); }

      virtual void From(const std::string & p) throw (Hexception)
        { ConvertFrom(p.c_str()); }

      virtual void ConvertFrom(const long &  p, PrimTypeCode_e fromcode)
        throw (Hexception) {
        HexceptionCode_e status;

        if (P_CHAR_P == Lim<T>::code) {
          status = P_ILLEGAL;
        } else if (!Lim<T>::is_integer) {
          // Converting to a floating type. Warn of possible
          // precision loss. Assume any floating type wont
          // overflow or underflow from a long.
          status = P_PRECISION;

        } else {
          // Converting from another integral type; think positively
          // although problems may be found below.
          status = P_OK;

          // Overflow is a possibility only if the sizes really
          // *are* incompatible.
          if (Lim<T>::is_smaller_than(fromcode)) {
            if (Lim<T>::is_signed) {
              // Smaller signed type.
              if ((long) Lim<T>::min > p) status = P_UNDERFLOW;
              else if ((long) Lim<T>::max < p)
                status = P_OVERFLOW;
            } else {
              // Smaller unsigned type. Lim<T>::min should always be 0
              // Check for underflow first because of the sign issue.
              if ((long) Lim<T>::min > p) status = P_UNDERFLOW;
              else if ((unsigned long) Lim<T>::max < (unsigned long) p)
                status = P_OVERFLOW;
            }

          }
          if (P_OK == status && !Lim<T>::is_signed) {
            // Converting to an unsigned type.
            status = P_SIGNEDNESS;
          }
        }

        // Check whether the size of the object being converted
        // to might be smaller than the source object on some
        // implementations.
        if ((P_OK == status || P_SIGNEDNESS == status) &&
            Lim<T>::maybe_smaller_than(fromcode))
          status = P_BADSIZE;

        // Perform the conversion if possible.
        if (P_UNDERFLOW == status) mData = Lim<T>::min;
        else if (P_OVERFLOW == status) mData = Lim<T>::max;
        else if (P_ILLEGAL != status) mData = (T) p;

        // Throw exception if everything not perfect.
        if (P_OK != status) throw Hexception(status);
      }

      virtual void ConvertFrom(const unsigned long & p, PrimTypeCode_e fromcode)
        throw (Hexception) {
        HexceptionCode_e status;

        if (P_CHAR_P == Lim<T>::code) {
          status = P_ILLEGAL;
        } else if (!Lim<T>::is_integer) {
          // Converting to a floating type. Warn of possible
          // precision loss. Assume any floating type wont
          // overflow or underflow from an unsigned long.
          status = P_PRECISION;

        } else {
          // Converting from another integral type; think positively
          // although problems may be found below.
          status = P_OK;

          // Overflow is a possibility only if the sizes really
          // *are* incompatible.
          if (Lim<T>::is_smaller_than(fromcode)) {
            if (!Lim<T>::is_signed) {
              // Smaller unsigned type.
              // The following else should never evaluate to true, but
              // the else might.
              if ((unsigned long) Lim<T>::min > p) status = P_UNDERFLOW;
              else if ((unsigned long) Lim<T>::max < p)
                status = P_OVERFLOW;
            } else {
              // Smaller signed type.
              // Check for overflow first because of the sign issue.
              if ((unsigned long) Lim<T>::max < (unsigned long) p)
                status = P_OVERFLOW;
              // The following else should never evaluate to true.
              else if ((long) Lim<T>::min > (long) p) status = P_UNDERFLOW;
            }

          }
          if (P_OK == status && Lim<T>::is_signed) {
            // Converting to a signed type.
            status = P_SIGNEDNESS;
          }
        }

        // Check whether the size of the object being converted
        // to might be smaller than the source object on some
        // implementations.
        if ((P_OK == status || P_SIGNEDNESS == status) &&
            Lim<T>::maybe_smaller_than(fromcode))
          status = P_BADSIZE;

        // Perform the conversion if possible.
        if (P_UNDERFLOW == status) mData = Lim<T>::min;
        else if (P_OVERFLOW == status) mData = Lim<T>::max;
        else if (P_ILLEGAL != status) mData = (T) p;

        // Throw exception if everything not perfect.
        if (P_OK != status) throw Hexception(status);
      }

      virtual void ConvertFrom(const long double & p, PrimTypeCode_e fromcode)
        throw (Hexception) {
        HexceptionCode_e status = P_ILLEGAL;

        if (P_CHAR_P == Lim<T>::code) status = P_ILLEGAL;
        else if (P_LONGDOUBLE == Lim<T>::code) status = P_OK;
        else if (Lim<T>::is_integer) {
          if ((long double) Lim<T>::min > p) status = P_UNDERFLOW;
          else if ((long double) Lim<T>::max < p) status = P_OVERFLOW;
          else status = P_PRECISION;
        } else {
          if ((long double) Lim<T>::min > p &&
              p/(long double) Lim<T>::min > 1. + Lim<T>::epsilon)
            status = P_UNDERFLOW;
          else if ((long double) Lim<T>::max < p &&
              p/(long double) Lim<T>::max > 1. + Lim<T>::epsilon)
            status = P_OVERFLOW;
          else status = P_OK;
        }

        if (P_OK == status) {
          if (Lim<T>::maybe_smaller_than(fromcode)) status = P_BADSIZE;
        }

        // Perform the conversion if possible.
        if (P_UNDERFLOW == status) mData = Lim<T>::min;
        else if (P_OVERFLOW == status) mData = Lim<T>::max;
        else if (P_ILLEGAL != status) mData = (T) p;

        // Throw exception if everything not perfect.
        if (P_OK != status) throw Hexception(status);
      }

      // Conversion from a string.
      virtual void ConvertFrom(const char * p) throw (Hexception) {
        PrimTypeCode_e typecode = Lim<T>::code;
        HexceptionCode_e status = p ? P_OK : P_STR_NULL;

        // Skip leading whitespace.
        while (*p && isspace(*p)) ++p;

        // Lie about type char; determine whether it's signed or unsigned
        // but warn about the choice.
        if (P_CHAR == typecode) {
          if (Lim<char>::is_signed) typecode = P_SCHAR;
          else typecode = P_UCHAR;
          status = P_SIGNEDNESS;
        }

        switch (typecode) {
          case P_BOOL:
            if (!strcasecmp(p, "yes") || !strcasecmp(p, "y") ||
                !strcasecmp(p, "true") || !strcasecmp(p, "t") ||
                !strcasecmp(p, "1")) {
              mData = true;
              p = "";
            } else if (!strcasecmp(p, "no") || !strcasecmp(p, "n") ||
                !strcasecmp(p, "false") || !strcasecmp(p, "f") ||
                !strcasecmp(p, "0")) {
              mData = false;
              p = "";
            } else status = P_STR_INVALID;
            break;
          case P_CHAR:
            // Should not happen; P_CHAR should have been converted above to
            // P_SCHAR or P_UCHAR.
            status = P_ILLEGAL;
            break;
          case P_SCHAR:
          case P_SHORT:
          case P_INT:
          case P_LONG:
            {
              char * r = 0;
              errno = 0;
              long val = strtol(p, &r, 0);
              if (ERANGE == errno) {
                mData = Lim<T>::max;
                status = P_STR_OVERFLOW;
              } else if ((long) Lim<T>::min > val) {
                mData = Lim<T>::min;
                status = P_UNDERFLOW;
              } else if ((long) Lim<T>::max < val) {
                mData = Lim<T>::max;
                status = P_OVERFLOW;
              } else {
                mData = (T) val;
              }
              p = r;
            }
            break;
          case P_UCHAR:
          case P_USHORT:
          case P_UINT:
          case P_ULONG:
            {
              char * r = 0;
              errno = 0;
              unsigned long val = strtoul(p, &r, 0);
              if (ERANGE == errno) {
                mData = Lim<T>::max;
                status = P_STR_OVERFLOW;
              } else if ((unsigned long) Lim<T>::max < val) {
                mData = Lim<T>::max;
                status = P_OVERFLOW;
              } else {
                mData = (T) val;
              }
              p = r;
            }
            break;
          case P_FLOAT:
          case P_DOUBLE:
          case P_LONGDOUBLE:
            {
              char * r = 0;
              errno = 0;
              long double val = strtod(p, &r);
              if (ERANGE == errno) {
                mData = Lim<T>::max;
                status = P_STR_OVERFLOW;
              } else if ((long double) Lim<T>::min > val &&
                 val/(long double) Lim<T>::min > 1. + Lim<T>::epsilon) {
                mData = Lim<T>::min;
                status = P_UNDERFLOW;
              } else if ((long double) Lim<T>::max < val &&
                 val/(long double) Lim<T>::max > 1. + Lim<T>::epsilon) {
                mData = Lim<T>::max;
                status = P_OVERFLOW;
              } else {
                mData = (T) val;
              }
              p = r;
            }
            break;
          case P_CHAR_P:
            // Should not happen because there is a specialization
            // for type char *.
            status = P_ILLEGAL;
            break;
          default:
            status = P_ILLEGAL;
            break;
        }
        if ((P_OK == status || P_SIGNEDNESS == status) && !IsBlank(p))
          status = P_STR_INVALID;

        if (P_OK != status) throw Hexception(status);
      }

      // Utility template function ConvertTo. Creates a
      // temporary Prim type and calls its From. Exceptions
      // are trapped and rethrown so that the value will
      // be assigned even if an error occurs. This allows
      // the caller to ignore the exception.
      template <typename S>
      void ConvertTo(S & p) const throw (Hexception) {
        Prim<S> tmp;
        try {
          tmp.From(mData);
        } catch (const Hexception & x) {
          if (P_ILLEGAL != x.Code()) p = tmp.Data();
          throw;
        }
        p = tmp.Data();
      }

      // "To" family. Calls the appropriate ConvertTo function for
      // its primitive's underlying family type.
      virtual void To(IPrim & p) const throw (Hexception)
        { p.From(mData); }

      virtual void To(bool & p) const throw (Hexception)
        { ConvertTo<bool>(p); }

      virtual void To(char & p) const throw (Hexception)
        { ConvertTo<char>(p); }

      virtual void To(signed char & p) const throw (Hexception)
        { ConvertTo<signed char>(p); }

      virtual void To(short & p) const throw (Hexception)
        { ConvertTo<short>(p); }

      virtual void To(int & p) const throw (Hexception)
        { ConvertTo<int>(p); }

      virtual void To(long & p) const throw (Hexception)
        { ConvertTo<long>(p); }

      virtual void To(unsigned char & p) const throw (Hexception)
        { ConvertTo<unsigned char>(p); }

      virtual void To(unsigned short & p) const throw (Hexception)
        { ConvertTo<unsigned short>(p); }

      virtual void To(unsigned int & p) const throw (Hexception)
        { ConvertTo<unsigned int>(p); }

      virtual void To(unsigned long & p) const throw (Hexception)
        { ConvertTo<unsigned long>(p); }

      virtual void To(float & p) const throw (Hexception)
        { ConvertTo<float>(p); }

      virtual void To(double & p) const throw (Hexception)
        { ConvertTo<double>(p); }

      virtual void To(long double & p) const throw (Hexception)
        { ConvertTo<long double>(p); }

      // Strings need special handling.
      virtual void To(char *& p) const throw (Hexception) {
        Prim<char *> tmp;
        try {
          tmp.From(mData);
        } catch (const Hexception & x) {
          if (P_ILLEGAL != x.Code()) p = CpyStr(tmp.Data());
          throw;
        }
        p = CpyStr(tmp.Data());
      }

      virtual void To(std::string & p) const throw (Hexception) {
        Prim<char *> tmp;
        try {
          tmp.From(mData);
        } catch (const Hexception & x) {
          if (P_ILLEGAL != x.Code()) p = tmp.Data();
          throw;
        }
        p = tmp.Data();
      }

      virtual IPrim * Clone() const { return new Prim<T>(*this); }

      virtual std::string StringData() const throw() { 
        std::string r;
        try {
          To(r);
        } catch (const Hexception &) {
          // Ignore errors -- there really shouldn't be any.
        }
        return r;
      }

      const T Data() const { return mData; }

    private:
      T mData;
  };
  //////////////////////////////////////////////////////////////////////////////

  // Specializations for char *. Needed because it's not
  // numeric, and it uses dynamic memory!
  //////////////////////////////////////////////////////////////////////////////
  template <>
  inline Prim<char *>::Prim(const Prim & p) throw() { mData = CpyStr(p.mData); }

  template <>
  inline Prim<char *>::~Prim() throw() { delete [] mData; }

  template <>
  inline Prim<char *> & Prim<char *>::operator =(const Prim<char *> & p)
    throw (Hexception) {
    if (&p != this) {
      delete [] mData;
      mData = CpyStr(p.mData);
    }
    return *this;
  }

  template <>
  inline void Prim<char *>::From(const IPrim & p) throw (Hexception) {
    if (&p != this) {
      delete [] mData;
      p.To(mData);
    }
  }

  // Conversions from primitives.
  template <>
  inline void Prim<char *>::ConvertFrom(const long & p, PrimTypeCode_e fromcode)
    throw (Hexception) {
    char tmp[128];
    sprintf(tmp, "%ld", p);
    delete [] mData;
    mData = CpyStr(tmp);
  }

  template <>
  inline void Prim<char *>::ConvertFrom
    (const unsigned long & p, PrimTypeCode_e fromcode) throw (Hexception) {
    char tmp[128];
    if (P_BOOL == fromcode)
      if (p) strcpy(tmp, "true"); else strcpy(tmp, "false");
    else sprintf(tmp, "%lu", p);
    delete [] mData;
    mData = CpyStr(tmp);
  }

  template <>
  inline void Prim<char *>::ConvertFrom
    (const long double & p, PrimTypeCode_e fromcode) throw (Hexception) {
    char tmp[256];
    char fmt[32];
    switch (fromcode) {
      case P_FLOAT:
        sprintf(fmt, "%%1.%dLg", Lim<float>::digits10);
        break;
      case P_DOUBLE:
        sprintf(fmt, "%%1.%dLg", Lim<double>::digits10);
        break;
      case P_LONGDOUBLE:
        sprintf(fmt, "%%1.%dLg", Lim<long double>::digits10);
        break;
      default:
        throw Hexception(P_UNEXPECTED);
        break;
    }
    sprintf(tmp, fmt, p);
    delete [] mData;
    mData = CpyStr(tmp);
  }

  template <>
  inline void Prim<char *>::ConvertFrom(const char * p) throw (Hexception) {
    delete [] mData;
    mData = CpyStr(p);
  }

  IPrim * PrimFactory::NewIPrim(const IPrim * p) const { return p->Clone(); }
  IPrim * PrimFactory::NewIPrim(const bool & p) const
    { return new Prim<bool>(p); }
  IPrim * PrimFactory::NewIPrim(const char & p) const
    { return new Prim<char>(p); }
  IPrim * PrimFactory::NewIPrim(const signed char & p) const
    { return new Prim<signed char>(p); }
  IPrim * PrimFactory::NewIPrim(const short & p) const
    { return new Prim<short>(p); }
  IPrim * PrimFactory::NewIPrim(const int & p) const
    { return new Prim<int>(p); }
  IPrim * PrimFactory::NewIPrim(const long & p) const
    { return new Prim<long>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned char & p) const
    { return new Prim<unsigned char>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned short & p) const
    { return new Prim<unsigned char>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned int & p) const
    { return new Prim<unsigned char>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned long & p) const
    { return new Prim<unsigned char>(p); }
  IPrim * PrimFactory::NewIPrim(const float & p) const
    { return new Prim<float>(p); }
  IPrim * PrimFactory::NewIPrim(const double & p) const
    { return new Prim<double>(p); }
  IPrim * PrimFactory::NewIPrim(const long double & p) const
    { return new Prim<long double>(p); }
  IPrim * PrimFactory::NewIPrim(const char * p) const
    { return new Prim<char *>(p); }
  IPrim * PrimFactory::NewIPrim(const std::string & p) const
    { return new Prim<char *>(p.c_str()); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static variable definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static function definitions.
  //////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
  // Windows has no strcasecmp function.
  static int strcasecmp(const char *s1, const char *s2) {
    const char *p1 = s1;
    const char *p2 = s2;
    int diff = 0;
    while (!diff) {
      diff = toupper(*p1) - toupper(*p2);
      if (*p1 && *p2) { ++p1; ++p2; }
      else { diff = *p1 - *p2; break; }
    }
    return diff;
  }
#endif

  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function definitions.
  //////////////////////////////////////////////////////////////////////////////
  std::ostream &operator <<(std::ostream &os, const IPrim &p) {
    os << p.StringData();
    return os;
  }
  //////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * $Log$
 * Revision 1.4  2003/05/15 04:02:16  peachey
 * Use standard <limits> header only if HAVE_LIMITS is defined. Otherwise,
 * use hoops_limits.h.
 *
 * Revision 1.3  2003/05/14 15:28:00  peachey
 * Add strcasecmp for Windows.
 *
 * Revision 1.2  2003/05/14 15:17:06  peachey
 * Further encapsulate Prim class by hiding the templates in hoops_prim.cxx
 * and using a factory class.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
