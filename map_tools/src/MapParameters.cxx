/** @file MapParameters.cxx
*   @brief Implementation for class that reads parameters for image description
* @author Toby Burnett 
*
* $Header$
*/

#include "map_tools/MapParameters.h"

using namespace map_tools;

//! Constructor

MapParameters::MapParameters( int argc, char *argv[])
: Parameters(argc, argv)
{

    // Read number of pixels
    m_npix = getInt("npix");
    m_npix_y= getInt("npixy", m_npix);
    m_npix_z= getInt("layers", 1);

    // Read Image size
    m_imgSizeX= getInt( "imgsize" );
    m_imgSizeY= getInt( "imgsizey", m_imgSizeX);


    // Read xref
    m_xref = getValue<double>("xref");
    m_yref = getValue<double>("yref");

    m_rot = getValue<double>("rot");

    m_projType = getValue<std::string>("projtype");
    m_use_lb = getValue<bool>("uselb", false);

    m_raName= getValue<std::string>("ra_name", "ra");
    m_decName = getValue<std::string>("dec_name", "dec");

}

