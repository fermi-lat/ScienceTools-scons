/**
* @file Parameters.cxx
* @brief Implementation for class that reads parameters needed for tools
* @author Toby Burnett
*
* $Header$
*/

#include <sstream>

#include "facilities/Util.h"

#include "map_tools/Parameters.h"

#include "pil.h"
#include <stdexcept>
#include <cmath>

using namespace map_tools;

bool Parameters::s_verboseMode = false;
bool Parameters::s_clobber = 0;
int  Parameters::s_chatter = 10;


//! Constructor
Parameters::Parameters( int argc, char *argv[])
{
    int status = 0;
    std::string exename(argv[0]);
    int first = std::max(exename.find_last_of("/"),exename.find_last_of("\\"))+1;
    int last = exename.find_last_of(".");
    std::string short_name=exename.substr(first, last-first);
    std::strcpy(argv[0], short_name.c_str()); // assume space
    status = PILInit(argc, argv);

    if (status != 0)  {
        std::ostringstream os;
        os << "Could not initialte PIL...check that file '" 
            << argv[0] << ".par' exists in the path $PFILES\nerror code:"<<  status;
        throw std::invalid_argument(os.str());
    }
    s_chatter = getInt("chatter");

    s_clobber = getBool("clobber");

    // Read name of the file containing events data and expand any
    // environment variables.
    m_eventFile = getString("infile");
    facilities::Util::expandEnvVar(&m_eventFile);

    m_filter = getString("filter", "");

    m_oname = getString("outfile");
    facilities::Util::expandEnvVar(&m_oname);

    if( s_clobber ) m_oname= "!"+m_oname;  // FITS convention to rewrite file


}

std::string Parameters::getString(const std::string& name){
    static char temp[100];
    int ret = PILGetString(name.c_str(), temp);
    if( ret ==0 ){return std::string(temp);}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw std::invalid_argument(buf.str());
    }
}

int Parameters::getInt(const std::string &name){
    int temp;
    int ret = PILGetInt(name.c_str(), &temp);
    if( ret ==0 ){return temp;}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw std::invalid_argument(buf.str());
    }
}
bool Parameters::getBool(const std::string &name){
    int temp;
    int ret = PILGetBool(name.c_str(), &temp);
    if( ret ==0 ){return temp!=0;}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw std::invalid_argument(buf.str());
    }
}

double Parameters::getDouble(const std::string &name){
    float temp;
    int ret = PILGetReal4(name.c_str(), &temp);
    if( ret ==0 ){
        m_dictionary[name]=temp;
        return temp;
    }
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw std::invalid_argument(buf.str());
    }
}

std::string Parameters::getString(const std::string& name, const std::string& deflt)
{
    try{ return getString(name);} catch(...){ return deflt;}
}

int         Parameters::getInt(const std::string& name,   int deflt)
{
    try{ return getInt(name);} catch(...){return deflt;}
}

double      Parameters::getDouble(const std::string& name, double deflt)
{
    try{ return getDouble(name); }catch(...){
        m_dictionary[name]=deflt;
        return deflt;}
}

bool       Parameters::getBool(const std::string& name, bool deflt)
{
    try { return getBool(name);}catch(...){return deflt;}
}


double Parameters::operator[](const std::string& name)const{
    DoubleDict::const_iterator it =  m_dictionary.find(name);
    if( it == m_dictionary.end() ){
        throw std::invalid_argument(name+" not found in double dictionary");
    }
    return (*it).second;
}

//! Method to close PIL
Parameters::~Parameters()
{
    PILClose(PIL_OK);
}
