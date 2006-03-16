/** @file Photon.h
@brief Define the Photon class

$Header$
*/

#ifndef astro_Photon_h
#define astro_Photon_h

#include "astro/SkyDir.h"


namespace astro {

    /** @class Photon
    @brief add energy and event class attributes to a SkyDir.


    */
    class Photon : public astro::SkyDir {
    public:
        Photon(const astro::SkyDir& dir, double energy=0, int event_class=-1)
            : astro::SkyDir(dir)
            , m_energy(energy)
            , m_event_class(event_class)
        {}
        double energy()const{return m_energy;}
        int eventClass()const{return m_event_class;}

    private:
        double m_energy;
        int m_event_class;
    };

}// namespace astro

#endif
