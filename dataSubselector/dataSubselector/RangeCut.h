/**
 * @file RangeCut.h
 * @brief Cuts based on a valid a range of column values.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_RangeCut_h
#define dataSubselector_RangeCut_h

#include "dataSubselector/CutBase.h"

namespace dataSubselector {

/**
 * @class RangeCut
 * @brief Cut on FITS binary table column values.
 * @author J. Chiang
 *
 * $Header$
 */

class RangeCut : public CutBase {

public:

   typedef enum {CLOSED=0, MINONLY=1, MAXONLY=2} IntervalType;

   RangeCut(const std::string & colname, const std::string & unit,
            double minVal, double maxVal, IntervalType type=CLOSED, 
            unsigned int indx=0);

   RangeCut(const std::string & type, const std::string & unit, 
            const std::string & value, unsigned int indx);

   virtual ~RangeCut() {}

   virtual bool accept(tip::ConstTableRecord & row) const;

   virtual bool accept(const std::map<std::string, double> & params) const;

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

   /// @brief The interval type.
   IntervalType intervalType() const {
      return m_intervalType;
   }

protected:

   virtual bool equals(const CutBase & rhs) const;

   virtual void getKeyValues(std::string & type, std::string & unit,
                             std::string & value, std::string & ref) const;

private:

   std::string m_colname;
   std::string m_unit;
   double m_min;
   double m_max;
   IntervalType m_intervalType;
   unsigned int m_index;
   std::string m_fullName;

   bool accept(double value) const;
   double extractValue(tip::ConstTableRecord & row) const;
   void setFullName();

};

} // namespace dataSubselector

#endif // dataSubselector_RangeCut_h
