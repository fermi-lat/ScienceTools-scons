/**
* @file Parameters.h
* @brief Tool Input Parameter Reader base class
*
* $Header$
*/
#ifndef MAP_TOOLS_PARAMETERS_H
#define MAP_TOOLS_PARAMETERS_H 

#include "hoopsUtil/ParametersBase.h"

#include <string>
#include <map>
namespace map_tools {
/**
* @class Parameters
* @brief Input reader base class for tools
*
* It uses PIL to read parameters from the par file.
* The description of PIL is available at
* href="http://www-glast.slac.stanford.edu/sciencetools/userInterface/doc/pil.pdf">PIL user
* manual</a>.
*
* @author Toby Burnett [originally from Sandhia Bansall]
*
* $Header$
*/

class Parameters : public hoopsUtil::ParametersBase
{
public:
    // Constructors
    Parameters( int argc, char *argv[]);
    ~Parameters(){};


    // Accessor Methods
    const std::string &eventFile() const   { return m_inFile; }
    const std::string &inputFile() const   { return m_inFile; }
    const std::string &filter() const      { return m_filter; }
    const std::string &outputFile() const  { return m_outFile; }
    bool verboseMode()  const            { return m_verboseMode; }
    bool clobber()      const            { return m_clobber; }
    short chatter()     const            { return m_chatter; }
protected:
//    std::string getString(const std::string& name){return getValue<std::string>(name);}
//    std::string getString(const std::string& name, const std::string& deflt){return getValue<std::string>(name,deflt);};
#if 1 
    double      getDouble(const std::string& name){return getValue<double>(name);}
    double      getDouble(const std::string& name,double deflt){return getValue<double>(name, deflt);}
#endif

    int         getInt(const std::string& name){return getValue<long>(name);}
    bool        getBool(const std::string& name){return getValue<bool>(name);}
    int         getInt(const std::string& name,   int deflt){return getValue<long>(name, deflt);}
    bool        getBool(const std::string& name, bool deflt){return getValue<bool>(name, deflt);}

    // special
    typedef std::map<std::string,double> DoubleDict;
private:
    // Data Members
    std::string   m_inFile;
    std::string   m_outFile;
    std::string   m_filter;
    bool        m_verboseMode;
    bool        m_clobber;
    int         m_chatter;
};
} // namespace map_tools
#endif
