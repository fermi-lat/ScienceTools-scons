/**
 * @file Ft2file.cxx
 * @brief Implementation for class interface to spacecraft data file.
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include "astro/SkyDir.h"
#include "astro/Quaternion.h"

#include "fitsGen/Ft2File.h"

namespace fitsGen {

void Ft2File::setScAxes(double ra_scz, double dec_scz,
                        double ra_scx, double dec_scx) {
   (*this)["ra_scz"].set(ra_scz);
   (*this)["dec_scz"].set(dec_scz);
   (*this)["ra_scx"].set(ra_scx);
   (*this)["dec_scx"].set(dec_scx);

   astro::SkyDir scz(ra_scz, dec_scz);
   astro::SkyDir scx(ra_scx, dec_scx);

   astro::Quaternion orientation(scz(), scx());

   (*this)["qsj_1"].set(orientation.vector().x());
   (*this)["qsj_2"].set(orientation.vector().y());
   (*this)["qsj_3"].set(orientation.vector().z());
   (*this)["qsj_4"].set(orientation.scalar());
}

} // namespace fitsGen
