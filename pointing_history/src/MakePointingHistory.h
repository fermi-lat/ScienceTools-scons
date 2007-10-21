/** @file MakePointingHistory.h
    @ brief application to create a pointing history


*/

#include "astro/PointingHistory.h"
namespace astro { class PointingInfo; }
#include <string>


typedef std::pair<double, double> coords;

/** @class MakePointingHistory
@brief make a new pointing history from and existing FT2 file, and a SAA definition

Wraps an astro::PointingHistory object, creates new list including SAA, arbitrary intervals

*/
class MakePointingHistory {
    
public:
    /// @brief ctor loads a FITS pointing history object
    MakePointingHistory(std::string filename);

    ~MakePointingHistory();

    /// @brief make the list
    void run(double begin=0, double end=0,  double interval=30);

    /// @brief generate an entry
    void make_entry(const astro::PointingInfo& point, double start, double stop, bool inSAA);

    /// @brief find the SAA transition
    /// @param start, stop ends of the interval containing the transition
    /// @param steps number of steps to make: accuracy will be (stop-start)/2**steps
    double saa_transition(double start, double stop, int steps=5);

    bool SAA(double time); ///< SAA status at the given time

    //! @brief provide help
    static void help();

    /// @brief example main
    static void main(int argc, char* argv[]);
    
private:
    astro::PointingHistory m_history;
    double m_begin; // for reference

};

