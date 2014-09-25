//============================================================================
// Name        : EmbedPython.cpp
// Author      : Tom Stephens
// Version     :
// Copyright   : Your copyright notice
// Description : Embedded Python helper functions
//============================================================================
#include <EmbedPython.h>
#include <iostream>
#include <cstdarg>

#define error(msg) do {std::cout << msg << std::endl; exit(1); } while (1)

int EP_Init(){
	return 0;
}

PyObject * EP_LoadModule(std::string name){
	PyObject *pmod;
	Py_Initialize();
	pmod = PyImport_ImportModule(name.c_str());
	if (NULL == pmod){
		std::string msg = "Can't load module " + name;
		error(msg);
	}
	return pmod;
}

PyObject * EP_CreateObject(PyObject * module, std::string className, std::string argTypes, va_list argsList){
//    va_list argsList;
//    va_start(argsList, argTypes);
    PyObject *pclass, *pargs, *pinst;

	pclass = PyObject_GetAttrString(module, className.c_str());
	if (NULL == pclass){
		std::string msg = "Can't get class " + className + " from module ";// + module->
		error(msg);
	}
	pargs = Py_VaBuildValue(argTypes.c_str(),argsList);
	if (NULL == pargs){
		Py_DECREF(pclass);
		error("Can't build argument list");
	}

	pinst = PyObject_CallObject(pclass,pargs);
	Py_DECREF(pclass);
	Py_DECREF(pargs);
	if (NULL == pinst){
		PyErr_Print();
		std::string msg = "Error creating " + className + " object";
		error(msg);
	}
	va_end(argsList);
	return pinst;
}

PyObject * EP_CreateObject(PyObject * module, std::string className, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pinst;

    pinst =  EP_CreateObject(module,className,argTypes,argsList);
	va_end(argsList);
	return pinst;
}

PyObject * EP_CreateObject(std::string moduleName, std::string className, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pmod, *pinst;

    pmod = EP_LoadModule(moduleName);
    pinst = EP_CreateObject(pmod,className,argTypes,argsList);

	va_end(argsList);
	return pinst;
}

PyObject * EP_CallMethod(PyObject * obj, std::string funcName, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pfunc, *pargs, *pres;

	pfunc = PyObject_GetAttrString(obj, funcName.c_str());
	if (NULL == pfunc)
		error("Cant' fetch " + funcName + " method");

	pargs = Py_VaBuildValue(argTypes.c_str(), argsList);
	if (NULL == pargs){
		Py_DECREF(pfunc);
		error("Can't build arguments list");
	}

	pres = PyEval_CallObject(pfunc, pargs);
	Py_DECREF(pfunc);
	Py_DECREF(pargs);
	if(NULL == pres)
		error("Error calling " + funcName + " method");

    va_end(argsList);
	return pres;
}

PyObject * EP_CallMethod(std::string moduleName, std::string funcName, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pfunc, *pargs, *pres;

    PyObject * pmod = EP_LoadModule(moduleName);

	pfunc = PyObject_GetAttrString(pmod, funcName.c_str());
	if (NULL == pfunc)
		error("Cant' fetch " + funcName + " method");

	pargs = Py_VaBuildValue(argTypes.c_str(), argsList);
	if (NULL == pargs){
		Py_DECREF(pfunc);
		error("Can't build arguments list");
	}

	pres = PyEval_CallObject(pfunc, pargs);
	Py_DECREF(pfunc);
	Py_DECREF(pargs);
	if(NULL == pres) {
		PyErr_Print();
		error("Error calling " + funcName + " method");
	}

    va_end(argsList);
	return pres;
}

bool EP_IsType(PyObject *obj, std::string moduleName, std::string className){
	PyObject *pmod, *pclass;

	pmod = EP_LoadModule(moduleName);
	pclass = PyObject_GetAttrString(pmod, className.c_str());
	Py_DECREF(pmod);
	if (NULL == pclass)
		error("Can't get class from module");
	bool result = PyObject_IsInstance(obj,pclass);
	Py_DECREF(pclass);
	return result;
}

PyObject * EP_GetMethod(PyObject * obj, std::string funcName){
	PyObject * pfunc = PyObject_GetAttrString(obj, funcName.c_str());
	if (NULL == pfunc)
		error("Cant' fetch " + funcName + " method");
	return pfunc;
}

PyObject * EP_CallKWMethod(PyObject * obj, std::string funcName, PyObject *kwargs, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pfunc, *pargs, *pres;

	pfunc = PyObject_GetAttrString(obj, funcName.c_str());
	if (NULL == pfunc)
		error("Cant' fetch " + funcName + " method");

	pargs = Py_VaBuildValue(argTypes.c_str(), argsList);
	if (NULL == pargs){
		Py_DECREF(pfunc);
		error("Can't build arguments list");
	}

	pres = PyObject_Call(pfunc, pargs, kwargs);
	Py_DECREF(pfunc);
	Py_DECREF(pargs);
	if(NULL == pres)
		error("Error calling " + funcName + " method");

    va_end(argsList);
	return pres;
}

PyObject * EP_CallMethod(std::string moduleName, std::string funcName, PyObject *kwargs, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pfunc, *pargs, *pres;

    PyObject * pmod = EP_LoadModule(moduleName);

	pfunc = PyObject_GetAttrString(pmod, funcName.c_str());
	if (NULL == pfunc)
		error("Cant' fetch " + funcName + " method");

	pargs = Py_VaBuildValue(argTypes.c_str(), argsList);
	if (NULL == pargs){
		Py_DECREF(pfunc);
		error("Can't build arguments list");
	}

	pres = PyObject_Call(pfunc, pargs, kwargs);
	Py_DECREF(pfunc);
	Py_DECREF(pargs);
	if(NULL == pres) {
		PyErr_Print();
		error("Error calling " + funcName + " method");
	}

    va_end(argsList);
	return pres;
}

PyObject * EP_CreateKWObject(std::string moduleName, std::string className, PyObject *kwargs, std::string argTypes, ...){
    va_list argsList;
    va_start(argsList, argTypes);
    PyObject *pmod, *pclass, *pargs, *pinst;

    pmod = EP_LoadModule(moduleName);
	pclass = PyObject_GetAttrString(pmod, className.c_str());
	if (NULL == pclass){
		std::string msg = "Can't get class " + className + " from module ";// + module->
		error(msg);
	}
	Py_DECREF(pmod);

	pargs = Py_VaBuildValue(argTypes.c_str(),argsList);
	if (NULL == pargs){
		Py_DECREF(pclass);
		error("Can't build argument list");
	}

	pinst = PyObject_Call(pclass,pargs,kwargs);
	Py_DECREF(pclass);
	Py_DECREF(pargs);
	if (NULL == pinst){
		PyErr_Print();
		std::string msg = "Error creating " + className + " object";
		error(msg);
	}
	va_end(argsList);
	return pinst;
}

