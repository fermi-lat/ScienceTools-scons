/*----------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
------------------------------------------------------------------------------
$Log$
----------------------------------------------------------------------------*/
/**
 * @file GSkyDir.h
 * @brief GSkyDir class definition.
 * @author J. Knodlseder
 */

#ifndef GSKYDIR_H
#define GSKYDIR_H

/* __ Includes ___________________________________________________________ */


/* Namespace definition __________________________________________________ */
namespace sourceIdentify {


/***********************************************************************//**
 * @class GSkyDir
 *
 * @brief GSkyDir class interface defintion
 ***************************************************************************/
class GSkyDir {

    // I/O friends
    friend std::ostream& operator<< (std::ostream& os, const GSkyDir& dir);

public:
    // Constructors and destructors
    GSkyDir();
    GSkyDir(const GSkyDir& dir);
    virtual ~GSkyDir();

    // Operators
    GSkyDir& operator= (const GSkyDir& dir);

    // Methods
    void   radec(const double& ra, const double& dec);
    void   radec_deg(const double& ra, const double& dec);
    void   lb(const double& l, const double& b);
    void   lb_deg(const double& l, const double& b);
    double l(void);
    double l_deg(void);
    double b(void);
    double b_deg(void);
    double ra(void);
    double ra_deg(void);
    double dec(void);
    double dec_deg(void);

private:
    // Private methods
    void init_members(void);
    void copy_members(const GSkyDir& dir);
    void free_members(void);
    void equ2gal(void);
    void gal2equ(void);

    // Private data area
    int    m_has_lb;     //!< Has galactic coordinates
    int    m_has_radec;  //!< Has equatorial coordinates
    double m_l;          //!< Galactic longitude in radians
    double m_b;          //!< Galactic latitude in radians
    double m_ra;         //!< Right Ascension in radians
    double m_dec;        //!< Declination in radians
};

/* Namespace ends ___________________________________________________________ */
}

#endif /* GSKYDIR_H */
