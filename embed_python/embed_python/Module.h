/** @file Module.h
    @brief declare Module class

    $Header$
*/
#ifndef embed_python_Module_h
#define embed_python_Module_h

#include <map>
#include <vector>
#include <string>

// this needed to avoid include of Python.h, since PyObject is a struct
#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif
namespace embed_python {
/** @class Module
    @brief mangage a Python module, for convenient program initialization

    @author T.Burnett

*/
class Module {
public:
    /// ctor 
    /// @param path path to module. If empty, assume cwd
    /// @param module name of module to load, without ".py".  Must be on pythonpath
    /// @param verbose [false] useful output if debugging?
    Module(std::string path, std::string module, bool verbose=false);
    ~Module();

    /// return a numeric type
    double operator[](std::string key);

    /// set a numeric value
    void getValue(std::string attribute, double & value);

    /// set a string value
    void getValue(std::string attribute, std::string& value);

    /// set a list of strings
    void getList(std::string listname, std::vector<std::string>& names);

    /// examine type of a PyObject
    std::string type(const PyObject* obj)const;

    /// call a callable object
    PyObject* call(PyObject* o, PyObject* args=0 )const;

    /// return the attribute -- exception if does not exist
    PyObject * attribute(std::string name);

    /// access root, if path was defined
    std::string root()const{return m_root;}

    /// test program, to be called directly from a main.
    static int test(int argc, char* argv[], std::string modulename);

private:

    PyObject * m_module; ///< the module we manage

    bool m_verbose;
    bool verbose()const{return m_verbose;}
    void check_error(const std::string& text)const;
    std::string m_root; ///< python path
};

}// namespace

#endif
