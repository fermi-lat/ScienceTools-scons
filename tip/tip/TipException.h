/** \file TipException.h

    \brief Exceptions thrown by table objects.

    \author James Peachey, HEASARC
*/
#ifndef tip_TipException_h
#define tip_TipException_h

#include <exception>
#include <sstream>
#include <string>

namespace tip {

  /** \class TipException

      \brief Base class for exceptions thrown by table objects.
  */
  class TipException : public std::exception {
    public:
      TipException(const std::string & msg = "Table component exception"): std::exception(), m_msg(msg) {}
      virtual ~TipException() throw() {}
      virtual const char * what() const throw() { return m_msg.c_str(); }

    protected:
      std::string m_msg;
  };

  class FitsTipException : public TipException {
    public:
      FitsTipException(int status, const std::string & msg = "FITS Table component exception"): TipException(msg),
        m_status(status) {
        std::ostringstream os;
        os << m_msg << " (CFITSIO status is " << m_status << ")";
        m_msg = os.str();
      }

      virtual ~FitsTipException() throw() {}

      int code() const { return m_status; }

    private:
      int m_status;
  };
}

#endif
