/** \file RootExtensionUtils.cxx

    \brief Implementation of utilities to help manage Root specific table access.

    \author James Peachey, HEASARC
*/

#include <cctype>
#include <iostream>
#include <utility>

#include "TBranch.h"
#include "TFile.h"
#include "TIterator.h"
#include "TLeaf.h"
#include "TKey.h"

#include "RootExtensionManager.h"
#include "table/HeaderData.h"
#include "table/IData.h"
#include "table/TableException.h"
#include "table/TabularData.h"

namespace table {

  LeafBuffer::LeafBuffer(TTree * tree, const std::string & leaf_name, const std::string &): m_leaf_name(leaf_name),
    m_tree(tree), m_buf(0) {
    if (!m_tree) throw TableException("LeafBuffer::LeafBuffer(TTree *, string, string): "
      "Cannot create LeafBuffer object with a NULL TTree pointer");
    m_buf = new double[1];
    *static_cast<double *>(m_buf) = 137.;
    m_tree->SetBranchAddress(m_leaf_name.c_str(), m_buf);
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 1);
  }

  LeafBuffer::~LeafBuffer() {
    TBranch * branch = m_tree->GetBranch(m_leaf_name.c_str());
    if (branch) branch->ResetAddress();
    m_tree->SetBranchStatus(m_leaf_name.c_str(), 0);
    delete [] static_cast<double *>(m_buf);
  }

  // Construct without opening the file.
  RootExtensionUtils::RootExtensionUtils(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_branch_lookup(), m_leaves(), m_num_records(0), m_fp(0),
    m_tree(0), m_header(0), m_data(0) { open(); }

  // Close file automatically while destructing.
  RootExtensionUtils::~RootExtensionUtils() { delete m_data; delete m_header; close(); }

  IHeaderData * RootExtensionUtils::getHeaderData() {
    if (!m_header) m_header = new HeaderData<RootExtensionUtils>(*this);
    return m_header;
  }

  ITabularData * RootExtensionUtils::getTabularData() {
    ITabularData * retval = 0;
    if (!m_data) {
      retval =  new TabularData<RootExtensionUtils>(*this);
      m_data = retval;
    }
    return retval;
  }

  // Subclasses call this to open the file and position it to the desired extension.
  void RootExtensionUtils::open() {
    // Most of the following block of code was taken from the tuple package, by Toby Burnett.
    // tuple/src/RootTable.cxx: RootTable::RootTable(const std::string &, const std::string &, const std::string &);
    // cvs revision 1.8
    // Begin theft:
#ifdef WIN32 // needed for windows.
    gSystem->Load("libTree.dll");
#endif

    m_fp = new TFile(m_file_name.c_str());
    if( !m_fp->IsOpen()){
        delete m_fp; m_fp = 0; // JP Added.
        throw TableException(std::string("Could not open ROOT file ")+m_file_name);
    }
    if( ! m_ext_name.empty() ){
        m_tree = (TTree*) m_fp->Get(m_ext_name.c_str());
    }else{
        TIter nextTopLevelKey(m_fp->GetListOfKeys());
        TKey *key;

        // loop on keys, get the first top-level TTree 
        while  ( (key=(TKey*)nextTopLevelKey()) ) {
            TString className(key->GetClassName());
            if( className.CompareTo("TTree")==0 )  {
            // Found It
                m_tree = (TTree*)m_fp->Get(key->GetName());
                break; // JP added.
            }
        }
    }
    if( m_tree==0) {
        delete m_fp; m_fp = 0; // JP Added.
        throw TableException(std::string("Could not find tree ")+m_ext_name);
    }
    m_num_records = static_cast<Index_t>(m_tree->GetEntries());
//    std::cout << "Opened ROOT file \"" << m_file_name 
//        << "\"\n\t    tree \"" << m_tree->GetName() << "\"" << std::endl;
//    if( !filter.empty() ) {
//        std::cout << "\t  filter \""<< filter << "\" ..." << std::endl;
//    }
//    if( ! filter.empty() ){ // apply filter expression
//        TFile * dummy = new TFile("dummy.root", "recreate");
//        TTree * tnew = m_tree->CopyTree(filter.c_str() );
//        int size = tnew->GetEntries();
//        if( size == 0) {
//            throw Exception(std::string("Filter expression \"")+filter+"\" yielded no events");
//        }
//        m_tree = tnew;
//        std::cout << "\t " << size << "/" << m_num_records << " events" << std::endl;
//        m_num_records = size;
//    }
    // turn off all branches: enable them as requested for the event loop
    m_tree->SetBranchStatus("*", 0);
    // End theft.
  }

  // Close file.
  void RootExtensionUtils::close() {
    m_branch_lookup.clear();
    for (std::vector<LeafBuffer *>::reverse_iterator it = m_leaves.rbegin(); it != m_leaves.rend(); ++it)
      delete *it;
    m_leaves.clear();
    delete m_fp;
  }

  void RootExtensionUtils::openTable() {
    // Open the actual file and move to the right extension.
    if (0 == m_fp) open();

  }

  FieldIndex_t RootExtensionUtils::getFieldIndex(const std::string & field_name) const {
    // Look up the given field (branch) name:
    std::map<std::string, FieldIndex_t>::iterator itor = m_branch_lookup.find(field_name);

    // Add it if it was not found:
    if (m_branch_lookup.end() == itor) {
      // Most of the following block of code was taken from the tuple package, by Toby Burnett.
      // tuple/src/RootTable.cxx: RootTable::select(const std::string &);
      // cvs revision 1.8
      // Begin theft:
      const char * cname = field_name.c_str();
      TBranch* branch = m_tree->GetBranch(cname);
      if( 0==branch ){
          std::stringstream msg; msg << "branch \"" << field_name << "\" not found";
          throw TableException(formatWhat(msg.str()));
      }
      int n = branch->GetNleaves(); 
      if( n!=1) throw TableException(formatWhat(std::string("branch ")+field_name+" has more than one leaf"));
      TObjArray * leaves = branch->GetListOfLeaves();
      TLeaf* leaf = (TLeaf*)leaves->At(0);
      std::string type(leaf->GetTypeName());
      if( type != "Double_t") throw TableException(formatWhat(std::string("branch ")+field_name+" is "+type+" not double"));

      // TODO: allow for Float_t, Int_t, and provide for conversion
      m_tree->SetBranchStatus(cname, 1);
//      m_leafList.push_back(leaf);
//      m_tuple.push_back(0);
      // End theft.
      branch->SetAutoDelete(kFALSE);

      // Create buffer for leaf:
      // Use insert instead of push_back so that we can easily get the distance from the beginning of the array:
      std::vector<LeafBuffer *>::iterator litor = m_leaves.insert(m_leaves.end(), new LeafBuffer(m_tree, field_name, type));
      itor = m_branch_lookup.insert(itor, std::make_pair(field_name, litor - m_leaves.begin()));

    }

    return itor->second;
  }

  Index_t RootExtensionUtils::getFieldNumElements(FieldIndex_t) const {
    return 1;
  }

  std::string RootExtensionUtils::formatWhat(const std::string & msg) const {
    std::string retval = msg;
    if (!m_ext_name.empty()) retval += std::string(" in extension ") + m_ext_name;
    retval += " in file " + m_file_name;
    return retval;
  }

}
