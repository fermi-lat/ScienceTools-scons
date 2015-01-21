/**
 * @file MeritFile2.cxx
 * @brief Interface to merit files that uses ROOT directly.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "TChain.h"
#include "TError.h"
#include "TEventList.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TObjArray.h"
#include "TSystem.h"
#include "TTree.h"

#include "fitsGen/MeritFile2.h"

namespace fitsGen {

MeritFile2::MeritFile2(const std::string & meritfile,
                       const std::string & tree,
                       const std::string & filter) :
   m_file(0), m_tree(0), m_index(0) {
/// Vain effort to suppress uninformative error messages from ROOT.
/// This is borrowed from tip::RootTable.cxx.
   long root_err_level = gErrorIgnoreLevel;
   m_file = TFile::Open(meritfile.c_str());
   gErrorIgnoreLevel = root_err_level;

   if (m_file == 0) {
      throw std::runtime_error("Failed to load " + meritfile);
   }

   m_tree = dynamic_cast<TTree *>(m_file->Get(tree.c_str()));
   if (m_tree == 0) {
      throw std::runtime_error("Failed to load tree '" 
                               + tree + "' from file " + meritfile);
   }
   
// Use TTree::Draw function to apply the filter string and get a
// TEventList.
   Long64_t first(0);
   Long64_t nmax(m_tree->GetEntries());
   m_tree->Draw(">>merit_event_list", filter.c_str(), "", nmax, first);
   m_eventList = (TEventList *)gDirectory->Get("merit_event_list");

   m_nrows = m_eventList->GetN();

   m_tstart = operator[]("EvtElapsedTime");
   setEntry(m_nrows - 1);
   m_tstop = operator[]("EvtElapsedTime");
   rewind();
}

MeritFile2::MeritFile2(const std::vector<std::string> & meritFiles,
                       const std::string & tree,
                       const std::string & filter) :
   m_file(0), m_tree(new TChain(tree.c_str())), m_index(0) {
/// Vain effort to suppress uninformative error messages from ROOT.
/// This is borrowed from tip::RootTable.cxx.
   long root_err_level = gErrorIgnoreLevel;
   gErrorIgnoreLevel = root_err_level;

   for (size_t i(0); i < meritFiles.size(); i++) {
      dynamic_cast<TChain *>(m_tree)->Add(meritFiles[i].c_str());
   }
   m_tree->SetBranchStatus("*", 1);
   
// Use TTree::Draw function to apply the filter string and get a
// TEventList.
   m_tree->Draw(">>merit_event_list", filter.c_str(), "");
   m_eventList = (TEventList *)gDirectory->Get("merit_event_list");

   m_nrows = m_eventList->GetN();

   m_tstart = operator[]("EvtElapsedTime");
   setEntry(m_nrows - 1);
   m_tstop = operator[]("EvtElapsedTime");
   rewind();
}

MeritFile2::~MeritFile2() {
   BranchMap_t::iterator it = m_branches.begin();
   for ( ; it != m_branches.end(); ++it) {
      delete_branch_pointer(it->second);
   }
   if (dynamic_cast<TChain *>(m_tree)) {
      delete m_tree;
   }
   delete m_file;
}

Long64_t MeritFile2::next() {
   if (m_index < m_nrows) {
      m_index++;
   }
   if (m_index < m_nrows) {
      setEntry();
   }
   return m_index;
}

Long64_t MeritFile2::prev() {
   if (m_index > 0) {
      m_index--;
   }
   if (m_index >= 0) {
      setEntry();
   }
   return m_index;
}

Long64_t MeritFile2::rewind() {
   m_index = 0;
   setEntry();
   return m_index;
}

void MeritFile2::setEntry() {
   setEntry(m_index);
}

void MeritFile2::setEntry(Long64_t index) {
   Long64_t entry_value = m_eventList->GetEntry(index);
   if (entry_value == -1) {
      std::cout << "Missing index error from TEventList::GetEntry "
                << "for index " << index << std::endl;
   }
   Int_t status = m_tree->GetEvent(entry_value);
   if (status == -1) {
      std::ostringstream message;
      message << "MeritFile2::setEntry: " 
              << "TTree::GetEvent == -1 for index " << index << "\n";
      throw std::runtime_error(message.str());
   }
}

double MeritFile2::operator[](const std::string & fieldname) {
   std::string branch_name(fieldname);
   std::string::size_type pos;
   int offset(0);
   if ((pos=fieldname.find_first_of("[")) != std::string::npos) {
      // Array is being accessed, so cast the substring in the square
      // brackets to get the element number desired.
      std::string my_substr(fieldname.substr(pos));
      std::string::size_type end_pos(my_substr.find_first_of("]"));
      if (end_pos == std::string::npos) {
         throw std::runtime_error("Badly formed column name for merit access: "
                                  + fieldname);
      }
      offset = std::atoi(my_substr.substr(1, end_pos).c_str());
      // Get the recognized branch name for arrays from the leaf name.
      branch_name = branchName(fieldname.substr(0, pos));
   }
   BranchMap_t::iterator it = m_branches.find(branch_name);
   if (it == m_branches.end()) {
      BranchData_t branch_data(get_branch_pointer(branch_name));
      m_tree->SetBranchAddress(branch_name.c_str(), branch_data.first);
      m_branches[branch_name] = branch_data;
      setEntry();
      return recast_as_double(branch_data, offset);
   }
   return recast_as_double(it->second, offset);
}

const std::string & MeritFile2::
branchName(const std::string & truncated_fieldname) {
   std::map<std::string, std::string>::const_iterator 
      it(m_branchNames.find(truncated_fieldname));
   if (it != m_branchNames.end()) {
      return it->second;
   }
   // Find the desired branch name from the TTree and save it.
   TObjArray * branch_list = m_tree->GetListOfBranches();
   Long64_t nbranches = branch_list->GetEntries();
   std::string branchname;
   for (Long64_t i(0); i < nbranches; i++) {
      branchname = branch_list->At(i)->GetName();
      if (branchname.substr(0, truncated_fieldname.size()) 
          == truncated_fieldname) {
         m_branchNames[truncated_fieldname] = branchname;
         break;
      }
   }
   return m_branchNames.find(truncated_fieldname)->second;
}

short int MeritFile2::conversionType() const {
   double layer(const_cast<MeritFile2 *>(this)->operator[]("Tkr1FirstLayer"));
   if (17 - layer < 11.5) {
      return 0;
   }
   return 1;
}

//BranchData_t MeritFile2::
std::pair<void *, std::string> MeritFile2::
get_branch_pointer(const std::string & fieldname) const {
   std::string leaf_name(fieldname);
   std::string::size_type pos;
   if ((pos=fieldname.find_first_of('[')) != std::string::npos) {
      leaf_name = fieldname.substr(0, pos);
   }
      
   TLeaf * leaf = m_tree->GetLeaf(leaf_name.c_str());
   if (!leaf) {
      throw std::runtime_error("leaf " + leaf_name + " not found.");
   }
   void * pointer(0);
   std::string type(leaf->GetTypeName());
   if (type == "Double_t") {
      pointer = new Double_t;
   } else if (type == "Float_t") {
      pointer = new Float_t;
   } else if (type == "Int_t") {
      pointer = new Int_t;
   } else if (type == "UInt_t") {
      pointer = new UInt_t;
   } else if (type == "Long_t") {
      pointer = new Long_t;
   } else if (type == "ULong_t") {
      pointer = new ULong_t;
   }
   return std::make_pair(pointer, type);
}

double MeritFile2::
recast_as_double(const BranchData_t & branch_data, int offset) const {
   if (branch_data.second == "Double_t") {
      return static_cast<double>(*(reinterpret_cast<Double_t *>
                                   (branch_data.first) + offset));
   } else if (branch_data.second == "Float_t") {
      return static_cast<double>(*(reinterpret_cast<Float_t *>
                                   (branch_data.first) + offset));
   } else if (branch_data.second == "Int_t") {
      return static_cast<double>(*(reinterpret_cast<Int_t *>
                                   (branch_data.first) + offset));
   } else if (branch_data.second == "UInt_t") {
      return static_cast<double>(*(reinterpret_cast<UInt_t *>
                                   (branch_data.first) + offset));
   } else if (branch_data.second == "Long_t") {
      return static_cast<double>(*(reinterpret_cast<Long_t *>
                                   (branch_data.first) + offset));
   } else if (branch_data.second == "ULong_t") {
      return static_cast<double>(*(reinterpret_cast<ULong_t *>
                                   (branch_data.first) + offset));
   }
}

void MeritFile2::delete_branch_pointer(const BranchData_t & branch_data) const {
   if (branch_data.second == "Double_t") {
      delete reinterpret_cast<Double_t *>(branch_data.first);
   } else if (branch_data.second == "Float_t") {
      delete reinterpret_cast<Float_t *>(branch_data.first);
   } else if (branch_data.second == "Int_t") {
      delete reinterpret_cast<Int_t *>(branch_data.first);
   } else if (branch_data.second == "UInt_t") {
      delete reinterpret_cast<UInt_t *>(branch_data.first);
   } else if (branch_data.second == "Long_t") {
      delete reinterpret_cast<Long_t *>(branch_data.first);
   } else if (branch_data.second == "ULong_t") {
      delete reinterpret_cast<ULong_t *>(branch_data.first);
   }
}

bool MeritFile2::resetSigHandlers() {
   if (0 == gSystem) {
      return false;
   }
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
   return true;
}

} //namespace fitsGen
