/**
* @file Parameters.h
* @brief Tool Input Parameter Reader base class
*
* $Header$
*/
#ifndef PARAMETERS_H
#define PARAMETERS_H 

#include <string>

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

#include <map>

class Parameters
{
public:
    // Constructors
    Parameters( int argc, char *argv[]);
    ~Parameters();


    // Accessor Methods
    const std::string &eventFile() const   { return m_eventFile; }
    const std::string &inputFile() const   { return m_eventFile; }
    const std::string &filter() const      { return m_filter; }
    const std::string &outputFile() const  { return m_oname; }
    const std::string &templateFile() const  { return m_templateFile; }
    static bool verboseMode()              { return s_verboseMode; }
    static bool clobber()                  { return s_clobber; }
    static short chatter()                 { return s_chatter; }

    // special lookup by name
    double operator[](const std::string& name)const;


protected:  // make accessible to sub class
    std::string getString(const std::string& name);
    int         getInt(const std::string& name);
    double      getDouble(const std::string& name);
    bool        getBool(const std::string& name);

    std::string getString(const std::string& name, const std::string& deflt);
    int         getInt(const std::string& name,   int deflt);
    double      getDouble(const std::string& name, double deflt);
    bool        getBool(const std::string& name, bool deflt);

    // special
    typedef std::map<std::string,double> DoubleDict;
private:
    // Data Members
    std::string   m_eventFile;
    std::string   m_oname;
    std::string   m_filter;
    std::string   m_templateFile;
    DoubleDict m_dictionary;

    static std::string s_tool;
    static bool        s_verboseMode;
    static bool        s_clobber;
    static int         s_chatter;


};

#endif
