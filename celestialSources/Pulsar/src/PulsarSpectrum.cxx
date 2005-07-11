/////////////////////////////////////////////////
// File PulsarSpectrum.cxx
// Implementation of PulsarSpectrum class
//////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <iomanip>
#include "Pulsar/PulsarSpectrum.h"
#include "Pulsar/PulsarConstants.h"
#include "SpectObj/SpectObj.h"
#include "flux/SpectrumFactory.h"

#define DEBUG 0

using namespace cst;

/////////////////////////////////////////////////
ISpectrumFactory &PulsarSpectrumFactory() 
 {
   static SpectrumFactory<PulsarSpectrum> myFactory;
   return myFactory;
 }

/////////////////////////////////////////////////
/*!
 * \param params string containing the XML parameters
 *
 * This method takes from the XML file the name of the pulsar to be simulated, the model
 * to be used and the parameters specific for the choosen model. Then extract from the
 * TXT DataList file the characteristics of the choosen pulsar (e.g. period, flux, period derivatives
 * ,ephemerides, flux, etc...)
 * Then it calls the PulsarSim class in order to have the TH2D histogram.
 * For more informations and fot a brief tutorial about the use of PulsarSpectrum  please see:
 * <br>
 * <a href="#dejager02">http://www.pi.infn.it/~razzano/Pulsar/PulsarSpTutor/PulsarSpTutor.htm </a>
 */

//PulsarSpectrum::PulsarSpectrum(const std::string& params)
  //: m_params(params), m_solSys(astro::SolarSystem::EARTH)


PulsarSpectrum::PulsarSpectrum(const std::string& params)
  : m_params(params), m_solSys(astro::SolarSystem::EARTH)
{

  // Reset all variables;
  m_RA = 0.0;
  m_dec = 0.0;
  m_l = 0.0;
  m_b = 0.0;
  m_flux = 0.0;
  m_period = 0.0;
  m_pdot = 0.0;
  m_p2dot = 0.0;
  m_t0 = 0.0;
  m_phi0 = 0.0;
  m_model = 0;
  m_seed = 0;


  //Read from XML file
  m_PSRname    = parseParamList(params,0).c_str();            // Pulsar name

  m_RA = std::atof(parseParamList(params,1).c_str());         // Pulsar Right Ascension
  m_dec = std::atof(parseParamList(params,2).c_str());         // Pulsar Declination

  m_enphmin    = std::atof(parseParamList(params,3).c_str()); // minimum energy of extracted photons
  m_enphmax    = std::atof(parseParamList(params,4).c_str()); // minimum energy of extracted photons

  m_model      = std::atoi(parseParamList(params,5).c_str()); // choosen model

  m_seed       = std::atoi(parseParamList(params,6).c_str()); //Model Parameters: Random seed
  m_numpeaks   = std::atoi(parseParamList(params,7).c_str()); //Model Parameters: Number of peaks
  double ppar1 = std::atof(parseParamList(params,8).c_str()); // model parameters
  double ppar2 = std::atof(parseParamList(params,9).c_str());
  double ppar3 = std::atof(parseParamList(params,10).c_str());
  double ppar4 = std::atof(parseParamList(params,11).c_str());


  int Retrieved = getPulsarFromDataList();
  
  if (DEBUG==0)
    {
      if (Retrieved == 1)
	{
	  std::cout << "Pulsar " << m_PSRname << " found in Datalist file! " << std::endl;
	} 
      else
	{
	  std::cout << "ERROR! Pulsar " << m_PSRname << " NOT found in Datalist.File...Exit. " << std::endl;
	  exit(1);	
	}
    }

  m_period = m_periodVect[0];
  m_pdot = m_pdotVect[0];
  m_p2dot = m_p2dotVect[0];
  m_phi0 = m_phi0Vect[0];
  m_t0Init = m_t0InitVect[0];
  m_t0 = m_t0Vect[0];
  m_t0End = m_t0EndVect[0];


  //Init SolarSystem stuffs useful for barycentric decorrections

  astro::JulianDate JDStart(2007, 1, 1, 0.0);
  m_earthOrbit = new astro::EarthOrbit(JDStart); 
  
  //  astro::SolarSystem::Body body = astro::SolarSystem::MERCURY;
  //Body body = SUN;
  //std::cout << " body is " << body << std::endl;
  //  astro::SolarSystem m_solSys(Body body=EARTH);
  //  astro::SolarSystem m_solSys(astro::SolarSystem():EARTH);
  // m_solSys* = new astro::SolarSystem(astro::SolarSystem::EARTH);
  //PulsarSpectrum::PulsarSpectrum(const std::string& params)
  //: m_params(params), m_solSys(astro::SolarSystem::EARTH)



  astro::SkyDir m_PulsarDir(m_RA,m_dec,astro::SkyDir::EQUATORIAL);
  m_PulsarVectDir = m_PulsarDir.dir();
  m_GalDir = std::make_pair(m_PulsarDir.l(),m_PulsarDir.b());
  m_l = m_GalDir.first;
  m_b = m_GalDir.second;

  if (DEBUG)
    {
      //Writes out Pulsar Info
      std::cout << " ********   PulsarSpectrum initialized for Pulsar " << m_PSRname << std::endl;
      std::cout << "**   Name : " << m_PSRname << std::endl;
      std::cout << "**   Position : (RA,Dec)=(" << m_RA << "," << m_dec 
		<< ") ; (l,b)=(" << m_l << "," << m_b << ")" << std::endl; 
      std::cout << "**   Flux above 100 MeV : " << m_flux << " ph/cm2/s " << std::endl;
      
      for (unsigned int n=0; n < m_periodVect.size(); n++)
	{

	  m_f0 = 1.0/m_periodVect[n];
	  m_f1 = -m_pdot/(m_periodVect[n]*m_periodVect[n]);
	  m_f2 = 2*pow((m_pdotVect[n]/m_periodVect[n]),2.0)/m_periodVect[n] - m_p2dotVect[n]/(m_periodVect[n]*m_periodVect[n]);

	  std::cout << "**   Ephemerides valid from " << m_t0InitVect[n] 
		    << " to " << m_t0EndVect[n] << " (MJD): " << std::endl;
	  std::cout << "**     Epoch (MJD) :  " << m_t0Vect[n] << std::endl;
	  std::cout << "**     Phi0 (at Epoch t0) : " << m_phi0Vect[n] << std::endl;
	  std::cout << "**     Period : " << m_periodVect[n] << " s. | f0: " << m_f0 << std::endl;
	  std::cout << "**     Pdot : " <<  m_pdotVect[n]  << " | f1: " << m_f1 << std::endl; 
	  std::cout << "**     P2dot : " <<  m_p2dotVect[n]  << " | f2: " << m_f2 << std::endl; 
	}


      std::cout << "**\n**   Enphmin : " << m_enphmin << " keV | Enphmax: " << m_enphmax << " keV" << std::endl;
      std::cout << "**   Mission started at (MJD) : " << StartMissionDateMJD << " (" 
		<< std::setprecision(12) << (StartMissionDateMJD+JDminusMJD)*SecsOneDay 
		<< " sec.) - Jan,1 2007 00:00.00 (TT)" << std::endl;
      std::cout << "**************************************************" << std::endl;

    }

  //writes out an output log file

  std::string logLabel = m_PSRname + "Log.txt";

  ofstream PulsarLog(logLabel.c_str());
 
  
  PulsarLog << "\n********   PulsarSpectrum Log for pulsar" << m_PSRname << std::endl;
  PulsarLog << "**   Name : " << m_PSRname << std::endl;
  PulsarLog << "**   Position : (RA,Dec)=(" << m_RA << "," << m_dec 
	    << ") ; (l,b)=(" << m_l << "," << m_b << ")" << std::endl; 
  PulsarLog << "**   Flux above 100 MeV : " << m_flux << " ph/cm2/s " << std::endl;

  for (unsigned int n=0; n < m_periodVect.size(); n++)
    {
     
  
      m_f0 = 1.0/m_periodVect[n];
      m_f1 = -m_pdot/(m_periodVect[n]*m_periodVect[n]);
      m_f2 = 2*pow((m_pdotVect[n]/m_periodVect[n]),2.0)/m_periodVect[n] - m_p2dotVect[n]/(m_periodVect[n]*m_periodVect[n]);
  

      PulsarLog << "**   Ephemerides valid from " << m_t0InitVect[n] 
		<< " to " << m_t0EndVect[n] << " (MJD): " << std::endl;
      PulsarLog << "**     Epoch (MJD) :  " << m_t0Vect[n] << std::endl;
      PulsarLog << "**     Phi0 (at Epoch t0) : " << m_phi0Vect[n] << std::endl;
      PulsarLog << "**     Period : " << m_periodVect[n] << " s. | f0: " << m_f0 << std::endl;
      PulsarLog << "**     Pdot : " <<  m_pdotVect[n]  << " | f1: " << m_f1 << std::endl; 
      PulsarLog << "**     P2dot : " <<  m_p2dotVect[n]  << " | f2: " << m_f2 << std::endl; 
    }

  PulsarLog << "**\n**   Enphmin : " << m_enphmin << " keV | Enphmax: " << m_enphmax << " keV" << std::endl;
  PulsarLog << "**   Mission started at (MJD) : " << StartMissionDateMJD << " (" 
	    << std::setprecision(12) << (StartMissionDateMJD+JDminusMJD)*SecsOneDay 
	    << " sec.) - Jan,1 2007 00:00.00 (TT)" << std::endl;
  PulsarLog << "**************************************************" << std::endl;
  PulsarLog << "**   Model chosen : " << m_model << " --> Using Phenomenological Pulsar Model " << std::endl;  


  m_f0 = 1.0/m_periodVect[0];
  m_f1 = -m_pdot/(m_periodVect[0]*m_periodVect[0]);
  m_f2 = 2*pow((m_pdotVect[0]/m_periodVect[0]),2.0)/m_periodVect[0] - m_p2dotVect[0]/(m_periodVect[0]*m_periodVect[0]);

  //Instantiate an object of PulsarSim class
  m_Pulsar    = new PulsarSim(m_PSRname, m_seed, m_flux, m_enphmin, m_enphmax, m_period, m_numpeaks);
 
  //Instantiate an object of SpectObj class
  if (m_model == 1)
    {
  
      m_spectrum = new SpectObj(m_Pulsar->PSRPhenom(ppar1,ppar2,ppar3,ppar4),1);
  
      m_spectrum->SetAreaDetector(EventSource::totalArea());
  

      
      PulsarLog << "**   Effective Area set to : " << m_spectrum->GetAreaDetector() << " m2 " << std::endl; 
      if (DEBUG)
	{
	  std::cout << "**   Model chosen : " << m_model << " --> Using Phenomenological Pulsar Model " << std::endl;  
	  std::cout << "**  Effective Area set to : " << m_spectrum->GetAreaDetector() << " m2 " << std::endl; 
	}  
    }
  else 
    {
      std::cout << "ERROR!  Model choice not implemented " << std::endl;
      exit(1);
    }

  PulsarLog.close();

}

/////////////////////////////////////////////////
PulsarSpectrum::~PulsarSpectrum() 
{  
  delete m_Pulsar;
  delete m_spectrum;
  delete m_earthOrbit;
}

/////////////////////////////////////////////////
double PulsarSpectrum::flux(double time) const
{
  double flux;	  
  flux = m_spectrum->flux(time,m_enphmin);
  return flux;
}


/////////////////////////////////////////////////
/*!
 * \param time time to start the computing of the next photon
 *
 * This method find the time when the next photon will come. It takes into account decorrections due to
 * ephemerides and period derivativers and barycentric decorrections.
 * For the ephemerides decorrections the parameters used are:
 * <ul>
 * <li> <i>Epoch</i>, espressed in MJD;</li>
 * <li> <i>Initial Phase Phi0</i>;</li>
 * <li> <i>First Period derivative</i>;</li>
 * <li> <i>Second Period derivative</i>;</li>
 * </ul>
 * <br>
 * The barycentric decorrections takes into account conversion TDB->TT, Geometrical and Shapiro delays.
 * This method also calls the interval method in SpectObj class to determine the next photon in a rest-frame without 
 * ephemerides effects
 */
double PulsarSpectrum::interval(double time)
{  



  double timeTildeDecorr = time + (StartMissionDateMJD)*SecsOneDay; //Arrival time decorrected

  std::cout << std::setprecision(30) << " At " << time << " --> " << timeTildeDecorr << "coorection is**** " << std::endl;



  // std::cout << getBaryCorr(timeTildeDecorr) << std::endl; 

  //  std::cout << " Fine " << std::endl;




  if (((timeTildeDecorr/SecsOneDay) < m_t0Init) || ((timeTildeDecorr/SecsOneDay) > m_t0End))
    {
      


      for (unsigned int i=0; i < m_t0Vect.size(); i++)
	{
	  if (((timeTildeDecorr/SecsOneDay) > m_t0InitVect[i]) &&  ((timeTildeDecorr/SecsOneDay) < m_t0EndVect[i]))
	    {
	      m_t0 = m_t0Vect[i];
	      m_t0Init = m_t0InitVect[i];
	      m_t0End = m_t0EndVect[i];
	      m_period = m_periodVect[i];
	      m_pdot = m_pdotVect[i];
	      m_p2dot = m_p2dotVect[i];
	      m_phi0 = m_phi0Vect[i];

	      if (DEBUG)
		{
		  std::cout << " Switching ephemerides.. at " << timeTildeDecorr/SecsOneDay << " MJD " << std::endl;	
		  std::cout << " New ephemerides valid from " 
			    << m_t0Init << " to " << m_t0End << " ( Ref.Epoch:  " << m_t0  << " ) " << std::endl;
		}

	    }
	}
      
    }



  double timeTilde = timeTildeDecorr + getBaryCorr(timeTildeDecorr); //should be corrected before applying ephem de-corrections

    if (DEBUG)
  {
    if ((int(timeTilde - (StartMissionDateMJD)*SecsOneDay) % 1000) < 1.5)
      std::cout << "**  Time reached is: " << timeTilde-(StartMissionDateMJD)*SecsOneDay
		<< " seconds from Mission Start  - pulsar " << m_PSRname << std::endl;
  }

  //First part: Ephemerides calculations...
  double initTurns = getTurns(timeTilde); //Turns made at this time 
  double intPart; //Integer part
  double tStart = modf(initTurns,&intPart)*m_period; // Start time for interval
  //determines the interval to the next photon (at nextTime) in the case of period constant.
  double inte = m_spectrum->interval(tStart,m_enphmin); //deltaT (in system where Pdot = 0
  double inteTurns = inte/m_period; // conversion to # of turns
  double totTurns = initTurns + inteTurns; //Total turns at the nextTimeTilde
  double nextTimeTilde = retrieveNextTimeTilde(timeTilde, totTurns, (ephemCorrTol/m_period));
  
  //Second part: baycentric decorrection
  //Use the bisection method to find the inverse of the de-corrected time, i.e. the time (in TT)
  //that after correction is equal to Time in TDB

  double deltaMax = 510.0; //max deltat (s)
  double ttUp = nextTimeTilde + deltaMax;
  double ttDown = nextTimeTilde - deltaMax;
  double ttMid = (ttUp + ttDown)/2;

  int i = 0;
   double hMid = 1e30; //for the 1st iteration


    

  while (fabs(hMid)>baryCorrTol )
    {
      //      double hUp = (nextTimeTilde - (ttUp + getBaryCorr(ttUp)));
      double hDown = (nextTimeTilde - (ttDown + getBaryCorr(ttDown)));
      hMid = (nextTimeTilde - (ttMid + getBaryCorr(ttMid)));
                
      // std::cout << std::setprecision(30) 
      //	<< "\n" << i << "**ttUp " << ttUp << " ttDown " << ttDown << " mid " << ttMid << std::endl;
      //std::cout << "  hUp " << hUp << " hDown " << hDown << " hMid " << hMid << std::endl;
      
      if (fabs(hMid) < baryCorrTol) break;
      if ((hDown*hMid)>0)
	{
	  ttDown = ttMid;
	  ttMid = (ttUp+ttDown)/2;
	}
      else 
	{
	  ttUp = ttMid;
	  ttMid = (ttUp+ttDown)/2;
	}
       i++;
    }
  
   
   double nextTimeDecorr = ttMid;
  

   //double nextTimeDecorr = nextTimeTilde;

  if (DEBUG)
    { 
     std::cout << "\nTimeTildeDecorr at Spacecraft (TT) is: " 
	       << timeTildeDecorr - (StartMissionDateMJD)*SecsOneDay << "sec." << std::endl;
     std::cout << "TimeTilde at SSB (in TDB) is: " << timeTilde - (StartMissionDateMJD)*SecsOneDay << "sec." << std::endl;
     std::cout << "nextTimeTilde at SSB (in TDB) is:" << nextTimeTilde - (StartMissionDateMJD)*SecsOneDay << "sec." << std::endl;
     std::cout << " nextTimeTilde decorrected (TT)is" << nextTimeDecorr - (StartMissionDateMJD)*SecsOneDay << "sec." << std::endl;
     std::cout << " corrected is " << nextTimeDecorr + getBaryCorr(nextTimeDecorr) - (StartMissionDateMJD)*SecsOneDay << std::endl;
     std::cout << " interval is " <<  nextTimeDecorr - timeTildeDecorr << std::endl;
  }
  
    return nextTimeDecorr - timeTildeDecorr; //interval between the de-corected times
  //  return nextTimeTilde - timeTildeDecorr; //interval between the de-corected times
  
  // return 10.0;

}

/////////////////////////////////////////////
/*!
 * \param time time where the number of turns is computed
 *
 * This method compute the number of turns made by the pulsar according to the ephemerides.
 * <br>
 * <blockquote>
 * f(t) = f<sub>0</sub> + f<sub>1</sub>(t - t<sub>0</sub>) +
 *      f<sub>2</sub>/2(t - t<sub>0</sub>)<sup>2</sup>.
 * <br>The number of turns is related to the ephemerides as:<br>
 * dN(t) = f<sub>0</sub> + f<sub>1</sub>(t-t<sub>0</sub>) + 1/2f<sub>2</sub>(t-t<sub>0</sub>)<sup>2</sup>
 * </blockquote>
 * <br>where 
 * <blockquote>
 *<ul>
 * <li> t<sub>0</sub> is an ephemeris epoch.In this simulator epoch has to be expressed in MJD</li>
 * <li>f<sub>0</sub> pulse frequency at time t<sub>0</sub> (usually given in Hz)</li>,
 * <li>f<sub>1</sub> the 1st time derivative of frequency at time t< sub>0</sub> (Hz s<sup>-1</sup>);</li>
 * <li>f<sub>2</sub> the 2nd time derivative of frequency at time t<sub>0</sub> (Hz s<sup>-2</sup>).</li>
 * </ul>
 * </blockquote>
 */
double PulsarSpectrum::getTurns( double time )
{
  double dt = time - m_t0*SecsOneDay;
  return m_phi0 + m_f0*dt + 0.5*m_f1*dt*dt + ((m_f2*dt*dt*dt)/6.0);
}

/////////////////////////////////////////////
/*!
 * \param tTilde Time from where retrieve the nextTime;
 * \param totalTurns Number of turns completed by the pulsar at nextTime;
 * \param err Phase tolerance between totalTurns and the number of turns at nextTime
 *
 * <br>In this method a recursive way is used to find the <i>nextTime</i>. Starting at <i>tTilde</i>, the method returns 
 * nextTime, the time where the number of turns completed by the pulsar is equal to totalTurns.  
 */
double PulsarSpectrum::retrieveNextTimeTilde( double tTilde, double totalTurns, double err )
{

  double tTildeUp = tTilde;
  double tTildeDown = tTilde;  
  double NTdown = totalTurns - getTurns(tTildeDown); 
  double NTup = totalTurns - getTurns(tTildeUp);
  int u = 0;
  while ((NTdown*NTup)>0)
    {
      u++;
      tTildeUp = tTilde+m_period*pow(2.0,u); 
      NTdown = totalTurns - getTurns(tTildeDown); 
      NTup = totalTurns - getTurns(tTildeUp);
    }
  
  double tTildeMid = (tTildeDown+tTildeUp)/2.0;
  double NTmid = totalTurns - getTurns(tTildeMid);

  while(fabs(NTmid) > err )
    { 
      if (fabs(NTmid) < err) break;
      NTmid = totalTurns - getTurns(tTildeMid);
      NTdown = totalTurns - getTurns(tTildeDown); 
      if ((NTmid*NTdown)>0)
	{
	  tTildeDown = tTildeMid;
	  tTildeMid = (tTildeDown+tTildeUp)/2.0;
	} 
      else 
	{
	  tTildeUp = tTildeMid;
	  tTildeMid = (tTildeDown+tTildeUp)/2.0;
	}
    }
  if (DEBUG)
    {
      std::cout << "**  RetrieveNextTimeTilde " << std::endl;
      std::cout << std::setprecision(30) << "  Stop up is " << tTildeUp << " NT " << NTup << std::endl;
      std::cout << std::setprecision(30) << "        down is " << tTildeDown << " NT " << NTdown << " u= " << u << std::endl;
      std::cout << std::setprecision(30) << "        mid is " << tTildeMid << " NT " << NTmid << " u= " << u << std::endl;
      std::cout << "     nextTimeTilde is " << tTildeMid << std::endl;
    }

  return tTildeMid;
}

/////////////////////////////////////////////
/*!
 * \param ttInput Photon arrival time at spacecraft in Terrestrial Time (expressed in MJD converted in seconds)
 *
 * <br>
 * This method computes the barycentric corrections for the photon arrival time at spacecraft and returns 
 * the time in Solar System Barycentric Time. The corrections implemented at the moment are:
 * <ul>
 *  <li> Conversion from TT to TDB;
 *  <li> Geometric Time Delay due to light propagation in Solar System;
 *  <li> Relativistic Shapiro delay;
 * </ul>   
 */
double PulsarSpectrum::getBaryCorr( double ttInput )
{
  //Start Date;
  astro::JulianDate ttJD(2007, 1, 1, 0.0);
  ttJD = ttJD+(ttInput - (StartMissionDateMJD)*SecsOneDay)/SecsOneDay;

  if (DEBUG)
    {
      std::cout << std::setprecision(30) << "\nBarycentric Corrections for time " << ttJD << " (JD)" << std::endl;
    }

  // Conversion TT to TDB, from JDMissionStart (that glbary uses as MJDref)
  double tdb_min_tt = m_earthOrbit->tdb_minus_tt(ttJD);

  //Correction due to geometric time delay of light propagation 
  Hep3Vector GeomVect = (m_earthOrbit->position(ttJD)/clight) - m_solSys.getBarycenter(ttJD);
  double GeomCorr = GeomVect.dot(m_PulsarVectDir);

  //Correction due to Shapiro delay.
  Hep3Vector sunV = m_solSys.getSolarVector(ttJD);

  // Angle of source-sun-observer
  double costheta = - sunV.dot(m_PulsarVectDir) / ( sunV.mag() * m_PulsarVectDir.mag() );
  double m = 4.9271e-6; // m = G * Msun / c^3
  double ShapiroCorr = 2.0 * m * log(1+costheta);

  if (DEBUG)
    {
      std::cout << std::setprecision(20) << "** --> TDB-TT = " << tdb_min_tt << std::endl;
      std::cout << std::setprecision(20) << "** --> Geom. delay = " << GeomCorr << std::endl;
      std::cout << std::setprecision(20) << "** --> Shapiro delay = " << ShapiroCorr << std::endl;
      std::cout << std::setprecision(20) << "** ====> Total= " << tdb_min_tt + GeomCorr + ShapiroCorr  << " s." <<std::endl;
    }  

  return tdb_min_tt + GeomCorr + ShapiroCorr; //seconds

}

/////////////////////////////////////////////
int PulsarSpectrum::getPulsarFromDataList()
{
  int Status = 1;

  char* pulsar_root = ::getenv("PULSARROOT");
  char sourceFileName[80];
  std::ifstream PulsarDataTXT;
  //Read from PulsarDataList.txt and find the data correspondind go choosen pulsar

  sprintf(sourceFileName,"%s/data/PulsarDataList.txt",pulsar_root);  
  if (DEBUG)
    {
      std::cout << "\nOpening Pulsar Datalist File : " << sourceFileName << std::endl;
    }
  PulsarDataTXT.open(sourceFileName, std::ios::in);
  
  if (! PulsarDataTXT.is_open()) 
    {
      std::cout << "Error opening Datalist file " << sourceFileName  
		<< " (check whether $PULSARROOT is set" << std::endl; 
      Status = 0;
      exit(1);
    }
  
  char aLine[200];  
  PulsarDataTXT.getline(aLine,200); 

  char tempName[15] = "";

  double flux, period, pdot, p2dot, t0Init, t0, t0End, phi0;

  while ( PulsarDataTXT.eof() != 1)
    {

      PulsarDataTXT >> tempName >> flux >> period >> pdot >> p2dot >> t0Init >> t0 >> t0End  >> phi0;
     
      if (std::string(tempName) == m_PSRname)
	{
	  Status = 1;
	  m_flux = flux;
	  m_periodVect.push_back(period);
	  m_pdotVect.push_back(pdot);
	  m_p2dotVect.push_back(p2dot);
	  m_t0InitVect.push_back(t0Init);
	  m_t0Vect.push_back(t0);
	  m_t0EndVect.push_back(t0End);
	  m_phi0Vect.push_back(phi0);
	}
    }

  return Status;
}


/////////////////////////////////////////////
double PulsarSpectrum::energy(double time)
{
  return m_spectrum->energy(time,m_enphmin)*1.0e-3; //MeV
}

/////////////////////////////////////////////
/*!
 * \param input String to be parsed;
 * \param index Position of the parameter to be extracted from input;
 *
 * <br>
 * From a string contained in the XML file a parameter is extracted according to the position 
 * specified in <i>index</i> 
 */
std::string PulsarSpectrum::parseParamList(std::string input, unsigned int index)
{
  std::vector<std::string> output;
  unsigned int i=0;
  
  for(;!input.empty() && i!=std::string::npos;){
   
    i=input.find_first_of(",");
    std::string f = ( input.substr(0,i).c_str() );
    output.push_back(f);
    input= input.substr(i+1);
  } 

  if(index>=output.size()) return "";
  return output[index];
};
