// $Header$
// 

#ifndef _H_GRBSvc_
#define _H_GRBSvc_

// Include files
#include "IGRBSvc.h"
#include "FluxSvc/IFluxSvc.h"
#include "FluxSvc/IFlux.h"

// includes
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IRndmGenSvc.h"

#include "GaudiKernel/IParticlePropertySvc.h"

#include <list>

// GRB includes
#include "GRBSpectrum.h"
#include "../GRBmaker/GRBobsSpectrum.h"

//forward declarations
template <class TYPE> class SvcFactory;
class IFluxSvc;
class IParticlePropertySvc; 

//!  Service that implements the IFluxSvc interface, to return an IFlux object.
//!  FluxSvc handles the creation and interfacing with Flux objects.  
class GRBSvc : virtual public Service, virtual public IGRBSvc
{  
public:    
    //------------------------------------------------------------------
    //  stuff required by a Service
    
    /// perform initializations for this service. 
    StatusCode initialize () 
    {
        
        StatusCode  status =  Service::initialize ();
        
        // bind all of the properties for this service
        setProperties ();
        
        // open the message log
        MsgStream log( msgSvc(), name() );
        
        IFluxSvc* m_fsvc; /// pointer to the flux Service 
        // get the service
        StatusCode sc = service("FluxSvc", m_fsvc);
        
        if( sc.isFailure()) {
            log << MSG::ERROR << "Could not find FluxSvc" << endreq;
            return sc;
        }else{
            log << MSG::INFO << "Found FluxSvc" << endreq;
        }
        
        log << MSG::INFO << "Adding GRB Spectra..." << endreq;
        static RemoteSpectrumFactory<GRBSpectrum> factory(m_fsvc);
        const ISpectrumFactory& GRBSpectrumFactory = factory;

        log << MSG::INFO << "Adding Sandhia GRB Spectra..." << endreq;
        static RemoteSpectrumFactory<GRBobsSpectrum> factory2(m_fsvc);
        const ISpectrumFactory& GRBobsSpectrumFactory = factory2;
	
	return status;
    }
    
    /// perform the finalization, as required for a service.
    virtual StatusCode finalize ();
    
    /// Query interface
    virtual StatusCode queryInterface( const IID& riid, void** ppvUnknown );
    
protected: 
    
    /// Standard Constructor
    GRBSvc ( const std::string& name, ISvcLocator* al );
    
    /// destructor
    virtual ~GRBSvc ();
    
private:
    
    //IParticlePropertySvc* m_partSvc;
    
    /// Allow SvcFactory to instantiate the service.
    friend class SvcFactory<GRBSvc>;
    
};


#endif // _H_FluxSvc

