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

#include "tip/Header.h"
#include "tip/Table.h"

#include "astro/SkyDir.h"

#include "dataSubselector/Gti.h"

namespace dataSubselector {

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

   class CutBase;

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

   typedef enum {CLOSED=0, MINONLY=1, MAXONLY=2} RangeType;

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
                            RangeType type=CLOSED, unsigned int indx=0);

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

   /// @brief Do a member-wise comparision of each cut in m_cuts.
   /// The ordering of cuts must be the same.
   bool operator==(const Cuts & rhs) const;

   /// @brief Write a summary of the cuts to the output stream.  This
   /// summary contains the same information as the DSS keywords.
   void writeCuts(std::ostream & stream) const;

   /// @return A reference to the i-th cut.
   const CutBase & operator[](unsigned int i) const {return *(m_cuts.at(i));}

   /**
    * @class CutBase
    * @brief Nested base class for cuts to be applied to FITS data.
    * @author J. Chiang
    */
   class CutBase {
   public:

      /// @param type "range", "GTI", or "SkyCone"
      CutBase(const std::string & type="none") : m_type(type) {}

      virtual ~CutBase() {}

      /// @brief True if the Table::Record passes this cut.
      virtual bool accept(tip::ConstTableRecord & row) const = 0;

      /// @brief True if all of the key-value pairs passes this cut.
      virtual bool accept(const std::map<std::string, double> &params) const=0;

      /// @brief Do a member-wise comparison.
      virtual bool operator==(const CutBase & rhs) const = 0;

      virtual CutBase * clone() const = 0;

      /// @brief The cut type, "range", "GTI", or "SkyCone"
      virtual const std::string & type() const {return m_type;}

      /// @brief Write this cut to the output stream as the keynum-th
      ///        DSS keyword.
      virtual void writeCut(std::ostream & stream, unsigned int keynum) const;

      /// @brief Write this cut to the tip::Header object as the keynum-th
      ///        DSS keyword.
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;

      /// @brief True if this cut supercedes the cut passed as the
      ///        argument. A default value of false may lead to
      ///        redundancy but also ensures no cuts are missed for
      ///        subclasses that do not re-implement.
      virtual bool supercedes(const CutBase &) const {return false;}

   protected:
      virtual void 
      getKeyValues(std::string & type, std::string & unit,
                   std::string & value, std::string & ref) const = 0;
   private:
      std::string m_type;
      void writeDssKeywords(tip::Header & header, unsigned int keynum,
                            const std::string & type,
                            const std::string & unit,
                            const std::string & value,
                            const std::string & ref="") const;
   };

   /**
    * @class RangeCut
    * @brief Cut on FITS binary table column values.
    * @author J. Chiang
    */
   class RangeCut : public CutBase {
   public:
      RangeCut(const std::string & colname, const std::string & unit,
               double minVal, double maxVal, RangeType type=CLOSED, 
               unsigned int indx=0);
      RangeCut(const std::string & type, const std::string & unit, 
               const std::string & value, unsigned int indx);
      virtual ~RangeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual bool accept(const std::map<std::string, double> & params) const;
      virtual bool operator==(const CutBase & rhs) const;
      virtual CutBase * clone() const {return new RangeCut(*this);}
      virtual bool supercedes(const CutBase & cut) const;

      /// @brief The column name identifier for a range cut as it
      ///        appears in the DSTYPn keyword, e.g., "ENERGY" or
      ///        "CALIB_VERSION[1]".
      const std::string & colname() const {
         return m_fullName;
      }

      /// @brief The minimum value of the accepted range.
      double minVal() const {return m_min;}

      /// @brief The maximum value of the accepted range.
      double maxVal() const {return m_max;}
   protected:
      virtual void getKeyValues(std::string & type, std::string & unit,
                                std::string & value, std::string & ref) const;
   private:
      std::string m_colname;
      std::string m_unit;
      double m_min;
      double m_max;
      RangeType m_type;
      unsigned int m_index;
      std::string m_fullName;
      bool accept(double value) const;
      double extractValue(tip::ConstTableRecord & row) const;
      void setFullName();
   };

   /**
    * @class GtiCut
    * @brief Encapsulation of GTI cuts.
    * @author J. Chiang
    */
   class GtiCut : public CutBase {
   public:
      GtiCut(const std::string & filename, const std::string & ext="GTI") 
         : CutBase("GTI"), m_gti(Gti(filename, ext)) {}
      GtiCut(const tip::Table & gtiTable) : CutBase("GTI"), m_gti(gtiTable) {}
      GtiCut(const Gti & gti) : CutBase("GTI"), m_gti(gti) {}
      virtual ~GtiCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual bool accept(const std::map<std::string, double> & params) const;
      virtual bool operator==(const CutBase & rhs) const;
      virtual CutBase * clone() const {return new GtiCut(*this);}

      /// @brief A reference to the Gti object.
      const Gti & gti() const {return m_gti;}
   protected:
      virtual void getKeyValues(std::string & type, std::string & unit,
                                std::string & value, std::string & ref) const;
   private:
      const Gti m_gti;
      bool accept(double value) const;
   };

   /**
    * @class SkyConeCut
    * @brief Acceptance cone on the sky.
    * @author J. Chiang
    */
   class SkyConeCut : public CutBase {
   public:
      SkyConeCut(double ra, double dec, double radius) 
         : CutBase("SkyCone"), m_ra(ra), m_dec(dec),
         m_coneCenter(astro::SkyDir(ra, dec)), m_radius(radius) {}
      SkyConeCut(const astro::SkyDir & dir, double radius) : 
         CutBase("SkyCone"), m_coneCenter(dir), m_radius(radius) {
         m_ra = m_coneCenter.ra();
         m_dec = m_coneCenter.dec();
      }
      SkyConeCut(const std::string & type, const std::string & unit, 
                 const std::string & value);
      virtual ~SkyConeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual bool accept(const std::map<std::string, double> & params) const;
      virtual bool operator==(const CutBase & rhs) const;
      virtual CutBase * clone() const {return new SkyConeCut(*this);}
      virtual bool supercedes(const CutBase &) const;
      
      /// @brief The RA of the cone center (J2000 degrees)
      double ra() const {return m_ra;}

      /// @brief The Dec of the cone center (J2000 degrees)
      double dec() const {return m_dec;}

      /// @brief The cone opening half-angle (degrees)
      double radius() const {return m_radius;}
   protected:
      virtual void getKeyValues(std::string & type, std::string & unit,
                                std::string & value, std::string & ref) const;
   private:
      double m_ra;
      double m_dec;
      astro::SkyDir m_coneCenter;
      double m_radius;
      bool accept(double ra, double dec) const;
      void getArgs(const std::string & value, 
                   std::vector<std::string> & args) const;
   };

private:

   std::vector<CutBase *> m_cuts;

   unsigned int Cuts::parseColname(const std::string & colname,
                                   std::string & col) const;

   bool hasCut(const CutBase * newCut) const;

   unsigned int find(const CutBase * cut) const;

   unsigned int addCut(CutBase * newCut);

};

} // namespace dataSubselector

#endif // dataSubselector_Cuts_h
