#ifndef _H_IGRBSvc
#define _H_IGRBSvc

// includes
#include "GaudiKernel/IInterface.h"
#include <string>
#include <list>
#include <vector>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IGRBSvc(910, 1 , 0); 

// forward declaration
class IParticlePropertySvc;

//! Abstract interface for the flux service, FluxSvc.
class  IGRBSvc : virtual public IInterface {
public:
 
};

#endif  // _H_IGRBSvc
