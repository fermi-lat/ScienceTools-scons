/*============================================================================
*
*   WCSLIB 3.4 - an implementation of the FITS WCS convention.
*   Copyright (C) 1995-2004, Mark Calabretta
*
*   This library is free software; you can redistribute it and/or modify it
*   under the terms of the GNU Library General Public License as published
*   by the Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This library is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
*   General Public License for more details.
*
*   You should have received a copy of the GNU Library General Public License
*   along with this library; if not, write to the Free Software Foundation,
*   Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta
*                      Australia Telescope National Facility, CSIRO
*                      PO Box 76
*                      Epping NSW 1710
*                      AUSTRALIA
*
*=============================================================================
*
*   cylfix() fixes WCS FITS header cards for malformed cylindrical projections
*   that suffer from the problem described in Sect. 7.3.4 of
*
*      "Representations of celestial coordinates in FITS",
*      Calabretta, M.R., & Greisen, E.W. 2002, A&A, 395, 1077
*
*   Given and returned:
*      naxis    const int []
*                        Image axis lengths.
*      wcs      struct wcsprm*
*                        Coordinate transformation parameters (refer to the
*                        prologue of wcs.h).
*
*   Function return value:
*               int      Error status
*                          -1: No change required (not an error).
*                           0: Success.
*                           1: Null wcsprm pointer passed.
*                           2: Memory allocation error.
*                           3: Linear transformation matrix is singular.
*                           4: Inconsistent or unrecognized coordinate axis
*                              types.
*                           5: Invalid parameter value.
*                           6: Invalid coordinate transformation parameters.
*                           7: Ill-conditioned coordinate transformation
*                              parameters.
*                           8: All of the corner pixel coordinates are
*                              invalid.
*                           9: Could not determine reference pixel coordinate.
*                          10: Could not determine reference pixel value.
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   $Id$
*===========================================================================*/

#ifndef WCSLIB_CYLFIX
#define WCSLIB_CYLFIX

#include "wcs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char *cylfix_errmsg[];


#if __STDC__ || defined(__cplusplus)
   int cylfix (const int [], struct wcsprm *);
#else
   int cylfix();
#endif

#ifdef __cplusplus
};
#endif

#endif /* WCSLIB_CYLFIX */
