/** @file SkyStat.h
@brief Define the class SkyStat

@author Bruce Lesnick 

*/

#ifndef astro_SkyStat_h
#define astro_SkyStat_h

namespace astro {

    class HTM;
/**
@class SkyStat
@brief Examine a given level of an HTM structure and calculate the average, rms, min
and max values for a given SkyFunction.

*/
    class SkyStat
    {
        public:
            SkyStat(const astro::SkyFunction & sf,
                        const int level, const astro::HTM * h = 0);
                        
            ~SkyStat();
            
            /** @brief Return average value of Skyfunction over sphere */ 
            double ave() const;

            /** @brief Return RMS value of Skyfunction over sphere */ 
            double rms() const;
            
            /** @brief Return standard deviation value of Skyfunction over sphere */ 
            double sigma() const;

            /** @brief Return minimum value of Skyfunction over sphere */ 
            double min() const;

            /** @brief Return maximum value of Skyfunction over sphere */ 
            double max() const;

            /** @brief Return number or samples rejected  */ 
            int rejected() const { return m_rejected; }
                   
        private:
            const astro::HTM * m_h;
            bool m_HTM_Created;
            const astro::SkyFunction & m_sf;
            const int m_level;
            double m_ave;
            double m_rms;
            double m_sigma;
            double m_min;
            double m_max;
            int   m_rejected;
            
            /** @brief Calculate and store statistical values */ 
            int calculate_values();
    };
}
#endif