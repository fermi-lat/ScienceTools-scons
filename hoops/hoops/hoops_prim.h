/******************************************************************************
 *   File name: hoops_prim.h                                                  *
 *                                                                            *
 * Description: Type-safe primitive wrapper class.                            *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_PRIM_H
#define HOOPS_PRIM_H

////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <iosfwd>
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  class IPrim {
    public:
      virtual ~IPrim() {}

      // Assignment operators.
      virtual IPrim & operator =(const IPrim & p) = 0;

      // "From" family of functions.
      virtual void From(const IPrim & p) = 0;
      virtual void From(const bool & p) = 0;
      virtual void From(const char & p) = 0;
      virtual void From(const signed char & p) = 0;
      virtual void From(const short & p) = 0;
      virtual void From(const int & p) = 0;
      virtual void From(const long & p) = 0;
      virtual void From(const unsigned char & p) = 0;
      virtual void From(const unsigned short & p) = 0;
      virtual void From(const unsigned int & p) = 0;
      virtual void From(const unsigned long & p) = 0;
      virtual void From(const float & p) = 0;
      virtual void From(const double & p) = 0;
      virtual void From(const long double & p) = 0;
      virtual void From(const char * p) = 0;
      virtual void From(const std::string & p) = 0;

      // "To" family of functions. These are pure in the base class
      // because the base class doesn't know what type it really
      // contains.
      virtual void To(IPrim & p) const = 0;
      virtual void To(bool & p) const = 0;
      virtual void To(char & p) const = 0;
      virtual void To(signed char & p) const = 0;
      virtual void To(int & p) const = 0;
      virtual void To(long & p) const = 0;
      virtual void To(short & p) const = 0;
      virtual void To(unsigned char & p) const = 0;
      virtual void To(unsigned short & p) const = 0;
      virtual void To(unsigned int & p) const = 0;
      virtual void To(unsigned long & p) const = 0;
      virtual void To(float & p) const = 0;
      virtual void To(double & p) const = 0;
      virtual void To(long double & p) const = 0;
      virtual void To(char *& p) const = 0;
      virtual void To(std::string & p) const = 0;

      virtual IPrim * Clone() const = 0;
      virtual std::string StringData() const throw() = 0;

      // Utility CpyStr: copy a string, allocating a new string
      // just large enough to hold the string being copied.
      static char *CpyStr(const char *s) throw () {
        char *r = 0;
        if (s) {
          char *ptr;
          const char *c_ptr = s;
          while (*c_ptr) ++c_ptr;
          r = new char[c_ptr - s + 1];
          ptr = r;
          while (0 != (*ptr = *s)) { ++s; ++ptr; }
        }
        return r;
      }

      // Utility IsBlank: determine whether a given string has
      // any non-blank characters.
      static bool IsBlank(const char *s) throw () {
        if (s) while (*s) if(!isspace(*s++)) return false;
        return true;
      }
  };

  class IPrimFactory {
    public:
      virtual ~IPrimFactory() {}

      virtual IPrim * NewIPrim(const IPrim * p) const = 0;
      virtual IPrim * NewIPrim(const bool & p) const = 0;
      virtual IPrim * NewIPrim(const char & p) const = 0;
      virtual IPrim * NewIPrim(const signed char & p) const = 0;
      virtual IPrim * NewIPrim(const short & p) const = 0;
      virtual IPrim * NewIPrim(const int & p) const = 0;
      virtual IPrim * NewIPrim(const long & p) const = 0;
      virtual IPrim * NewIPrim(const unsigned char & p) const = 0;
      virtual IPrim * NewIPrim(const unsigned short & p) const = 0;
      virtual IPrim * NewIPrim(const unsigned int & p) const = 0;
      virtual IPrim * NewIPrim(const unsigned long & p) const = 0;
      virtual IPrim * NewIPrim(const float & p) const = 0;
      virtual IPrim * NewIPrim(const double & p) const = 0;
      virtual IPrim * NewIPrim(const long double & p) const = 0;
      virtual IPrim * NewIPrim(const char * p) const = 0;
      virtual IPrim * NewIPrim(const std::string & p) const = 0;
  };

  class PrimFactory: public IPrimFactory {
    public:
      virtual ~PrimFactory() throw() {}

      virtual IPrim * NewIPrim(const IPrim * p) const;
      virtual IPrim * NewIPrim(const bool & p) const;
      virtual IPrim * NewIPrim(const char & p) const;
      virtual IPrim * NewIPrim(const signed char & p) const;
      virtual IPrim * NewIPrim(const short & p) const;
      virtual IPrim * NewIPrim(const int & p) const;
      virtual IPrim * NewIPrim(const long & p) const;
      virtual IPrim * NewIPrim(const unsigned char & p) const;
      virtual IPrim * NewIPrim(const unsigned short & p) const;
      virtual IPrim * NewIPrim(const unsigned int & p) const;
      virtual IPrim * NewIPrim(const unsigned long & p) const;
      virtual IPrim * NewIPrim(const float & p) const;
      virtual IPrim * NewIPrim(const double & p) const;
      virtual IPrim * NewIPrim(const long double & p) const;
      virtual IPrim * NewIPrim(const char * p) const;
      virtual IPrim * NewIPrim(const std::string & p) const;
  };
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable forward declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function declarations.
  //////////////////////////////////////////////////////////////////////////////

  // Stream output.
  //////////////////////////////////////////////////////////////////////////////
//  template <typename T>
//  std::ostream &operator <<(std::ostream &os, const Prim<T> &p) {
//    os << p.Data();
//    return os;
//  }
  std::ostream &operator <<(std::ostream &os, const IPrim &p);
  //////////////////////////////////////////////////////////////////////////////

}
#endif

/******************************************************************************
 * $Log$
 * Revision 1.3  2003/05/15 04:03:11  peachey
 * Include iosfwd.
 *
 * Revision 1.2  2003/05/14 15:17:07  peachey
 * Further encapsulate Prim class by hiding the templates in hoops_prim.cxx
 * and using a factory class.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
