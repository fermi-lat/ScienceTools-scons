/**
 * @file XmlEventClassifier.h
 * @brief Wrap evtUtils code to read in xml event class definitions and
 * apply them to a merit file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_XmlEventClassifier_h
#define fitsGen_XmlEventClassifier_h

#include <map>
#include <string>
#include <utility>

class TFile;

namespace evtUtils {
   class EventClass;
}

#include "fitsGen/EventClassifier.h"

namespace fitsGen {

/**
 * @class XmlEventClassifier
 *
 */

class XmlEventClassifier : public EventClassifier {

public:

   XmlEventClassifier(const std::string & xmlFile,
                      const std::string & meritFile,
                      const std::string & filter="1",
                      const std::string & evtClassMap="FT1EventClass");

   virtual ~XmlEventClassifier() throw();

   virtual unsigned int operator()(tip::ConstTableRecord & row) const;

   virtual unsigned int 
   operator()(const std::map<std::string, double> & row) const;

   unsigned int operator()(unsigned int run,
                           unsigned int eventId) const;

   bool is_class_member(unsigned int run, 
                        unsigned int eventId, 
                        unsigned int evtclass) const;

   virtual std::string passVersion() const;

private:

   evtUtils::EventClass * m_evtClass;

   TFile * m_meritFile;

   typedef std::map<std::pair<unsigned int, unsigned int>,
                    unsigned int> EventClassMap_t;
   
   EventClassMap_t m_bitMaps;

   std::string m_passVersion;

};

} // namespace fitsGen

#endif // fitsGen_XmlEventClassifier_h
