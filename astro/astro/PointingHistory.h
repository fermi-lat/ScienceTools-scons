/** @file PointingHistory.h
    @brief declare class PointingHistory

    $Header$
    
*/

#ifndef ASTRO_POINTINGHISTORY_H
#define ASTRO_POINTINGHISTORY_H

#include "astro/PointingInfo.h"
#include <map>
#include <string>
#include <stdexcept>



namespace astro{
    /** @class PointingHistory
        @brief manage history of 

    */
    class PointingHistory {
    public:
        /// @param input file containing history information
        /// @param offset perhaps needed for ascii files that have times from 
        PointingHistory(const std::string& filename, double offset=0);
        ~PointingHistory(){}

        /// @param add a FITS file to the history
        void readFitsData(std::string filename);

#ifndef SWIG
        /** @class PointingHistory::TimeRangeError
            @brief inherit from std::runtime_error for backward compatibility

        */
        class TimeRangeError : public std::runtime_error {
        public:
            TimeRangeError(const std::string msg): std::runtime_error(msg){}
        };

#endif       
        /** @brief select configuration at the given time
            Note that if the time is not in the range between the start and end, it 
            will throw the TimeRangeError exception

        */
        const astro::PointingInfo& operator()(double time)const 
#ifndef SWIG
            throw(TimeRangeError)          
#endif
        ;

        double startTime()const{return m_startTime;}
        double endTime()const{return m_endTime;}

 
    private:

        mutable PointingInfo m_currentPoint;
        mutable double m_selected;
        std::map<double, astro::PointingInfo> m_data;
        double m_startTime, m_endTime;

        void readTextData(std::string filename, double offset);
        // for FITS setup
        bool haveFitsFile(std::string filename) const;
        //?void fitsReportError(FILE *, int) const;
    };
            
}
#endif
