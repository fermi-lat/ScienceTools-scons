/**
 * @file BitMaskCut.h
 * @brief Cuts based on a bit mask.  This is used for filtering on
 * EVENT_CLASS for Pass 7 IRFs, and also on EVENT_TYPE for Pass 8 and
 * later.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_BitMaskCut_h
#define dataSubselector_BitMaskCut_h

#include "dataSubselector/CutBase.h"

namespace dataSubselector {

/**
 * @class BitMaskCut
 *
 */

class BitMaskCut : public CutBase {

public:

   BitMaskCut(const std::string & colname,
              unsigned int mask,
              const std::string & pass_ver="");

   virtual ~BitMaskCut() {}

   virtual bool accept(tip::ConstTableRecord & row) const;

   virtual bool accept(const std::map<std::string, double> & params) const;

   virtual CutBase * clone() const {return new BitMaskCut(*this);}

   virtual bool supercedes(const CutBase & cut) const;

   virtual std::string filterString() const;

   const std::string & colname() const {
      return m_colname;
   }
   
   unsigned int mask() const {
      return m_mask;
   }

   const std::string & pass_ver() const {
      return m_pass_ver;
   }

   std::string dstype() const;

   static bool post_P7(const std::string & pass_ver);

   static void setValidityMasks(const std::string & evclassFile,
                                const std::string & evtypeFile);

   static const void * evclassValidityMasks() {
      return s_evclassPrecedence;
   }

   static const void * evtypeValidityMasks() {
      return s_evtypePrecedence;
   }

protected:

   virtual bool equals(const CutBase & rhs) const;

   virtual void getKeyValues(std::string & type, std::string & unit,
                             std::string & value, std::string & ref) const;

private:

   std::string m_colname;

   unsigned int m_mask;

   std::string m_pass_ver;

   bool m_post_P7;

   bool accept(unsigned int value) const;

   class BitMaskPrecedence {

   public:

      BitMaskPrecedence(const std::string & maskFile);
      
      /// Test if this cut has a valid mask given the current mask.
      bool validMask(const BitMaskCut & self,
                     unsigned int currentMask) const;

      const std::string & maskFile() const;

   private:

      std::string m_maskFile;

      std::map<unsigned int, unsigned int> m_validityMasks;
      

   };

   static BitMaskPrecedence * s_evclassPrecedence;

   static BitMaskPrecedence * s_evtypePrecedence;

};

} // namespace dataSubselector

#endif // dataSubselector_BitMaskCut_h
