/**
 * @file EventClassifier.h
 * @brief Set event classes for writing to FT1 files using
 * embed_python and a Python class to do the TCut parsing and event
 * partitioning and event class number assignment.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_EventClassifier_h
#define fitsGen_EventClassifier_h

#include <string>
#include <vector>

#include "embed_python/Module.h"

namespace tip {
   class ConstTableRecord;
}

namespace fitsGen {

class MeritFile2;

/**
 * @class EventClassifier
 * @brief Set event classes for writing to FT1 files using
 * embed_python and a Python class to do the TCut parsing and event
 * partitioning and event class number assignment.
 *
 */

class EventClassifier {

public:

   EventClassifier();

   /// @param classifierScript Module name of a Python script that 
   /// takes TCuts for defining event classes.
   EventClassifier(const std::string & classifierScript);

   virtual ~EventClassifier() throw();

   /// @return The event class id number.  This is -1 if the
   /// event does not fit into any class.
   /// @param row A row from a merit file, typically
   virtual unsigned int operator()(tip::ConstTableRecord & row) const;

   /// @return The event class id number.  This is -1 if the
   /// event does not fit into any class.
   /// @param merit MeritFile2 interface to merit data
   virtual unsigned int operator()(MeritFile2 & merit) const;

   /// @return The event class id number.  This is -1 if the
   /// event does not fit into any class.
   /// @param row A map containing the same relevant information as a
   /// merit file row.
   virtual unsigned int operator()(const std::map<std::string, 
                                   double> & row) const;

   virtual std::string passVersion() const {
      return "NONE";
   }

private:

   embed_python::Module * m_module;

   PyObject * m_classifier;

   /**
    * @class Nested class to encapsulate relevant merit variables for
    * access from event classifier script.
    */

   class MeritDict {

   public:

      MeritDict() : m_dict(0) {}

      MeritDict(embed_python::Module * module);

      ~MeritDict() throw();

      void setItem(const std::string & key,
                   double value);

      void setItems(tip::ConstTableRecord & row);

      void setItems(MeritFile2 & merit);

      void setItems(const std::map<std::string, double> & row);

      double getItem(const std::string & key) const;

      std::vector<std::string> keys() const {
         return m_keys;
      }

      PyObject * pyDict() {
         return m_dict;
      }

   private:

      std::vector<std::string> m_keys;

      PyObject * m_dict;

   } * m_meritDict;

   unsigned int value() const;

   std::string pythonPath() const;

};

} // namespace fitsGen

#endif // fitsGen_EventClassifier_h
