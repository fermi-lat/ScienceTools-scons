#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"

#include "Event/TopLevel/Event.h"
#include "Event/TopLevel/EventModel.h"
#include "Event/MonteCarlo/McParticle.h"
#include "Event/MonteCarlo/McIntegratingHit.h"
#include "Event/MonteCarlo/McPositionHit.h"
#include "Event/Digi/CalDigi.h"
#include "idents/CalXtalId.h"
//include files for ROOT...
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TFile.h"
// Include files
#include "FluxSvc/IFluxSvc.h"
#include <map>

/** @class TDSReadFluxAlg
 * @brief Takes the data relatives to the incoming particle from the TDS
 * and write a simple Root Tree
 *
 * @author Nicola Omodei  and Francesco Longo
 */
 	
std::pair<double,double> GalDir(HepVector3D GalDirVec)
{
	double x_g=GalDirVec.x();
	double y_g=GalDirVec.y();
	double z_g=GalDirVec.z();

	if(pow(x_g,2)<=1.e-15) x_g=0.0;
	if(pow(y_g,2)<=1.e-15) y_g=0.0;
	if(pow(z_g,2)<=1.e-15) z_g=0.0;

	double b = 360./M_2PI*asin(y_g);
	double l = 360./M_2PI*atan2(x_g,z_g);
	if(pow(b,2)<1.0e-15) b=0.;
	if(pow(l,2)<1.0e-15) l=0.;

	return std::make_pair(l,b);

} 

class TDSReadFluxAlg : public Algorithm
{	
public:
//    inline void setService(IFluxSvc* ptr){m_fsvc = ptr;}
    TDSReadFluxAlg(const std::string& name, ISvcLocator* pSvcLocator);
    
    StatusCode initialize();
   
    StatusCode execute();
    
    StatusCode finalize();   
        
private:
    IFluxSvc* m_fsvc; // pointer to the flux Service  
    HepVector3D dir,GalDirVec;
    StatusCode readMotherData();

    double energy,time,cos_theta,phi,l,b,rate;
};
 TTree *events;
  
static const AlgFactory<TDSReadFluxAlg>  Factory;
const IAlgFactory& TDSReadFluxAlgFactory = Factory;


TDSReadFluxAlg::TDSReadFluxAlg(const std::string& name, ISvcLocator* pSvcLocator) : 
Algorithm(name, pSvcLocator)
{
}

StatusCode TDSReadFluxAlg::initialize()
{
    StatusCode sc = StatusCode::SUCCESS;
    MsgStream log(msgSvc(), name());
    
    setProperties();

    sc = service("FluxSvc",m_fsvc);
    		
    // Open the file and set up the tree:
   events = new TTree("Events","Events");
    
//    sprintf(root_name,"%s%s",sources[i].c_str(),number);
	events->Branch("energy",&energy,"energy/D");
	events->Branch("time",&time,"time/D");
	events->Branch("rate",&rate,"rate/D");
	events->Branch("cos_theta",&cos_theta,"cos_theta/D");
	events->Branch("phi",&phi,"phi/D");
	events->Branch("l",&l,"l/D");
	events->Branch("b",&b,"b/D");

    return sc;
    
}

StatusCode TDSReadFluxAlg::execute()
{

    MsgStream log(msgSvc(), name());
    StatusCode sc = StatusCode::SUCCESS;
    
    sc = readMotherData();
    return sc;
}

StatusCode TDSReadFluxAlg::readMotherData() {
    // Purpose and Method: Retrieve Monte Carlo data from the TDS.
    
    MsgStream log(msgSvc(), name());
    StatusCode sc = StatusCode::SUCCESS;
    
    SmartDataPtr<Event::EventHeader> evt(eventSvc(), EventModel::EventHeader);
    if(!evt) return sc;
    time=(double)evt->time();

    SmartDataPtr<Event::McParticleCol> particles(eventSvc(), EventModel::MC::McParticleCol);
    if (particles) {
        log << MSG::DEBUG << "Retrieved McParticles from the TDS" << endreq;
        Event::McParticleCol::const_iterator p;
	p = particles->begin();
	
	energy=(*p)->finalFourMomentum().e();
	dir=(*p)->finalFourMomentum().vect().unit();
		cos_theta=(*p)->finalFourMomentum().cosTheta();
	phi=(*p)->finalFourMomentum().phi();
	if(phi < 0) phi = 2*M_PI + phi;
	phi=180*phi/M_PI; //degrees
	rate=0;
	
	GalDirVec=m_fsvc->transformGlastToGalactic(time)*dir;
	l = GalDir(GalDirVec).first;
    	b = GalDir(GalDirVec).second;
	//	std::cout<<" ---- Time (sec) "<<time<<std::endl;
	//	std::cout<<"      Energy     = "<<energy<<" (MeV)"<<std::endl;	
	//      std::cout<<"      Direction: Cos(theta) = "<<cos_theta<<std::endl;	
	//	std::cout<<"                        phi = "<<phi<<std::endl;
	//	std::cout<<"-------------------------------------------"<<std::endl;	
 	events->Fill();
   }

   return sc;
}
    

StatusCode TDSReadFluxAlg::finalize()
{
    StatusCode sc = StatusCode::SUCCESS;
    TFile f("Events.root","RECREATE"); //or update ?
    events->Write();
    f.Close();
    return sc;
}



