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

    // Read number of pixels along x
    m_npix = getValue<long>("npix");

    // y direction defaults to same as x (square)
    m_npix_y= getValue<long>("npixy", m_npix);

    // z direction (layers) defaults to 1.
    m_npix_z= getValue<long>("layers", 1);

    // Read image size (presumably degrees--don't know why it is integer)
    m_imgSizeX= getValue<long>( "imgsize" );
    // if y not specified, assume square
    m_imgSizeY= getValue<long>( "imgsizey", m_imgSizeX);

    // Read xref, yref
    m_xref = getValue<double>("xref");
    m_yref = getValue<double>("yref");

    // rotation angle defaults to zero.
    m_rot = getValue<double>("rot",0.);

    // projection type, flag for glactic transform
    m_projType = getValue<std::string>("projtype");
    m_use_lb = getValue<bool>("uselb", false);

    // names for ra and dec columns, with defaults
    m_raName= getValue<std::string>("ra_name", "ra");
    m_decName = getValue<std::string>("dec_name", "dec");

}

