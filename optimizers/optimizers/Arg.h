/** 
 * @file Arg.h
 * @brief Declaration of Arg class
 * @author J. Chiang
 * $Header$
 */

#ifndef optimizers_Arg_h
#define optimizers_Arg_h

namespace optimizers {

/** 
 * @class Arg
 *
 * @brief An abstract class that encapsulates argument type
 * information so that Function's value() and Parameter passing
 * methods can be overloaded transparently.
 *
 * @authors J. Chiang
 *    
 * $Header$
 */

class Arg {
    
public:

   Arg() {}
   
   virtual ~Arg() {}

protected:

//   Arg() {}

};

} // namespace optimizers

#endif // optimizers_Arg_h
