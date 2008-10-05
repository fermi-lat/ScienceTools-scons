/** \file PulsePhaseApp.h
    \brief Declaration of PulsePhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#ifndef pulsePhase_PulsePhaseApp_h
#define pulsePhase_PulsePhaseApp_h

#include "pulsarDb/PulsarToolApp.h"

#include "st_stream/StreamFormatter.h"

class PulsePhaseApp : public pulsarDb::PulsarToolApp {
  public:
    PulsePhaseApp();
    virtual ~PulsePhaseApp() throw();
    virtual void run();

  private:
    st_stream::StreamFormatter m_os;
};

#endif
