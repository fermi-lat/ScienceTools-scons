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
*   Author: Mark Calabretta, Australia Telescope National Facility
*   $Id$
*===========================================================================*/

#include <string.h>
#include "wcs.h"
#include "sph.h"

/* Maximum number of coordinate axes that can be handled. */
#define NMAX 16

#define WCSSET 137

/* Map error number to error message. */
const char *cylfix_errmsg[] = {
   0,
   "Null wcsprm pointer passed",
   "Memory allocation error", 
   "Linear transformation matrix is singular",
   "Inconsistent or unrecognized coordinate axis types",
   "Invalid parameter value",
   "Invalid coordinate transformation parameters",
   "Ill-conditioned coordinate transformation parameters",
   "All of the corner pixel coordinates are invalid",
   "Could not determine reference pixel coordinate",
   "Could not determine reference pixel value"};

int cylfix(naxis, wcs)

const int naxis[];
struct wcsprm *wcs;

{
   unsigned short icnr, indx[NMAX], ncnr;
   int    j, k, stat[4], status;
   double img[4][NMAX], lat, lng, phi[4], phi0, phimax, phimin, pix[4][NMAX],
          *pixj, theta[4], theta0, world[4][NMAX], x, y;

   /* Initialize if required. */
   if (wcs == 0) return 1;
   if (wcs->flag != WCSSET) {
     /* All instances of wcsset were changed to wcsset2 to avoid a namespace 
      * conflict that occurs with a Windows function of same name.  
      * - T. Hierath
      */
      if (status = wcsset2(wcs)) return status;
   }

   /* Check that we have a cylindrical projection. */
   if (wcs->cel.prj.category != CYLINDRICAL) return -1;
   if (wcs->naxis < 2) return -1;


   /* Compute the native longitude in each corner of the image. */
   ncnr = 1 << wcs->naxis;

   for (k = 0; k < NMAX; k++) {
      indx[k] = 1 << k;
   }

   status = 0;
   phimin =  1.0e99;
   phimax = -1.0e99;
   for (icnr = 0; icnr < ncnr;) {
      /* Do four corners at a time. */
      for (j = 0; j < 4; j++, icnr++) {
         pixj = pix[j];

         for (k = 0; k < wcs->naxis; k++) {
            if (icnr & indx[k]) {
               *(pixj++) = naxis[k] + 0.5;
            } else {
               *(pixj++) = 0.5;
            }
         }
      }

      if (!(status = wcsp2s(wcs, 4, NMAX, pix[0], img[0], phi, theta,
                            world[0], stat))) {
         for (j = 0; j < 4; j++) {
            if (phi[j] < phimin) phimin = phi[j];
            if (phi[j] > phimax) phimax = phi[j];
         }
      }
   }

   if (phimin > phimax) return status;

   /* Any changes needed? */
   if (phimin >= -180.0 && phimax <= 180.0) return -1;


   /* Compute the new reference pixel coordinates. */
   phi0 = (phimin + phimax) / 2.0;
   theta0 = 0.0;

   if (status = prjs2x(&(wcs->cel.prj), 1, 1, 1, 1, &phi0, &theta0, &x, &y,
                       stat)) {
      return (status == 2) ? 5 : 9;
   }

   for (k = 0; k < wcs->naxis; k++) {
      img[0][k] = 0.0;
   }
   img[0][wcs->lng] = x;
   img[0][wcs->lat] = y;

   if (status = linx2p(&(wcs->lin), 1, 0, img[0], pix[0])) {
      return status;
   }


   /* Compute celestial coordinates at the new reference pixel. */
   if (status = wcsp2s(wcs, 1, 0, pix[0], img[0], phi, theta, world[0],
                       stat)) {
      return status == 8 ? 10 : status;
   }

   /* Compute native coordinates of the celestial pole. */
   lng =  0.0;
   lat = 90.0;
   (void)sphs2x(wcs->cel.euler, 1, 1, 1, 1, &lng, &lat, phi, theta);

   wcs->crpix[wcs->lng] = pix[0][wcs->lng];
   wcs->crpix[wcs->lat] = pix[0][wcs->lat];
   wcs->crval[wcs->lng] = world[0][wcs->lng];
   wcs->crval[wcs->lat] = world[0][wcs->lat];
   wcs->lonpole = phi[0] - phi0;

    /* All instances of wcsset were changed to wcsset2 to avoid a namespace 
     * conflict that occurs with a Windows function of same name.  
     * - T. Hierath
     */
   return wcsset2(wcs);
}
