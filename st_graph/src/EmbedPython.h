#ifndef EMBED_PYTHON_H
#define EMBED_PYTHON_H

#include <Python.h>
#include <string>

int EP_Init();
PyObject * EP_LoadModule(std::string name);
PyObject * EP_CreateObject(PyObject * module, std::string className, std::string argTypes, ...);
PyObject * EP_CreateObject(std::string moduleName, std::string className, std::string argTypes, ...);
PyObject * EP_CallMethod(PyObject * obj, std::string funcName, std::string argTypes, ...);
PyObject * EP_CallMethod(std::string moduleName, std::string funcName, std::string argTypes, ...);
PyObject * EP_GetMethod(PyObject * obj, std::string funcName);
bool EP_IsType(PyObject *obj, std::string moduleName, std::string className);
PyObject * EP_CallKWMethod(PyObject * obj, std::string funcName, PyObject *kwargs, std::string argTypes, ...);
PyObject * EP_CallKWMethod(std::string moduleName, std::string funcName, PyObject *kwargs, std::string argTypes, ...);
PyObject * EP_CreateKWObject(std::string moduleName, std::string className, PyObject* kwargs, std::string argTypes, ...);

#endif //EMBED_PYTHON_H
