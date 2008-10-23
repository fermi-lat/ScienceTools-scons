/** \file OrbitalPhaseApp.h
    \brief Declaration of OrbitalPhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#ifndef pulsePhase_OrbitalPhaseApp_h
#define pulsePhase_OrbitalPhaseApp_h

#include "pulsarDb/PulsarToolApp.h"

#include "st_stream/StreamFormatter.h"

/** \class OrbitalPhaseApp
    \brief Main application class for orbital phase assignment.
*/
class OrbitalPhaseApp : public pulsarDb::PulsarToolApp {
  public:
    /// \brief Construct a OrbitalPhaseApp object.
    OrbitalPhaseApp();

    /// \brief Destruct this OrbitalPhaseApp object.
    virtual ~OrbitalPhaseApp() throw();

    /// \brief Run the application.
    virtual void runApp();

  private:
    st_stream::StreamFormatter m_os;
};

#endif
