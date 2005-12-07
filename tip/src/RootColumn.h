/** \file RootColumn.h
    \brief Root-specific interface to columnar data.
    \author James Peachey, HEASARC
*/
#ifndef tip_RootColumn_h
#define tip_RootColumn_h

#include <string>
#include <vector>

#include "TLeaf.h"
#include "TTree.h"

#include "tip/IColumn.h"
#include "tip/tip_types.h"

namespace tip {

  template <typename T>
  class RootColumn : public IColumn {
    public:
      typedef std::vector<double>::size_type size_type;

      RootColumn(TTree * tree, const std::string & leaf_name, const std::string & leaf_type, size_type num_elements);

      virtual ~RootColumn() throw();

      virtual void get(Index_t record_index, double & dest) const;

      virtual void get(Index_t record_index, std::vector<double> & dest) const;

      /** \brief Returns the name of the particular column implementation (subclass identifier).
      */
      virtual const std::string implementation() const  { return "Root"; }

    private:
      RootColumn(const RootColumn &); // Make sure nobody copies one of these.
      std::string m_leaf_name;
      TTree * m_tree;
      T * m_buf;
      size_type m_num_elements;
  };

  template <typename T>
  inline RootColumn<T>::RootColumn(TTree * tree, const std::string & leaf_name, const std::string &, size_type num_elements):
    m_leaf_name(leaf_name), m_tree(tree), m_buf(0), m_num_elements(num_elements) {
    if (0 == m_tree) throw TipException("RootColumn::RootColumn(TTree *, string, string): "
      "Cannot create RootColumn object with a NULL TTree pointer");
    if (0u == m_num_elements) throw TipException("RootColumn::RootColumn(TTree *, string, string): "
      "Cannot allocate space for object with no elements");
    TLeaf * leaf = m_tree->GetLeaf(m_leaf_name.c_str());
    if (0 == leaf) throw TipException("RootColumn::RootColumn(TTree *, string, string): cannot find leaf " + m_leaf_name);
    m_buf = new T[m_num_elements];
    leaf->SetAddress(m_buf);
    m_tree->SetBranchStatus(leaf->GetBranch()->GetName(), 1);
  }

  template <typename T>
  inline RootColumn<T>::~RootColumn() throw() {
    TLeaf * leaf = m_tree->GetLeaf(m_leaf_name.c_str());
    if (0 != leaf) {
      leaf->SetAddress(0);
      m_tree->SetBranchStatus(leaf->GetBranch()->GetName(), 0);
    }
    delete [] m_buf;
  }

  template <typename T>
  inline void RootColumn<T>::get(Index_t record_index, double & dest) const {
    if (1u != m_num_elements) throw TipException("RootColumn::get(Index_t, double &): Cannot convert vector to scalar");
    m_tree->GetEntry(record_index);
    dest = *m_buf;
  }

  template <typename T>
  inline void RootColumn<T>::get(Index_t record_index, std::vector<double> & dest) const {
    if (1u >= m_num_elements) throw TipException("RootColumn::get(Index_t, double &): Cannot convert scalar to vector");
    m_tree->GetEntry(record_index);
    dest.resize(m_num_elements);
    for (size_type ii = 0; ii != m_num_elements; ++ii) dest[ii] = m_buf[ii];
  }

}

#endif
