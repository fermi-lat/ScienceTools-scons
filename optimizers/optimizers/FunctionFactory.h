/**
 * @file FunctionFactory.h
 * @brief Prototype pattern for generating Function objects.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_FunctionFactory_h
#define optimizers_FunctionFactory_h

#include <string>
#include <map>

#include "optimizers/Function.h"
#include "optimizers/Exception.h"

namespace optimizers {

/**
 * @class FunctionFactory
 *
 * @brief Use the prototype pattern to supply customized Function
 * objects, the parameters of which are specified by an xml input
 * file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class FunctionFactory {

public:

   FunctionFactory();

   virtual ~FunctionFactory();

   Function *create(const std::string &name) throw(Exception);

   void addFunc(const std::string &name, Function *func,
                bool fromClone=true);

   void getFunctionNames(std::vector<std::string> &funcNames);

   void readXml(const std::string &xmlFile) throw(Exception);

   void writeXml(const std::string &outputFile);

private:

   std::map<std::string, Function *> m_prototypes;

};

} // namespace optimizers

#endif // optimizers_FunctionFactory_h
