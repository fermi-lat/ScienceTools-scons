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
#include "hoops/hoops_prompt_group.h"

using  map_tools::Parameters;


//! Constructor
Parameters::Parameters( int argc, char *argv[]) 
:  m_par(*new hoops::ParPromptGroup(argc, argv))
,  m_own_ppg(true)
{  
    setup();
}
//! Constructor
Parameters::Parameters( hoops::IParGroup & par) 
:  m_par(par), m_own_ppg(false)
{  
    setup();
}

Parameters::~Parameters() throw() {
    if( m_own_ppg) delete &m_par;
}

void Parameters::setup()
{
    // Prompt for all parameters in the order in the par file:

    dynamic_cast<hoops::ParPromptGroup&>(m_par).Prompt();
    dynamic_cast<hoops::ParPromptGroup&>(m_par).Save();
    m_chatter = m_par["chatter"];

    m_clobber = m_par["clobber"];
    m_debug = m_par["debug"];

    // Read name of the file containing events data and expand any
    // environment variables.
    std::string infile = m_par["infile"];
    m_inFile = infile;

    // a filter expression is optional
    try {
        facilities::Util::expandEnvVar(&m_inFile);
        std::string filter = m_par["filter"];
        m_filter = filter;
    }catch(...){}
    
    // if no output.
    try {
        std::string outfile = m_par["outfile"];
        m_outFile = outfile;
        facilities::Util::expandEnvVar(&m_outFile);
#if 0 // not needed
        if( m_clobber ) m_outFile= "!"+m_outFile;  // FITS convention to rewrite file
#endif
    }catch(...){}


    std::string table_name = m_par["table"];
    m_table_name = table_name;


}


