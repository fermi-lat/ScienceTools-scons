/** @file PointSourceLikelihood.h

$Header$
*/

#ifndef tools_PointSourceLikelihood_h
#define tools_PointSourceLikelihood_h
#include "pointlike/SkySpectrum.h"
#include "pointlike/SimpleLikelihood.h"
#include "astro/SkyDir.h"
#include <iostream>
#include <map>

namespace embed_python { class Module; }
namespace map_tools { class PhotonMap; }

namespace pointlike {


    /** @class PointSourceLikelihood
    @brief manage a set of SimpleLikelihood objects, one for each energy band

    Note that it is a map of the SimpleLikelihood objects, with the key the Healpix level,
    usually starting at 6, for 0.9 degree bins.


    */
    class PointSourceLikelihood : public  std::map<int, SimpleLikelihood*>, public pointlike::SkySpectrum{
    public:


        /** ctor
        @param data   source of the data to fit
        @param name   source name for printout
        @param dir    initial direction
        */
        PointSourceLikelihood(const map_tools::PhotonMap& data,
            std::string name,
            const astro::SkyDir& dir);;

        ~PointSourceLikelihood();

        //! fit to signal fraction for each level
        /// @return total TS
        /// @param skip levels to skip
        double  maximize(int skip=0);

        //! change the current direction
        void setDir(const astro::SkyDir& dir);

        /// @return the gradient, summed over all levels, skiping skip
        Hep3Vector gradient(int skip=0) const;

        ///@return the curvature, summed over all levels
        double curvature(int skip=0) const;

        /// @brief 
        void printSpectrum();

        /// @brief perform localization fit, maximizing joint likelihood
        /// @param skip [0] number of levels to skip
        /// @return error circle radius (deg) or negative if bad or no fit.
        double localize(int skip);

        /// @brief invoke localize with skip values from skip1 to skip 2 or until good fit
        double localize(int skip1, int skip2);

        /// @brief localate with iteration to refit the levels, using parameters set in ctor
        double localize();

        std::string name()const{return m_name;}

        const astro::SkyDir& dir()const{return m_dir;}

        double TS()const { return m_TS; } 

        /// @param level
        /// @return the invidual TS for the level
        double levelTS(int level)  { return (*this)[level]->TS();}

        double logL(int level){ return (*this)[level]->operator()();}

        double errorCircle(int skip=0)const{return  sqrt(1./curvature(skip))*180/M_PI;}

        void set_ostream(std::ostream& out){m_out=&out;}

        void set_verbose(bool verbosity=true){m_verbose=verbosity;}

        bool verbose()const{return m_verbose;}

        ///! implement the SkyFunction interface
           ///@brief return differential value 
        ///@param e energy in MeV
        virtual double value(const astro::SkyDir& dir, double e)const;

        ///@brief integral for the energy limits, in the given direction
        virtual double integral(const astro::SkyDir& dir, double a, double b)const;
        static void PointSourceLikelihood::setParameters(embed_python::Module& par);

        /// @brief set radius for individual fits
        static void setDefaultUmax(double umax){ SimpleLikelihood::s_defaultUmax=umax; }

        static std::vector<double> gamma_level;
        static std::vector<double> sigma_level;

        static double set_gamma_level(int level, double v){
            double t = gamma_level[level]; gamma_level[level]=v; return t;}

        static double set_sigma_level(int level, double v){
            double t = sigma_level[level]; sigma_level[level]=v; return t;}

        ///! Set diffuse function
        static void set_diffuse(const pointlike::SkySpectrum* diffuse){SimpleLikelihood::s_diffuse = diffuse;}

        //recalculate likelihoods using any static changes made to parameters
        void recalc(int level);

    private:
        void setup(const map_tools::PhotonMap& data,double radius, int minlevel, int maxlevel);
        std::vector<double> m_energies; ///< array of left edge energies, indexed by level-m_minlevel
        int m_minlevel, m_nlevels;      ///< from the data.
        std::string m_name;
        astro::SkyDir m_dir; ///< common direction
        double m_dir_sigma;  ///< error circle from fit (radians)
        double m_TS;         ///< total TS value

        bool m_verbose;
        std::ostream * m_out;
        std::ostream& out()const{return *m_out;}

        // the data to feed each guy, extracted from the database
        std::map<int, std::vector<std::pair<astro::HealPixel,int> > >m_data_vec;

        static SkySpectrum * s_diffuse;
        static double s_radius, s_minalpha, s_TSmin, s_tolerance;
        static int s_minlevel, s_maxlevel, s_skip1, s_skip2, s_itermax, s_verbose;
    };

}
#endif

