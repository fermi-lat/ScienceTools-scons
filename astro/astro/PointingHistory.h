/** @file PointingHistory.h
    @brief declare class PointingHistory

    $Header$
    
*/

#ifndef ASTRO_POINTINGHISTORY_H
#define ASTRO_POINTINGHISTORY_H

#include "astro/PointingInfo.h"
#include <map>
#include <string>

namespace astro{
    class PointingHistory {
    public:
        /// @param input file containing history information
        PointingHistory(const std::string& filename, double offset=0);
        ~PointingHistory(){}

        /** @brief select configuration at the given time

        */
        const astro::PointingInfo& operator()(double time)const;

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
        void readFitsData(std::string filename);
        //?void fitsReportError(FILE *, int) const;
    };
            
}
#endif
