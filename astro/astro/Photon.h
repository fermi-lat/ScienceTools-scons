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
        Photon(const astro::SkyDir& dir, double energy, double time, int event_class=-1, int source=-1)
            : astro::SkyDir(dir)
            , m_energy(energy)
            , m_time(time)
            , m_event_class(event_class)
            , m_source(source)
        {}
        double energy()const{return m_energy;}
        int eventClass()const{return m_event_class;}
        double time()const{return m_time;}
        int source()const{return m_source;}

    private:
        double m_energy;
        double m_time;
        int m_event_class;
        int m_source;

    };

}// namespace astro

#endif
