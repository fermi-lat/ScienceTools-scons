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

   class CutBase;

public: 

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

   unsigned int size() const {return m_cuts.size();}

   bool operator==(const Cuts & rhs) const;

   void writeCuts(std::ostream & stream) const;

private:

   std::vector<CutBase *> m_cuts;

   unsigned int Cuts::parseColname(const std::string & colname,
                                   std::string & col) const;

   /**
    * @class CutBase
    * @brief Nested base class for cuts to be applied to FITS data.
    * @author J. Chiang
    */
   class CutBase {
   public:
      CutBase() {}
      virtual ~CutBase() {}
      virtual bool accept(tip::ConstTableRecord & row) const = 0;
      virtual bool accept(const std::map<std::string, double> &params) const=0;
      virtual bool operator==(const CutBase & rhs) const = 0;
      virtual CutBase * clone() const = 0;
      virtual void writeCut(std::ostream & stream, unsigned int keynum) const;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;
   protected:
      virtual void 
      getKeyValues(std::string & type, std::string & unit,
                   std::string & value, std::string & ref) const = 0;
   private:
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
               unsigned int indx=0)
         : m_colname(colname), m_unit(unit), m_min(minVal), m_max(maxVal),
           m_type(type), m_index(indx) {}
      RangeCut(const std::string & type, const std::string & unit, 
               const std::string & value, unsigned int indx);
      virtual ~RangeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual bool accept(const std::map<std::string, double> & params) const;
      virtual bool operator==(const CutBase & rhs) const;
      virtual CutBase * clone() const {return new RangeCut(*this);}
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
      bool accept(double value) const;
      double extractValue(tip::ConstTableRecord & row) const;
   };

   /**
    * @class GtiCut
    * @brief Encapsulation of GTI cuts.
    * @author J. Chiang
    */
   class GtiCut : public CutBase {
   public:
      GtiCut(const std::string & filename, const std::string & ext="GTI") 
         : m_gti(Gti(filename, ext)) {}
      GtiCut(const tip::Table & gtiTable) : m_gti(gtiTable) {}
      GtiCut(const Gti & gti) : m_gti(gti) {}
      virtual ~GtiCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual bool accept(const std::map<std::string, double> & params) const;
      virtual bool operator==(const CutBase & rhs) const;
      virtual CutBase * clone() const {return new GtiCut(*this);}
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
      SkyConeCut(double ra, double dec, double radius) : m_ra(ra), m_dec(dec),
         m_coneCenter(astro::SkyDir(ra, dec)), m_radius(radius) {}
      SkyConeCut(const astro::SkyDir & dir, double radius) :
         m_coneCenter(dir), m_radius(radius) {
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

};

} // namespace dataSubselector

#endif // dataSubselector_Cuts_h
