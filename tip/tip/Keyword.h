/** \file Keyword.h

    \brief Helper classes to allow convenient and idiomatic access to ITabularData objects.

    \author James Peachey, HEASARC
*/
#ifndef table_Keyword_h
#define table_Keyword_h

#include "table/IExtensionData.h"
#include "table/TableException.h"

namespace table {

  /** \class Keyword

      \brief Encapsulation of a single keyword.

      The get() method will need to be overloaded for every type the Keyword may in practice contain.
  */
  class Keyword {
    public:
      /** \brief Construct a Keyword object associated with a particular Header.
          \param extension_data Pointer to the referent IExtensionData object.
          \param name The name of this Keyword.
      */
      Keyword(IExtensionData * extension_data, const std::string & name): m_extension_data(extension_data),
        m_name(name) {}

      /** \brief Get the current value of this Keyword.
          The type of the converted value is given by the template parameter.
          \param value The current value.
      */
      template <typename T>
      void get(T & value) const;

    private:
      IExtensionData * m_extension_data;
      std::string m_name;
  };

  template <typename T>
  inline void Keyword::get(T & value) const { m_extension_data->getKeyword(m_name, value); }

}

#endif
