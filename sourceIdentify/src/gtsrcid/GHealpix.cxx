/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.2  2010/04/16 21:53:16  jurgen
Fully implement HEALPix counterpart density maps

Revision 1.1  2010/04/16 16:16:19  jurgen
Implement HEALPix interface to read counterpart density maps

------------------------------------------------------------------------------*/
/**
 * @file GHealpix.cxx
 * @brief Implements HEALPix map methods.
 * @author J. Knodlseder
 */

/* Includes _________________________________________________________________ */
#include <iostream>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include "GHealpix.h"
#include "Log.h"

/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {

/* __ Local prototypes ___________________________________________________ */
unsigned int isqrt(unsigned int arg);
double       modulo(double v1, double v2);

/* __ Constants __________________________________________________________ */
const int    jrll[12]   = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};
const int    jpll[12]   = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};
const int    order_max  = 13;
const int    ns_max     = 1 << order_max;
const double pi         = 3.1415926535897931159979635;
const double twopi      = 2.0 * pi;
const double fourpi     = 4.0 * pi;
const double pihalf     = 0.5 * pi;
const double inv_pihalf = 1.0 / pihalf;
const double twothird   = 2.0 / 3.0;

/* __ Static conversion arrays ___________________________________________ */
static short ctab[0x100];
static short utab[0x100];


/*==========================================================================
 =                                                                         =
 =                     GHealpix constructors/destructors                   =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Constructor
 *
 * @param[in] nside Number of sides
 * @param[in] scheme Ordering scheme ('RING' or 'NESTED')
 * @param[in] coordsys Coordinate system ('EQU' or 'GAL')
 * @param[in] dimension Vector dimension of pixels
 *
 * @exception GException::healpix_bad_nside Invalid nside parameter.
 * @exception GException::healpix_bad_scheme Invalid scheme parameter.
 * @exception GException::healpix_bad_coords Invalid coordsys parameter.
 ***************************************************************************/
GHealpix::GHealpix(int nside, std::string scheme, std::string coordsys,
                   int dimension)
{
    // Initialise class members
    init_members();

    // Check nside parameter (power of 2 between 1 and 8192)
    int order = nside2order(nside);
    if (order == -1)
      throw std::string("GHealpix: invalid order (-1).");

    // Check scheme
    int i_scheme = -1;
    if (scheme == "RING")
        i_scheme = 0;
    else if (scheme == "NESTED")
        i_scheme = 1;
    else
      throw std::string("GHealpix: invalid scheme (has to be either RING or NESTED).");

    // Check coordinate system
    int i_coordsys;
    if (coordsys == "EQU")
        i_coordsys = 0;
    else if (coordsys == "GAL")
        i_coordsys = 1;
    else
      throw std::string("GHealpix: invalid coordinate system (has to be either EQU or GAL).");

    // Check dimension
    if (dimension < 1) dimension = 1;

    // Set Healpix parameters
    m_nside       = nside;
    m_scheme      = i_scheme;
    m_coordsys    = i_coordsys;
    m_size_pixels = dimension;

    // Derive Healpix parameters
    m_npface     = m_nside * m_nside;
    m_ncap       = 2 * (m_npface - m_nside);
    m_num_pixels = 12 * m_npface;
    m_fact2      = 4.0 / m_num_pixels;
    m_fact1      = 2 * m_nside * m_fact2;
    m_omega      = fourpi / m_num_pixels;
    m_order      = nside2order(m_nside);

    // Allocate pixels
    alloc_members();

    // Return
    return;
}


/***********************************************************************//**
 * @brief Constructor from FITS HDU table
 *
 * @param[in] filename FITS filename
 ***************************************************************************/
GHealpix::GHealpix(const std::string filename)
{
    // Declare local variables
    int       fstatus = 0;
    fitsfile *fptr;

    // Initialise class members
    init_members();

    // Open first table extension
    fstatus = fits_open_table(&fptr, (char*)filename.c_str(), 0, &fstatus);
    if (fstatus != 0)
        throw std::string("GHealpix: unable to open FITS file.");

    // Read pixels from FITS HDU
    read(fptr);

    // Close FITS file
    fstatus = fits_close_file(fptr, &fstatus);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Copy constructor
 *
 * @param pixels GHealpix instance which should be used for construction
 ***************************************************************************/
GHealpix::GHealpix(const GHealpix& pixels)
{
    // Initialise class members for clean destruction
    init_members();

    // Copy members
    copy_members(pixels);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Void constructor
 ***************************************************************************/
GHealpix::GHealpix(void)
{
    // Initialise class members for clean destruction
    init_members();

    // Return
    return;
}


/***********************************************************************//**
 * @brief Destructor
 ***************************************************************************/
GHealpix::~GHealpix()
{
    // Free members
    free_members();

    // Return
    return;
}


/*==========================================================================
 =                                                                         =
 =                          GHealpix operators                             =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Pixel access operator
 *
 * @param[in] pixel pixel number (starting from 0)
 * @param[in] element vector element number (starting from 0)
 ***************************************************************************/
double& GHealpix::operator() (int pixel, int element)
{
    // Check pixel validity
    if (pixel < 0 || pixel >= m_num_pixels || element < 0 || element >= m_size_pixels)
        throw std::string("GHealpix: pixel index is out of range.");

    // Return pixel
    return m_pixels[pixel*m_size_pixels+element];
}


/***********************************************************************//**
 * @brief Pixel access operator
 *
 * @param[in] pixel pixel number (starting from 0)
 * @param[in] element vector element number (starting from 0)
 ***************************************************************************/
const double& GHealpix::operator() (int pixel, int element) const
{
    // Check pixel validity
    if (pixel < 0 || pixel >= m_num_pixels || element < 0 || element >= m_size_pixels)
        throw std::string("GHealpix: pixel index is out of range.");

    // Return pixel
    return m_pixels[pixel*m_size_pixels+element];
}


/***********************************************************************//**
 * @brief Assignment operator
 *
 * @param[in] pixels GHealpix instance to be assigned
 ***************************************************************************/
GHealpix& GHealpix::operator= (const GHealpix& pixels)
{
    // Execute only if object is not identical
    if (this != &pixels) {

        // Free members
        free_members();

        // Initialise private members for clean destruction
        init_members();

        // Copy members
        copy_members(pixels);

    } // endif: object was not identical

    // Return this object
    return *this;
}


/*==========================================================================
 =                                                                         =
 =                         GHealpix public methods                         =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Read Healpix data from FITS table.
 *
 * @param[in] hdu FITS HDU containing the Healpix data
 *
 * @exception GException::healpix
 *            Unable to load Healpix data from table.
 ***************************************************************************/
void GHealpix::read(fitsfile *fptr)
{
    // Declare local variables
    int      fstatus = 0;
    int      anynul;
    long     numRows;
    long     elements;
    long int nside;
    double   nulval = 0.0;
    char     ordering[80];
    char     coordsys[80];

    // Free memory and initialise members
    free_members();
    init_members();

    // Read NSIDE keyword
    fstatus = fits_read_key_lng(fptr, "NSIDE", &nside, NULL, &fstatus);
    if (fstatus != 0)
        throw std::string("GHealpix: Unable to read HEALPIX keyword NSIDE.");
    m_nside = (int)nside;

    // Read ORDERING keyword
    fstatus = fits_read_key_str(fptr, "ORDERING", ordering, NULL, &fstatus);
    if (fstatus != 0)
        throw std::string("GHealpix: Unable to read HEALPIX keyword ORDERING.");

    // Read COORDSYS or HIER_CRD keywords
    fstatus = fits_read_key_str(fptr, "COORDSYS", coordsys, NULL, &fstatus);
    if (fstatus != 0) {
        fstatus = 0;
        fstatus = fits_read_key_str(fptr, "HIER_CRD", coordsys, NULL, &fstatus);
    }
    if (fstatus != 0)
        throw std::string("GHealpix: Unable to read HEALPIX keywords COORDSYS or HIER_CRD.");

    // Get Healpix resolution and determine number of pixels and solid angle
    m_npface      = m_nside * m_nside;
    m_ncap        = 2 * (m_npface - m_nside);
    m_num_pixels  = 12 * m_npface;
    m_fact2       = 4.0 / m_num_pixels;
    m_fact1       = 2 * m_nside * m_fact2;
    m_omega       = fourpi / m_num_pixels;
    m_order       = nside2order(m_nside);
    m_size_pixels = 1;

    // Get ordering scheme
    if (strcmp(ordering,"RING") == 0)
        m_scheme = 0;
    else if (strcmp(ordering,"NESTED") == 0)
        m_scheme = 1;
    else
        throw std::string("GHealpix: Invalid ordering scheme (should be either RING or NESTED).");

    // Decode coordinate system string
    if (strcmp(coordsys,"EQU") == 0)
        m_coordsys = 0;
    else if (strcmp(coordsys,"CEL") == 0)
        m_coordsys = 0;
    else if (strcmp(coordsys,"GAL") == 0)
        m_coordsys = 1;
    else if (strcmp(coordsys,"C") == 0)
        m_coordsys = 0;
    else if (strcmp(coordsys,"G") == 0)
        m_coordsys = 1;
    else {
        throw std::string("GHealpix: Invalid coordinate system (should be either EQU or GAL).");
    }

    // Continue only of we have pixels
    if (m_num_pixels > 0) {

        // Determine number of rows in table
        fstatus = fits_get_num_rows(fptr, &numRows, &fstatus);
        if (fstatus != 0)
            throw std::string("GHealpix: Unable to read number of rows in HEALPIX table.");

        // Determine number of elements per column
        fstatus = fits_get_coltype(fptr, 1, NULL, &elements, NULL, &fstatus);
        if (fstatus != 0)
            throw std::string("GHealpix: Unable to read number of rows in HEALPIX table.");

        // Check column consistency
        if (numRows*elements != m_num_pixels)
            throw std::string("GHealpix: NSIDE inconsistent with number of rows in HEALPIX table.");

        // Allocate pixels
        alloc_members();

        // Read pixels
        fstatus = fits_read_col(fptr, TDOUBLE, 1, 1, 1, m_num_pixels, &nulval, m_pixels, &anynul, &fstatus);
        if (fstatus != 0)
            throw std::string("GHealpix: Unable to read HEALPIX table.");

    } // endif: we had pixels

    // Return
    return;
}


/***********************************************************************//**
 * @brief Returns number of divisions of the side of each base pixel.
 ***************************************************************************/
int GHealpix::nside(void) const
{
    // Return nside
    return m_nside;
}


/***********************************************************************//**
 * @brief Returns ordering parameter.
 ***************************************************************************/
int GHealpix::order(void) const
{
    // Return order
    return m_order;
}


/***********************************************************************//**
 * @brief Returns number of pixels.
 ***************************************************************************/
int GHealpix::npix(void) const
{
    // Return nside
    return m_num_pixels;
}


/***********************************************************************//**
 * @brief Returns solid angle of pixel
 ***************************************************************************/
double GHealpix::omega(void) const
{
    // Return solid angle
    return m_omega;
}


/***********************************************************************//**
 * @brief Returns sky direction of pixel
 *
 * @param[in] ipix Pixel number (0,1,...,num_pixels)
 ***************************************************************************/
GSkyDir GHealpix::pix2ang(const int& ipix)
{
    // Declare result
    GSkyDir result;
    double  theta = 0.0;
    double  phi   = 0.0;

    // Perform ordering dependent conversion
    switch (m_scheme) {
    case 0:
        pix2ang_ring(ipix, &theta, &phi);
        break;
    case 1:
        pix2ang_nest(ipix, &theta, &phi);
        break;
    default:
        break;
    }

    // Store coordinate system dependent result
    switch (m_coordsys) {
    case 0:
        result.radec(phi, pihalf-theta);
        break;
    case 1:
        result.lb(phi, pihalf-theta);
        break;
    default:
        break;
    }

    // Return result
    return result;
}


/***********************************************************************//**
 * @brief Returns pixel for a given sky direction
 *
 * @param[in] ipix Pixel number (0,1,...,num_pixels)
 ***************************************************************************/
int GHealpix::ang2pix(GSkyDir dir) const
{
    // Declare result
    int    ipix;
    double z;
    double phi;

    // Compute coordinate system dependent (z,phi)
    switch (m_coordsys) {
    case 0:
        z   = cos(pihalf-dir.dec());
        phi = dir.ra();
        break;
    case 1:
        z   = cos(pihalf-dir.b());
        phi = dir.l();
        break;
    default:
        break;
    }

    // Perform ordering dependent conversion
    switch (m_scheme) {
    case 0:
        ipix = ang2pix_z_phi_ring(z, phi);
        break;
    case 1:
        ipix = ang2pix_z_phi_nest(z, phi);
        break;
    default:
        break;
    }

    // Return pixel index
    return ipix;
}


/*==========================================================================
 =                                                                         =
 =                         GHealpix private methods                        =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Initialise class members
 ***************************************************************************/
void GHealpix::init_members(void)
{
    // Initialise members
    m_nside       = 0;
    m_npface      = 0;
    m_ncap        = 0;
    m_order       = 0;
    m_scheme      = 0;
    m_coordsys    = 0;
    m_num_pixels  = 0;
    m_size_pixels = 0;
    m_fact1       = 0.0;
    m_fact2       = 0.0;
    m_omega       = 0.0;
    m_pixels      = NULL;

    // Construct conversion arrays
    for (int m = 0; m < 0x100; ++m) {
    ctab[m] =
         (m&0x1 )       | ((m&0x2 ) << 7) | ((m&0x4 ) >> 1) | ((m&0x8 ) << 6)
      | ((m&0x10) >> 2) | ((m&0x20) << 5) | ((m&0x40) >> 3) | ((m&0x80) << 4);
    utab[m] =
         (m&0x1 )       | ((m&0x2 ) << 1) | ((m&0x4 ) << 2) | ((m&0x8 ) << 3)
      | ((m&0x10) << 4) | ((m&0x20) << 5) | ((m&0x40) << 6) | ((m&0x80) << 7);
    }

    // Return
    return;
}


/***********************************************************************//**
 * @brief Allocate class members
 ***************************************************************************/
void GHealpix::alloc_members(void)
{
    // Compute data size
    int size = m_num_pixels * m_size_pixels;

    // Continue only if there are pixels    
    if (size > 0) {

        // Allocate pixels and initialize them to 0
        m_pixels = new double[size];
        for (int i = 0; i < size; ++i)
            m_pixels[i] = 0.0;

    } // endif: there were pixels

    // Return
    return;
}


/***********************************************************************//**
 * @brief Copy class members
 *
 * @param[in] pixels GHealpix instance from which members should be copied
 ***************************************************************************/
void GHealpix::copy_members(const GHealpix& pixels)
{
    // Copy attributes
    m_nside       = pixels.m_nside;
    m_npface      = pixels.m_npface;
    m_ncap        = pixels.m_ncap;
    m_order       = pixels.m_order;
    m_scheme      = pixels.m_scheme;
    m_coordsys    = pixels.m_coordsys;
    m_num_pixels  = pixels.m_num_pixels;
    m_size_pixels = pixels.m_size_pixels;
    m_fact1       = pixels.m_fact1;
    m_fact2       = pixels.m_fact2;
    m_omega       = pixels.m_omega;

    // Copy arrays
    if (m_num_pixels > 0) {
        if (m_size_pixels > 0 && pixels.m_pixels != NULL) {
            int size = m_num_pixels*m_size_pixels;
            m_pixels = new double[size];
            memcpy(m_pixels, pixels.m_pixels, size*sizeof(double));
        }
    }

    // Return
    return;
}


/***********************************************************************//**
 * @brief Delete class members
 ***************************************************************************/
void GHealpix::free_members(void)
{
    // Free memory
    if (m_pixels != NULL) delete [] m_pixels;

    // Mark memory as free
    m_pixels = NULL;

    // Return
    return;
}


/***********************************************************************//**
 * @brief Clone Healpix representation
 ***************************************************************************/
GHealpix* GHealpix::clone(void) const
{
    return new GHealpix(*this);
}


/***********************************************************************//**
 * @brief Convert nside to order
 *
 * @param[in] nside Number of sides.
 ***************************************************************************/
int GHealpix::nside2order(int nside)
{
    // Initialise order
    int order = -1;

    // Determine order
    for (int m = 0; m <= order_max; ++m) {
        int nstest = 1 << m;
        if (nside == nstest) {
            order = m;
            break;
        }
        if (nside < nstest)
            break;
    }

    // Return order
    return order;
}


/***********************************************************************//**
 * @brief Convert pixel index to (x,y) coordinate
 *
 * @param[in] ipix Pixel index for which (x,y) are to be computed.
 * @param[out] x Pointer to x coordinate.
 * @param[out] y Pointer to y coordinate.
 ***************************************************************************/
void GHealpix::pix2xy(const int& ipix, int* x, int* y)
{
    // Set x coordinate
    int raw = (ipix & 0x5555) | ((ipix & 0x55550000) >> 15);
    *x      = ctab[raw & 0xff] | (ctab[raw >> 8] << 4);

    // Set y coordinate
    raw = ((ipix & 0xaaaa) >> 1) | ((ipix & 0xaaaa0000) >> 16);
    *y  = ctab[raw & 0xff] | (ctab[raw >> 8] << 4);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Convert (x,y) coordinate to pixel
 *
 * @param[in] x x coordinate.
 * @param[in] y y coordinate.
 ***************************************************************************/
int GHealpix::xy2pix(int x, int y) const
{
    // Return pixel
    return utab[x&0xff] | (utab[x>>8]<<16) | (utab[y&0xff]<<1) | (utab[y>>8]<<17);
}


/***********************************************************************//**
 * @brief Convert pixel index to (theta,phi) angles for ring ordering
 *
 * @param[in] ipix Pixel index for which (theta,phi) are to be computed.
 * @param[out] theta Pointer to result zenith angle in radians.
 * @param[out] phi Pointer to result azimuth angle in radians.
 *
 * @exception GException::out_of_range Pixel index is out of range.
 ***************************************************************************/
void GHealpix::pix2ang_ring(int ipix, double* theta, double* phi)
{
    // Check if ipix is in range
    if (ipix < 0 || ipix >= m_num_pixels)
        throw std::string("GHealpix: pixel index is out of range.");

    // Handle North Polar cap
    if (ipix < m_ncap) {
        int iring = int(0.5*(1+isqrt(1+2*ipix))); // counted from North pole
        int iphi  = (ipix+1) - 2*iring*(iring-1);
        *theta    = acos(1.0 - (iring*iring) * m_fact2);
        *phi      = (iphi - 0.5) * pi/(2.0*iring);
    }

    // Handle Equatorial region
    else if (ipix < (m_num_pixels - m_ncap)) {
        int    ip    = ipix - m_ncap;
        int    iring = ip/(4*m_nside) + m_nside;   // counted from North pole
        int    iphi  = ip%(4*m_nside) + 1;
        double fodd  = ((iring+m_nside)&1) ? 1 : 0.5;
        int    nl2   = 2*m_nside;
        *theta       = acos((nl2 - iring) * m_fact1);
        *phi         = (iphi - fodd) * pi/nl2;
    }

    // Handle South Polar cap    
    else {
        int ip    = m_num_pixels - ipix;
        int iring = int(0.5*(1+isqrt(2*ip-1)));    // Counted from South pole
        int iphi  = 4*iring + 1 - (ip - 2*iring*(iring-1));
        *theta    = acos(-1.0 + (iring*iring) * m_fact2);
        *phi      = (iphi - 0.5) * pi/(2.*iring);
    }

    // Return
    return;
}


/***********************************************************************//**
 * @brief Convert pixel index to (theta,phi) angles for nested ordering
 *
 * @param[in] ipix Pixel index for which (theta,phi) are to be computed.
 * @param[out] theta Pointer to result zenith angle in radians.
 * @param[out] phi Pointer to result azimuth angle in radians.
 *
 * @exception GException::out_of_range Pixel index is out of range.
 ***************************************************************************/
void GHealpix::pix2ang_nest(int ipix, double* theta, double* phi)
{
    // Check if ipix is in range
    if (ipix < 0 || ipix >= m_num_pixels)
        throw std::string("GHealpix: pixel index is out of range.");

    // Get face number and index in face
    int nl4      = 4 * m_nside;
    int face_num = ipix >> (2*m_order);      // Face number in {0,11}
    int ipf      = ipix & (m_npface - 1);

    // Get pixel coordinates
    int ix;
    int iy;
    pix2xy(ipf, &ix, &iy);

    // Computes the z coordinate on the sphere
    int jr = (jrll[face_num] << m_order) - ix - iy - 1;

    // Declare result variables
    int    nr;
    double z;
    int    kshift;

    // North pole region
    if (jr < m_nside) {
        nr     = jr;
        z      = 1. - nr*nr*m_fact2;
        kshift = 0;
    }

    // South pole region
    else if (jr > 3*m_nside) {
        nr     = nl4 - jr;
        z      = nr*nr*m_fact2 - 1;
        kshift = 0;
    }

    // Equatorial region
    else {
        nr     = m_nside;
        z      = (2*m_nside-jr) * m_fact1;
        kshift = (jr-m_nside) & 1;
    }

    // Computes the phi coordinate on the sphere, in [0,2Pi]
    int jp = (jpll[face_num]*nr + ix - iy + 1 + kshift) / 2;
    if (jp > nl4) jp -= nl4;
    if (jp <   1) jp += nl4;

    // Computes Theta and Phi
    *theta = acos(z);
    *phi   = (jp - (kshift+1)*0.5) * (pihalf / nr);

    // Return
    return;
}


/***********************************************************************//**
 * @brief Returns pixels which contains angular coordinates (z,phi)
 *
 * @param[in] z Cosine of zenith angle - cos(theta).
 * @param[in] phi Azimuth angle in radians.
 ***************************************************************************/
int GHealpix::ang2pix_z_phi_ring(double z, double phi) const
{
    // Initialise pixel
    int ipix = 0;

    // Setup
    double za = fabs(z);
    double tt = modulo(phi,twopi) * inv_pihalf; // in [0,4)

    // Equatorial region
    if (za <= twothird) {
        double temp1  = m_nside*(0.5+tt);
        double temp2  = m_nside*z*0.75;
        int    jp     = int(temp1-temp2);           // index of ascending edge line
        int    jm     = int(temp1+temp2);           // index of descending edge line
        int    ir     = m_nside + 1 + jp - jm;      // in {1,2n+1}
        int    kshift = 1 - (ir & 1);               // kshift=1 if ir even, 0 otherwise
        int    ip     = (jp+jm-m_nside+kshift+1)/2; // in {0,4n-1}
        ip            = int(modulo(ip,4*m_nside));
        ipix          = m_ncap + (ir-1)*4*m_nside + ip;
    }

    // North & South polar caps
    else {
        double tp  = tt - int(tt);
        double tmp = m_nside * sqrt(3*(1-za));
        int    jp  = int(tp*tmp);       // increasing edge line index
        int    jm  = int((1.0-tp)*tmp); // decreasing edge line index
        int    ir  = jp + jm + 1;       // ring number counted from the closest pole
        int    ip  = int(tt*ir);        // in {0,4*ir-1}
        ip = int(modulo(ip,4*ir));
        if (z>0)
            ipix = 2*ir*(ir-1) + ip;
        else
            ipix = m_num_pixels - 2*ir*(ir+1) + ip;
    }

    // Return pixel
    return ipix;
}


/***********************************************************************//**
 * @brief Returns pixels which contains angular coordinates (z,phi)
 *
 * @param[in] z Cosine of zenith angle - cos(theta).
 * @param[in] phi Azimuth angle in radians.
 ***************************************************************************/
int GHealpix::ang2pix_z_phi_nest(double z, double phi) const
{
    // Initialise face and pixel numbers
    int face_num;
    int ix;
    int iy;

    // Setup
    double za = fabs(z);
    double tt = modulo(phi,twopi) * inv_pihalf; // in [0,4)

    // Equatorial region
    if (za <= twothird) {
        double temp1 = ns_max*(0.5+tt);
        double temp2 = ns_max*z*0.75;
        int    jp    = int(temp1-temp2); // index of  ascending edge line
        int    jm    = int(temp1+temp2); // index of descending edge line
        int    ifp   = jp >> order_max;  // in {0,4}
        int    ifm   = jm >> order_max;
        if (ifp == ifm)                  // faces 4 to 7
            face_num = (ifp==4) ? 4: ifp+4;
        else if (ifp < ifm)              // (half-)faces 0 to 3
            face_num = ifp;
        else                             // (half-)faces 8 to 11
            face_num = ifm + 8;
        ix = jm & (ns_max-1);
        iy = ns_max - (jp & (ns_max-1)) - 1;
    }

    // Polar region, za > 2/3
    else {
        int    ntt = int(tt);
        double tp  = tt-ntt;
        double tmp = ns_max*sqrt(3*(1-za));
        int    jp  = int(tp*tmp);        // increasing edge line index
        int    jm  = int((1.0-tp)*tmp);  // decreasing edge line index
        if (jp >= ns_max) jp = ns_max-1; // for points too close to the boundary
        if (jm >= ns_max) jm = ns_max-1;
        if (z >= 0) {
            face_num = ntt;              // in {0,3}
            ix       = ns_max - jm - 1;
            iy       = ns_max - jp - 1;
        }
        else {
            face_num = ntt + 8;          // in {8,11}
            ix       =  jp;
            iy       =  jm;
        }
    }

    // Get pixel
    int ipf = xy2pix(ix, iy);
    ipf >>= (2*(order_max - m_order));     // in {0, nside**2 - 1}
    return ipf + (face_num<<(2*m_order));  // in {0, 12*nside**2 - 1}
}



/*==========================================================================
 =                                                                         =
 =                             GHealpix friends                            =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Output operator
 *
 * @param[in] os Output stream
 * @param[in] column Healpix array to put in output stream
 ***************************************************************************/
std::ostream& operator<< (std::ostream& os, const GHealpix& pixels)
{
    // Put header in stream
    os << "=== GHealpix ===" << std::endl;
    os << " Nside (number of divisions): " << pixels.m_nside << std::endl;
    os << " Npface (pixels per face) ..: " << pixels.m_npface << std::endl;
    os << " Ncap (number of cap pixels): " << pixels.m_ncap << std::endl;
    os << " Npix (number of pixels) ...: " << pixels.m_num_pixels << std::endl;
    os << " Order .....................: " << pixels.m_order << std::endl;
    os << " Pixel vector size .........: " << pixels.m_size_pixels << std::endl;
    os << " Solid angle ...............: " << std::scientific << pixels.m_omega 
       << std::fixed << " sr" << std::endl;

    // Put ordering in stream
    os << " Ordering ..................: ";
    switch (pixels.m_scheme) {
    case 0:
        os << "Ring" << std::endl;
        break;
    case 1:
        os << "Nested" << std::endl;
        break;
    case -1:
        os << "*** Unknown ***" << std::endl;
        break;
    default:
        os << "*** Invalid ***" << std::endl;
        break;
    }

    // Put coordinate system in stream
    os << " Coordinate system .........: ";
    switch (pixels.m_coordsys) {
    case 0:
        os << "Equatorial (RA,Dec)";
        break;
    case 1:
        os << "Galactic (l,b)";
        break;
    case -1:
        os << "*** Unknown ***";
        break;
    default:
        os << "*** Invalid ***";
        break;
    }

    // Return output stream
    return os;
}


/*==========================================================================
 =                                                                         =
 =                     Other functions used by GHealpix                    =
 =                                                                         =
 ==========================================================================*/

/***********************************************************************//**
 * @brief Integer n that fulfills n*n <= arg < (n+1)*(n+1)
 *
 * @param[in] arg Argument.
 *
 * Returns the integer \a n, which fulfills \a n*n <= arg < (n+1)*(n+1).
 ***************************************************************************/
unsigned int isqrt(unsigned int arg)
{
    // Return
    return unsigned(sqrt(arg+0.5));
}


/* Namespace ends ___________________________________________________________ */
}
