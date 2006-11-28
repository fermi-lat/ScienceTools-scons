/** @file Module.cxx
    @brief define Module class

*/

#include "embed_python/Module.h"

#ifdef _DEBUG
#undef _DEBUG /* Link with python24.lib and not python24_d.lib */ 
#endif
#include <Python.h>

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <iterator>

#ifdef WIN32
 #include <direct.h> // for chdir
#else
# include <unistd.h>
namespace {
    int _chdir( const char * d){return ::chdir(d);}
    char * _getcwd(  char * buffer,   size_t maxlen ){return ::getcwd(buffer, maxlen);}
}
#endif
using namespace embed_python;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Module::Module(std::string path, std::string module, bool verbosity)
: m_verbose(verbosity)
, m_root("")
{
    char oldcwd[128], newcwd[128];
    _getcwd(oldcwd, sizeof(oldcwd));

    if( !path.empty() ) {
        if( _chdir(path.c_str()) !=0 ){
            throw std::runtime_error("Setup: could not find folder " +path);
        }
        // save current working directory.
        _getcwd(newcwd, sizeof(newcwd));
        if( verbose()) std::cout << "switched to folder" << newcwd << std::endl;
        m_root=newcwd;
    }
    Py_Initialize();

    m_module = PyImport_ImportModule(const_cast<char*>(module.c_str()));
    check_error("Module: error parsing module "+module);
   
    if( verbose() ) std::cout << "Read python module "
        << PyString_AsString(attribute("__file__"))<<std::endl;
#if 0
    if( !path.empty() ){
        _chdir(oldcwd);
    }
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string Module::type(const PyObject* o)const{
    return o->ob_type->tp_name;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PyObject* Module::call(PyObject* o, PyObject* args)const
{
    PyObject* ret = PyObject_CallObject(o, args);
    check_error("Module::call failed");
    return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Module::~Module()
{
    Py_DECREF(m_module);
    Py_Finalize();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PyObject* Module::attribute(std::string name)
{
    ///@todo. If compound name, "module.attribute", parse it and check for module
    PyObject* ret = PyObject_GetAttrString(m_module,const_cast<char*>(name.c_str()));
    check_error("Module: did not find attribute "+name);
    return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double Module::operator[](std::string key)
{ 
    PyObject* obj = attribute(key);
    double dval (PyFloat_AsDouble(attribute(key)));
    check_error("Module::operator[] -- "+key+" not a numeric type");
    return dval;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(std::string key, double& value)
{ 
    value = PyFloat_AsDouble(attribute(key));
    check_error("Module::getAttribute -- "+key+" not a numeric type");
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(std::string key, std::string& value)
{ 
    char * str = PyString_AsString(attribute(key));
    check_error("Module::getAttribute-- "+key+" not a str type");
    value = std::string(str);
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getList(std::string listname, std::vector<std::string>& names)
{
    PyObject* plist( attribute(listname) )
        , *iterator( PyObject_GetIter(plist) )
        , *item(0);
    if( iterator==NULL) {
        throw std::invalid_argument("Module: "+ listname +" is not a list");
    }
    if( verbose() ) std::cout << "Parsing list " << listname << std::endl;
    while (item = PyIter_Next(iterator)) {
        char * name (PyString_AsString(item) );
        check_error("Module::parse_list: "+listname + " contains non-string item");
        if( verbose()) std::cout << "\t" << name << std::endl;
        names.push_back(name);
        Py_DECREF(item);
    }

    Py_DECREF(iterator);

    check_error("Module: failure parsing"+ listname);
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::check_error(const std::string& text)const
{    if (PyErr_Occurred()) {
        PyErr_Print();
        throw std::invalid_argument(text);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Module::test(int argc, char* argv[], std::string modulename)
{
    int ret(0);
    try{

        const char * mypath =::getenv("EMBED_PYTHONROOT");

      //  Py_SetProgramName(const_cast<char*>((std::string(mypath)+"/python").c_str()));

        std::cout << "Test of Module, loading module " << modulename<< std::endl;
        Module setup("", modulename, "true");

        std::cout <<"python path: " << Py_GetPath() << std::endl;
        double x;
        std::string z;

        setup.getValue("x", x);  std::cout << "x=" << x << std::endl;
        setup.getValue("z", z);  std::cout << "z=" << z << std::endl;

        // assume the module imports math, defines pi
        std::cout << "pi=" << setup["pi"]<< std::endl;

        std::vector<std::string> names;
        setup.getList("names", names);

        std::copy(names.begin(), names.end(), std::ostream_iterator<std::string>(std::cout, ", "));
        std::cout << std::endl;

    }catch( const std::exception & e){
        std::cerr<< "Module::test -- caught exception " << e.what() << std::endl;
        ret=1;
    }
    return ret;
}    
