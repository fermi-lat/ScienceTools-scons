/** \file Header.h

    \brief High level encapsulation of a FITS-like header.

    \author James Peachey, HEASARC
*/
#ifndef table_Header_h
#define table_Header_h

#include <map>
#include <string>

#include "table/Keyword.h"
#include "table/Header.h"
#include "table/table_types.h"

namespace table {

  class IHeaderData;

  /** \class Header

      \brief High level encapsulation of a FITS-like header.
  */
  class Header {
    public:
      /** \brief For convenience typedef the underlying keyword container.
      */
      typedef std::map<std::string, Keyword> KeywordCont_t;

      /** \brief Construct a new Header object from the given abstract header data.
          \param header_data The header data. Concrete objects will be FITS or Root-specific.
      */
      Header(IHeaderData * header_data): m_keywords(), m_header_data(header_data) {}

      /** \brief Random read/write keyword access.
          \param name The name of the keyword.
      */
      Keyword & operator [](const std::string & name) { return find_or_make(name); }

      /** \brief Random read only keyword access.
          \param name The name of the keyword.
      */
      const Keyword & operator [](const std::string & name) const { return find_or_make(name); }

    protected:
      /** \brief Internal utility to add keywords when they are looked up.
      */
      Keyword & find_or_make(const std::string & name) const;

    private:
      KeywordCont_t m_keywords;
      IHeaderData * m_header_data;
  };

  inline Keyword & Header::find_or_make(const std::string & name) const {
    // Because inquiries for keywords for a constant object can still add them to the Header, need to
    // get rid of const.
    Header & header = const_cast<Header &>(*this);

    // Look for keyword.
    KeywordCont_t::iterator itor = header.m_keywords.find(name);

    // If not found, create a new one.
    if (header.m_keywords.end() == itor)
      itor = header.m_keywords.insert(itor, std::make_pair(name, Keyword(header.m_header_data, name)));

    // Return the keyword.
    return itor->second;
  }

}

#endif
