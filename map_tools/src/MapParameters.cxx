/** @file MapParameters.cxx
*   @brief Implementation for class that reads parameters for image description
* @author Toby Burnett 
*
* $Header$
*/

#include "map_tools/MapParameters.h"
#include <iostream>

using namespace map_tools;
//! Constructor
MapParameters::MapParameters(int argc, char * argv[])
: Parameters(argc, argv)
{
    setup();
}
MapParameters::MapParameters( hoops::IParGroup& hpar)
: Parameters(hpar)
{
    setup();
}
void MapParameters::setup()
{

    // Read number of pixels along x
    m_npix = getValue<long>("npix");

    // y direction defaults to same as x (square)
    m_npix_y= getValue<long>("npixy", m_npix);

    // z direction (layers) defaults to 1.
    m_npix_z= getValue<long>("layers", 1);

    // Read image size (presumably degreesr)
    m_imgSizeX= getValue<double>( "imgsize" , 0);
    // if y not specified, assume square
    m_imgSizeY= getValue<double>( "imgsizey", m_imgSizeX);

    if( m_imgSizeX==0 ) { // not specified: require a "pixelsize" card
        double pixelsize=0;
        try { pixelsize = getValue<double>("pixelsize"); } catch(...){
            std::cerr << "MapParameters::MapParameters: neither imgsize or pixelsize specified" << std::endl;
            throw;
        }
        m_imgSizeX = m_npix* pixelsize;
        m_imgSizeY = m_npix_y*pixelsize;
    }

    // Read xref, yref (standard is center)
    m_xref = getValue<double>("xref",0.);
    m_yref = getValue<double>("yref", 0.);

    // rotation angle defaults to zero.
    m_rot = getValue<double>("rot",0.);

    // projection type, flag for glactic transform
    m_projType = getValue<std::string>("projtype");
    m_use_lb = getValue<bool>("uselb", false);

    // names for ra and dec columns, with defaults
    m_raName= getValue<std::string>("ra_name", "ra");
    m_decName = getValue<std::string>("dec_name", "dec");

}

