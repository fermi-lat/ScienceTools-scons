/**
* @file MapParameters.h
* @brief Map Parameter Reader
*
* $Header$
*/

#ifndef MAP_TOOLS_MAPPARAMETERS_H
#define MAP_TOOLS_MAPPARAMETERS_H 

#include "Parameters.h"
#include <string>
namespace hoops { class IParGroup; }

namespace map_tools {

/**
* @class MapParameters
* @brief Input reader class for count_map tool.
*
* This class provides methods to read user parameters needed for EventBin tool. 
* It uses PIL to read parameters from the par file.
* The description of PIL is available at
* href="http://www-glast.slac.stanford.edu/sciencetools/userInterface/doc/pil.pdf">PIL user
* manual</a>.
*
* $Header$
*/

class MapParameters : public Parameters
{
public:
    // Constructors
    MapParameters(hoops::IParGroup & pars);
    MapParameters(int argc, char * argv[]);

    int npix() const                   { return m_npix; }
    int npixX() const                   { return m_npix; }
    int npixY() const                   { return m_npix_y; }
    int npixZ() const                   { return m_npix_z; }
    double imgSize() const                { return m_imgSizeX; }
    double imgSizeX() const                { return m_imgSizeX; }
    double imgSizeY() const                { return m_imgSizeY; }
    double xref() const                 { return m_xref; }
    double yref() const                 { return m_yref; }
    double rot() const                  { return m_rot; }
    std::string projType() const        { return m_projType; }
    std::string raName() const          { return m_raName; }
    std::string decName() const         { return m_decName; }
   std::string tableName() const         { return m_tableName; }

    bool uselb()const           {return m_use_lb;}


private:

    void setup();

    int             m_npix, m_npix_y, m_npix_z;
    double         m_imgSizeX, m_imgSizeY;
    double         m_xref,    m_yref;
    double          m_rot;
    std::string     m_projType;
    bool            m_use_lb;
    std::string     m_raName, m_decName;
    std::string  m_tableName;

};
}//namespace map_tools

#endif
