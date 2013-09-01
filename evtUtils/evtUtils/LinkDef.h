// -*- Mode: c++ -*-
/** @file LinkDef.h
    @brief for rootcint
 $Header$

*/
#ifdef __CINT__


#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// in TMine/util
#pragma link C++ namespace evtUtils;
#pragma link C++ class evtUtils::EventCategory;
#pragma link C++ class evtUtils::EventMap;
#pragma link C++ class evtUtils::EventClass;

#endif

