/******************************************************************************
 *   File name: hoops_par.cxx                                                 *
 *                                                                            *
 * Description: Implementation of standard parameter type.                    *
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
#include <cstring>
#include <iostream>
#include "hoops/hoops_par.h"
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
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions.
  //////////////////////////////////////////////////////////////////////////////
  Par::Par() throw(): mName(), mType(), mMode(), mValue(0), mMin(), mMax(),
    mPrompt(), mComment(), mValString() {}

  Par::Par(const Par & p) throw(std::bad_alloc): mName(p.mName),
    mType(p.mType), mMode(p.mMode), mValue(0), mMin(), mMax(),
    mPrompt(p.mPrompt), mComment(p.mComment), mValString() {
    if (p.mValue) mValue = p.mValue->Clone();
  }

  Par::Par(const IPar & p) throw(std::bad_alloc): mName(p.Name()),
    mType(p.Type()), mMode(p.Mode()), mValue(0), mMin(), mMax(),
    mPrompt(p.Prompt()), mComment(p.Comment()), mValString() {
    if (!p.Value().empty()) From(p.Value());
  }

  Par::Par(const std::string & name, const std::string & type,
    const std::string & mode, const std::string & value,
    const std::string & min, const std::string & max,
    const std::string & prompt, const std::string & comment)
    throw(std::bad_alloc): mName(name), mType(type), mMode(mode),
    mValue(0), mMin(min), mMax(max), mPrompt(prompt),
    mComment(comment), mValString() {
    if (!value.empty()) From(value);
  }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Destructor.
  //////////////////////////////////////////////////////////////////////////////
  Par::~Par() throw() {
    delete mValue;
  }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Assignments.
  //////////////////////////////////////////////////////////////////////////////
  void Par::From(const IPar & p) throw(std::bad_alloc, Hexception) {
    if (!p.Value().empty()) From(p.Value());
    else if (!p.Type().empty() && !Type().empty()) {
      // Allow conversion from a "null" parameter if dest and source
      // are well defined parameter type.
      delete mValue;
      mValue = 0;
    } else {
      // At least one parameter is of undefined type. This is illegal.
      throw Hexception(PAR_ILLEGAL_CONVERSION);
    }
  }

  void Par::From(const IPrim & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const IPrim &>(p, mValue, mType); }

  void Par::From(const bool & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const bool &>(p, mValue, mType); }

  void Par::From(const char & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const char &>(p, mValue, mType); }

  void Par::From(const signed char & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const signed char &>(p, mValue, mType); }

  void Par::From(const short & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const short &>(p, mValue, mType); }

  void Par::From(const int & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const int &>(p, mValue, mType); }

  void Par::From(const long & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const long &>(p, mValue, mType); }

  void Par::From(const unsigned char & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const unsigned char &>(p, mValue, mType); }

  void Par::From(const unsigned short & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const unsigned short &>(p, mValue, mType); }

  void Par::From(const unsigned int & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const unsigned int &>(p, mValue, mType); }

  void Par::From(const unsigned long & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const unsigned long &>(p, mValue, mType); }

  void Par::From(const float & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const float &>(p, mValue, mType); }

  void Par::From(const double & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const double &>(p, mValue, mType); }

  void Par::From(const long double & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const long double &>(p, mValue, mType); }

  void Par::From(const char * p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const std::string &>(p, mValue, mType); }

  void Par::From(const std::string & p) throw(std::bad_alloc, Hexception)
    { ConvertFrom<const std::string &>(p, mValue, mType); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Conversions.
  //////////////////////////////////////////////////////////////////////////////
  Par::operator bool () const throw(Hexception)
    { bool r; ConvertTo<bool>(mValue, r); return r; }
  Par::operator char () const throw(Hexception)
    { char r; ConvertTo<char>(mValue, r); return r; }
  Par::operator signed char () const throw(Hexception)
    { signed char r; ConvertTo<signed char>(mValue, r); return r; }
  Par::operator short () const throw(Hexception)
    { short r; ConvertTo<short>(mValue, r); return r; }
  Par::operator int () const throw(Hexception)
    { int r; ConvertTo<int>(mValue, r); return r; }
  Par::operator long () const throw(Hexception)
    { long r; ConvertTo<long>(mValue, r); return r; }
  Par::operator unsigned char () const throw(Hexception)
    { unsigned char r; ConvertTo<unsigned char>(mValue, r); return r; }
  Par::operator unsigned short () const throw(Hexception)
    { unsigned short r; ConvertTo<unsigned short>(mValue, r); return r; }
  Par::operator unsigned int () const throw(Hexception)
    { unsigned int r; ConvertTo<unsigned int>(mValue, r); return r; }
  Par::operator unsigned long () const throw(Hexception)
    { unsigned long r; ConvertTo<unsigned long>(mValue, r); return r; }
  Par::operator float () const throw(Hexception)
    { float r; ConvertTo<float>(mValue, r); return r; }
  Par::operator double () const throw(Hexception)
    { double r; ConvertTo<double>(mValue, r); return r; }
  Par::operator long double () const throw(Hexception)
    { long double r; ConvertTo<long double>(mValue, r); return r; }

  // Difference between this and Value() is that the latter handles exceptions.
  Par::operator const char *() const throw(std::bad_alloc, Hexception) {
    ConvertTo<std::string>(mValue, mValString);
    return mValString.c_str();
  }

  Par::operator const std::string &() const throw(std::bad_alloc, Hexception) {
    ConvertTo<std::string>(mValue, mValString);
    return mValString;
  }

  void Par::To(bool & p) const throw(Hexception)
    { ConvertTo<bool>(mValue, p); }

  void Par::To(char & p) const throw(Hexception)
    { ConvertTo<char>(mValue, p); }

  void Par::To(signed char & p) const throw(Hexception)
    { ConvertTo<signed char>(mValue, p); }

  void Par::To(short & p) const throw(Hexception)
    { ConvertTo<short>(mValue, p); }

  void Par::To(int & p) const throw(Hexception)
    { ConvertTo<int>(mValue, p); }

  void Par::To(long & p) const throw(Hexception)
    { ConvertTo<long>(mValue, p); }

  void Par::To(unsigned char & p) const throw(Hexception)
    { ConvertTo<unsigned char>(mValue, p); }

  void Par::To(unsigned short & p) const throw(Hexception)
    { ConvertTo<unsigned short>(mValue, p); }

  void Par::To(unsigned int & p) const throw(Hexception)
    { ConvertTo<unsigned int>(mValue, p); }

  void Par::To(unsigned long & p) const throw(Hexception)
    { ConvertTo<unsigned long>(mValue, p); }

  void Par::To(float & p) const throw(Hexception)
    { ConvertTo<float>(mValue, p); }

  void Par::To(double & p) const throw(Hexception)
    { ConvertTo<double>(mValue, p); }

  void Par::To(long double & p) const throw(Hexception)
    { ConvertTo<long double>(mValue, p); }

  void Par::To(std::string & p) const throw(std::bad_alloc, Hexception)
    { ConvertTo<std::string>(mValue, p); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Member access.
  //////////////////////////////////////////////////////////////////////////////
  const std::string & Par::Value() const throw(std::bad_alloc) {
    try { ConvertTo<std::string>(mValue, mValString); }
    catch (const Hexception &) {}
    return mValString;
  }
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
  std::ostream & operator <<(std::ostream & os, const IPar & p) throw() {
    if (!p.Name().empty()) {
      os << p.Name() << "," << p.Type() << "," << p.Mode() << ",";
      if (!p.Type().compare("f") || !p.Type().compare("s")) {
        os << '"' << p.Value() << '"' << ",";
        if (!p.Min().empty()) os << '"' << p.Min() << '"';
        os << ",";
        if (!p.Max().empty()) os << '"' << p.Max() << '"';
      } else if (!p.Type().compare("b")) {
        const char * value = p.Value().c_str();
        if (!strcasecmp(value, "true"))
          os << "\"yes\"," << p.Min() << "," << p.Max();
        else if (!strcasecmp(value, "false"))
          os << "\"no\"," << p.Min() << "," << p.Max();
        else
          os << p.Value() << "," << p.Min() << "," << p.Max();
      } else {
        os << p.Value() << "," << p.Min() << "," << p.Max();
      }
      os << "," << '"' << p.Prompt() << '"';
    }
    if (!p.Comment().empty()) os << p.Comment();
    return os;
  }

  //////////////////////////////////////////////////////////////////////////////

}

/******************************************************************************
 * $Log$
 * Revision 1.8  2003/11/26 18:50:02  peachey
 * Merging changes made to SLAC repository into Goddard repository.
 *
 * Revision 1.7  2003/11/26 17:54:09  peachey
 * Explicitly zero-initialize all pointers, as VS does not adhere to
 * the ISO standard for default-initialized pointers.
 *
 * Revision 1.6  2003/11/13 20:53:21  peachey
 * Remove dummy exception variable to silence warning on Windows.
 *
 * Revision 1.5  2003/11/10 18:16:12  peachey
 * Moved header files into hoops subdirectory.
 *
 * Revision 1.4  2003/11/07 18:12:05  peachey
 * Do not use fully qualified names for static functions. This confuses
 * Visual Studio 7
 *
 * Revision 1.3  2003/06/18 18:17:42  peachey
 * Remove method to return char * because IPrim no longer supports
 * char * as a specializable type.
 *
 * Revision 1.1.1.1  2003/11/04 01:48:29  jchiang
 * First import
 *
 * Revision 1.2  2003/05/14 15:28:00  peachey
 * Add strcasecmp for Windows.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
