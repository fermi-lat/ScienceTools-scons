/**
* @file Parameters.cxx
* @brief Implementation for class that reads parameters needed for tools
* @author Toby Burnett
*
* $Header$
*/

#include <sstream>

#include "facilities/Util.h"
#include "table/Exception.h"

#include "map_tools/Parameters.h"

#include "pil.h"
#include <exception>

bool Parameters::s_verboseMode = false;
bool Parameters::s_clobber = 0;
int  Parameters::s_chatter = 10;


//! Constructor
Parameters::Parameters( int argc, char *argv[])
{
    int status = 0;
 //   argv[0]="count_map";
    status = PILInit(argc, argv);

    if (status != 0)  {
        std::ostringstream os;
        os << "Could not initialte PIL...check that file '" 
            << argv[0] << "' exists in the path $PFILES\nerror code:"<<  status;
        throw table::Exception(os.str());
    }
    s_chatter = getInt("chatter");

    s_clobber = getBool("clobber");

    // Read name of the file containing events data
    // if the name starts with "/", assume root. Otherwise assume it is in $INFILES
    m_eventFile = getString("eventfile");
    if( m_eventFile.substr(0,1)!="/" ){

        std::string test= "$(INFILES)/" + m_eventFile;
        try {
            facilities::Util::expandEnvVar(&test);
            m_eventFile = test;
        }catch(...){}
    }

    m_filter = getString("filter", "");

    m_oname = getString("outfile");

    if( m_oname.substr(0,1)!="/" ){

        m_oname = "$(OUTFILES)/" + m_oname;
        facilities::Util::expandEnvVar(&m_oname);
    }

    m_templateFile = getString("templatefile");

    if( s_clobber ) m_oname= "!"+m_oname;  // FITS convention to rewrite file


}

std::string Parameters::getString(const std::string& name){
    static char temp[100];
    int ret = PILGetString(name.c_str(), temp);
    if( ret ==0 ){return std::string(temp);}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw table::Exception(buf.str());
    }
}

int Parameters::getInt(const std::string &name){
    int temp;
    int ret = PILGetInt(name.c_str(), &temp);
    if( ret ==0 ){return temp;}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw table::Exception(buf.str());
    }
}
bool Parameters::getBool(const std::string &name){
    int temp;
    int ret = PILGetBool(name.c_str(), &temp);
    if( ret ==0 ){return temp!=0;}
    else{
        std::ostringstream buf;
        buf << "PIL parameter '" << name << "' not found";
        throw table::Exception(buf.str());
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
        throw table::Exception(buf.str());
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
        throw std::runtime_error(std::string(name)+" not found in double dictionary");
    }
    return (*it).second;
}

//! Method to close PIL
Parameters::~Parameters()
{
    PILClose(PIL_OK);
}
