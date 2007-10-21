/** @file MakePointingHistory.cxx
@ brief application to create a pointing history


*/
#include "MakePointingHistory.h"

#include "astro/EarthCoordinate.h"
#include "astro/PointingInfo.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cassert>

using namespace astro;

typedef std::pair<double, double> coords;
namespace {
    // default SAA boundary
    double 
        latv[]={-30,-26,-20,-17,-10, 1, 2, -3, -8,-12,-19,-30},
        lonv[]={ 45, 41, 31, 9,-11,-34,-46,-62,-79,-85,-89,-87};
}

MakePointingHistory::MakePointingHistory(std::string filename)
:m_history(filename)
{
    std::cout << "loaded history file " << filename << ": begin=" 
        << std::setprecision(8) <<  m_history.startTime() <<", endtime="<< m_history.endTime() 
        << std::endl;
}
MakePointingHistory::~MakePointingHistory(){}

/// make the list
void MakePointingHistory::run(double begin, double end,  double interval)
{
    if( begin==0) begin = m_history.startTime();
    if( end==0)   end   = m_history.endTime();

    m_begin = begin;
    int entries(0), saa(0);
    bool lastSAA(SAA(begin));

    for( double time=begin; time<end; time +=interval, ++entries){
        const PointingInfo& point = m_history(time);
        bool inSAA = point.earthCoord().insideSAA();
        if(  inSAA != lastSAA && time>begin){
            ++saa;
            lastSAA = inSAA;
            double transition = saa_transition(time-interval, time);
            make_entry(point, time, transition, !inSAA);
            make_entry(m_history(transition), transition, time+interval, inSAA);
        }else{
            make_entry(point, time, time+interval, inSAA);

        }
    }
    std::cout << "Generated " << entries << " entries, found " << saa << " SAA transitions" << std::endl;
}

void MakePointingHistory::make_entry(const PointingInfo& point, double start, double stop, bool inSAA)
{
    std::cout << std::left<< std::setw(15) << start-m_begin << std::setw(15) << stop-m_begin << " "<< inSAA << std::endl; 

}
/// @brief find the SAA transition
/// @param start, stop ends of the interval containing the transition
/// @param steps number of steps to make: accuracy will be (stop-start)/2**steps
double MakePointingHistory::saa_transition(double start, double stop, int steps)
{
    double a(start), b(stop), t;
    for( int i(0); i<steps; ++i ){
        bool in(SAA(a)), out(SAA(b));
        assert(in !=out); // should not happen
        t = 0.5*(a+b);
        if( SAA(t)==in )a=t; 
        else            b=t;
    }
    return t;
}
bool MakePointingHistory::SAA(double time){
    return m_history(time).earthCoord().insideSAA();
}

void MakePointingHistory::help(){
    std::cout << 
        "usage:\n\t pointing_history FT2-file  [tstart] [tstop] [interval] [SAA-file]\n\n"
        "   FT2-file: input file name of a FT2-format FITS file\n"
        "   tstart: start time, default 0 to indicate start of input file\n"
        "   tstop:  stop time, default 0 to indicate end of input file\n"
        "   interval: interval, default 30 s.\n"
        "   SAA-file: ascii file with pairs of lat,lon coordinates for boundary. Default from Rob.\n"
        << std::endl;
}

void MakePointingHistory::main(int argc, char* argv[])
{
    if( argc<2 ){
        throw std::invalid_argument("No input file specified");
    }
    int n(1);
    std::string filename(argv[n++]);

    // then take the interval
    double begin(0), end(0), interval(30);  // range: if zero, use whole list
    if( argc>n) begin = ::atof(argv[n++]);
    if( argc>n) end   = ::atof(argv[n++]);
    if( argc>n) interval = ::atof(argv[n++]);


    // set the SAA boundary polygon

    std::vector<coords > boundary;

    if( argc>n) {
        // the SAA boundary file
        std::ifstream input_file;
        std::string filename(argv[n++]);
        input_file.open(filename.c_str());

        if(!input_file.is_open()) {
            std::cerr << "ERROR:  Unable to open:  " << filename << std::endl;
            throw std::invalid_argument("could not open SAA file");
        }else{
            while (!input_file.eof()){
                std::string line; std::getline(input_file, line);
                if( line[0]=='#') continue;
                double lat, lon;
                std::stringstream buf(line); buf >> lat >> lon;
                boundary.push_back(coords(lat, lon) );
            }
        }
    }else{
 

        for (size_t i = 0; i < sizeof(latv)/sizeof(double); ++i)  {
            boundary.push_back(coords(latv[i], lonv[i]));
        } 

    }

    EarthCoordinate::setSAAboundary(boundary);

    MakePointingHistory hgen(filename);
    hgen.run(begin, end, interval);
}
