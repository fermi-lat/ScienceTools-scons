/** @file Exposure.cxx
   $Header$
*/
#include "map_tools/Exposure.h"
#include "astro/SkyDir.h"
#include "tuple/ITable.h"


#include <iostream>
#include <fstream>
#include <stdexcept>
#include <numeric>
#include <algorithm>

double  Exposure::Index::skybinsize=1;
int     Exposure::Index::ra_factor=360;
int     Exposure::Index::dec_factor=180;
int     Exposure::Index::cosfactor=40;
double  Exposure::Index::cosmin = 0;

//------------------------------------------------------------------------------
Exposure::Exposure(double skybin, double costhetabin) : m_total(0)
{
    // set binsizes in the key
    Index::skybinsize = int(skybin);
    Index::ra_factor  = int(360./skybin);
    Index::dec_factor = int(180./skybin);
    Index::cosfactor  = int((1.-Index::cosmin)/costhetabin);

    //total size to reserve
    unsigned int size= Index::ra_factor * Index::dec_factor * Index::cosfactor;
    m_exposureMap.resize(size);
    std::cout << "Creating a exposure hypercube, size " << size 
        << "="<< Index::ra_factor << " x "<< Index::dec_factor << " x "<< Index::cosfactor << std::endl;
    std::fill(m_exposureMap.begin(), m_exposureMap.end(), 0);
}
//------------------------------------------------------------------------------
Exposure::Exposure(const ExposureCube& cube, double total)
: m_exposureMap(cube), m_total(total)
{
    unsigned int size= Index::ra_factor * Index::dec_factor * Index::cosfactor;

    std::cout << "Loaded exposure map from a hypercube, size is " << m_exposureMap.size();
    if( size != m_exposureMap.size() ) {
        throw std::invalid_argument("wrong size");
    }
#if 0
    double tot = std::accumulate(m_exposureMap.begin(), m_exposureMap.end(), 0.0);
    std::cout << ", average exposure: " << tot/m_exposureMap.size() << std::endl;
#endif
}

//------------------------------------------------------------------------------
void Exposure::load(const std::string& textInputFilename, double tfirst, double tlast) {
    //find out if we're using an ASCII file for input here:
    std::ifstream input_file;
    if(! textInputFilename.empty()){
        
        input_file.open(textInputFilename.c_str());
        if(! input_file.is_open())
        {
            throw std::invalid_argument(std::string("Unable to open file:  ")+ textInputFilename);
            exit(0);
        }
        //ok, the file is open - now, get all the data while the file still has more:
        double intrvalstart,posx,posy,posz,raz,decz,rax,decx,razenith,deczenith,lon,lat,alt,count=0;
        double m_currentLastTime;
        m_total=0;
        while (!input_file.eof() /*&& count<2*/ ){
            count++;	

            input_file >>intrvalstart; 
            input_file >>posx >> posy >>posz;
            input_file >>raz  >> decz;
            input_file >>rax  >> decx;
            input_file >>razenith >>deczenith;
            input_file >>lon >>lat >>alt;
            //set the initial livetime, if it needs it:
            if(count==1)m_currentLastTime = intrvalstart;

            if( intrvalstart<tfirst) continue;
            if( tlast!=0 && intrvalstart>tlast) break;

            double deltat = intrvalstart-m_currentLastTime;

            findExposed(raz,decz,deltat);
            m_currentLastTime = intrvalstart;
            m_total+=deltat;
        }
    }
}
//------------------------------------------------------------------------------
void Exposure::load(tuple::ITable& tuple, double tstart, double tend)
{
    const double & ra = tuple.selectColumn("ra_scz");
    const double & dec = tuple.selectColumn("dec_scz");
    const double & start = tuple.selectColumn("start");
    const double & stop = tuple.selectColumn("stop");
    const double & livetime = tuple.selectColumn("livetime");

    for( tuple::Iterator it=tuple.begin(); it!=tuple.end(); ++it){
       if( start < tstart ) continue;
       if( stop > tend ) break;
       double deltat = livetime > 0 ? livetime : stop-start;
       findExposed( ra, dec, deltat); 
        m_total += deltat;
    }
}
//------------------------------------------------------------------------------
double Exposure::operator()(double ra, double dec, const Aeff& fun)const
{
    ///integrate the exposure at (ra,dec):

    double currentExposure = 0.;
    int index = Index(ra,dec); 
    for( int i=0; i< Index::cosfactor; ++i){
        double cosTheta = Index::costheta_value(i), 
            aeff_val = fun(cosTheta),
            map_val =  m_exposureMap[index];
        currentExposure += map_val * aeff_val;
        index+= Index::ra_factor*Index::dec_factor;
    }
    return currentExposure;
}
//------------------------------------------------------------------------------
double Exposure::operator()(const astro::SkyDir& dir, const Aeff& fun)const
{ 
    return operator()(dir.ra(), dir.dec(), fun);
}
//------------------------------------------------------------------------------
void Exposure::findExposed(double ra,double dec, double deltat){ 
    astro::SkyDir pos(ra,dec);

    if(deltat==0) return;
    for(double lprime=-180.+0.5*Index::skybinsize; lprime <180. ; lprime += Index::skybinsize){
        for(double bprime=-90.+0.5*Index::skybinsize; bprime <90. ; bprime += Index::skybinsize){
            astro::SkyDir prime(lprime, bprime);
            double cosdiff = pos()*prime();
            if( cosdiff> Index::cosmin)
                m_exposureMap[Index(lprime, bprime, cosdiff)]+=deltat;
        }
    }
}
