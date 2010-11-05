/** @file exposure_map.cxx
@brief Classes specific to the exposure_map application

@author Toby Burnett

See the <a href="exposure_map_guide.html"> user's guide </a>.
$Header$
*/
#define CAN_IGNORE_PHI // as of version 1.6 of irfInterface::IAeff an undocumented entry was added avoid phi dependence

#include "map_tools/SkyImage.h"
#include "map_tools/Exposure.h"

#include "astro/SkyDir.h"

#include "irfInterface/IAeff.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"
#include "irfLoader/Loader.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"
#include "st_facilities/Util.h"

#include "dataSubselector/Cuts.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include <sstream>
#include <iomanip>
#include <iterator> // for ostream_iterator
#include <algorithm>
#include <iomanip>


#include <stdexcept>

namespace {
   void writeEnergies(const std::string & filename,
                      const std::vector<double> & energies) {
      std::string ext("ENERGIES");

      tip::IFileSvc & fileSvc(tip::IFileSvc::instance());
      fileSvc.appendTable(filename, ext);
      tip::Table * table = fileSvc.editTable(filename, ext);

      table->appendField("Energy", "1D");
      table->setNumRecords(energies.size());

      tip::Table::Iterator row = table->begin();
      tip::Table::Record & record = *row;
      
      std::vector<double>::const_iterator energy = energies.begin();
      for ( ; energy != energies.end(); ++energy, ++row) {
         record["Energy"].set(*energy);
      }

      delete table;
   }
   bool have_phi_dependence(false); // set below   
   double deltaphi(5.), // phi step for averaging
       phioffset(15.);  // central phi value, will be used if step is 45.
}

using namespace map_tools;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class IrfAeff
@brief function class implements effective area, as adapter to irfInterface::IAeff
*/
class IrfAeff { //: public Exposure::Aeff{
public:
    /**
    @param aeff an object from the CALDB stuff. If zero, implement linear 
    @param energy energy to evaluate
    @param cutoff limit for cos(theta)
    */
    IrfAeff(const irfInterface::IAeff* aeff, double energy, double cutoff=0.25)
        :m_aeff(aeff),m_energy(energy), m_cutoff(cutoff)
    {}

    double operator()(double costh) const
    {
        // callback if no phi dependence
        if( costh<m_cutoff ) return 0;
        if( m_aeff==0 ){
            return (costh-m_cutoff)/(1.-m_cutoff);
        }
        // make an average here
        double theta(acos(costh)*180/M_PI);
#ifdef CAN_IGNORE_PHI // new use of changed interface to avoid need to explicitly average
        const_cast<irfInterface::IAeff*>(m_aeff)->setPhiDependence(false);
        return m_aeff->value(m_energy,theta , phioffset);
#else // old explicit average code
        double sum(0); int n(0);
        for( double phi=offset; phi<45+phioffset; phi+=deltaphi, n++){
            sum+=  m_aeff->value(m_energy,theta , phi+phioffset);
        }
        return sum/n;
#endif 
    }

 
    double integral(double costh, double phi) const
    {
        // callback if found phi dependence in the livetime cube
        if( m_aeff==0 ){
            return costh<m_cutoff? 0 : (costh-m_cutoff)/(1.-m_cutoff);
        }
        if (costh<m_cutoff) return 0;
        double theta(acos(costh)*180/M_PI);
        double ret(0), phibin(3.);
        int n(0);
        return m_aeff->value(m_energy, theta, phi);
        
    }
    /// calculate etendue: integral over solid angle
    double etendue(double phi=-1) const
    {
        double dz(0.5);
        double zmin(m_cutoff+dz/2), zmax(1.0),  sum(0);
        int n(0);
        for( double z= zmin; z<zmax; z+=dz, ++n){
            if( phi<0) sum+= this->operator()(z);
            else sum+= integral(z,phi);
        }
        return sum/n*(2*M_PI)*(1-m_cutoff)/2;

    }
    const irfInterface::IAeff* m_aeff;
    double m_energy;
    double m_cutoff;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class RequestExposure 
@brief function class requests a point from the exposure
*/
template< class F>
class RequestExposure : public astro::SkyFunction
{
public:
    RequestExposure(const Exposure& exp, const F& aeff, double norm=1.0)
        : m_exp(exp)
        , m_aeff(aeff)
        , m_norm(norm)
    {}
    double operator()(const astro::SkyDir& s)const{
        if( have_phi_dependence)  return m_norm*m_exp.integral(s, m_aeff);
        return m_norm*m_exp(s,m_aeff);
    }
private:
    const Exposure& m_exp;
    const F& m_aeff;
    double m_norm;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class ExposureMapApp
@brief the exposure_map application class

*/
class ExposureMapApp : public  st_app::StApp  {
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** \brief Create an application object, performing initializations needed for running the application.
    */
    ExposureMapApp()
        : st_app::StApp()
        , m_f("ExposureMapApp", "", 2)
        , m_pars(st_app::StApp::getParGroup("gtexpcube")) 
    {
    }
        ~ExposureMapApp() throw() {} // required by StApp with gcc

    /**
    */


    irfInterface::IAeff* findAeff(std::string rspfunc)
    {
        using namespace irfInterface;
        m_f.setMethod("findAeff");

        class AeffSum : public irfInterface::IAeff {
        public:  
            AeffSum(const std::vector<std::string>& irflist){
                for(std::vector<std::string>::const_iterator sit= irflist.begin(); sit!=irflist.end(); ++sit){
                    irfInterface::Irfs* irf=IrfsFactory::instance()->create(*sit);
                    // this line means that phi dependence is never used
                    // attempts to do so have been a disaster.
                    irf->aeff()->setPhiDependence(false);
                    m_aeff.push_back(irf->aeff());
                }
            }

            virtual double value(double /*energy*/, 
                const astro::SkyDir & /*srcDir*/, 
                const astro::SkyDir &/*scZAxis*/,
                const astro::SkyDir &/*scXAxis*/,
                                 double /*time=0*/) const 
              {return 0;}

            virtual double value(double energy, double theta, double phi,
                                 double /*time=0*/) const 
            {
                // should use accumulate here
                double value = m_aeff[0]->value(energy, theta, phi);
                for( unsigned int i = 1; i < m_aeff.size(); ++i){
                    value+=m_aeff[i]->value(energy, theta, phi);
                }
                return value;
            }
            virtual IAeff * clone(){throw std::runtime_error("clone?"); return 0;};

            virtual double upperLimit() const {
               double total(0);
               for( unsigned int i = 1; i < m_aeff.size(); ++i){
                  total+=m_aeff[i]->upperLimit();
               }
               return total;
            }

        private:
            std::vector<const irfInterface::IAeff*> m_aeff;
        };


        // set up irf stuff, and translate the IRF name        

        irfLoader::Loader::go();
 
        m_f.info() << "Using Aeff(s) " ;

        if( rspfunc=="SIMPLE") {
            m_f.info() << "Simple linear form " << std::endl;
            return 0;
        }
        std::map<std::string, std::vector<std::string> > idMap = irfLoader::Loader::respIds();
        std::vector<std::string> irf_list = idMap[rspfunc];
        if( irf_list.empty()) {

            std::vector<std::string> irfnames;
            IrfsFactory::instance()->getIrfsNames(irfnames);
            if( std::find(irfnames.begin(), irfnames.end(), rspfunc) == irfnames.end() ){
            std::cerr << "\nResponse function \""<< rspfunc<< "\" Not recognized: Valid list of individual irfs: \n\t";
            std::copy(irfnames.begin(), irfnames.end(), 
                std::ostream_iterator<std::string>(std::cerr, "\n\t "));
            std::cerr <<std::endl;

            std::cerr<< "Names for groups of irfs:\n \t";
            for( std::map<std::string, std::vector<std::string> >::const_iterator it = irfLoader::Loader::respIds().begin();
                it != irfLoader::Loader::respIds().end() ; ++it)
            {
                std::cerr << "\n\t" << it->first << "\t ";
                std::copy(it->second.begin(), it->second.end(), std::ostream_iterator<std::string>(std::cerr, " "));
            }
            std::cerr << std::endl;

            throw std::invalid_argument(
                std::string("Response function not recognized: "+rspfunc));
            }
            irf_list.push_back(rspfunc);
        }

        std::stringstream buf;
        m_f.info() << "\nCombining exposure from the response function(s), specified by \""<< rspfunc<< "\": \n\t";
        std::copy(irf_list.begin(), irf_list.end(), std::ostream_iterator<std::string>(buf, "\n\t"));
        m_f.info() << buf.str()<< std::endl;
    
        return new AeffSum(irf_list );

    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void run() {

        m_f.setMethod("run()");

        prompt();
				
        // create the exposure, read it in from the FITS input file
        m_f.info() << "Creating an Exposure object from file " << m_pars["infile"].Value() << std::endl;

        ///todo: create from file, usnig HealpixArrayIO.
        std::string in_file = m_pars["infile"];
        std::string table = m_pars["table"];
        Exposure ex(in_file, table);

        const SkyBinner& bins = ex.data();
        const healpix::CosineBinner& bin = bins[0];
        int nbins = bin.size();
        bool ignorephi = m_pars["ignorephi"];
        have_phi_dependence = nbins>40 && ! ignorephi;
        if(! have_phi_dependence){
            deltaphi = m_pars["deltaphi"];
            std::clog << (ignorephi ? "\t ==> has phi dependence but ignoring it,"
                                    : "\t ==> has no phi dependence,");
#ifdef CAN_IGNORE_PHI
            std::clog << "using special flag to effective area to ignore phi"<<std::endl;
#else
            std::clog <<" will average Aeff, using deltaphi="<<deltaphi << std::endl;
#endif
        }else{
            std::clog << "\t ==> has phi dependence" << std::endl;
        }


        irfInterface::IAeff* aeff = findAeff(m_pars["irfs"]);

        //Read in theta cuts from the event file
        std::string event_file = m_pars["evfile"];
        //Initialize the cut to have the whole range
        dataSubselector::RangeCut thetaCut("THETA", "deg", ":90", 0);
        if (event_file != "" && event_file !="NONE" ) {
            std::string evtable = m_pars["evtable"];
            std::vector<std::string> eventFiles;
            st_facilities::Util::resolve_fits_files(event_file, eventFiles);
            //Read the cuts, skipping time and event cuts
            dataSubselector::Cuts * cuts = new dataSubselector::Cuts(eventFiles, evtable, false, true, true);
            //Loop over the cuts, finding cuts in Theta
            for (unsigned int i = 0; i < cuts->size(); ++i) {
                dataSubselector::CutBase & cut = const_cast<dataSubselector::CutBase &>(cuts->operator[](i));
                if (cut.type() == "range") {
                    dataSubselector::RangeCut & rangeCut = dynamic_cast<dataSubselector::RangeCut &>(cut);
                    if (rangeCut.colname() == "THETA") {
                        if (rangeCut.maxVal() < thetaCut.maxVal()) {
                            thetaCut = rangeCut;
                        }
                    }
                }
            }
            delete cuts;
        }
        double ctcutoff = cos(thetaCut.maxVal()*M_PI/180);

        m_f.info() << "cos theta cutoff used: " << (abs(ctcutoff)< 1e-6? 0: ctcutoff) << std::endl;


        // create the image object, fill it from the exposure, write out
        std::clog << "Creating an Image, will write to file " << m_pars["outfile"].Value() << std::endl;
        SkyImage image(m_pars); 
        std::vector<double> energy;
        image.getEnergies(energy);
        std::clog << "Layer  energy    etendue  miniumum    mean        maximum" << std::endl;
                  //    0    208.11      4032   5.63e+009   6.89e+009   7.93e+009
        for ( std::vector<double>::size_type layer = 0; layer != energy.size(); ++layer){
            
            IrfAeff a(IrfAeff(aeff, energy[layer],ctcutoff));
            std::clog << std::setprecision(5) 
                      << std::setw(3) << layer << std::setw(10)<< int(energy[layer]+0.5) 
                      << std::setw(10)<< int(a.etendue()+0.5)  ;

            RequestExposure<IrfAeff> req(ex, a, 1.); 
            image.fill(req, layer);
            std::clog << std::setprecision(3)
                    << std::setw(12)<< image.minimum() 
                    << std::setw(12)<< (image.count()>0? image.total()/image.count() : 0)
                    << std::setw(12)<<  image.maximum() << std::endl;
        }
        ::writeEnergies(m_pars["outfile"], energy);
    }

    void prompt() {
        m_pars.Prompt("infile");
	m_pars.Prompt("evfile");
        m_pars.Prompt("cmfile");
        m_pars.Prompt("outfile");
        m_pars.Prompt("irfs");
    
        std::string uc_cm_file = m_pars["cmfile"];
        for ( std::string::iterator itor = uc_cm_file.begin(); itor != uc_cm_file.end(); ++itor) *itor = std::toupper(*itor);
        if ("NONE" == uc_cm_file) {
            m_pars.Prompt("nxpix");
            m_pars.Prompt("nypix");
            m_pars.Prompt("pixscale");
            m_pars.Prompt("coordsys");
            m_pars.Prompt("xref");
            m_pars.Prompt("yref");
            m_pars.Prompt("axisrot");
            m_pars.Prompt("proj");
            m_pars.Prompt("emin");
            m_pars.Prompt("emax");
            m_pars.Prompt("enumbins");
        }else{
            std::clog << "Image properties copied from file " << m_pars["cmfile"].Value() << std::endl;
        }
    
        m_pars.Prompt("bincalc");
        m_pars.Prompt("deltaphi");
        m_pars.Prompt("ignorephi");
        m_pars.Prompt("table");
				m_pars.Prompt("evtable");
        m_pars.Prompt("chatter");
        m_pars.Prompt("clobber");
        m_pars.Prompt("debug");
        m_pars.Prompt("gui");
        m_pars.Save();
    }

private:
    st_stream::StreamFormatter m_f;
    st_app::AppParGroup& m_pars;
};
// Factory which can create an instance of the class above.
st_app::StAppFactory<ExposureMapApp> g_factory("gtexpcube");

/** @page exposure_map_guide exposure_map users's Guide

 - Input: an exposure cube FITS file and an effective area function.
 - Output: an image FITS file with with multiple layers for different energies

 This application  reads an exposure cube, as generated by the 
 <a href="exposure_cube_guide.html">exposure_cube</a> application. The third dimension is bins
 in theta.
 
  It creates an exposure map, or maps, for a specific effective area function, defined by the 
  "rspfunc" parameter. Values are
  @param SIMPLE - simple linear function
  @param DC1F - DC1 parametrization for the front
  @param DC1B - DC1 parametraizatio for the back
  @param DC1FB - sum o the above

  The parameters describing the output image are 
  @param pixelsize degrees per pixel
  @param npix  number of pixels across
  @param projtype CAR for cartesian, AIT for Hammer-Aitoff, etc.


- The parameter file
 @verbinclude exposure_map.par
  Print Version 
exposure_map
Generates an exposure map, or a set of exposure maps for different energies, from a livetime cube written by gtltcube. 
@verbatim
Prerequisites
Input Files: 
Livetime or Exposure cube FITS file from gtltcube 
Optionally, a counts map FITS file from gtbin, for which exposure_map should match the coordinate projection and gridding.  The counts map also specifies the effective area response functions for exposure_map to use. 
Links: 

Basic FTOOL Parameter Usage 
General Parameters
  infile [file]  
    Exposure or Livetime cube input file.  
      
  cmfile [file] 
    Count map input file (NONE to determine map geometry from parameters). 

  evtfile [file]
    Scan for cuts to apply to theta. Ignore if NONE.

  outfile [file] 
    Exposure map output file name (FITS format  image). 
      
  resptype = "P6_V1_DIFFUSE" [string]
     Response function. Default DC2 means the sum of classA and class B   
    
  nxpix = 1 [int] 
    Size of the X axis in pixels. Default (1) for full sky
    
  nypix = 1 [int] 
    Size of the Y axis in pixels. Default (1) to copy numxpix, or full sky
    
  pixscale = 1.0 [float]  
    Image scale (in degrees/pixel).  
    
  coordsys = "CEL" [string] 
    Coordinate system, CEL or GAL. 
    
  xref = 0. [float] 
    First coordinate of image center in degrees (RA or Galactic l). (default 0) 
    
  yref = 0. [float]  
    Second coordinate of image center in degrees (DEC or Galactic b). (default 0)
    
  axisrot=0. [float] 
    Rotation angle of image axis, in degrees. (default 0)
    
  (proj = "AIT") [string] 
    Coordinate projection (AIT|ZEA|ARC|CAR|GLS|MER|NCP|SIN|STG|TAN); see Calabretta & Greisen 2002, A&A, 395, 1077 for definitions of the projections.  
    Must be AIT, ZEA or CAR for auto full sky. 
    
  emin = 30 [float]
     Start value for first energy bin (MeV); must be >=30 MeV. 
    
  emax = 200000 [float] 
    Stop value for last energy bin; must be <=200 GeV. Will use this if enumbins is 1
    
  enumbins [int] 
    Number of logarithmically uniform energy bins, defaults to 8. 
    
  (bincalc = "CENTER") [string] 
     How are energy layers computed from count map ebounds?  Options are CENTER and EDGE
    
  (table = "Exposure")  
    Exposure cube extension. 
    
  (chatter = 2) [int] 
    Chattiness of output. 
    
  (clobber = "yes") [boolean]  
    Overwrite existing output files with new output files. 
    
  (debug = "no") [boolean] 
    Debugging mode activated. 
    
  (gui = "no") [boolean]  
    Gui mode activated. 
    
  (mode = "ql") [string] 
    Mode of automatic parameters. 

Also See
gtlivetimecube 
Basic FTOOL Parameter Usage 

--------------------------------------------------------------------------------

 

Owned by: Toby Burnett  

Generated on: Mar 16 21:53:43 2006 

Last updated by: Chuck Patterson 03/20/2006 Back to Top  

 @endverbatim
 

 

 

 
 

 

*/
