/** \file RootColumn.h
    \brief Root-specific interface to columnar data.
    \author James Peachey, HEASARC
*/
#ifndef tip_RootColumn_h
#define tip_RootColumn_h

#include <string>

#include "tip/IColumn.h"
#include "tip/tip_types.h"

namespace tip {

  class RootColumn : public IColumn {
    public:
      RootColumn(TTree * tree, const std::string & leaf_name, const std::string & leaf_type);

      virtual ~RootColumn() throw();

      virtual void get(Index_t record_index, double & dest) const;

      /** \brief Returns the name of the particular column implementation (subclass identifier).
      */
      virtual const std::string implementation() const  { return "Root"; }

    private:
      RootColumn(const RootColumn &); // Make sure nobody copies one of these.
      std::string m_leaf_name;
      TTree * m_tree;
      double m_buf;
  };

}

#endif
