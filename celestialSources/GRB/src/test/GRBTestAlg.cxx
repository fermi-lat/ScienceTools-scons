// $Header$

// Include files
#include "FluxSvc/IFluxSvc.h"
#include "FluxSvc/IFlux.h"

// GlastEvent for creating the McEvent stuff
#include "Event/TopLevel/Event.h"
#include "Event/TopLevel/MCEvent.h"
#include "Event/MonteCarlo/McParticle.h"
#include "Event/TopLevel/EventModel.h"

// Gaudi system includes
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include <list>
#include <string>
#include <vector>
#include "GaudiKernel/ParticleProperty.h"


// GRB includes
#include "../GRB/GRBSpectrum.h"
#include "other/GRBTest.cxx"
//include files for ROOT...
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TFile.h"

/*! \class GRBTestAlg
  \brief 
  
*/

using namespace std;

//! The size of the energy array is taken from GRBConstant.h
#define ENERGYSIZE cst::enstep
//! The size of the time array is taken from GRBConstant.h
#define TIMESIZE cst::nstep
/*! the flux is a bidimentional array (matrix?) funtion of energy and time.
 * Its Unities are 
 *
 */

class GRBTestAlg : public Algorithm {
  
public:
  //! Constructor of this form must be provided
  GRBTestAlg(const std::string& name, ISvcLocator* pSvcLocator);   
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  IFluxSvc* m_fsvc; /// pointer to the flux Service 
  IParticlePropertySvc * m_partSvc;
  
  int pippo;
  
  double TIME;
  int EVENTS;
  
  std::string m_source_name;
  std::string m_background_name;
  double m_observation_time;
  //  char* flux_source;
  //std::string flux_source;//="GRBSpectrum";
};


static const AlgFactory<GRBTestAlg>  Factory;
const IAlgFactory& GRBTestAlgFactory = Factory;

//------------------------------------------------------------------------------
//
GRBTestAlg::GRBTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),TIME(10.0),EVENTS(100000),
  m_source_name("GRBSpectrumSpectrum"),m_background_name(" "){
  
  declareProperty("source_name", m_source_name);
  declareProperty("background_name", m_background_name);
  declareProperty("observation_time", TIME);
  declareProperty("EvtMax",EVENTS);
}


//------------------------------------------------------------------------------
/*! */
StatusCode GRBTestAlg::initialize() {
  
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initializing..." << endreq;
  
  // Use the Job options service to set the Algorithm's parameters
  setProperties();
  
  // get the service
  StatusCode sc = service("FluxSvc", m_fsvc);
  pippo=0;
  return sc;
}


//------------------------------------------------------------------------------
StatusCode GRBTestAlg::execute() {
  pippo++;
  StatusCode  sc = StatusCode::SUCCESS;
  MsgStream   log( msgSvc(), name() );    
  
  std::vector<char*> arguments;
  arguments.push_back(" ");
  
  char o1[100];
  char o2[100];
  char o3[100];

  sprintf(o1,"%f",TIME);
  sprintf(o2,"%d",EVENTS);
  sprintf(o3,"%s%d",m_source_name.c_str(),pippo);
  
  char* time_max= o1;
  char* events_max= o2;
  //  char* source_name=
  //char* source_name=m_source_name.c_str();
  /*
  cout<<"Time = "<<time_max
      <<" events = "<<events_max
      <<" source name = "<<o3<<endl;
  */
  if(m_background_name!=" ") cout<<" background  = "<<m_background_name.c_str()<<endl;
  
  arguments.push_back("-time");
  arguments.push_back(time_max);
  arguments.push_back("-events");
  arguments.push_back(events_max);
  arguments.push_back("-name");
  arguments.push_back(o3);
  arguments.push_back(m_source_name.c_str());
  if(m_background_name!=" ") arguments.push_back(m_background_name.c_str());

  GRBTest* m_GRBTest = new GRBTest();
  
  m_GRBTest->setService(m_fsvc);
  //m_GRBTest->help();
  //m_GRBTest->listSpectra();
  m_GRBTest->Start(arguments);
  
  return sc;
}


//------------------------------------------------------------------------------
StatusCode GRBTestAlg::finalize() {
    
    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
