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

#include <string>
#include <vector>

#include "tip/Header.h"
#include "tip/Table.h"

#include "astro/SkyDir.h"

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

   ~Cuts();

   bool accept(tip::ConstTableRecord & row) const;

   typedef enum {CLOSED=0, MINONLY=1, MAXONLY=2} RangeType;

   unsigned int addRangeCut(const std::string & keyword, 
                            const std::string & unit,
                            double minVal, double maxVal,
                            RangeType type=CLOSED);

   unsigned int addGtiCut(const tip::Table & gtiTable);

   unsigned int addSkyConeCut(double ra, double dec, double radius);

   void writeDssKeywords(tip::Header & header) const;

private:

   std::vector<CutBase *> m_cuts;
   
   class CutBase {
   public:
      CutBase() {}
      virtual ~CutBase() {}
      virtual bool accept(tip::ConstTableRecord & row) const = 0;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const = 0;
   protected:
      void writeDssKeywords(tip::Header & header, unsigned int keynum,
                            const std::string & type,
                            const std::string & unit,
                            const std::string & value,
                            const std::string & ref="") const;
   };

   class RangeCut : public CutBase {
   public:
      RangeCut(const std::string & keyword, const std::string & unit,
               double minVal, double maxVal, RangeType type=CLOSED) :
         m_keyword(keyword), m_unit(unit), m_min(minVal), m_max(maxVal),
         m_type(type) {}
      virtual ~RangeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;
   private:
      std::string m_keyword;
      std::string m_unit;
      double m_min;
      double m_max;
      RangeType m_type;
   };

   class GtiCut : public CutBase {
   public:
      GtiCut(const tip::Table & gtiTable) : m_table(gtiTable) {}
      virtual ~GtiCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;
   private:
      const tip::Table & m_table;
   };

   class SkyConeCut : public CutBase {
   public:
      SkyConeCut(double ra, double dec, double radius) : 
         m_coneCenter(astro::SkyDir(ra, dec)), m_radius(radius) {}
      SkyConeCut(const astro::SkyDir & dir, double radius) :
         m_coneCenter(dir), m_radius(radius) {}
      virtual ~SkyConeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header, 
                                    unsigned int keynum) const;
   private:
      astro::SkyDir m_coneCenter;
      double m_radius;
   };
};
} // namespace dataSubselector

#endif // dataSubselector_Cuts_h
