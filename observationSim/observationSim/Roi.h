/**
 * @file Roi.h
 * @brief Region-of-interest class declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef observationSim_Roi_h
#define observationSim_Roi_h

#include "astro/SkyDir.h"
//#include "FluxSvc/../src/EventSource.h"

namespace observationSim {

/**
 * @class Roi
 *
 * @brief Region-of-interest, defined here simply as the acceptance
 * cone about some direction.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Roi {

public:

   Roi(double glon, double glat, double coneAngle, 
       astro::SkyDir::CoordSystem inputType=astro::SkyDir::GALACTIC) 
      : m_center(astro::SkyDir(glon, glat, inputType)), 
        m_coneAngle(coneAngle) {}

   Roi(const astro::SkyDir &center, double coneAngle)
      : m_center(center), m_coneAngle(coneAngle) {}

   bool operator()(EventSource *);

private:

   astro::SkyDir m_center;

   double m_coneAngle;

};

} // namespace observationSim

#endif // observationSim_Roi_h
