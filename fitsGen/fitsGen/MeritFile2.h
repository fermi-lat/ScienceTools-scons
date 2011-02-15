/**
 * @file MeritFile2.h
 * @brief Interface to merit file that uses ROOT directly so that it
 * can take advantage of a TEventList when applying a filter without
 * having to generate a temporary file.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_MeritFile2_h
#define fitsGen_MeritFile2_h

#include <map>
#include <string>
#include <utility>

#include "TTree.h"

class TFile;
class TEventList;

namespace fitsGen {

class MeritFile2 {

public:
   
   MeritFile2(const std::string & meritfile,
              const std::string & tree="MeritTuple",
              const std::string & filter="");

   ~MeritFile2();

   /// Increment operator to advance to the next row. Returns the
   /// current row index after the operation.  If called when already
   /// at the last row, it returns m_nrows.
   Long64_t next();

   /// Decrement operator to move to the previous row.  Returns the
   /// current row index after the operation.  If called when already
   /// at the first row, it does nothing.
   Long64_t prev();

   /// Move to first row of filtered data. Returns 0.
   Long64_t rewind();
   
   /// Value of named column for current row.
   double operator[](const std::string & fieldname);

   /// Number of rows in filtered file.
   Long64_t nrows() const {
      return m_nrows;
   }

   /// Start time given by first event in filtered file.
   double tstart() const {
      return m_tstart;
   }

   /// Start time given by last event in filtered file.
   double tstop() const {
      return m_tstop;
   }

   /// @return Conversion type (e.g., front=0, back=1) of current row.
   short int conversionType() const;

private:

   TFile * m_file;
   TTree * m_tree;
   TEventList * m_eventList;
   Long64_t m_nrows;

   Long64_t m_index;

   typedef std::pair<void *, std::string> BranchData_t;
   typedef std::map<std::string, BranchData_t > BranchMap_t;
   BranchMap_t m_branches;

   double m_tstart;
   double m_tstop;

   void setEntry();
   
   BranchData_t get_branch_pointer(const std::string & fieldname) const;
   double recast_as_double(const BranchData_t & branch_data) const;
   void delete_branch_pointer(const BranchData_t & branch_data) const;

};

} // namespace fitsGen

#endif // fitsGen_MeritFile2_h
