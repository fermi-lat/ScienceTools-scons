/** \file RootColumn.cxx
    \brief Implementation of Root-specific interface to columnar data.
    \author James Peachey, HEASARC
*/
#include "TLeaf.h"
#include "TTree.h"

#include "RootColumn.h"

namespace tip {

  RootColumn::RootColumn(TTree * tree, const std::string & leaf_name, const std::string &): m_leaf_name(leaf_name),
    m_tree(tree) {
    if (0 == m_tree) throw TipException("RootColumn::RootColumn(TTree *, string, string): "
      "Cannot create RootColumn object with a NULL TTree pointer");
    m_tree->SetBranchAddress(m_leaf_name.c_str(), &m_buf);
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 1);
  }

  RootColumn::~RootColumn() throw() {
    TBranch * branch = m_tree->GetBranch(m_leaf_name.c_str());
    if (0 != branch) branch->ResetAddress();
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 0);
  }

  void RootColumn::get(Index_t record_index, double & dest) const {
    m_tree->GetEntry(record_index);
    dest = m_buf;
  }

}
