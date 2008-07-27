"""
@brief Photometric light curves with exposure and aperture corrections.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

import numpy as num
import pyfits
from FitsNTuple import FitsNTuple
from GtApp import GtApp

gtbin = GtApp('gtbin')
gtexposure = GtApp('gtexposure')
gtselect = GtApp('gtselect')

class LcHandler(object):
    def __init__(self, ft1File, ft2File, irfs='P6_V1_SOURCE',
                 emin=100, emax=3e5, tlims=None, dt=8.64e4):
        self.ft1File = ft1File
        self.ft2File = ft2File
        self.irfs = irfs
        self.emin, self.emax = emin, emax
        self.tlims, self.dt = tlims, dt
        if self.tlims is None:
            ft1 = pyfits.open(ft1File)
            self.tlims = ft1[1].header['TSTART'], ft1[1].header['TSTOP']
    def make_light_curve(self, ra_src, dec_src, ra_bg, dec_bg, 
                         radius, prefix, specin=-2.1):
        "Use separate source and background regions, with equal areas."
        lcsrc = self._counts_lc(ra_src, dec_src, radius, '%s_src' % prefix)
        self._exposure(lcsrc, specin)

        lcbg = self._counts_lc(ra_bg, dec_bg, radius, '%s_bg' % prefix)
        self._exposure(lcbg, specin)

        src = FitsNTuple(lcsrc)
        bg = FitsNTuple(lcbg)

        flux = (src.COUNTS - bg.COUNTS)/src.EXPOSURE
        fluxerr = num.sqrt(src.COUNTS + bg.COUNTS)/src.EXPOSURE
        
        self._write_flux_lc(src.TIME, flux, fluxerr, prefix)
    def makelc(self, ra, dec, radius, prefix, specin=-2.1):
        """Use concentric source and background regions centered on
        the source position.  If these regions have the same solid
        angle, then the expression for the background-subtracted,
        aperture-corrected flux is particularly simple.
        """
        outer_radius = num.arccos(2*num.cos(radius*num.pi/180.) - 1)*180./num.pi
        
        lcsrc = self._counts_lc(ra, dec, outer_radius, '%s_src' % prefix)
        self._exposure(lcsrc, specin)

        lcbg = self._counts_lc(ra, dec, radius, '%s_bg' % prefix)
        self._exposure(lcbg, specin)

        src = FitsNTuple(lcsrc)
        bg = FitsNTuple(lcbg)

        exposure_factor = src.EXPOSURE - 2*bg.EXPOSURE

        flux = (src.COUNTS - 2*bg.COUNTS)/exposure_factor
        fluxerr = num.sqrt(src.COUNTS + 2*bg.COUNTS)/exposure_factor

        self._write_flux_lc(src.TIME, flux, fluxerr, prefix)
    def _counts_lc(self, ra, dec, radius, prefix):
        ft1src = 'ft1_%s.fits' % prefix
        gtselect.run(infile=self.ft1File, outfile=ft1src,
                     emin=self.emin, emax=self.emax,
                     ra=ra, dec=dec, rad=radius)
        lcsrc = 'lc_%s.fits' % prefix
        gtbin.run(evfile=ft1src, outfile=lcsrc, algorithm='LC',
                  tbinalg='LIN', tstart=self.tlims[0], tstop=self.tlims[1],
                  dtime=self.dt)
        return lcsrc
    def _exposure(self, lcfile, specin):
        gtexposure.run(infile=lcfile, scfile=self.ft2File,
                       irfs=self.irfs, srcmdl='none', specin=specin)
    def _write_flux_lc(self, time, flux, fluxerr, prefix):
        outfile = 'flux_lc_%s.txt' % prefix
        output = open(outfile, 'w')
        for x, y, dy in zip(time, flux, fluxerr):
            if y == y:
                output.write('%.2f  %12.4e  %12.4e\n' % (x, y, dy))

if __name__ == '__main__':
    lcHandler = LcHandler('FT1_source_zmax105.fits', 'FT2.fits')

    lcHandler.make_light_curve(343.491, 16.148, 351, 9.5, 3, '3c454.3')
    lcHandler.makelc(343.491, 16.148, 3, '3c454.3_new')

#    lcHandler.make_light_curve(128.8355, -45.176, 121, -35, 2, 'Vela_1.8', 
#                               specin=-1.8)
#    lcHandler.make_light_curve(35.665, 43.035, 27, 44, 3, '3c66A', 
#                               specin=-2)
#    lcHandler.make_light_curve(98.475, 17.771, 101, 10, 3, 'Geminga',
#                               specin=-1.66)
#    lcHandler.make_light_curve(279.02, 59.42, 272, 56.4, 3, '1835',
#                               specin=-2)

