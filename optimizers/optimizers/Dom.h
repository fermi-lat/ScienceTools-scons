/**
 * @file Dom.h
 * @brief Declaration of static routines for manipulating XML data.
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>
#include <string>

#include <xercesc/dom/DOM_Element.hpp>

#include "optimizers/Exception.h"

namespace optimizers {

class Parameter;

/**
 * @class Dom
 *
 * @brief Static routines to supplement those available through
 * xml/Dom.h
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Dom {

public:

   static void checkTag(const DOM_Element &element, 
                        const std::string &tagName, 
                        const std::string &callingRoutine="") throw(Exception);

   static bool checkTagName(const DOM_Element &element, 
                            const std::string &tagName);
   
   static void getElements(const DOM_Element &parent, 
                           const std::string &tagName,
                           std::vector<DOM_Element> &children);

};

} // namespace optimizers
