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
 * @brief Handle data selections and DSS keywords for packages outside of
 * dataSubselector.
 * @author J. Chiang
 *
 * $Header$
 */

class Cuts {

public: 

   class CutBase;

   Cuts() {}

   Cuts(const std::string & eventFile, const std::string & extension="EVENTS");

   Cuts(const Cuts & rhs);

   ~Cuts();

   bool accept(tip::ConstTableRecord & row) const;

   bool accept(const std::map<std::string, double> & params) const;

   typedef enum {CLOSED=0, MINONLY=1, MAXONLY=2} RangeType;

   unsigned int addRangeCut(const std::string & colname, 
                            const std::string & unit,
                            double minVal, double maxVal,
                            RangeType type=CLOSED, unsigned int indx=0);

   unsigned int addGtiCut(const tip::Table & gtiTable);

   unsigned int addSkyConeCut(double ra, double dec, double radius);

   void writeDssKeywords(tip::Header & header) const;

   void Cuts::writeGtiExtension(const std::string & filename) const;

   unsigned int size() const {return m_cuts.size();}

   bool operator==(const Cuts & rhs) const;

   void writeCuts(std::ostream & stream) const;

   const CutBase & operator[](unsigned int i) const {return *(m_cuts.at(i));}

   /**
    * @class CutBase
    * @brief Nested base class for cuts to be applied to FITS data.
    * @author J. Chiang
    */
   class CutBase {
   public:
      CutBase(const std::string & type="none") : m_type(type) {}
      virtual ~CutBase() {}
      virtual bool accept(tip::ConstTableRecord & row) const = 0;
      virtual bool accept(const std::map<std::string, double> &params) const=0;
      virtual bool operator==(const CutBase & rhs) const = 0;
      virtual CutBase * clone() const = 0;
      virtual const std::string & type() const {return m_type;}
      virtual void writeCut(std::ostream & stream, unsigned int keynum) const;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;
      /// A default value of false may lead to redundancy but also
      /// ensures no cuts are missed for subclasses that do not
      /// re-implement.
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

      const std::string & colname() const {
         return m_fullName;
      }
      double minVal() const {return m_min;}
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

      double ra() const {return m_ra;}
      double dec() const {return m_dec;}
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

   bool hasCut(CutBase * newCut) const;

   unsigned int find(CutBase * cut) const;

};

} // namespace dataSubselector

#endif // dataSubselector_Cuts_h
