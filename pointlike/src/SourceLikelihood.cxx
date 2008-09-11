/** @file SourceLikelihood.cxx

$Header$

*/

#include "pointlike/SourceLikelihood.h"
#include "skymaps/DiffuseFunction.h"
#include "skymaps/CompositeSkySpectrum.h"
#include "skymaps/BinnedPhotonData.h"

#include "embed_python/Module.h"
#include "astro/SkyDir.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <TMinuit.h>

using namespace pointlike;

namespace {

  int gFitCounter = 0;
  pointlike::SourceLikelihood* gSourcePointer = NULL;
  CLHEP::Hep3Vector gFitStartDir(0.,0.,0.);
  CLHEP::Hep3Vector gFitDeltaX(0.,0.,0.);
  CLHEP::Hep3Vector gFitDeltaY(0.,0.,0.);
  double gTSvalue=0;


//// minuit likelihood wrapper function to calculate maximum TS

  
  void minuit_likelihood_wrapper(Int_t &npar, Double_t* derivative, 
				 Double_t & loglike, Double_t par[], Int_t iflag){
    if(!gSourcePointer) 
      throw std::invalid_argument("SourceLikelihood::gSourcePointer not set");
    
    double x = par[0];
    double y = par[1];

    // set source parameters     

    std::vector<double> src_par(npar-2,0);
    for (int i=0;i<npar-2;i++) src_par[i]=par[i+2];
    
    // calculate new direction vector from fit parameters (delta(theta),delta(phi))

    CLHEP::Hep3Vector newDir = gFitStartDir+ x* gFitDeltaX + y* gFitDeltaY;

    //      std::cout<<"new dir: "<<newDir<<" "<<newDir.mag()<<std::endl;

    newDir.setMag(1.);
    astro::SkyDir dirNew(newDir);

    //      std::cout<<"new dir: dec="<<dirNew.dec()<<" ra="<<dirNew.ra()<<std::endl;
    //      astro::SkyDir dirNew = astro::SkyDir(x, y, astro::SkyDir::EQUATORIAL);
    
    gSourcePointer->setDir(dirNew,src_par,false);

    double  TS   = gSourcePointer->maximize();
    loglike = -0.5*TS; //m_loglike;
    
    if (iflag==2) { 
      const std::vector<double>& gradient = gSourcePointer->gradient();
      for (int i=0;i<npar;i++) derivative[i]=gradient[i];
    };
    
    std::cout << "**** Iteration "<<gFitCounter<<" **** Testing parameters:"
	      << std::fixed<< std::setprecision(4) 
	      << " delta(X)=" << (par[0]*180./M_PI)
	      <<" deg\tdelta(Y)="<<(par[1]*180./M_PI)<<" deg"<< std::scientific;
    for (int i=2;i<npar;i++)  std::cout <<"\tp"<<i<<"="<<par[i];

    //       std::cout << "\tTS: " << TS <<" "<<skip<<std::endl;

    std::cout << "\tTS: " << TS <<" iflag: "<<iflag<<std::endl;
    ++gFitCounter;
    
  }

  
}

namespace {
  double s_TScut(2.);  // only combine energy bands
  double gamma_front[] ={0,0,0,0,0, 2.25,  2.27,  2.22,  2.31,  2.30,  2.31,  2.16,  2.19,  2.07};
  double sigma_front[] ={0,0,0,0,0,0.343, 0.4199,0.4249 ,0.4202 ,0.4028 ,0.4223 ,0.4438 ,0.5113 ,0.5596 };
  double gamma_back[] ={0,0,0,0,0, 2.25,  2.27,  2.22,  2.31,  2.30,  2.31,  2.16,  2.19,  2.07};
  double sigma_back[] ={0,0,0,0,0,0.343, 0.4199,0.4249 ,0.4202 ,0.4028 ,0.4223 ,0.4438 ,0.5113 ,0.5596 };
};


// //  ----- static (class) variables -----

skymaps::SkySpectrum* SourceLikelihood::s_diffuse(0);
double                SourceLikelihood::s_emin(100.); 
double                SourceLikelihood::s_emax(1e6);
std::vector<double>   SourceLikelihood::s_gamma_front(gamma_front,gamma_front+14);
std::vector<double>   SourceLikelihood::s_sigma_front(sigma_front,sigma_front+14);
std::vector<double>   SourceLikelihood::s_gamma_back(gamma_back,gamma_front+14);
std::vector<double>   SourceLikelihood::s_sigma_back(sigma_back,sigma_front+14);
double                SourceLikelihood::s_minalpha(0.00);     
int                   SourceLikelihood::s_itermax(1);	      
double                SourceLikelihood::s_TSmin(0.0);	      
int                   SourceLikelihood::s_verbose(0);	      
double                SourceLikelihood::s_maxstep(0.25);      
int                   SourceLikelihood::s_useMinuit(1);       
int                   SourceLikelihood::s_useSimplex(0);	
int                   SourceLikelihood::s_useGradient(1);	      
int                   SourceLikelihood::s_useMinos(0);	      
double                SourceLikelihood::s_accuracy(0.0001);	



///////////////////////////////////////////////////////
//////// Static methods of SourceLikelihood //////////
//////////////////////////////////////////////////////

// manage energy range for selection of bands to fit 

void SourceLikelihood::set_energy_range(double emin, double emax){
  s_emin = emin; s_emax=emax;
}

// verbosity level
void SourceLikelihood::set_verbose(bool verbosity){s_verbose=verbosity;}
bool SourceLikelihood::verbose(){return s_verbose;}

// set parameters for source likelihood

void SourceLikelihood::setParameters(const embed_python::Module& par){
  static std::string prefix("SourceLikelihood.");
  
  par.getValue(prefix+"emin",      s_emin, s_emin);
  par.getValue(prefix+"emax",      s_emax, s_emax);
  par.getValue(prefix+"minalpha",  s_minalpha, s_minalpha);
  par.getValue(prefix+"itermax",   s_itermax, s_itermax);
  par.getValue(prefix+"TSmin",     s_TSmin, s_TSmin);
  par.getValue(prefix+"UseMinuit", s_useMinuit, s_useMinuit);
  par.getValue(prefix+"verbose",   s_verbose, s_verbose);
  par.getValue(prefix+"maxstep",   s_maxstep, s_maxstep); // override with global
  par.getValue(prefix+"Simplex",   s_useSimplex,s_useSimplex); //courtesy function to stay compatible
  par.getValue(prefix+"UseSimplex",s_useSimplex,s_useSimplex);
  par.getValue(prefix+"UseMinos",  s_useMinos,s_useMinos);
  par.getValue(prefix+"UseGradient",s_useGradient,s_useGradient);
  par.getValue(prefix+"accuracy",  s_accuracy,s_accuracy);
  
  // needed by ExtendedLikelihood
  double umax(pointlike::ExtendedLikelihood::defaultUmax());
  par.getValue(prefix+"umax", umax, umax);
  pointlike::ExtendedLikelihood::setDefaultUmax(umax);
  
  double tolerance(pointlike::ExtendedLikelihood::tolerance());
  par.getValue("Diffuse.tolerance",  tolerance, tolerance);
  pointlike::ExtendedLikelihood::setTolerance(tolerance);

  double roi(pointlike::ExtendedLikelihood::defaultRoI());
  par.getValue(prefix+"regionOfInterest",roi,roi);
  pointlike::ExtendedLikelihood::setDefaultRoI(roi);
  
  s_gamma_front.clear(); s_gamma_back.clear(); 
  s_sigma_front.clear(); s_sigma_back.clear(); 
  par.getList(prefix+"gammaFront", s_gamma_front);
  par.getList(prefix+"gammaBack", s_gamma_back);
  par.getList(prefix+"sigmaFront", s_sigma_front);
  par.getList(prefix+"sigmaBack", s_sigma_back);
  
  std::string diffusefile;
  par.getValue("Diffuse.file", diffusefile);
  double exposure(1.0);
  par.getValue("Diffuse.exposure", exposure);
  int interpolate(0);
  par.getValue("interpolate", interpolate, interpolate);
  if( ! diffusefile.empty() ) {

    set_diffuse(new skymaps::CompositeSkySpectrum( new skymaps::DiffuseFunction(diffusefile, interpolate!=0), exposure) );
    
    std::cout << "Using diffuse definition "<< diffusefile 
	      << " with exposure factor " << exposure << std::endl; 
  }
}


///////////////////////////////////////////////////////
//////  SourceLikelihood: fit parameters of 
//////  potentially extended sources
///////////////////////////////////////////////////////


SourceLikelihood::SourceLikelihood(skymaps::BinnedPhotonData& data,    
				   std::string name,
				   const astro::SkyDir& dir,
				   const std::string type,
				   const std::vector<double>& src_param)
  : m_name(name)
  , m_dir(dir)
  , m_type(type)
  , m_npar(src_param.size())
  , m_sourceParameters(src_param)
  , m_sourceParErrors(src_param.size(),0)
  , m_out(&std::cout)
  , m_background(0){
  
  if( s_diffuse !=0){
    m_background = new skymaps::CompositeSkySpectrum(s_diffuse);
  }else {
    // may not be valid?
    m_background = 0; //new skymaps::CompositeSkySpectrum();
  }
  
  setup( data);

  setDir(m_dir, m_sourceParameters,false);
}

//////////////////////////////////////////////////
/////   setup: initialize variables/arrays/etc.
/////////////////////////////////////////////////

void SourceLikelihood::setup(skymaps::BinnedPhotonData& data){
  
  unsigned int i=0,k=0;
  for( skymaps::BinnedPhotonData::iterator bit = 
	 data.begin(); bit!=data.end(); ++bit){
         skymaps::Band& b = *bit;
    
    if(b.event_class()==0){
      if(s_gamma_front.size()>i) { 
         b.setGamma(s_gamma_front[i]);
	 std::cout<<"Setting gamma_front for bin with emin="<<b.emin()<<" MeV to gamma="<<s_gamma_front[i]<<std::endl;
      };	 
      if(s_sigma_front.size()>i) {
         b.setSigma(s_sigma_front[i]);
	 std::cout<<"Setting sigma_front for bin with emin="<<b.emin()<<" MeV to sigma="<<s_sigma_front[i]<<std::endl;
      };	 
      i++;
    } else {
      if(s_gamma_back.size()>k) {
         b.setGamma(s_gamma_back[k]);
	 std::cout<<"Setting gamma_back for bin with emin="<<b.emin()<<" MeV to gamma="<<s_gamma_back[k]<<std::endl;
      };	 
      if(s_sigma_back.size()>k) {
         b.setSigma(s_sigma_back[k]);
	 std::cout<<"Setting sigma_back for bin with emin="<<b.emin()<<" MeV tosigma="<<s_sigma_back[k]<<std::endl;
      };	 
      k++;
    };    
    
    double emin(floor(b.emin()+0.5) ), emax(floor(b.emax()+0.5));
    if( emin < s_emin && emax < s_emin ) continue;
    if( emax > s_emax ) break;
    
    pointlike::ExtendedLikelihood* sl 
      = new pointlike::ExtendedLikelihood(b, m_dir, 
					  m_type,m_sourceParameters,
					  pointlike::ExtendedLikelihood::defaultUmax(), 
					  m_background);
    this->push_back(sl);
    
  }
  if( this->empty()){
    throw std::invalid_argument("SourceLikelihood::setup: no bands to fit.");
  }
  
}

////////////////////////////////////
/////// Destructor
////////////////////////////////////

pointlike::SourceLikelihood::~SourceLikelihood()
{
  for( iterator it = begin(); it!=end(); ++it){
    delete *it;
  }
  delete m_background;
}

/////////////////////////////////////
////////// maximize: find best alphas for 
//////////           given position
////////////////////////////////////

double pointlike::SourceLikelihood::maximize()
{
  
#ifdef DEBUG
  std::cout << "**** Testing position: " 
	    << std::setw(10)<< std::setprecision(5)
	    << m_dir.ra() << " " << m_dir.dec() 
	    << std::endl; 
  
  std::cout << std::left << std::setw(20) 
	    <<"  iteration" << " alpha   alpha'   alpha''   loglike\n";
#endif
  
  m_TS = 0;
  m_loglike = 0;
  int photons=0;
  iterator it = begin();
  for( ; it!=end(); ++it){
    pointlike::ExtendedLikelihood& like = **it;
    std::pair<double,double> a(like.maximize());

#ifdef DEBUG	
    std::cout << "  --  Summary -------  Energy band: " << like.band().emin() <<"MeV - "<< like.band().emax()<<"MeV"  
	      << " alpha: " << a.first << " +- " << a.second << std::endl;
#endif        

    if( a.first > s_minalpha ) {
      m_loglike += logL();
      m_TS+= like.TS();
      photons+=like.photons();
    }	
  }
  
#ifdef DEBUG
  std::cout << "**** Iteration: " << gFitCounter 
	    << "**** Position: " 
	    << std::setw(10)<< std::setprecision(5)
	    << m_dir.ra() << " " << m_dir.dec() 
	    << " - TS: " << m_TS <<" photons:" << photons<<std::endl;
#endif
  
  return m_TS;
}

///////////////////////////////////
///// setDir: set direction, loop over energy bands
///////////////////////////////////


void pointlike::SourceLikelihood::setDir(const astro::SkyDir& dir, bool subset){
  for( iterator it = begin(); it!=end(); ++it){
    (*it)->setDir(dir,subset);
  }
  m_dir = dir;
}

void pointlike::SourceLikelihood::setDir(const astro::SkyDir& dir, 
					 const std::vector<double>& srcparam, 
					 bool subset){
  for( iterator it = begin(); it!=end(); ++it){
    (*it)->setDir(dir,srcparam,subset);
  }
  m_dir = dir;
}

/////////////////////////////////////////////////
///////// gradient: sum over gradients form energy bands
/////////////////////////////////////////////////

const std::vector<double> pointlike::SourceLikelihood::gradient() const{
  std::vector<double> gradient(2+m_npar,0);  
  const_iterator it = begin();
  for( ; it!=end(); ++it){
    const std::vector<double>& level_grad=(*it)->gradient(gFitDeltaX,gFitDeltaY);
    for(unsigned int j =0;j<gradient.size(); j++) gradient[j]+= level_grad[j];
  }
  //   for(unsigned int k=0;k<gradient.size();k++) std::cout<<"gradient("<<k<<")="<< gradient[k]<<std::endl;
  return gradient;
}


/////////////////////////////////////////////
////////// printSpectrum: output variables of the energy bands
/////////////////////////////////////////////

void pointlike::SourceLikelihood::printSpectrum()
{
  
  using std::setw; using std::left; using std::setprecision; 
  out() << "\nSpectrum of source " << m_name << " at ra, dec=" 
        << setprecision(6) << m_dir.ra() << ", "<< m_dir.dec() << std::endl;
  
  out() << "  emin eclass events   signal_fract    TS " << std::right << std::endl;
  
  m_TS =0;
  for( const_iterator it = begin(); it!=end(); ++it){
    
    pointlike::ExtendedLikelihood& levellike = **it;
    const skymaps::Band& band ( levellike.band() );
    
    double bkg(levellike.background());
    out()  << std::fixed << std::right 
	   << setw(7) << static_cast<int>( band.emin()+0.5 )
	   << setw(5) << band.event_class()
	   << setw(8) << levellike.photons()
	   << setw(10);
    if(bkg>=0) {
      out() << setprecision(1) << levellike.background();
    }else{
      // 	  out() << "     -    ";
    }
    
    if( levellike.photons()==0)  out() << std::endl; 
    
    if( levellike.photons()==0) {
      continue;
    }
    
    std::pair<double,double> a(levellike.maximize());
    
    double ts(levellike.TS()); 
    if( a.first > s_minalpha ) {
      m_TS+=ts;
    }
  
    out() << setprecision(2) << setw(6)<< a.first<<" +/- "
	  << setw(4)<< a.second 
	  << setw(6)<< setprecision(0)<< ts;
    out() << std::endl;

  }
  
  if( s_minalpha>0.1){
    out() << "\tTS sum  (alpha>"<<s_minalpha<<")  ";
  }else{
    out() << setw(30) << "sum  ";
  }
  out() << setw(14) << m_TS << std::endl;
}

//////////////////////////////////////////////////////
/////// energyList: create a vector with energy bin boundaries
//////////////////////////////////////////////////////

std::vector<double> pointlike::SourceLikelihood::energyList() const {
  
  std::vector<double> energies;
  if( size()>0) {
    const_iterator it (begin());
    for( ; it!= end(); ++it){
      double emin((*it)->band().emin());
      if( energies.size()==0 || energies.back()!= emin){
	energies.push_back(emin);
      }
    }
    energies.push_back( back()->band().emax() );
  }
  return energies;
}


//////////////////////////////////////////////////////
/////// localize: fit spectrum, localization and source extension
//////////////////////////////////////////////////////

double pointlike::SourceLikelihood::localize(){
  double t(100);
  gFitCounter = 0;
  if (s_useMinuit) 
    t = localizeMinuit();
  else 
    throw std::runtime_error("useMinuit=0 not supported yet.");
  return t;
}


//////////////////////////////////////////////////////
/////// localizeMinuit: fit spectrum, localization and source extension with Minuit
//////////////////////////////////////////////////////
 
double pointlike::SourceLikelihood::localizeMinuit()
{
  using std::setw; using std::left; using std::setprecision; using std::right;
  using std::fixed;
  int wd(10);
  
  if( verbose()){
    out() 
      << "      Searching for best position \n"
      << setw(wd) << left<< "Gradient   " 
      << setw(wd) << left<< "delta  "   
      << setw(wd) << left<< "ra"
      << setw(wd) << left<< "dec "
      << setw(wd) << left<< "error "
      << setw(wd) << left<< "Ts "
      <<std::endl;
  }
  
  astro::SkyDir last_dir(dir()); // save current direction
  setDir(dir(), m_sourceParameters, false);    // initialize
  
  gFitStartDir = dir()();
  
  std::cout<<std::setprecision(4)<<"Fit start direction: "<<gFitStartDir<<" "<<gFitStartDir.mag()<<" "<<dir()()<<std::endl;
  
  gFitDeltaX = CLHEP::Hep3Vector(gFitStartDir.y(),-gFitStartDir.x(),0.);
  if( gFitDeltaX.mag2()<1e-10) 
    gFitDeltaX = gFitStartDir.z()>0 ? Hep3Vector(1.,0.,0.) : Hep3Vector(-1.,0.,0.);
  gFitDeltaX.setMag(1.);	     
  gFitDeltaY = gFitStartDir.cross(gFitDeltaX);
  gFitDeltaY.setMag(1.);
  
  std::cout<<"Fit coordinate system: x="<<gFitDeltaX<<" y="<<gFitDeltaY<<std::endl;
  
  int npar = 2 + m_sourceParameters.size();

  TMinuit gMinuit(npar);

  std::cout << "Setting likelihood for "<<npar<<" parameters : " << this << std::endl;
  
  // Set the pointer for access in the minuit function
  gSourcePointer = this;
  gMinuit.SetFCN(minuit_likelihood_wrapper);

  std::vector<double> par(npar), stepSize(npar),minVal(npar),maxVal(npar);
  std::vector<std::string> parName(npar);

  par[0] = 0.;            par[1] = 0.;
  stepSize[0] = 0.01;     stepSize[1] = 0.01;
  minVal[0] = -0.5;       minVal[1] = -0.5;
  maxVal[0] = 0.5;        maxVal[1] = 0.5;
  
  parName[0] = std::string("Delta(theta)"); 
  parName[1] = std::string("Delta(phi)"); 
  
  for(int i=2; i<npar; i++){ 
    par[i]      = m_sourceParameters[i-2];
    stepSize[i] = 0.01;
    minVal[i]   = 0;
    maxVal[i]   = 0.5;
    std::stringstream nameStream(parName[i]);
    nameStream<<"srcparam("<<i-2<<")"; 
    parName[i]=nameStream.str();
  };   
  
  for (int i = 0; i < npar; ++i) {
    gMinuit.DefineParameter(i, parName[i].c_str(), par[i], 
			    stepSize[i], minVal[i], maxVal[i]);
    if (minVal[i]==maxVal[i] && maxVal[i]==0.) gMinuit.FixParameter(i);
  }; 
  Int_t ierflag = 0; // the minuit output flag. Can be queried after each command
  
//// Errors for likelihood fitting
  Double_t arglist[2];
  arglist[0] = 0.5; 
  int nargs = 1;
  gMinuit.mnexcm("SET ERR", arglist, nargs, ierflag);

  if (s_useGradient) { 
      nargs=1; arglist[0] = 1; 
      gMinuit.mnexcm("SET GRA", arglist, nargs, ierflag);
      gMinuit.SetPrintLevel(0);
  };
    
  nargs=1; arglist[0] = 1; 
  gMinuit.mnexcm("SET STR", arglist, nargs, ierflag);
  gMinuit.SetPrintLevel(0);
  
  // SF: eventually read itermax from the config-file
  //     arglist[0] = itermax; 
  arglist[0] = 2000; 
  // approximate maximum number of function calls
  // even if the minimisation hanot converged
  if (s_useSimplex==1) arglist[1]=s_accuracy;
  else arglist[1] = s_accuracy*1000.; 
  
  // tolerance (in units of 0.001*UP)
  // minimisation will stop if estimated vertical
  // distance to minimum is less than 0.001*tolerance*UP
  nargs = 2;
  if (s_useSimplex==1)
    gMinuit.mnexcm("SIMPLEX", arglist, nargs, ierflag);
  else
    gMinuit.mnexcm("MIGRAD", arglist, nargs, ierflag);  
  if ((ierflag == 4 || s_useSimplex==1)&&(!s_useMinos)){
    gMinuit.mnexcm("HESSE", arglist, nargs, ierflag);
  };
  if(s_useMinos){
    nargs = 0;
    gMinuit.mnexcm("MINOS", arglist, nargs, ierflag);
  }  
  
  double x = 0;
  double y = 0;
  gMinuit.GetParameter(0, x, m_errorX);
  gMinuit.GetParameter(1, y, m_errorY);
  for (int i=2; i<npar; i++) gMinuit.GetParameter(i, m_sourceParameters[i-2],m_sourceParErrors[i-2]);
  
  CLHEP::Hep3Vector newDir = gFitStartDir+ x* gFitDeltaX + y* gFitDeltaY;
  newDir.setMag(1.);
  astro::SkyDir dirNew(newDir);
  
  setDir(dirNew,m_sourceParameters,false);
  
  if (ierflag == 4) {
    // fitting did not converge
    std::cerr<<"WARNING: Minuit returned ierflag=4: Fit did not converge."<<std::endl;
    //      setDir(last_dir()); // restore position  
    return -1;    
  }
  
  return sqrt(m_errorX*m_errorX + m_errorY*m_errorY);
}  

/////////////////////////////////////////////////////////////
/////  fit: run localize,maximize and print spectrum iteratively
//////////////////////////////////////////////////////////


double pointlike::SourceLikelihood::fit()
{
  int itermax(s_itermax);
  double TSmin(s_TSmin);
  
  double sig(99);
  
  double currentTS(TS());
  if(verbose()) printSpectrum();
  
  for( int iter(0); iter<itermax; ++iter){
    if( TS()>TSmin) {
      sig = localize(); // may skip low levels
      if( sig<1) { 
	maximize();
	printSpectrum();
      }
    } else {
      std::cout<<"TS:"<<TS()<<" "<<TSmin<<std::endl;
    }
    if( TS() < currentTS+0.1 ) break; // done if didn't improve
    currentTS = TS();
  }
  return sig;
}


/////////////////////////////////////////////////////////////
/////  likelihood: get likelihood value from all bands
//////////////////////////////////////////////////////////


double pointlike::SourceLikelihood::value(const astro::SkyDir& dir, double energy) const
{
  double result(0);
  const_iterator it = begin();
  for( ; it!=end(); ++it){
    const skymaps::Band& band ( (*it)->band() );
    if( energy >= band.emin() && energy < band.emax() ){
      result += (**it)(dir);
    }
  }
  return result;
}



double pointlike::SourceLikelihood::display(const astro::SkyDir& dir, double energy, int mode) const
{
    const_iterator it = begin();
    for( ; it!=end(); ++it){
        const skymaps::Band& band ((*it)->band());
        if( energy >= band.emin() && energy < band.emax() )break;
    }
    if( it==end() ){
        throw std::invalid_argument("SourceLikelihood::display--no fit for the requested energy");
    }
    return (*it)->display(dir, mode);
}

///@brief integral for the energy limits, in the given direction
double pointlike::SourceLikelihood::integral(const astro::SkyDir& dir, double emin, double emax)const
{
  // implement by just finding the right bin
  return value(dir, sqrt(emin*emax) );
}


skymaps::SkySpectrum* SourceLikelihood::set_diffuse(skymaps::SkySpectrum* diffuse, 
						    double /*exposure*/)
{  
  skymaps::SkySpectrum* ret = s_diffuse;
  s_diffuse = diffuse;
  return ret;
}


void pointlike::SourceLikelihood::addBackgroundPointSource(const pointlike::SourceLikelihood* fit)
{
  if( fit==this){
    throw std::invalid_argument("SourceLikelihood::setBackgroundFit: cannot add self as background");
  }
  if( m_background==0){
    throw std::invalid_argument("SourceLikelihood::setBackgroundFit: no diffuse background");
  }
  
  if( s_verbose>0 ) {
    std::cout << "Adding source " << fit->name() << " to background" << std::endl;
  }
  m_background->add(fit);
  setDir(dir()); // recomputes background for each SimpleLikelihood object
}

void pointlike::SourceLikelihood::clearBackgroundPointSource()
{
  if( m_background==0){
    throw std::invalid_argument("SourceLikelihood::setBackgroundFit: no diffuse to add to");
  }
  while( m_background->size()>1) {
    m_background->pop_back();
  }
  setDir(dir()); // recomputes background for each SimpleLikelihood object
}

const skymaps::SkySpectrum * pointlike::SourceLikelihood::background()const
{
    return m_background;
}

/// @brief set radius for individual fits
void pointlike::SourceLikelihood::setDefaultUmax(double umax)
{ 
  pointlike::ExtendedLikelihood::setDefaultUmax(umax); 
}


double pointlike::SourceLikelihood::set_tolerance(double tol)
{
  double old(pointlike::ExtendedLikelihood::tolerance());
  pointlike::ExtendedLikelihood::setTolerance(tol);
  return old;
}


//=======================================================================
//         SLdisplay implementation
pointlike::SLdisplay::SLdisplay(const pointlike::SourceLikelihood & psl, int mode)
  : m_psl(psl)
    , m_mode(mode)
{}

double SLdisplay::value(const astro::SkyDir& dir, double e)const{
  return m_psl.display(dir, e, m_mode);
}

///@brief integral for the energy limits, in the given direction -- not impleme
double SLdisplay::integral(const astro::SkyDir& dir, double a, double b)const{
  return value(dir, sqrt(a*b));
}

std::string SLdisplay::name()const
{
  static std::string type[]={"density", "data", "background", "fit", "residual"};
    if( m_mode<0 || m_mode>4) return "illegal";
    return m_psl.name()+"--"+type[m_mode];
    
}



/////////////////////////////
//// Deprecated code  ///////
/////////////////////////////

/// minpack likelihood wrapper

#if 0
 
  int minpack_likelihood_wrapper (void *p, int m, int npar, const double *par, 
				  double *fvec,int iflag ) {
    
    if(!gSourcePointer) 
      throw std::invalid_argument("SourceLikelihood::gSourcePointer not set");
    
    double x = par[0]/180.*M_PI;
    double y = par[1]/180.*M_PI;
    //       int skip = gSourcePointer->minuitLevelsToSkip();
    // set source parameters     
    std::vector<double> src_par(npar-2,0);
    for (int i=0;i<npar-2;i++) src_par[i]=par[i+2]/180.*M_PI;
    
    // calculate new direction vector from fit parameters (delta(theta),delta(phi))
    
    CLHEP::Hep3Vector newDir = gFitStartDir+ x* gFitDeltaX + y* gFitDeltaY;
    newDir.setMag(1.);
    astro::SkyDir dirNew(newDir);
    gSourcePointer->setDir(dirNew,src_par,false);
    
    //       double TS = gSourcePointer->maximize(skip);
    double TS = gSourcePointer->maximize();
    
    int im =0;
    double llike=0;
//     for( pointlike::SourceLikelihood::iterator it = gSourcePointer->begin(); 
// 	 it!= gSourcePointer->end(); ++it){
//       pointlike::ExtendedLikelihood& like = *(it->second);
//       const std::vector<double>& gradient = like.gradient(gFitDeltaX,gFitDeltaY);
//       const std::vector<double>& sqrtLike = like.residual();
//       for(int k=0;k<sqrtLike.size();k++) {
// 	fvec[im++]=sqrtLike[k];
// 	llike+=sqrtLike[k]*sqrtLike[k];
// 	if(im==m) throw std::runtime_error("minpack vector size too small.");
//       };
//     };
    for (int k=im;k<m;k++) fvec[k]=0.;
    
    std::cout << "**** Iteration "<<gFitCounter<<" **** Testing parameters:"
	      << std::fixed<< std::setprecision(4) 
	      << " delta(X)=" << par[0]<<" deg\tdelta(Y)="<<par[1] <<" deg"
	      << std::scientific;

    for (int i=2;i<npar;i++)  std::cout <<"\tp"<<i<<"="<<par[i];
    std::cout << "\tTS: " << TS <<" im="<<im<<" llike="<<(2*llike)<<std::endl;
    ++gFitCounter;
    
    return iflag;
    
  }

void pointlike::SourceLikelihood::setBackgroundDensity(const std::vector<double>& density)
{
  std::vector<double>::const_iterator id = density.begin();
  for( iterator it = begin(); it!=end(); ++it, ++id){
    double bk(*id);
    it->second->setBackgroundDensity(bk);
  }
}

const Hep3Vector& pointlike::SourceLikelihood::ps_gradient() const{
  m_gradient=Hep3Vector(0);  
  const_iterator it = begin();
  for( ; it!=end(); ++it){
    if( (*it)->TS()< s_TScut) continue;
    Hep3Vector grad((*it)->ps_gradient());
    double curv((*it)->ps_curvature());
    if( curv > 0 ) m_gradient+= grad;
  }
  return m_gradient;
}


double pointlike::SourceLikelihood::ps_curvature() const{
  double t(0);
  const_iterator it = begin();
  for( ; it!=end(); ++it){
    if( (*it)->TS()< s_TScut) continue;
#if 0 // Marshall?
    it->second->ps_gradient();
#endif
    double curv((*it)->ps_curvature());
    if( curv>0 )  t+= curv;
  }
  return t;
}

double pointlike::SourceLikelihood::localizeMinpack(int skip){
  using std::setw; using std::left; using std::setprecision; using std::right;
  using std::fixed;
  int wd(10);
  
  astro::SkyDir last_dir(dir()); // save current direction
  setDir(dir(), m_sourceParameters, false);    // initialize
  gFitStartDir = dir()();
  
  gFitDeltaX = CLHEP::Hep3Vector(gFitStartDir.y(),-gFitStartDir.x(),0.);
  if( gFitDeltaX.mag2()<1e-10) 
    gFitDeltaX = gFitStartDir.z()>0 ? Hep3Vector(1.,0.,0.) : Hep3Vector(-1.,0.,0.);
  gFitDeltaX.setMag(1.);	     
  gFitDeltaY = gFitStartDir.cross(gFitDeltaX);
  gFitDeltaY.setMag(1.);
  
  std::cout<<"Fit start direction  : "<<gFitStartDir<<" "<<gFitStartDir.mag()<<std::endl;
  std::cout<<"Fit coordinate system: x="<<gFitDeltaX<<" y="<<gFitDeltaY<<std::endl;
  
  int npar = 2 + m_sourceParameters.size();
  
  minpack::Minpack& mini(minpack::Minpack::Minimizer());
  mini.init(20000,npar,1e-7);
  
  std::cout << "Setting likelihood for "<<npar<<" parameters : " << this << std::endl;
  
  // Set the pointer for access in the minuit function
  gSourcePointer = this;
  mini.setResidualFunc(minpack_likelihood_wrapper);
  
  setMinuitLevelSkip(skip);
  
  std::vector<double> parameter(npar);
  parameter[0]=0;
  parameter[1]=0;
  for (int i=2; i<npar; i++) parameter[i] = m_sourceParameters[i-2]*180./M_PI;
  mini.setX(parameter);
  
  mini.minimize();
  
  std::vector<double> result = mini.getX(); 
  
  for (int i=2; i<npar; i++) {
    m_sourceParameters[i-2]=result[i];
    m_sourceParErrors[i-2]=0.;
  };
  
  CLHEP::Hep3Vector newDir = gFitStartDir+ result[0]* gFitDeltaX + result[1]* gFitDeltaY;
  newDir.setMag(1.);
  astro::SkyDir dirNew(newDir);
  m_errorX=0;
  m_errorY=0;
  
  setDir(dirNew,m_sourceParameters,false);
  
  return sqrt(m_errorX*m_errorX + m_errorY*m_errorY);
}  

#endif
