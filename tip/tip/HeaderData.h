/** \file HeaderData.h

    \brief Templated implementation of the IHeaderData interface which forwards all actions to
    a referent object of the type given by the template parameter. The allows the virtual interface
    to take advantage of templated code in the referent object.

    \author James Peachey, HEASARC
*/
#ifndef tip_HeaderData_h
#define tip_HeaderData_h

#include <string>

#include "tip/IHeaderData.h"

namespace tip {

  /** \class HeaderData

      \brief Templated implementation of the IHeaderData interface which forwards all actions to
      a referent object of the type given by the template parameter. The allows the virtual interface
      to take advantage of templated code in the referent object.
  */
  template <typename Implementor>
  class HeaderData : public IHeaderData {
    public:
      /** \brief Create a HeaderData object which refers to the given utility object.
          \param implementor Pointer to the object which does the real work.
      */
      HeaderData(Implementor & implementor): m_implementor(implementor) {}

      virtual ~HeaderData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, double & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, float & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, char & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed char & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed short & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed int & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed long & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned char & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned short & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned int & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned long & value) const
        { m_implementor.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, std::string & value) const
        { m_implementor.getKeywordGeneric(name, value); }

      /** \brief Set a keyword in this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const double & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const float & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed char & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed short & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed int & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed long & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned char & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned short & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned int & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned long & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const std::string & value)
        { m_implementor.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char * const & value)
        { m_implementor.setKeywordGeneric(name, value); }

    private:
      Implementor & m_implementor;
  };

}

#endif
