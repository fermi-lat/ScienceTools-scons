/** \file RootExtensionUtils.cxx

    \brief Implementation of utilities to help manage Root specific table access.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "TFile.h"
#include "TKey.h"
#include "TTree.h"

#include "RootExtensionUtils.h"
#include "RootHeaderData.h"
#include "RootTabularData.h"
#include "table/IData.h"
#include "table/TableException.h"

namespace table {

  // Construct without opening the file.
  RootExtensionUtils::RootExtensionUtils(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_col_name_lookup(), m_col_num_lookup(), m_num_records(0), m_fp(0) {}

  // Close file automatically while destructing.
  RootExtensionUtils::~RootExtensionUtils() { close(); }

  IHeaderData * RootExtensionUtils::createHeader() { return new RootHeaderData(this); }

  IData * RootExtensionUtils::createData() { return new RootTabularData(this); }

  // Subclasses call this to open the file and position it to the desired extension.
  void RootExtensionUtils::open() {
    // Most of this code was taken from the tuple package, by Toby Burnett.
    // tuple/src/RootTable.cxx: RootTable::RootTable(const std::string &, const std::string &, const std::string &);
    // cvs revision 1.8
#ifdef WIN32 // needed for windows.
    gSystem->Load("libTree.dll");
#endif

    m_fp = new TFile(m_file_name.c_str());
    if( !m_fp->IsOpen()){
        delete m_fp; m_fp = 0;
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
            }
        }
    }
    if( m_tree==0) {
        delete m_fp; m_fp = 0;
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

  }

  // Close file.
  void RootExtensionUtils::close() {
    delete m_fp;
  }

  void RootExtensionUtils::openTable() {
    // Open the actual file and move to the right extension.
    if (0 == m_fp) open();

#ifdef FITS
    int column_status = 0;
    int status = 0;
    long nrows = 0;

    // Read the number of rows present in the table.
    fits_get_num_rows(m_fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows < 0) {
      close();
      throw TableException();
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    char * match_all = "*";
    char name[128]; // jp fix this: what is the maximum length of a Root column name?
    int col_num;
    long repeat;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      *name = '\0';
      col_num = 0;
      repeat = 0;
      // Get each column's name.
      fits_get_colname(m_fp, CASEINSEN, match_all, name, &col_num, &column_status);
      if (0 == column_status || COL_NOT_UNIQUE == column_status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);

        // Also get its repeat count.
        fits_get_coltype(m_fp, col_num, 0, &repeat, 0, &status);
        if (0 != status) {
          close();
          throw TableException();
        }

        // Save values iff successful getting all the information.
        m_col_name_lookup[name].m_name = name;
        m_col_name_lookup[name].m_col_num = col_num;
        m_col_name_lookup[name].m_repeat = repeat;
        m_col_num_lookup[col_num].m_name = name;
        m_col_num_lookup[col_num].m_col_num = col_num;
        m_col_num_lookup[col_num].m_repeat = repeat;
      }
    }
#endif
  }

  FieldIndex_t RootExtensionUtils::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string tmp = field_name;
    for (std::string::iterator itor = tmp.begin(); itor != tmp.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, ColumnInfo>::const_iterator itor = m_col_name_lookup.find(tmp);
    if (itor == m_col_name_lookup.end())
      throw TableException(formatWhat(std::string("Could not get field index for field ") + field_name));

    // Get the number of the column.
    return itor->second.m_col_num;
  }

  Index_t RootExtensionUtils::getFieldNumElements(FieldIndex_t field_index) const {
    // Find field_index in container of columns. Complain if not found.
    std::map<FieldIndex_t, ColumnInfo>::const_iterator itor = m_col_num_lookup.find(field_index);
    if (itor == m_col_num_lookup.end()) {
      std::ostringstream s;
      s << "Could not get number of elements in field number " << field_index;
      throw TableException(formatWhat(s.str()));
    }
    return itor->second.m_repeat;
  }

  std::string RootExtensionUtils::formatWhat(const std::string & msg) const {
    std::string retval = msg;
    if (!m_ext_name.empty()) retval += std::string(" in extension ") + m_ext_name;
    retval += " in file " + m_file_name;
    return retval;
  }

}
