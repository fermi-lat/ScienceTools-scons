/********************************************************************
* RootCint.h
********************************************************************/
#ifdef __CINT__
#error RootCint.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableRootCint();
extern void G__cpp_setup_inheritanceRootCint();
extern void G__cpp_setup_typetableRootCint();
extern void G__cpp_setup_memvarRootCint();
extern void G__cpp_setup_globalRootCint();
extern void G__cpp_setup_memfuncRootCint();
extern void G__cpp_setup_funcRootCint();
extern void G__set_cpp_environmentRootCint();
}


#include "TROOT.h"
#include "TMemberInspector.h"
#include "RootFrame.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__RootCintLN_G__longlong;
extern G__linked_taginfo G__RootCintLN_G__ulonglong;
extern G__linked_taginfo G__RootCintLN_TClass;
extern G__linked_taginfo G__RootCintLN_TBuffer;
extern G__linked_taginfo G__RootCintLN_TMemberInspector;
extern G__linked_taginfo G__RootCintLN_TQObjSender;
extern G__linked_taginfo G__RootCintLN_st_graph;
extern G__linked_taginfo G__RootCintLN_st_graphcLcLIFrame;
extern G__linked_taginfo G__RootCintLN_TGFrame;
extern G__linked_taginfo G__RootCintLN_st_graphcLcLIEventReceiver;
extern G__linked_taginfo G__RootCintLN_st_graphcLcLRootFrame;
extern G__linked_taginfo G__RootCintLN_setlEst_graphcLcLIFramemUcOlesslEst_graphcLcLIFramemUgRcOallocatorlEst_graphcLcLIFramemUgRsPgR;

/* STUB derived class for protected member access */
