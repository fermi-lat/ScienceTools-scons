/**
 * @file XmlEventClassifier.h
 * @brief Wrap EvtUtils code to read in xml event class definitions and
 * apply them to a merit file.
 *
 * $Header$
 */

#ifndef fitsGen_XmlEventClassifier_h
#define fitsGen_XmlEventClassifier_h

#include <map>
#include <string>
#include <utility>

class TFile;

namespace EvtUtils {
   class EventClass;
}

namespace fitsGen {

class XmlEventClassifier {

public:

   XmlEventClassifier(const std::string & xmlFile,
                      const std::string & meritFile);

   unsigned int operator()(unsigned int run,
                           unsigned int eventId) const;
private:

   EvtUtils::EventClass * m_evtClass;
   TFile * m_meritFile;

   typedef std::map<std::pair<unsigned int, unsigned int>,
                    unsigned int> EventClassMap_t;
   
   EventClassMap_t m_eventClasses;
};

} // namespace fitsGen

#endif // fitsGen_XmlEventClassifier_h
