/** \file Header.h

    \brief High level encapsulation of a Fits-like header.

    \author James Peachey, HEASARC
*/
#ifndef table_Header_h
#define table_Header_h

#include <map>
#include <string>

#include "table/Keyword.h"
#include "table/Header.h"
#include "table/IExtensionData.h"
#include "table/table_types.h"

namespace table {

  /** \class Header

      \brief High level encapsulation of a Fits-like header.
  */
  class Header {
    public:
      typedef std::map<std::string, Keyword> KeywordCont_t;

      Header(IExtensionData * extension_data): m_keywords(), m_extension_data(extension_data) {}

      Keyword & operator [](const std::string & name) { return find_or_make(name); }

      const Keyword & operator [](const std::string & name) const { return find_or_make(name); }

    protected:
      Keyword & find_or_make(const std::string & name) const;

    private:
      KeywordCont_t m_keywords;
      IExtensionData * m_extension_data;
  };

  inline Keyword & Header::find_or_make(const std::string & name) const {
    Header & header = const_cast<Header &>(*this);
    KeywordCont_t::iterator itor = header.m_keywords.find(name);
    if (header.m_keywords.end() == itor)
      itor = header.m_keywords.insert(itor, std::make_pair(name, Keyword(header.m_extension_data, name)));
    return itor->second;
  }

}

#endif
