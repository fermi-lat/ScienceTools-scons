/******************************************************************************
 *   File name: hoops_prim.cxx                                                *
 *                                                                            *
 * Description: Implementation of universal primitive type converter.         *
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
#include <string>
#include "hoops/hoops_exception.h"
#include "hoops/hoops_limits.h"
#include "hoops/hoops_prim.h"
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static function declarations.
  //////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
  static int strcasecmp(const char *s1, const char *s2);
#endif

  static const char * float_format();
  static const char * double_format();
  static const char * long_double_format();

  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static variable definitions.
  //////////////////////////////////////////////////////////////////////////////
  static const char * const sFloatFormat = float_format();
  static const char * const sDoubleFormat = double_format();
  static const char * const sLongDoubleFormat = long_double_format();
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions.
  //////////////////////////////////////////////////////////////////////////////
  class Conv {
    public:
      static void Convert(const bool & s, bool & d) { d = s; }
      static void Convert(const bool & s, char & d) { d = s; }
      static void Convert(const bool & s, signed char & d) { d = s; }
      static void Convert(const bool & s, signed short & d) { d = s; }
      static void Convert(const bool & s, signed int & d) { d = s; }
      static void Convert(const bool & s, signed long & d) { d = s; }
      static void Convert(const bool & s, unsigned char & d) { d = s; }
      static void Convert(const bool & s, unsigned short & d) { d = s; }
      static void Convert(const bool & s, unsigned int & d) { d = s; }
      static void Convert(const bool & s, unsigned long & d) { d = s; }
      static void Convert(const bool & s, float & d) { d = s; }
      static void Convert(const bool & s, double & d) { d = s; }
      static void Convert(const bool & s, long double & d) { d = s; }
      static void Convert(const bool & s, std::string & d)
        { if (s) d = "true"; else d = "false"; }

      static void Convert(const char & s, bool & d) {
        if (Lim<char>::is_signed && s < char(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > char(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const char & s, char & d) { d = s; }
      static void Convert(const char & s, signed char & d) {
        if (!Lim<char>::is_signed && s > (char) Lim<signed char>::max)
          { d = Lim<signed char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed char)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, signed short & d) {
        if (!Lim<char>::is_signed &&
          (unsigned short) s > (unsigned short) Lim<signed short>::max)
          { d = Lim<signed short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed short)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, signed int & d) {
        if (!Lim<char>::is_signed &&
          (unsigned int) s > (unsigned int) Lim<signed int>::max)
          { d = Lim<signed int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed int)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, signed long & d) {
        if (!Lim<char>::is_signed &&
          (unsigned long) s > (unsigned long) Lim<signed long>::max)
          { d = Lim<signed long>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed long)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, unsigned char & d) {
        if (Lim<char>::is_signed && s < (char) Lim<unsigned char>::min)
          { d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned char)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, unsigned short & d) {
        if (Lim<char>::is_signed &&
          (signed short) s < (signed short) Lim<unsigned short>::min)
          { d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned short)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, unsigned int & d) {
        if (Lim<char>::is_signed &&
          (signed int) s < (signed int) Lim<unsigned int>::min)
          { d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned int)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, unsigned long & d) {
        if (Lim<char>::is_signed &&
          (signed long) s < (signed long) Lim<unsigned long>::min)
          { d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned long)(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const char & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const char & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const char & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const char & s, std::string & d)
        { char buf[16]; sprintf(buf, "%d", s); d = buf; }

      static void Convert(const signed char & s, bool & d) {
        if (s < char(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > char(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed char & s, char & d) {
        if (!Lim<char>::is_signed && s < (signed char) Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else { d = char(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed char & s, signed char & d) { d = s; }
      static void Convert(const signed char & s, signed short & d) { d = s; }
      static void Convert(const signed char & s, signed int & d) { d = s; }
      static void Convert(const signed char & s, signed long & d) { d = s; }
      static void Convert(const signed char & s, unsigned char & d) {
        if (s < (signed char) Lim<unsigned char>::min)
          { d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned char) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed char & s, unsigned short & d) {
        if (s < (signed short) Lim<unsigned short>::min)
          { d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed char & s, unsigned int & d) {
        if (s < (signed int) Lim<unsigned int>::min)
          { d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed char & s, unsigned long & d) {
        if (s < (signed long) Lim<unsigned long>::min)
          { d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW); }
        else { d = (unsigned long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed char & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed char & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed char & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed char & s, std::string & d)
        { char buf[16]; sprintf(buf, "%d", s); d = buf; }

      static void Convert(const signed short & s, bool & d) {
        if (s < (signed short)(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > (signed short)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed short & s, char & d) {
        if (s < (signed short) Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned short) s > (unsigned short) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed short & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed short & s, signed short & d) { d = s; }
      static void Convert(const signed short & s, signed int & d) { d = s; }
      static void Convert(const signed short & s, signed long & d) { d = s; }
      static void Convert(const signed short & s, unsigned char & d) {
        if (s < (signed short) Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned short) s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed short & s, unsigned short & d) {
        if (s < (signed short) Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed short & s, unsigned int & d) {
        if (s < (signed int) Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed short & s, unsigned long & d) {
        if (s < (signed long) Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed short & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed short & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed short & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed short & s, std::string & d)
        { char buf[16]; sprintf(buf, "%hd", s); d = buf; }

      static void Convert(const signed int & s, bool & d) {
        if (s < (signed int)(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > (signed int)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, char & d) {
        if (s < (signed int) Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned int) s > (unsigned int) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, signed short & d) {
        if (s < Lim<signed short>::min) {
          d = Lim<signed short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed short>::max) {
          d = Lim<signed short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed short)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, signed int & d) { d = s; }
      static void Convert(const signed int & s, signed long & d) { d = s; }
      static void Convert(const signed int & s, unsigned char & d) {
        if (s < (signed int) Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned int) s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, unsigned short & d) {
        if (s < (signed int) Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned int) s > Lim<unsigned short>::max) {
          d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed int & s, unsigned int & d) {
        if (s < (signed int) Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed int & s, unsigned long & d) {
        if (s < (signed long) Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed int & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed int & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed int & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed int & s, std::string & d)
        { char buf[32]; sprintf(buf, "%d", s); d = buf; }

      static void Convert(const signed long & s, bool & d) {
        if (s < (signed long)(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > (signed long)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, char & d) {
        if (s < (signed long) Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned long) s > (unsigned long) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, signed short & d) {
        if (s < Lim<signed short>::min) {
          d = Lim<signed short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed short>::max) {
          d = Lim<signed short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed short)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, signed int & d) {
        if (s < Lim<signed int>::min) {
          d = Lim<signed int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed int>::max) {
          d = Lim<signed int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed int)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, signed long & d) { d = s; }
      static void Convert(const signed long & s, unsigned char & d) {
        if (s < (signed long) Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned long) s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, unsigned short & d) {
        if (s < (signed long) Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned long) s > Lim<unsigned short>::max) {
          d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, unsigned int & d) {
        if (s < (signed long) Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else if ((unsigned long) s > Lim<unsigned int>::max) {
          d = Lim<unsigned int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_BADSIZE); }
      }
      static void Convert(const signed long & s, unsigned long & d) {
        if (s < (signed long) Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const signed long & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed long & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed long & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const signed long & s, std::string & d)
        { char buf[64]; sprintf(buf, "%ld", s); d = buf; }

      static void Convert(const unsigned char & s, bool & d) {
        if (s > (unsigned char)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned char & s, char & d) {
        if (Lim<char>::is_signed && s > (unsigned char) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned char & s, signed char & d) {
        if (s > (unsigned char) Lim<signed char>::max)
          { d = Lim<signed char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed char) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned char & s, signed short & d) {
        if (s > (unsigned short) Lim<signed short>::max)
          { d = Lim<signed short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned char & s, signed int & d) {
        if (s > (unsigned int) Lim<signed int>::max)
          { d = Lim<signed int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned char & s, signed long & d) {
        if (s > (unsigned long) Lim<signed long>::max)
          { d = Lim<signed long>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned char & s, unsigned char & d) { d = s; }
      static void Convert(const unsigned char & s, unsigned short & d)
        { d = s; }
      static void Convert(const unsigned char & s, unsigned int & d) { d = s; }
      static void Convert(const unsigned char & s, unsigned long & d) { d = s; }
      static void Convert(const unsigned char & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned char & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned char & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned char & s, std::string & d)
        { char buf[16]; sprintf(buf, "%u", s); d = buf; }

      static void Convert(const unsigned short & s, bool & d) {
        if (s > (unsigned short)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned short & s, char & d) {
        if (Lim<char>::is_signed && s > (unsigned short) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned short & s, signed char & d) {
        if (s > (unsigned short) Lim<signed char>::max)
          { d = Lim<signed char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed char) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned short & s, signed short & d) {
        if (s > (unsigned short) Lim<signed short>::max)
          { d = Lim<signed short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned short & s, signed int & d) {
        if (s > (unsigned int) Lim<signed int>::max)
          { d = Lim<signed int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned short & s, signed long & d) {
        if (s > (unsigned long) Lim<signed long>::max)
          { d = Lim<signed long>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned short & s, unsigned char & d) {
        if (s > Lim<unsigned char>::max)
          { d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned short & s, unsigned short & d)
        { d = s; }
      static void Convert(const unsigned short & s, unsigned int & d) { d = s; }
      static void Convert(const unsigned short & s, unsigned long & d)
        { d = s; }
      static void Convert(const unsigned short & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned short & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned short & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned short & s, std::string & d)
        { char buf[16]; sprintf(buf, "%hu", s); d = buf; }

      static void Convert(const unsigned int & s, bool & d) {
        if (s > (unsigned int)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned int & s, char & d) {
        if (Lim<char>::is_signed && s > (unsigned int) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned int & s, signed char & d) {
        if (s > (unsigned int) Lim<signed char>::max)
          { d = Lim<signed char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed char) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned int & s, signed short & d) {
        if (s > (unsigned int) Lim<signed short>::max)
          { d = Lim<signed short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned int & s, signed int & d) {
        if (s > (unsigned int) Lim<signed int>::max)
          { d = Lim<signed int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned int & s, signed long & d) {
        if (s > (unsigned long) Lim<signed long>::max)
          { d = Lim<signed long>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned int & s, unsigned char & d) {
        if (s > Lim<unsigned char>::max)
          { d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned int & s, unsigned short & d) {
        if (s > Lim<unsigned short>::max)
          { d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned short)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned int & s, unsigned int & d) { d = s; }
      static void Convert(const unsigned int & s, unsigned long & d) { d = s; }
      static void Convert(const unsigned int & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned int & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned int & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned int & s, std::string & d)
        { char buf[32]; sprintf(buf, "%u", s); d = buf; }

      static void Convert(const unsigned long & s, bool & d) {
        if (s > (unsigned long)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned long & s, char & d) {
        if (Lim<char>::is_signed && s > (unsigned long) Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = char(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned long & s, signed char & d) {
        if (s > (unsigned long) Lim<signed char>::max)
          { d = Lim<signed char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed char) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned long & s, signed short & d) {
        if (s > (unsigned long) Lim<signed short>::max)
          { d = Lim<signed short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed short) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned long & s, signed int & d) {
        if (s > (unsigned long) Lim<signed int>::max)
          { d = Lim<signed int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed int) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned long & s, signed long & d) {
        if (s > (unsigned long) Lim<signed long>::max)
          { d = Lim<signed long>::max; throw Hexception(P_OVERFLOW); }
        else { d = (signed long) s; throw Hexception(P_SIGNEDNESS); }
      }
      static void Convert(const unsigned long & s, unsigned char & d) {
        if (s > Lim<unsigned char>::max)
          { d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned char)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned long & s, unsigned short & d) {
        if (s > Lim<unsigned short>::max)
          { d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned short)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned long & s, unsigned int & d) {
        if (s > Lim<unsigned int>::max)
          { d = Lim<unsigned int>::max; throw Hexception(P_OVERFLOW); }
        else { d = (unsigned int)(s); throw Hexception(P_BADSIZE); }
      }
      static void Convert(const unsigned long & s, unsigned long & d)
        { d = s; }
      static void Convert(const unsigned long & s, float & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned long & s, double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned long & s, long double & d)
        { d = s; throw Hexception(P_PRECISION); }
      static void Convert(const unsigned long & s, std::string & d)
        { char buf[64]; sprintf(buf, "%lu", s); d = buf; }

      static void Convert(const float & s, bool & d) {
        if (s < float(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > float(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, char & d) {
        if (s < Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, signed short & d) {
        if (s < Lim<signed short>::min) {
          d = Lim<signed short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed short>::max) {
          d = Lim<signed short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, signed int & d) {
        if (s < Lim<signed int>::min) {
          d = Lim<signed int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed int>::max) {
          d = Lim<signed int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, signed long & d) {
        if (s < Lim<signed long>::min) {
          d = Lim<signed long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed long>::max) {
          d = Lim<signed long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, unsigned char & d) {
        if (s < Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, unsigned short & d) {
        if (s < Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned short>::max) {
          d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, unsigned int & d) {
        if (s < Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned int>::max) {
          d = Lim<unsigned int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, unsigned long & d) {
        if (s < Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned long>::max) {
          d = Lim<unsigned long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const float & s, float & d) { d = s; }
      static void Convert(const float & s, double & d) { d = s; }
      static void Convert(const float & s, long double & d) { d = s; }
      static void Convert(const float & s, std::string & d)
        { char buf[32]; sprintf(buf, sFloatFormat, s); d = buf; }

      static void Convert(const double & s, bool & d) {
        if (s < double(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > double(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, char & d) {
        if (s < Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, signed short & d) {
        if (s < Lim<signed short>::min) {
          d = Lim<signed short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed short>::max) {
          d = Lim<signed short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, signed int & d) {
        if (s < Lim<signed int>::min) {
          d = Lim<signed int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed int>::max) {
          d = Lim<signed int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, signed long & d) {
        if (s < Lim<signed long>::min) {
          d = Lim<signed long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed long>::max) {
          d = Lim<signed long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, unsigned char & d) {
        if (s < Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, unsigned short & d) {
        if (s < Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned short>::max) {
          d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, unsigned int & d) {
        if (s < Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned int>::max) {
          d = Lim<unsigned int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, unsigned long & d) {
        if (s < Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned long>::max) {
          d = Lim<unsigned long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const double & s, float & d) {
        if (s < Lim<float>::min) {
          d = Lim<float>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<float>::max) {
          d = Lim<float>::max; throw Hexception(P_OVERFLOW);
        } else { d = float(s); }
      }
      static void Convert(const double & s, double & d) { d = s; }
      static void Convert(const double & s, long double & d) { d = s; }
      static void Convert(const double & s, std::string & d)
        { char buf[64]; sprintf(buf, sDoubleFormat, s); d = buf; }

      static void Convert(const long double & s, bool & d) {
        if (s < (long double)(Lim<bool>::min)) {
          d = Lim<bool>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > (long double)(Lim<bool>::max)) {
          d = Lim<bool>::max; throw Hexception(P_OVERFLOW);
        } else { d = bool(s); throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, char & d) {
        if (s < Lim<char>::min) {
          d = Lim<char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<char>::max) {
          d = Lim<char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, signed char & d) {
        if (s < Lim<signed char>::min) {
          d = Lim<signed char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed char>::max) {
          d = Lim<signed char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, signed short & d) {
        if (s < Lim<signed short>::min) {
          d = Lim<signed short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed short>::max) {
          d = Lim<signed short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, signed int & d) {
        if (s < Lim<signed int>::min) {
          d = Lim<signed int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed int>::max) {
          d = Lim<signed int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, signed long & d) {
        if (s < Lim<signed long>::min) {
          d = Lim<signed long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<signed long>::max) {
          d = Lim<signed long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (signed long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, unsigned char & d) {
        if (s < Lim<unsigned char>::min) {
          d = Lim<unsigned char>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned char>::max) {
          d = Lim<unsigned char>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned char) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, unsigned short & d) {
        if (s < Lim<unsigned short>::min) {
          d = Lim<unsigned short>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned short>::max) {
          d = Lim<unsigned short>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned short) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, unsigned int & d) {
        if (s < Lim<unsigned int>::min) {
          d = Lim<unsigned int>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned int>::max) {
          d = Lim<unsigned int>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned int) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, unsigned long & d) {
        if (s < Lim<unsigned long>::min) {
          d = Lim<unsigned long>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<unsigned long>::max) {
          d = Lim<unsigned long>::max; throw Hexception(P_OVERFLOW);
        } else { d = (unsigned long) s; throw Hexception(P_PRECISION); }
      }
      static void Convert(const long double & s, float & d) {
        if (s < Lim<float>::min) {
          d = Lim<float>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<float>::max) {
          d = Lim<float>::max; throw Hexception(P_OVERFLOW);
        } else { d = float(s); }
      }
      static void Convert(const long double & s, double & d) {
        if (s < Lim<double>::min) {
          d = Lim<double>::min; throw Hexception(P_UNDERFLOW);
        } else if (s > Lim<double>::max) {
          d = Lim<double>::max; throw Hexception(P_OVERFLOW);
        } else { d = double(s); }
      }
      static void Convert(const long double & s, long double & d) { d = s; }
      static void Convert(const long double & s, std::string & d)
        { char buf[128]; sprintf(buf, sLongDoubleFormat, s); d = buf; }

      static void Convert(const std::string & s, bool & d) {
        if (!strcasecmp(s.c_str(), "yes") || !strcasecmp(s.c_str(), "y") ||
          !strcasecmp(s.c_str(), "true") || !strcasecmp(s.c_str(), "t") ||
          !strcasecmp(s.c_str(), "1")) d = true;
        else {
          d = false;
          if (strcasecmp(s.c_str(), "no") && strcasecmp(s.c_str(), "n") &&
            strcasecmp(s.c_str(), "false") && strcasecmp(s.c_str(), "f") &&
            strcasecmp(s.c_str(), "0")) throw Hexception(P_STR_INVALID);
        }
      }
      static void Convert(const std::string & s, char & d) {
        char * r = 0;
        errno = 0;
        signed long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        signed long val = strtol(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<char>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if ((signed long) Lim<char>::min > val) {
          d = Lim<char>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<char>::max) < val) {
          d = Lim<char>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (char) val;
          if (val < Lim<unsigned char>::min) throw Hexception(P_SIGNEDNESS);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, signed char & d) {
        char * r = 0;
        errno = 0;
        signed long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        signed long val = strtol(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<signed char>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<signed char>::min > val) {
          d = Lim<signed char>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<signed char>::max) < val) {
          d = Lim<signed char>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (signed char)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, signed short & d) {
        char * r = 0;
        errno = 0;
        signed long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
                signed long val = strtol(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<signed short>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<signed short>::min > val) {
          d = Lim<signed short>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<signed short>::max) < val) {
          d = Lim<signed short>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (signed short)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, signed int & d) {
        char * r = 0;
        errno = 0;
        signed long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        signed long val = strtol(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<signed int>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<signed int>::min > val) {
          d = Lim<signed int>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<signed int>::max) < val) {
          d = Lim<signed int>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (signed int)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, signed long & d) {
        char * r = 0;
        errno = 0;
        signed long val = strtol(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<signed short>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else {
          d = val;
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, unsigned char & d) {
        char * r = 0;
        errno = 0;
        unsigned long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        unsigned long val = strtoul(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<unsigned char>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<unsigned char>::min > val) {
          d = Lim<unsigned char>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<unsigned char>::max) < val) {
          d = Lim<unsigned char>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (unsigned char)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, unsigned short & d) {
        char * r = 0;
        errno = 0;
        unsigned long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        unsigned long val = strtoul(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<unsigned short>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<unsigned short>::min > val) {
          d = Lim<unsigned short>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<unsigned short>::max) < val) {
          d = Lim<unsigned short>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (unsigned short)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, unsigned int & d) {
        char * r = 0;
        errno = 0;
        unsigned long tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        unsigned long val = strtoul(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<unsigned int>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<unsigned int>::min > val) {
          d = Lim<unsigned int>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<unsigned int>::max) < val) {
          d = Lim<unsigned int>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = (unsigned int)(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, unsigned long & d) {
        char * r = 0;
        errno = 0;
        unsigned long val = strtoul(s.c_str(), &r, 0);
        if (ERANGE == errno) {
          d = Lim<unsigned long>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else {
          d = val;
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, float & d) {
        char * r = 0;
        errno = 0;
        double tmpval; // Prevent spurious Visual Studio 7.0 compiler bug.
        double val = strtod(s.c_str(), &r);
        if (ERANGE == errno) {
          d = Lim<float>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else if (Lim<float>::min > val) {
          d = Lim<float>::min;
          throw Hexception(P_UNDERFLOW);
        } else if ((tmpval = Lim<float>::max) < val) {
          d = Lim<float>::max;
          throw Hexception(P_OVERFLOW);
        } else {
          d = float(val);
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, double & d) {
        char * r = 0;
        errno = 0;
        double val = strtod(s.c_str(), &r);
        if (ERANGE == errno) {
          d = Lim<double>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else {
          d = val;
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, long double & d) {
        char * r = 0;
        errno = 0;
        double val = strtod(s.c_str(), &r);
        if (ERANGE == errno) {
          d = Lim<long double>::max;
          throw Hexception(P_STR_OVERFLOW);
        } else {
          d = val;
        }
        if (!IPrim::IsBlank(r)) throw Hexception(P_STR_INVALID);
      }
      static void Convert(const std::string & s, std::string & d) { d = s; }
  };
  // Template class Prim. Any Prim class can be constructed from
  // any primitive type, though exceptions can be thrown if the
  // types have some sort of conversion problem!
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class Prim: public IPrim {
    public:
      Prim(): mData() {}
      Prim(T x): mData(x) {}
      virtual ~Prim() {}

      virtual void From(const IPrim & x) { x.To(mData); }
      virtual void From(const bool & x) { Conv::Convert(x, mData); }
      virtual void From(const char & x) { Conv::Convert(x, mData); }
      virtual void From(const signed char & x) { Conv::Convert(x, mData); }
      virtual void From(const signed short & x) { Conv::Convert(x, mData); }
      virtual void From(const signed int & x) { Conv::Convert(x, mData); }
      virtual void From(const signed long & x) { Conv::Convert(x, mData); }
      virtual void From(const unsigned char & x) { Conv::Convert(x, mData); }
      virtual void From(const unsigned short & x) { Conv::Convert(x, mData); }
      virtual void From(const unsigned int & x) { Conv::Convert(x, mData); }
      virtual void From(const unsigned long & x) { Conv::Convert(x, mData); }
      virtual void From(const float & x) { Conv::Convert(x, mData); }
      virtual void From(const double & x) { Conv::Convert(x, mData); }
      virtual void From(const long double & x) { Conv::Convert(x, mData); }
      virtual void From(const std::string & x) { Conv::Convert(x, mData); }

      virtual void To(IPrim & x) const { x.From(mData); }
      virtual void To(bool & x) const { Conv::Convert(mData, x); }
      virtual void To(char & x) const { Conv::Convert(mData, x); }
      virtual void To(signed char & x) const { Conv::Convert(mData, x); }
      virtual void To(signed short & x) const { Conv::Convert(mData, x); }
      virtual void To(signed int & x) const { Conv::Convert(mData, x); }
      virtual void To(signed long & x) const { Conv::Convert(mData, x); }
      virtual void To(unsigned char & x) const { Conv::Convert(mData, x); }
      virtual void To(unsigned short & x) const { Conv::Convert(mData, x); }
      virtual void To(unsigned int & x) const { Conv::Convert(mData, x); }
      virtual void To(unsigned long & x) const { Conv::Convert(mData, x); }
      virtual void To(float & x) const { Conv::Convert(mData, x); }
      virtual void To(double & x) const { Conv::Convert(mData, x); }
      virtual void To(long double & x) const { Conv::Convert(mData, x); }
      virtual void To(std::string & x) const { Conv::Convert(mData, x); }

      virtual std::string StringData() const throw()
        { std::string r; To(r); return r; }

      virtual IPrim * Clone() const { return new Prim<T>(mData); }

    protected:
      T mData;
  };

  //////////////////////////////////////////////////////////////////////////////
  IPrim * PrimFactory::NewIPrim(const bool & p) const
    { return new Prim<bool>(p); }
  IPrim * PrimFactory::NewIPrim(const char & p) const
    { return new Prim<char>(p); }
  IPrim * PrimFactory::NewIPrim(const signed char & p) const
    { return new Prim<signed char>(p); }
  IPrim * PrimFactory::NewIPrim(const signed short & p) const
    { return new Prim<signed short>(p); }
  IPrim * PrimFactory::NewIPrim(const signed int & p) const
    { return new Prim<signed int>(p); }
  IPrim * PrimFactory::NewIPrim(const signed long & p) const
    { return new Prim<signed long>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned char & p) const
    { return new Prim<unsigned char>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned short & p) const
    { return new Prim<unsigned short>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned int & p) const
    { return new Prim<unsigned int>(p); }
  IPrim * PrimFactory::NewIPrim(const unsigned long & p) const
    { return new Prim<unsigned long>(p); }
  IPrim * PrimFactory::NewIPrim(const float & p) const
    { return new Prim<float>(p); }
  IPrim * PrimFactory::NewIPrim(const double & p) const
    { return new Prim<double>(p); }
  IPrim * PrimFactory::NewIPrim(const long double & p) const
    { return new Prim<long double>(p); }
  IPrim * PrimFactory::NewIPrim(const std::string & p) const
    { return new Prim<std::string>(p); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable definitions.
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

  static const char * float_format() {
    static char r[16];
    sprintf(r, "%%1.%dg", Lim<float>::digits10);
    return r;
  }
  static const char * double_format() {
    static char r[16];
    sprintf(r, "%%1.%dg", Lim<double>::digits10);
    return r;
  }
  static const char * long_double_format() {
    static char r[16];
    sprintf(r, "%%1.%dLg", Lim<long double>::digits10);
    return r;
  }

  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function definitions.
  //////////////////////////////////////////////////////////////////////////////
  std::ostream & operator <<(std::ostream & os, const IPrim & p) {
    os << p.StringData();
    return os;
  }
  //////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * $Log$
 * Revision 1.9  2003/11/26 18:50:02  peachey
 * Merging changes made to SLAC repository into Goddard repository.
 *
 * Revision 1.8  2003/11/13 21:59:25  peachey
 * Add casts to clarify some conversions and silence some warnings on
 * Windows.
 *
 * Revision 1.7  2003/11/10 18:16:12  peachey
 * Moved header files into hoops subdirectory.
 *
 * Revision 1.6  2003/11/07 18:16:42  peachey
 * Prevent spurious internal compiler error on Visual Studio 7.0 by
 * assigning to a temporary variable rather than using the return value
 * of Lim<T>::max directly.
 *
 * Revision 1.5  2003/06/18 18:15:05  peachey
 * Major reworking of Prim class template to avoid specialization, which
 * has been causing many portability problems. Also relocated things
 * related to Lim class elsewhere, so this now contains only
 * IPrim-related classes.
 *
 * Revision 1.1.1.1  2003/11/04 01:48:29  jchiang
 * First import
 *
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
