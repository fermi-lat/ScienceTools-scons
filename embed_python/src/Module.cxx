/** @file Module.cxx
    @brief define Module class

    $Header$
*/

#include "embed_python/Module.h"

#ifdef _DEBUG
#undef _DEBUG /* Link with python24.lib and not python24_d.lib */ 
#endif
#include <Python.h>

#include <stdexcept>
#include <iostream>
#include <iomanip>
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

namespace {
    bool initialized(false);
}

using namespace embed_python;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Module::Module(const std::string& path, const std::string& module,
               const std::string & python_dir, bool verbosity)
: m_moduleName(module)
, m_verbose(verbosity)
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
    if( ! initialized ){
        initialized = true;
        Py_Initialize();
    }

    // Set a default sys.argv[0] for naive modules (e.g., numarray)
    // that expect this.
    m_module = PyImport_ImportModule("sys");
    check_error("Module: error parsing module sys");
    PyObject * sys_dict_setitem(attribute("__dict__.__setitem__"));
    PyObject * mylist(Py_BuildValue("[s]", "embed_python"));
    PyObject * args(Py_BuildValue("(sO)", "argv", mylist));
    call(sys_dict_setitem, args);
    Py_DECREF(sys_dict_setitem);
    Py_DECREF(args);
    Py_DECREF(mylist); 
    Py_DECREF(m_module);

    if (!python_dir.empty()) {
       insert_path(python_dir);
    }
    insert_path(".");

    m_module = PyImport_ImportModule(const_cast<char*>(module.c_str()));
    check_error("Module: error parsing module "+module);
   
    if( verbose() ) std::cout << "Read python module "
        << PyString_AsString(attribute("__file__"))<<std::endl;

}
Module::Module(const std::string & python_dir,
        const std::string& module, 
        int argc, char** argv)
: m_moduleName(module)
, m_verbose(false)
{

    if( ! initialized ){
        initialized = true;
        Py_Initialize();
    }

    m_module = PyImport_ImportModule("sys");
    check_error("Module: error parsing module sys");
    PyObject * sys_dict_setitem(attribute("__dict__.__setitem__"));
    PyObject * mylist(PyList_New(0)); 
    for( int i(0); i<argc; ++i){
        PyList_Append(mylist, Py_BuildValue("s", argv[i]));
    }
    PyObject * args(Py_BuildValue("(sO)", "argv", mylist));
    call(sys_dict_setitem, args);
    Py_DECREF(sys_dict_setitem);
    Py_DECREF(args);
    Py_DECREF(mylist); 
    Py_DECREF(m_module);

    if (!python_dir.empty()) {
       insert_path(python_dir);
    }
    insert_path(".");

    m_module = PyImport_ImportModule(const_cast<char*>(module.c_str()));
    check_error("Module: error parsing module "+module);   
}


void Module::insert_path(const std::string & dir) {
   m_module = PyImport_ImportModule("sys");
   PyObject * sys_path_insert(attribute("path.insert"));
   PyObject * args(Py_BuildValue("(is)", 0, dir.c_str()));
   call(sys_path_insert, args);
   Py_DECREF(args);
   Py_DECREF(sys_path_insert);
   Py_DECREF(m_module);
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
PyObject* Module::attribute(const std::string& name, bool check)const
{
    unsigned int c (name.find_first_of("."));
    if( c != std::string::npos) {
        PyObject * t = attribute(name.substr(0,c));
        PyObject * ret = PyObject_GetAttrString(t, const_cast<char*>(name.substr(c+1).c_str()));
        if ( check)  check_error("Module: did not find attribute "+name); else PyErr_Clear();
        return ret;
    }
    PyObject* ret = PyObject_GetAttrString(m_module,const_cast<char*>(name.c_str()));
    if ( check ) check_error("Module: did not find attribute "+name); else PyErr_Clear();
    return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double Module::operator[](const std::string& key)const
{ 
    PyObject* obj = attribute(key);
    double dval (PyFloat_AsDouble(obj));
    check_error("Module::operator[] -- "+key+" not a numeric type");
    return dval;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, int& value)const
{ 
    value = PyFloat_AsDouble(attribute(key));
    check_error("Module::getValue -- "+key+" not an integer type");
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, int& value, int default_value)const
{ 
    PyObject* o = attribute(key, false); 
    if( o==0){ 
        value = default_value;

    }else{
        value =PyInt_AsLong(o);
        check_error("Module::getValue -- "+key+" not an integer type"); 
    }
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, double& value)const
{ 
    value = PyFloat_AsDouble(attribute(key));
    check_error("Module::getValue -- "+key+" not a numeric type");
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, double& value, double default_value)const
{ 
    PyObject* o = attribute(key, false); 
    if( o==0){ 
        value = default_value;

    }else{
        value =PyFloat_AsDouble(o);
        check_error("Module::getValue -- "+key+" not a numeric type"); 
    }
    return;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, std::string& value)const
{ 
    char * str = PyString_AsString(attribute(key));
    check_error("Module::getValue-- "+key+" not a string type");
    value = std::string(str);
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getValue(const std::string& key, std::string& value, std::string default_value)const
{ 
    PyObject* o = attribute(key, false); 
    if( o==0){ 
        value = default_value;

    }else{
        char* str =PyString_AsString(o);
        check_error("Module::getValue -- "+key+" not a string type");
        value = std::string(str);
    }
    return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getList(const std::string& listname, std::vector<std::string>& names)const
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
void Module::getList(const std::string& listname, std::vector<double>& values)const
{
    PyObject* plist( attribute(listname) )
        , *iterator( PyObject_GetIter(plist) )
        , *item(0);
    if( iterator==NULL) {
        throw std::invalid_argument("Module: "+ listname +" is not a list");
    }
    if( verbose() ) std::cout << "Parsing list " << listname << std::endl;
    while (item = PyIter_Next(iterator)) {
        double value( PyFloat_AsDouble(item) );
        check_error("Module::parse_list: "+listname + " contains non-numeric item");
        if( verbose()) std::cout << "\t" << value << std::endl;
        values.push_back(value);
        Py_DECREF(item);
    }

    Py_DECREF(iterator);

    check_error("Module: failure parsing"+ listname);
    
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getDict(const std::string& dictname, std::map<std::string,double>& valuemap)const
{
    PyObject* pdict( attribute(dictname) )
        , *iterator( PyObject_GetIter(pdict) );

    if( iterator==NULL) {
        throw std::invalid_argument("Module: "+ dictname +" is not a dictionary");
    }
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pdict, &pos, &key, &value)) {
        double dvalue = PyFloat_AsDouble(value);
        std::string skey = PyString_AsString(key);
        valuemap[skey]=dvalue;
    }

    Py_DECREF(iterator);

    check_error("Module: failure parsing"+ dictname);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::getDict(const std::string& dictname, std::map<std::string,std::vector<double> >& valuemap)const
{
    PyObject* pdict( attribute(dictname) )
        , *dict_iterator( PyObject_GetIter(pdict) );

    if( dict_iterator==NULL) {
        throw std::invalid_argument("Module: "+ dictname +" is not a dictionary");
    }
    PyObject *key, *values;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pdict, &pos, &key, &values)) {
        std::string skey = PyString_AsString(key);
        PyObject *list_iterator( PyObject_GetIter(values) );
        if( list_iterator==NULL) {
            throw std::invalid_argument("Module: dictionary value for "+ skey +" is not a list");
        }
        std::vector<double> v;
        while (PyObject* item = PyIter_Next(list_iterator)) {
            double value( PyFloat_AsDouble(item) );
            check_error("Module::getDict: dictionary" + dictname+"["+ skey +"] contains non-numeric item");
            if( verbose()) std::cout << "\t" << value << std::endl;
            v.push_back(value);
            Py_DECREF(item);
        }
        valuemap[skey] = v;

        Py_DECREF(list_iterator);
    }

    Py_DECREF(dict_iterator);

    check_error("Module: failure parsing"+ dictname);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Module::check_error(const std::string& text)const
{    if (PyErr_Occurred()) {
        PyErr_Print();
        throw std::invalid_argument(text);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Module::test(int argc, char* argv[], const std::string& modulename)
{
    int ret(0);
    try{

        std::cout << "Test of Module, loading module " << modulename<< std::endl;
        Module setup("", modulename, argc, argv);

        std::cout <<"python path: " << Py_GetPath() << std::endl;
        double x;
        std::string z;

        setup.getValue("x", x);  std::cout << "x=" << x << std::endl;
        setup.getValue("z", z);  std::cout << "z=" << z << std::endl;

        // assume the module imports math, defines pi
        try{
            setup["pi"];  // should fail
            std::cout << "This should have failed!" << std::endl; 
            ret = 1;
        }catch(const std::exception& e){
            std::cout << "caught exception \""<< e.what() << "\" as expected" << std::endl;
        }

        setup.getValue("junk", x, 99);
            if( x!=99) {std::cerr << "failed to set default" << std::endl;
            ret=1;\
        }
        // now check compound 
        std::cout << "math.pi=" << setup["math.pi"] << std::endl;

        std::vector<std::string> names;
        setup.getList("names", names);

        std::copy(names.begin(), names.end(), std::ostream_iterator<std::string>(std::cout, ", "));
        std::cout << std::endl;

        // check list of numbers
        std::vector<double> numbers;
        setup.getList("values", numbers);
        std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<double>(std::cout, ", "));
        std::cout << std::endl;

        // Try a call--get the PyObject that is a function
        setup.call(setup.attribute("callable"));

        // finally, a local class definition with a few attributes (class variables)
        std::cout << "Local class: A.a= " << setup["A.a"] << std::endl;

        // and, much later, test a dictionary of doubles

        std::map<std::string, double> mymap;
        setup.getDict("mymap", mymap);
        std::cout << "mymap contents:\n";
        for( std::map<std::string, double>::const_iterator mip(mymap.begin()); mip!=mymap.end(); ++mip){
            std::cout << std::left<< std::setw(20) << mip->first << std::setw(20) << mip->second << std::endl;
        }
    }catch( const std::exception & e){
        std::cerr<< "Module::test -- caught exception " << e.what() << std::endl;
        ret=1;
    }
    return ret;
}    
