/** \file RootColumn.h
    \brief Root-specific interface to columnar data.
    \author James Peachey, HEASARC
*/
#ifndef tip_RootColumn_h
#define tip_RootColumn_h

#include <string>

#include "TLeaf.h"
#include "TTree.h"

#include "tip/IColumn.h"
#include "tip/tip_types.h"

namespace tip {

  template <typename T>
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
      T m_buf;
  };

  template <typename T>
  inline RootColumn<T>::RootColumn(TTree * tree, const std::string & leaf_name, const std::string &): m_leaf_name(leaf_name),
    m_tree(tree) {
    if (0 == m_tree) throw TipException("RootColumn::RootColumn(TTree *, string, string): "
      "Cannot create RootColumn object with a NULL TTree pointer");
    m_tree->SetBranchAddress(m_leaf_name.c_str(), &m_buf);
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 1);
  }

  template <typename T>
  inline RootColumn<T>::~RootColumn() throw() {
    TBranch * branch = m_tree->GetBranch(m_leaf_name.c_str());
    if (0 != branch) branch->ResetAddress();
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 0);
  }

  template <typename T>
  inline void RootColumn<T>::get(Index_t record_index, double & dest) const {
    m_tree->GetEntry(record_index);
    dest = m_buf;
  }

}

#endif
