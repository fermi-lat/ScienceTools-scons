/**
 * @file EventClassifier.cxx
 * @brief Set event classes for writing to FT1 files using
 * embed_python and a Python class to do the TCut parsing and event
 * partitioning and event class number assignment.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>

#include <Python.h>

#include "tip/Table.h"

#include "facilities/commonUtilities.h"

#include "fitsGen/EventClassifier.h"
#include "fitsGen/MeritFile2.h"

namespace fitsGen {

EventClassifier::EventClassifier() 
  : m_module(0), m_classifier(0), m_meritDict(0) {
   /// Sadly, this is required.  Would be nice if commonUtilities were
   /// implemented so that clients wouldn't need to do this.
   facilities::commonUtilities::setupEnvironment();
}

EventClassifier::EventClassifier(const std::string & classifierScript) 
   : m_module(0), m_classifier(0), m_meritDict(0) {
   /// Sadly, this is required.  Would be nice if commonUtilities were
   /// implemented so that clients wouldn't need to do this.
   facilities::commonUtilities::setupEnvironment();

   m_module = new embed_python::Module("", classifierScript, 
                                       pythonPath(), false);
   m_classifier = m_module->attribute("eventClassifier");
   m_meritDict = new MeritDict(m_module);
}

EventClassifier::~EventClassifier() throw() {
   try {
      delete m_meritDict;
      if (m_classifier) {
// Omit this from the Windows builds since it seems to require the
// package to be linked against the debug version of python.
// See http://www.scipy.org/Cookbook/C_Extensions
#ifndef WIN32
         Py_DECREF(m_classifier);
#endif
      }
      delete m_module;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (...) {
   }
}

unsigned int EventClassifier::operator()(tip::ConstTableRecord & row) const {
   const_cast<MeritDict *>(m_meritDict)->setItems(row);
   return value();
}

unsigned int EventClassifier::operator()(MeritFile2 & merit) const {
   const_cast<MeritDict *>(m_meritDict)->setItems(merit);
   return value();
}

unsigned int EventClassifier::
operator()(const std::map<std::string, double> & row) const {
   const_cast<MeritDict *>(m_meritDict)->setItems(row);
   return value();
}

unsigned int EventClassifier::value() const {
   PyObject * args(Py_BuildValue("(O)", m_meritDict->pyDict()));
   PyObject * result = m_module->call(m_classifier, args);
   long ret(PyInt_AsLong(result));
#ifndef WIN32
   Py_DECREF(result);
   Py_DECREF(args);
#endif
   return static_cast<unsigned int>(ret);
}

std::string EventClassifier::pythonPath() const {
   std::string root_path(facilities::commonUtilities::getPackagePath("fitsGen"));
   if (root_path == "") {
      throw std::runtime_error("Unable to determine path to fitsGen");
   }
   return facilities::commonUtilities::joinPath(root_path, "python");
}

EventClassifier::MeritDict::MeritDict(embed_python::Module * module) 
   : m_dict(0) {
   m_dict = PyDict_New();
   module->getList("meritVariables", m_keys);
   for (size_t i = 0; i < m_keys.size(); i++) {
      setItem(m_keys.at(i), 0);
   }
}

EventClassifier::MeritDict::~MeritDict() throw() {
   if (m_dict) {
#ifndef WIN32
      Py_DECREF(m_dict);
#endif
   }
}

void EventClassifier::
MeritDict::setItem(const std::string & key, double value) {
   PyObject * py_key(PyString_FromString(const_cast<char *>(key.c_str())));
   PyObject * py_value(PyFloat_FromDouble(value));
   PyDict_SetItem(m_dict, py_key, py_value);
#ifndef WIN32
   Py_DECREF(py_key);
   Py_DECREF(py_value);
#endif
}

void EventClassifier::
MeritDict::setItems(tip::ConstTableRecord & row) {
   for (size_t i = 0; i < m_keys.size(); i++) {
      setItem(m_keys.at(i), row[m_keys.at(i)].get());
   }
}

void EventClassifier::
MeritDict::setItems(MeritFile2 & merit) {
   for (size_t i = 0; i < m_keys.size(); i++) {
      setItem(m_keys.at(i), merit[m_keys.at(i)]);
   }
}

void EventClassifier::
MeritDict::setItems(const std::map<std::string, double> & row) {
   std::map<std::string, double>::const_iterator variable(row.begin());
   for ( ; variable != row.end(); ++variable) {
      setItem(variable->first, variable->second);
   }
}

} //namespace fitsGen
