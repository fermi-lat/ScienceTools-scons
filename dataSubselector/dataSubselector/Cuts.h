/**
 * @file Cuts.h
 * @brief Handle data selections and DSS keywords in a portable way.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubelector_Cuts_h
#define dataSubelector_Cuts_h

#include "tip/Header.h"
#include "tip/Table.h"

#include "astro/SkyDir.h"

namespace dataSubselector {

/**
 * @class Cuts
 *
 */

class Cuts {

public: 

   Cuts(tip::Header & header);

   ~Cuts();

   bool accept(tip::ConstTableRecord & row) const;

   unsigned int addRangeCut(const std::string & keyword, 
                            double minVal, double maxVal);

   unsigned int addTableCut(const std::string & keyword,
                            tip::ConstTableRecord & table);

   unsigned int addSkyConeCut(double ra, double dec, double radius);

private:

   tip::Header & m_header;

   std::vector<CutBase *> m_cuts;
   
   class CutBase {
   public:
      CutBase() {}
      virtual ~CutBase() {}
      virtual bool accept(tip::ConstTableRecord & row) const = 0;
      virtual void writeDssKeywords(tip::Header & header) const = 0;
   };

   class RangeCut : public CutBase {
   public:
      RangeCut(const std::string & keyword, double minVal, double maxVal) :
         m_keyword(keyword), m_min(minVal), m_max(maxVal) {}
      virtual ~RangeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header) const;
   private:
      std::string m_keyword;
      double m_min;
      double m_max;
   };

   class TableCut : public CutBase {
   public:
      TableCut(const std::string & keyword, tip::ConstTableRecord & table) :
         m_keyword(keyword), m_table(table) {}
      virtual ~TableCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header) const;
   private:
      std::string m_keyword;
      tip::ConstTableRecord & m_table;
   };

   class SkyConeCut : public CutBase {
   public:
      SkyConeCut(double ra, double dec, double radius) : 
         m_coneCenter(astro::SkyDir(ra, dec)), m_radius(radius) {}
      SkyConeCut(const astro::SkyDir & dir, double radius) :
         m_coneCenter(dir), m_radius(radius) {}
      virtual ~SkyConeCut() {}
      virtual bool accept(tip::ConstTableRecord & row) const;
      virtual void writeDssKeywords(tip::Header & header) const;
   private:
      astro::SkyDir m_coneCenter;
      double m_radius;
   };
};
} // namespace dataSubselector

#endif dataSubelector_Cuts_h
