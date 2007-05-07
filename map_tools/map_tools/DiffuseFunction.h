/** @file DiffuseFunction.h

$Header$

*/
#ifndef DiffuseFunction_h
#define DiffuseFunction_h
#include "astro/SkyFunction.h"
#include "astro/SkyDir.h"
#include "map_tools/SkyImage.h"

#include <vector>
#include <cassert>
#include <string>

namespace map_tools {

class Aeff;    // forward declration for convolution with effective area
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class DiffuseFunction
    @brief a SkyFunction that adapts a diffuse map. also computes extragal diffuse

*/

class DiffuseFunction : public astro::SkyFunction {
public:
    /** ctor
    @param diffuse_cube_file FITS file describing the diffuse
    @param egal_flux  flux>100 MeV for extragalactic component
    @param egal_index spectral index
    @param energy initial energy 
    
    */
    DiffuseFunction(std::string diffuse_cube_file, double egal_flux=1.5e-5, 
        double egal_index=2.1, double energy=1000.)
        : m_data(diffuse_cube_file)
        , m_egal_flux(egal_flux)
        , m_egal_index(egal_index)
        , m_fract(0)
        , m_emin(0), m_emax(0)
    {
        setEnergy(energy);
    }


    double extraGal(double energy) const; ///< access to the extra galactic component

    /// set energy for use by function
    void setEnergy(double e);
    void setDirection(const astro::SkyDir& dir){m_dir=dir;}

    void setLayer(int layer){ 
        m_layer=layer;
        m_fract=0;
    }
    /// @brief set the energy range for integrals: note const
    void setEnergyRange(double emin, double emax){
        m_emin = emin; m_emax=emax;
        assert( emin>0 && emax/emin<3); // otherwise integral not (now) valid
    }
    
    /// Implement SkyFunction
    ///@return interpolation of the table for given direction and current energy 
    /// 
    double operator()(const astro::SkyDir& dir)const; 

    /// functor that allows energy as well
    double operator()(const astro::SkyDir& dir, double energy)
    {
        setEnergy(energy); return (*this)(dir);
    }
    /// functor for energy only (must set dirction)
    double operator()(double energy);

    ///@return integral for the energy limits, in the given direction
    double integral(const astro::SkyDir& dir, double a, double b)const;

#if 1 // not implemented yet. 
    ///@return integral for the energy limits, over the function, in the given direction
    double integral(const astro::SkyDir& dir, const Aeff& f, double a, double b);
#endif


    //-------------------------------------------------------
    /** @brief set vector of values for set of energy bins
        @param dir  direction 
        @param energies vector of the bin edges.
        @return result vector of the values
    */
    std::vector<double> integral(const astro::SkyDir& dir, 
        const std::vector<double>&energies)const;

    /// @return number of layers
    /// @todo: get number from file
    int layers()const { return 17;}


private:
    static double s_emin;
    double m_egal_flux;  ///< flux >100 Mev for egal (cm**-2 sr**-1 s**-1)
    double m_egal_index; ///< egal spectral index
    double m_energy;
    astro::SkyDir m_dir; ///< current direction for energy function
    int layer(double e)const;
    static double h(double r, double alpha);

    static double energy_bin(int k);
    map_tools::SkyImage m_data;
    int m_layer;
    double m_fract; ///< current fractional
    double m_emin, m_emax; ///< range for integral
};

} // namespace
#endif

