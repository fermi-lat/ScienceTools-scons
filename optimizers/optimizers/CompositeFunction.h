/** 
 * @file CompositeFunction.h
 * @brief Declaration of CompositeFunction class
 * @author J. Chiang
 * $Header$
 */

#ifndef optimizers_CompositeFunction_h
#define optimizers_CompositeFunction_h

#include <sstream>
#include <stdexcept>

#include "optimizers/Function.h"

namespace optimizers {

/** 
 * @class CompositeFunction
 *
 * @brief Base class for Functions that are composites (sum or product)
 * of two other Functions.
 *
 * A type-checking mechanism has been implemented to ensure that only
 * Functions that operate on the same Arg subclasses are combined.
 *
 */
    
class CompositeFunction : public Function {

public:

   CompositeFunction(Function & a, Function & b);

   CompositeFunction(const CompositeFunction &);

   virtual ~CompositeFunction() {
      delete m_a;
      delete m_b;
   }

   /// setParam method to include function name checking
   virtual void setParam(const Parameter & param, const std::string & funcName);

   /// group parameter access (note name mangling for inheritance 
   /// from Function)
   virtual std::vector<double>::const_iterator setParamValues_(
      std::vector<double>::const_iterator it) {
      it = m_a->setParamValues_(it);
      it = m_b->setParamValues_(it);
      return it;
   }

   virtual std::vector<double>::const_iterator setFreeParamValues_(
      std::vector<double>::const_iterator it) {
      it = m_a->setFreeParamValues_(it);
      it = m_b->setFreeParamValues_(it);
      return it;
   }

   /// Parameter access including Function name specification
   virtual const Parameter & getParam(const std::string & paramName, 
                                      const std::string & funcName) const;

   virtual bool rescale(double factor) {
      return m_a->rescale(factor) && m_b->rescale(factor);
   }
   
protected:

   // pointers to the Functions forming the composite
   Function * m_a;

   Function * m_b;

   /// method to sync the m_parameter vector with those of the two Functions
   void syncParams();

private:

   /// disable this since Parameters may no longer have unique names
   double derivByParamImp(const Arg &, const std::string &) const {return 0;}

};

} // namespace optimizers

#endif // optimizers_CompositeFunction_h
