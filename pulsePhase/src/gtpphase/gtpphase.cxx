/** \file gtpphase.cxx
    \brief Pulse phase assignment tool that computes a pulse phase value for each photon and writes it back to input event file(s).
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#include "PulsePhaseApp.h"

#include "st_app/StAppFactory.h"

#include "timeSystem/EventTimeHandler.h"
#include "timeSystem/GlastTimeHandler.h"

// List supported event file format(s).
timeSystem::EventTimeHandlerFactory<timeSystem::GlastScTimeHandler> glast_sctime_handler;

st_app::StAppFactory<PulsePhaseApp> g_factory("gtpphase");
