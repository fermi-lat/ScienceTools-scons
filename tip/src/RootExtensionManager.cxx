/** \file RootExtensionManager.cxx

    \brief Implementation of utilities to help manage Root specific table access.

    \author James Peachey, HEASARC
*/

#include <iostream>
#include <utility>

#include "TBranch.h"
#include "TError.h"
#include "TFile.h"
#include "TIterator.h"
#include "TKey.h"
#include "TLeaf.h"
#include "TString.h"
#include "TSystem.h"

#include "RootColumn.h"
#include "RootExtensionManager.h"
#include "tip/TipException.h"

namespace tip {

  void RootExtensionManager::resetSigHandlers() {
    gSystem->ResetSignal(kSigBus);
    gSystem->ResetSignal(kSigSegmentationViolation);
    gSystem->ResetSignal(kSigSystem);
    gSystem->ResetSignal(kSigPipe);
    gSystem->ResetSignal(kSigIllegalInstruction);
    gSystem->ResetSignal(kSigQuit);
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigWindowChanged);
    gSystem->ResetSignal(kSigAlarm);
    gSystem->ResetSignal(kSigChild);
    gSystem->ResetSignal(kSigUrgent);
    gSystem->ResetSignal(kSigFloatingException);
    gSystem->ResetSignal(kSigTermination);
    gSystem->ResetSignal(kSigUser1);
    gSystem->ResetSignal(kSigUser2);
  }

  bool RootExtensionManager::isValid(const std::string & file_name) {
    // Save current error chattiness level:
    long root_err_level = gErrorIgnoreLevel;

    // Tell root to ignore problems opening files instead of issuing annoying warnings.
    gErrorIgnoreLevel = 100000;

    // Try to open the file.
    TFile file(file_name.c_str(), "READ");

    // Restore Root chattiness level.
    gErrorIgnoreLevel = root_err_level;

    // Test whether file was opened successfully.
    if (!file.IsZombie()) return true;

    return false;
  }

  // Construct without opening the file.
  RootExtensionManager::RootExtensionManager(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool): m_file_name(file_name), m_ext_name(ext_name), m_filter(filter),
    m_branch_lookup(), m_leaves(), m_fields(), m_num_records(0), m_fp(0), m_tree(0) { open(); }

  // Close file automatically while destructing.
  RootExtensionManager::~RootExtensionManager() { close(); }

  // Subclasses call this to open the file and position it to the desired extension.
  void RootExtensionManager::open() {
    static bool first_time = true;
    // Most of the following block of code was taken from the tuple package, by Toby Burnett.
    // tuple/src/RootTable.cxx: RootTable::RootTable(const std::string &, const std::string &, const std::string &);
    // cvs revision 1.8
    // Begin theft:
// TODO 5: 4/2/2004: The following block are similar idiosyncrasies probably resulting from using
// incomplete Root link lines. Really this should be resolved by correcting this behavior in the
// requirements file.
    if (first_time) {
      first_time = false;
#ifdef WIN32 // needed for windows.
      gSystem->Load("libTree.dll");
// JP added:
#else
      gSystem->Load("libHist.so");
#endif
    }

    // JP added: Prevent Root warning about file from being logged:
    // Save current error chattiness level:
    long root_err_level = gErrorIgnoreLevel;

    // Set root to ignore a recoverable problem opening the file:
    gErrorIgnoreLevel = 3000;
    m_fp = new TFile(m_file_name.c_str());

    // Restore root chattiness level:
    gErrorIgnoreLevel = root_err_level;

    if( !m_fp->IsOpen()){
        delete m_fp; m_fp = 0; // JP Added.
        throw TipException(std::string("Could not open ROOT file ")+m_file_name);
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
        throw TipException(std::string("Could not find tree ")+m_ext_name);
    }
    m_num_records = static_cast<Index_t>(m_tree->GetEntries());
//    std::cout << "Opened ROOT file \"" << m_file_name
//        << "\"\n\t    tree \"" << m_tree->GetName() << "\"" << std::endl;
//    if( !filter.empty() ) {
//        std::cout << "\t  filter \""<< filter << "\" ..." << std::endl;
//    }
    if( ! m_filter.empty() ){ // apply filter expression
        TFile * dummy = new TFile("dummy.root", "recreate");
        // JP added this check:
        if (!dummy->IsOpen()) {
          delete dummy;
          throw TipException("Could not create temporary filtered ROOT file");
        }
        TTree * tnew = m_tree->CopyTree(m_filter.c_str() );
        Index_t size = Index_t(tnew->GetEntries());
        if( size == 0) {
            throw TipException(std::string("Filter expression \"")+m_filter+"\" yielded no events");
        }
        m_tree = tnew;
//        std::cout << "\t " << size << "/" << m_num_records << " events" << std::endl;
        m_num_records = size;
        delete m_fp;
        m_fp = dummy;
    }
    // turn off all branches: enable them as requested for the event loop
    m_tree->SetBranchStatus("*", 0);
    // End theft.

    // Get names of all leaves in this tree:
    TIter nextLeaf(m_tree->GetListOfBranches());
    TKey * leafKey;
    while (0 != (leafKey = (TKey *) nextLeaf())) {
      m_fields.push_back(leafKey->GetName());
    }
  }

  // Close file.
  void RootExtensionManager::close() {
    m_branch_lookup.clear();
    for (std::vector<IColumn *>::reverse_iterator it = m_leaves.rbegin(); it != m_leaves.rend(); ++it)
      delete *it;
    m_fields.clear();
    m_leaves.clear();
    delete m_fp;
  }

  void RootExtensionManager::openTable() {
  }

  void RootExtensionManager::setNumRecords(Index_t) {
    // Not supported for now.
    throw TipException("Changing the size of a Root table is not currently supported.");
  }

  const IExtensionData::FieldCont & RootExtensionManager::getValidFields() const {
    return m_fields;
  }

  FieldIndex_t RootExtensionManager::getFieldIndex(const std::string & field_name) const {
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
          throw TipException(formatWhat(msg.str()));
      }
      int n = branch->GetNleaves();
      if( n!=1) throw TipException(formatWhat(std::string("branch ")+field_name+" has more than one leaf"));
      TObjArray * leaves = branch->GetListOfLeaves();
      TLeaf* leaf = (TLeaf*)leaves->At(0);
      std::string type(leaf->GetTypeName());
      if( type != "Double_t") throw TipException(formatWhat(std::string("branch ")+field_name+" is "+type+" not double"));

      // TODO: allow for Float_t, Int_t, and provide for conversion
      m_tree->SetBranchStatus(cname, 1);
//      m_leafList.push_back(leaf);
//      m_tuple.push_back(0);
      // End theft.
      branch->SetAutoDelete(kFALSE);

      // Create buffer for leaf:
      // Use insert instead of push_back so that we can easily get the distance from the beginning of the array:
      std::vector<IColumn *>::iterator litor = m_leaves.insert(m_leaves.end(), new RootColumn(m_tree, field_name, type));
      itor = m_branch_lookup.insert(itor, std::make_pair(field_name, litor - m_leaves.begin()));

    }

    return itor->second;
  }

  void RootExtensionManager::copyCell(const IExtensionData *, FieldIndex_t, Index_t, FieldIndex_t, Index_t) {
    throw TipException("Copying cells to a Root table is not supported");
  }

  void RootExtensionManager::copyRecord(const IExtensionData *, Index_t, Index_t) {
    throw TipException("Copying records to a Root table is not supported");
  }

  // Append field to a table extension.
  void RootExtensionManager::appendField(const std::string &, const std::string &) {
    throw TipException("Adding fields to a Root table is not supported");
  }

  const std::vector<PixOrd_t> & RootExtensionManager::getImageDimensions() const {
    throw TipException("Images in Root files not supported");
  }

  void RootExtensionManager::setImageDimensions(const std::vector<PixOrd_t> & ) {
    throw TipException("Images in Root files not supported");
  }

  void RootExtensionManager::getPixel(const std::vector<PixOrd_t> &, double &) const {
    throw TipException("Images in Root files not supported");
  }

  void RootExtensionManager::setPixel(const std::vector<PixOrd_t> &, const double &) {
    throw TipException("Images in Root files not supported");
  }

  void RootExtensionManager::getImage(std::vector<float> &) const {
    throw TipException("Images in Root files not supported");
  }

  void RootExtensionManager::setImage(const std::vector<float> &) {
    throw TipException("Images in Root files not supported");
  }

  std::string RootExtensionManager::formatWhat(const std::string & msg) const {
    std::string retval = msg;
    if (!m_ext_name.empty()) retval += std::string(" in extension ") + m_ext_name;
    retval += " in file " + m_file_name;
    return retval;
  }

}
