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

#include "fitsGen/EventClassifier.h"

#include "facilities/commonUtilities.h"

namespace fitsGen {

EventClassifier::EventClassifier(const std::string & classifierScript) 
   : m_module(0), m_classifier(0), m_meritDict(0) {
   m_module = new embed_python::Module("", classifierScript, 
                                       pythonPath(), false);
   m_classifier = m_module->attribute("eventClassifier");
   m_meritDict = new MeritDict(m_module);
}

EventClassifier::~EventClassifier() throw() {
   try {
      delete m_meritDict;
      Py_DECREF(m_classifier);
      delete m_module;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (...) {
   }
}

long EventClassifier::operator()(tip::ConstTableRecord & row) {
   m_meritDict->setItems(row);
   return value();
}

long EventClassifier::
operator()(const std::map<std::string, double> & row) {
   m_meritDict->setItems(row);
   return value();
}

long EventClassifier::value() const {
   PyObject * args(Py_BuildValue("(O)", m_meritDict->pyDict()));
   PyObject * result = m_module->call(m_classifier, args);
   long ret(PyInt_AsLong(result));
   Py_DECREF(result);
   Py_DECREF(args);
   return ret;
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
      Py_DECREF(m_dict);
   }
}

void EventClassifier::
MeritDict::setItem(const std::string & key, double value) {
   PyObject * py_key(PyString_FromString(const_cast<char *>(key.c_str())));
   PyObject * py_value(PyFloat_FromDouble(value));
   PyDict_SetItem(m_dict, py_key, py_value);
   Py_DECREF(py_key);
   Py_DECREF(py_value);
}

void EventClassifier::
MeritDict::setItems(tip::ConstTableRecord & row) {
   for (size_t i = 0; i < m_keys.size(); i++) {
      setItem(m_keys.at(i), row[m_keys.at(i)].get());
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
