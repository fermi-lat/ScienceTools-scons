/**
 * @file CutBase.h
 * @brief Base class for cuts.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dataSubselector_CutBase_h
#define dataSubselector_CutBase_h

#include <map>
#include <iostream>
#include <string>

namespace tip {
   class ConstTableRecord;
   class Header;
}

namespace dataSubselector {

/**
 * @class CutBase
 * @brief Base class for cuts that are applied to FITS data.
 * @author J. Chiang
 * 
 * $Header$
 */

class CutBase {

public:

   /// @param type "range", "GTI", or "SkyCone"
   CutBase(const std::string & type="none") : m_type(type) {}
   
   virtual ~CutBase() {}
   
   /// @brief True if the Table::Record passes this cut.
   virtual bool accept(tip::ConstTableRecord & row) const = 0;

   /// @brief True if all of the key-value pairs pass this cut.
   virtual bool accept(const std::map<std::string, double> &params) const = 0;

   virtual CutBase * clone() const = 0;

   /// @brief Do a member-wise comparison.
   /// This should not be over-ridden, so it is nonvirtual. See GOF, p. 329.
   bool operator==(const CutBase & rhs) const;

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

   /// @brief Hook method for use by operator==(...)
   virtual bool equals(const CutBase & rhs) const = 0;

   virtual void getKeyValues(std::string & type, std::string & unit,
                             std::string & value, std::string & ref) const = 0;

private:

   std::string m_type;

   void writeDssKeywords(tip::Header & header, unsigned int keynum,
                         const std::string & type,
                         const std::string & unit,
                         const std::string & value,
                         const std::string & ref="") const;

};

} // namespace dataSubselector

#endif // dataSubselector_CutBase_h
