// $Header$
// 
//

#include "GRBSvc.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "GaudiKernel/IParticlePropertySvc.h"

#include "FluxSvc/IFluxSvc.h"

#include <algorithm>

// GRB includes
#include "GRBSpectrum.h"

// declare the service factories for the FluxSvc
static SvcFactory<GRBSvc> a_factory;
const ISvcFactory& GRBSvcFactory = a_factory;

// ------------------------------------------------
// Implementation of the GRBSvc class
// ------------------------------------------------
/// Standard Constructor
GRBSvc::GRBSvc(const std::string& name,ISvcLocator* svc)
: Service(name,svc)
{}

/// Standard Destructor
GRBSvc::~GRBSvc()  
{}

// finalize
StatusCode GRBSvc::finalize ()
{
    StatusCode  status = StatusCode::SUCCESS;

    return status;
}

/// Query interface
StatusCode GRBSvc::queryInterface(const IID& riid, void** ppvInterface)  {
    if ( IID_IGRBSvc.versionMatch(riid) )  {
        *ppvInterface = (IGRBSvc*)this;
    }
    else  {
        return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return SUCCESS;
}

void WARNING (const char * text ){  std::cerr << "WARNING: " << text << '\n';}
void FATAL(const char* s){std::cerr << "\nERROR: "<< s;}
