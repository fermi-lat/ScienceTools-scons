/*!\class GRBTestAlg
 * \brief Test the GRB Gaudi algorithm.
 * 
 * It contains the structure of a general flux algorithm of Gaudi.
 * All the option available are declered in the joboptions file.
 * 
 */

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
//#include "GaudiKernel/ParticleProperty.h"


// GRB includes
//#include "../GRB/GRBSpectrum.h"
#include "other/GRBTest.cxx"
//include files for ROOT...
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TFile.h"


using namespace std;

class GRBTestAlg : public Algorithm {
  
public:
  //! Constructor of this form must be provided 
  GRBTestAlg(const std::string& name, ISvcLocator* pSvcLocator);   
  
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  //! pointer to the flux Service 
  IFluxSvc* m_fsvc;
    
  int m_loop;
  double m_time;
  int m_events;

  std::string m_source_name;
  std::string m_background_name;
  std::string m_save_file;
  double m_observation_time;
  
};


static const AlgFactory<GRBTestAlg>  Factory;
const IAlgFactory& GRBTestAlgFactory = Factory;

//------------------------------------------------------------------------------
//
GRBTestAlg::GRBTestAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),m_time(10.0),m_events(100000),
  m_source_name("GRBSpectrum"),m_background_name(" "),m_save_file("no"){
  
  declareProperty("source_name", m_source_name);
  declareProperty("background_name",  m_background_name);
  declareProperty("observation_time", m_time);
  declareProperty("EvtMax",   m_events);
  declareProperty("savefile", m_save_file);

}


//------------------------------------------------------------------------------
/*! 
 * Initialize the algorithm, use the Job options service to set the 
 * Algorithm's parameters, and points to flux service 
 */
StatusCode GRBTestAlg::initialize() {
  
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "initializing..." << endreq;
  
  // Use the Job options service to set the Algorithm's parameters
  setProperties();
  
  // get the service
  StatusCode sc = service("FluxSvc", m_fsvc);
  m_loop=0;
  return sc;
}


//------------------------------------------------------------------------------
/*! 
 * Execute the algorithm, calling GRBTest.
 */
StatusCode GRBTestAlg::execute() {
  m_loop++;
  StatusCode  sc = StatusCode::SUCCESS;
  MsgStream   log( msgSvc(), name() );    
  
  std::vector<char*> arguments;
  arguments.push_back(" ");
  
  char arg1[50];
  char arg2[50];
  char arg3[50];

  sprintf(arg1,"%f",m_time);
  sprintf(arg2,"%d",m_events);
  sprintf(arg3,"%s%d",m_source_name.c_str(),m_loop);
  
  if(m_background_name!=" ") cout<<" background  = "<<m_background_name.c_str()<<endl;
  
  arguments.push_back("-time");
  arguments.push_back(arg1);
  arguments.push_back("-events");
  arguments.push_back(arg2);
  if(m_save_file=="yes") 
    {
      arguments.push_back("-save");
      arguments.push_back(arg3);
    }
  
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
/*!
 * Finalize the algorithm.
 */
StatusCode GRBTestAlg::finalize() {
    
    return StatusCode::SUCCESS;
}

//------------------------------------------------------------------------------
