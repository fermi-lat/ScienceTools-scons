/** \file OrbitalPhaseApp.h
    \brief Declaration of OrbitalPhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#ifndef pulsePhase_OrbitalPhaseApp_h
#define pulsePhase_OrbitalPhaseApp_h

#include "pulsarDb/PulsarToolApp.h"

#include "st_stream/StreamFormatter.h"

class OrbitalPhaseApp : public pulsarDb::PulsarToolApp {
  public:
    OrbitalPhaseApp();
    virtual ~OrbitalPhaseApp() throw();
    virtual void runApp();

  private:
    st_stream::StreamFormatter m_os;
};

#endif
