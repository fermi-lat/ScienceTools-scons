/** \file PulsePhaseApp.h
    \brief Declaration of PulsePhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#ifndef pulsePhase_PulsePhaseApp_h
#define pulsePhase_PulsePhaseApp_h

#include "pulsarDb/PulsarToolApp.h"

#include "st_stream/StreamFormatter.h"

/** \class PulsePhaseApp
    \brief Main application class for pulse phase assignment.
*/
class PulsePhaseApp : public pulsarDb::PulsarToolApp {
  public:
    /// \brief Construct a PulsePhaseApp object.
    PulsePhaseApp();

    /// \brief Destruct this PulsePhaseApp object.
    virtual ~PulsePhaseApp() throw();

    /// \brief Run the application.
    virtual void runApp();

  private:
    st_stream::StreamFormatter m_os;
};

#endif
