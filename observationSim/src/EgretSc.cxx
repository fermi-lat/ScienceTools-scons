/**
 * @file EgretSc.cxx
 * @brief Implementation for EGRET spacecraft class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include "observationSim/../src/EgretSc.h"

namespace observationSim {

HepRotation EgretSc::InstrumentToCelestial(double) {

// This implementation *should* ensure that an orthogonal set of axes
// are fed to the HepRotation constructor.
   Hep3Vector z_axis = m_zAxis();
   Hep3Vector x_axis = m_xAxis();
   Hep3Vector yAxis = z_axis.cross(x_axis);

   return HepRotation(yAxis.cross(z_axis), yAxis, z_axis);
}

} // namespace observationSim

