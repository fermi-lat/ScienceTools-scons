/** \file gtophase.cxx
    \brief Orbital phase assignment tool that computes an orbital phase value for each photon and writes it back to input event file(s).
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#include "OrbitalPhaseApp.h"

#include "st_app/StAppFactory.h"

#include "timeSystem/EventTimeHandler.h"
#include "timeSystem/GlastTimeHandler.h"

// List supported event file format(s).
timeSystem::EventTimeHandlerFactory<timeSystem::GlastScTimeHandler> glast_sctime_handler;

st_app::StAppFactory<OrbitalPhaseApp> g_factory("gtophase");
