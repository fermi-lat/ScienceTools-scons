/**
 * @file Cuts.h
 * @brief Handle data selections and DSS keywords for packages outside of
 * dataSubselector.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_Cuts_h
#define dataSubselector_Cuts_h

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "dataSubselector/RangeCut.h"

namespace tip {
   class ConstTableRecord;
   class Header;
   class Table;
}

namespace dataSubselector {

class Gti;

/**
 * @class Cuts
 * @brief Handle data selections and Data Subspace (DSS) keywords for
 * packages outside of dataSubselector.
 * @author J. Chiang
 *
 * $Header$
 */

class Cuts {

public: 

   Cuts() {}

   /// @brief This constructor reads the data selections from the event 
   ///        extension header.
   /// @param eventFile FITS file name.
   /// @param extension Name of the HDU containing the cuts.
   /// @param check_columns DSS range cuts may be specified to 
   ///        have a functional dependence on other column values,
   ///        e.g., to specify an energy dependent zenith angle cut,
   ///        the DSTYPn keyword value would be
   ///        "ZENITH_ANGLE(ENERGY)".  Currently, these classes only
   ///        support simple range cuts that do not have these
   ///        functional dependences.  If check_columns is true, an
   ///        exception will be thrown if a non-bare column name is
   ///        encountered as the DSS type.  If false, the range cut
   ///        will be stored, but if applied to a tip::TableRecord,
   ///        tip will throw an exception.
   Cuts(const std::string & eventFile, const std::string & extension="EVENTS",
        bool check_columns=true);

   /// A copy constructor is needed since there are pointer data members.
   Cuts(const Cuts & rhs);

   /// Deletes the CutBase pointers.
   ~Cuts();

   /// @brief True if the data in the row passes all of the cuts.
   /// @param row A row of FITS binary table.
   bool accept(tip::ConstTableRecord & row) const;

   /// @brief True if the data in the map passes all of the cuts.
   /// @param params A map of keyword-value pairs for use in contexts
   ///        where filtering is desired but the data do not exist in
   ///        FITS format.
   bool accept(const std::map<std::string, double> & params) const;

   /// @brief This method will add a cut if it is not equal to
   ///        and if it is not superceded by an existing cut.
   ///        If the added cut supercedes an existing cut, that cut
   ///        will be replaced.
   /// @return The current number of cuts stored.
   /// @param colname The column name of the value to be selected on.
   /// @param unit The units of this quantity, e.g., 's', 'deg', 'MeV'
   /// @param minVal The minimum value to be accepted by this cut.
   /// @param maxVal The maximum value to be accepted by this cut.
   /// @param type Specifies whether the cut range is open (bounded 
   ///        from below or above) or closed.
   /// @param indx If the column is a vector, indx is the index of the
   ///        array to be cut on.  FITS follows the FORTRAN convention
   ///        that the first element is indx=1.
   unsigned int addRangeCut(const std::string & colname, 
                            const std::string & unit,
                            double minVal, double maxVal,
                            RangeCut::IntervalType type=RangeCut::CLOSED,
                            unsigned int indx=0);

   /// @brief Add a GTI cut.  Here an existing GTI extension is 
   ///        read from a FITS file as a tip::Table.
   /// @return The current number of cuts stored.
   /// @param gtiTable The GTI extension read using tip.
   unsigned int addGtiCut(const tip::Table & gtiTable);

   /// @brief Add a GTI cut using a Gti object.
   /// @return The current number of cuts stored.
   /// @param gti A dataSubselector::Gti object (NB: This class derives
   ///        from evtbin::Gti).
   unsigned int addGtiCut(const Gti & gti);

   /// @brief An acceptance cone on the sky.
   /// @return The current number of cuts stored.
   /// @param ra Right Ascension of cone center in (J2000 degrees).
   /// @param dec Declination of cone center (J2000 degrees).
   /// @param radius Cone half-opening angle (degrees)
   unsigned int addSkyConeCut(double ra, double dec, double radius);

   /// @brief Add the DSS keywords to a write-enabled tip::Header.
   /// @param header The reference returned from the tip::Table::getHeader
   ///        method where the table has been opened using the
   ///        tip::IFileSvc::editTable method.
   void writeDssKeywords(tip::Header & header) const;

   /// @brief Add the first GTI extension in the m_cuts vector to
   ///        the specified FITS file.  It is tacitly assumed that
   ///        there is only one GTI extension.  If a GTI extension
   ///        already exists in the FITS file, then nothing is done.
   /// @param filename The FITS file name.
   /// @todo This method should check if the existing extension is
   /// compatible with the one to be written. If it is, then the old
   /// one is should be replaced.  If it is not, then a runtime
   /// exception should be thrown.
   void writeGtiExtension(const std::string & filename) const;

   /// @brief The number of cuts contained herein.
   unsigned int size() const {return m_cuts.size();}

   /// @brief Do a member-wise comparison of each cut in m_cuts.
   /// The ordering of cuts must be the same.
   bool operator==(const Cuts & rhs) const;

   /// @brief Write a summary of the cuts to the output stream.  This
   /// summary contains the same information as the DSS keywords.
   void writeCuts(std::ostream & stream) const;

   /// @return A reference to the i-th cut.
   const CutBase & operator[](unsigned int i) const {return *(m_cuts.at(i));}

   std::string filterString() const;

private:

   std::vector<CutBase *> m_cuts;

   unsigned int Cuts::parseColname(const std::string & colname,
                                   std::string & col) const;

   bool hasCut(const CutBase * newCut) const;

   unsigned int find(const CutBase * cut) const;

   /// @brief Add a cut. The passed cut will not be added if an 
   ///        existing cut supercedes it, but it will be deleted.  If
   ///        added, this cut will be deleted by the destructor ~Cut().
   unsigned int addCut(CutBase * newCut);

};

} // namespace dataSubselector

#endif // dataSubselector_Cuts_h
