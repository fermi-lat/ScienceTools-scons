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


using namespace map_tools;


//! Constructor
Parameters::Parameters( int argc, char *argv[]) 
:  hoops::ParPromptGroup(argc, argv)
{   
// Prompt for all parameters in the order in the par file:
    Prompt();
    m_chatter = (*this)["chatter"];

    m_clobber = (*this)["clobber"];

    // Read name of the file containing events data and expand any
    // environment variables.
    std::string infile = (*this)["infile"];
    m_inFile = infile;

    facilities::Util::expandEnvVar(&m_inFile);

    std::string filter = (*this)["filter"];
    m_filter = filter;

    std::string outfile = (*this)["outfile"];
    m_outFile = outfile;
    facilities::Util::expandEnvVar(&m_outFile);

    if( m_clobber ) m_outFile= "!"+m_outFile;  // FITS convention to rewrite file

}

