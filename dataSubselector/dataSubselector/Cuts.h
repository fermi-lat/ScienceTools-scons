#include "tip/Header.h"

namespace dataSubselector {

class Cuts {

public: 

   Cuts(tip::Header & header);

   ~Cuts();

   bool operator()(tip::ConstTableRecord & row) const;

   unsigned int addRangeCut(const std::string & keyword, 
                            double minVal, double maxVal);

   unsigned int addTableCut(const std::string & keyword,
                            const std::string & table);

   unsigned int addSkyConeCut(double ra, double dec, double radius);

private:

   tip::Header & m_header;

   std::vector<CutBase &> m_cuts;
   
   class CutBase {
   public:
      CutBase() {}
      virtual bool accept(tip::ConstTableRecord & row) const = 0;
   };

   class RangeCut : public CutBase {
   public:
      RangeCut(const std::string & keyword, double minVal, double maxVal);
   private:
      std::string m_keyword;
      double m_min;
      double m_max;
   };

   class TableCut : public CutBase {
   public:
      TableCut(const std::string & keyword, const std::string & table);
   private:
      std::string m_keyword;
      std::string m_table;
   };

   class SkyConeCut : public CutBase {
   public:
      SkyConeCut(double ra, double dec, double radius);
   private:
      double m_ra;
      double m_dec;
      double m_radius;
   };
};
