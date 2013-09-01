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
  Par::Par() throw(): mName(), mType(), mMode(), mValue(), mMin(), mMax(),
    mPrompt(), mComment(), mValString() {}

  Par::Par(const Par & p) throw(std::bad_alloc): mName(p.mName),
    mType(p.mType), mMode(p.mMode), mValue(), mMin(), mMax(),
    mPrompt(p.mPrompt), mComment(p.mComment), mValString() {
    if (p.mValue) mValue = p.mValue->Clone();
  }

  Par::Par(const IPar & p) throw(std::bad_alloc): mName(p.Name()),
    mType(p.Type()), mMode(p.Mode()), mValue(), mMin(), mMax(),
    mPrompt(p.Prompt()), mComment(p.Comment()), mValString() {
    if (!p.Value().empty()) From(p.Value());
  }

  Par::Par(const std::string & name, const std::string & type,
    const std::string & mode, const std::string & value,
    const std::string & min, const std::string & max,
    const std::string & prompt, const std::string & comment)
    throw(std::bad_alloc): mName(name), mType(type), mMode(mode),
    mValue(), mMin(min), mMax(max), mPrompt(prompt),
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
    { Par::ConvertFrom<const IPrim &>(p, mValue, mType); }

  void Par::From(const bool & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const bool &>(p, mValue, mType); }

  void Par::From(const char & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const char &>(p, mValue, mType); }

  void Par::From(const signed char & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const signed char &>(p, mValue, mType); }

  void Par::From(const short & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const short &>(p, mValue, mType); }

  void Par::From(const int & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const int &>(p, mValue, mType); }

  void Par::From(const long & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const long &>(p, mValue, mType); }

  void Par::From(const unsigned char & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const unsigned char &>(p, mValue, mType); }

  void Par::From(const unsigned short & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const unsigned short &>(p, mValue, mType); }

  void Par::From(const unsigned int & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const unsigned int &>(p, mValue, mType); }

  void Par::From(const unsigned long & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const unsigned long &>(p, mValue, mType); }

  void Par::From(const float & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const float &>(p, mValue, mType); }

  void Par::From(const double & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const double &>(p, mValue, mType); }

  void Par::From(const long double & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const long double &>(p, mValue, mType); }

  void Par::From(const char * p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const char *>(p, mValue, mType); }

  void Par::From(const std::string & p) throw(std::bad_alloc, Hexception)
    { Par::ConvertFrom<const std::string &>(p, mValue, mType); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Conversions.
  //////////////////////////////////////////////////////////////////////////////
  Par::operator bool () const throw(Hexception)
    { bool r; Par::ConvertTo<bool>(mValue, r); return r; }
  Par::operator char () const throw(Hexception)
    { char r; Par::ConvertTo<char>(mValue, r); return r; }
  Par::operator signed char () const throw(Hexception)
    { signed char r; Par::ConvertTo<signed char>(mValue, r); return r; }
  Par::operator short () const throw(Hexception)
    { short r; Par::ConvertTo<short>(mValue, r); return r; }
  Par::operator int () const throw(Hexception)
    { int r; Par::ConvertTo<int>(mValue, r); return r; }
  Par::operator long () const throw(Hexception)
    { long r; Par::ConvertTo<long>(mValue, r); return r; }
  Par::operator unsigned char () const throw(Hexception)
    { unsigned char r; Par::ConvertTo<unsigned char>(mValue, r); return r; }
  Par::operator unsigned short () const throw(Hexception)
    { unsigned short r; Par::ConvertTo<unsigned short>(mValue, r); return r; }
  Par::operator unsigned int () const throw(Hexception)
    { unsigned int r; Par::ConvertTo<unsigned int>(mValue, r); return r; }
  Par::operator unsigned long () const throw(Hexception)
    { unsigned long r; Par::ConvertTo<unsigned long>(mValue, r); return r; }
  Par::operator float () const throw(Hexception)
    { float r; Par::ConvertTo<float>(mValue, r); return r; }
  Par::operator double () const throw(Hexception)
    { double r; Par::ConvertTo<double>(mValue, r); return r; }
  Par::operator long double () const throw(Hexception)
    { long double r; Par::ConvertTo<long double>(mValue, r); return r; }

  // Difference between this and Value() is that the latter handles exceptions.
  Par::operator const char *() const throw(std::bad_alloc, Hexception) {
    Par::ConvertTo<std::string>(mValue, mValString);
    return mValString.c_str();
  }

  Par::operator const std::string &() const throw(std::bad_alloc, Hexception) {
    Par::ConvertTo<std::string>(mValue, mValString);
    return mValString;
  }

  void Par::To(bool & p) const throw(Hexception)
    { Par::ConvertTo<bool>(mValue, p); }

  void Par::To(char & p) const throw(Hexception)
    { Par::ConvertTo<char>(mValue, p); }

  void Par::To(signed char & p) const throw(Hexception)
    { Par::ConvertTo<signed char>(mValue, p); }

  void Par::To(short & p) const throw(Hexception)
    { Par::ConvertTo<short>(mValue, p); }

  void Par::To(int & p) const throw(Hexception)
    { Par::ConvertTo<int>(mValue, p); }

  void Par::To(long & p) const throw(Hexception)
    { Par::ConvertTo<long>(mValue, p); }

  void Par::To(unsigned char & p) const throw(Hexception)
    { Par::ConvertTo<unsigned char>(mValue, p); }

  void Par::To(unsigned short & p) const throw(Hexception)
    { Par::ConvertTo<unsigned short>(mValue, p); }

  void Par::To(unsigned int & p) const throw(Hexception)
    { Par::ConvertTo<unsigned int>(mValue, p); }

  void Par::To(unsigned long & p) const throw(Hexception)
    { Par::ConvertTo<unsigned long>(mValue, p); }

  void Par::To(float & p) const throw(Hexception)
    { Par::ConvertTo<float>(mValue, p); }

  void Par::To(double & p) const throw(Hexception)
    { Par::ConvertTo<double>(mValue, p); }

  void Par::To(long double & p) const throw(Hexception)
    { Par::ConvertTo<long double>(mValue, p); }

  void Par::To(char *& p) const throw(std::bad_alloc, Hexception)
    { Par::ConvertTo<char *>(mValue, p); }

  void Par::To(std::string & p) const throw(std::bad_alloc, Hexception)
    { Par::ConvertTo<std::string>(mValue, p); }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Member access.
  //////////////////////////////////////////////////////////////////////////////
  const std::string & Par::Value() const throw(std::bad_alloc) {
    try { Par::ConvertTo<std::string>(mValue, mValString); }
    catch (const Hexception & x) {}
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
 * Revision 1.2  2003/05/14 15:28:00  peachey
 * Add strcasecmp for Windows.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
