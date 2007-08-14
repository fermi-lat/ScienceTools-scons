#!/usr/bin/env python
"""
@brief Cursory validation of FT1 files based on file content and
header keywords.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
import sys
import numarray as num
import pyfits
from FitsNTuple import FitsNTuple

class Ft1Validator(object):
    def __init__(self, ft1File):
        self.ft1File = ft1File
        foo = pyfits.open(ft1File)

        self.tstart = foo['EVENTS'].header['TSTART']
        self.tstop = foo['EVENTS'].header['TSTOP']

        self.events = FitsNTuple(ft1File, 'EVENTS')
        self.gti = FitsNTuple(ft1File, 'GTI')

        self._passed = 0
        self.quiet = False
    def runTests(self, quiet=False):
        self.quiet = quiet
        self.checkEventTimes()
        self.checkGtis()
        if self._passed == 1:
            self._write("\n\n1 test passed\n")
        else:
            self._write("\n\n%i tests passed\n" % self._passed)
        return self._passed
    def checkEventTimes(self, tol=1e-5):
        if (min(self.events.TIME) < self.tstart-tol or 
            max(self.events.TIME) > self.tstop+tol):
            self._write("Found event times outside of TSTART, TSTOP:\n")
            self._write("TSTART = %i\n" % self.tstart)
            self._write("TSTOP = %i\n" % self.tstop)
            self._write("  min(TIME) = %f\n" % min(self.events.TIME))
            self._write("  max(TIME) = %f\n" % max(self.events.TIME))
            self.eventTimesPassed = False
        else:
            self.eventTimesPassed = True
            self._passed += 1
            self._write('.')
    def _gtbin_lc(self, dtime):
        from GtApp import GtApp
        gtbin = GtApp('gtbin')
        gtbin['algorithm'] = 'LC'
        gtbin['tbinalg'] = 'LIN'
        gtbin['evfile'] = self.ft1File
        gtbin['outfile'] = 'lc.fits'
        gtbin['tstart'] = self.tstart
        gtbin['tstop'] = self.tstop
        gtbin['dtime'] = dtime
        gtbin['chatter'] = 0
        gtbin.run()
        lc = FitsNTuple('lc.fits', 'RATE')
        return lc.COUNTS
    def _get_lc(self, dtime):
        ii = lambda x : int((x - self.tstart)/dtime)
        lc = num.zeros(ii(self.tstop) + 1, type=num.Float)
        for tt in self.events.TIME:
            if tt >= self.tstart and tt <= self.tstop:
                lc[ii(tt)] += 1
        return lc
    def checkGtis(self, dtime=30):
        tstart, tstop = self.tstart, self.tstop
        time = lambda y : y*dtime + tstart
        lc = self._get_lc(dtime)

        indx = num.where(lc != 0)[0]
        gti_indx = [indx[0]]
        for i, item in enumerate(indx[1:]):
            if item - 1 != indx[i]:
                gti_indx.extend((indx[i], item))
        gti_indx.append(indx[-1])

        istart = gti_indx[::2]
        istop = gti_indx[1::2]

        tmin, tmax = [], []
        for istart, istop in zip(gti_indx[::2], gti_indx[1::2]):
            tmin.append(time(istart) - dtime/2)
            tmax.append(time(istop) + dtime/2)
        tmin = num.array(tmin)
        tmax = num.array(tmax)

        self.data_integral = sum(tmax - tmin)
        self.gti_integral = sum(self.gti.STOP - self.gti.START)
        if ( len(tmin) != len(self.gti.START) or
             (max(abs(tmin - self.gti.START)) > dtime) or
             (max(abs(tmax - self.gti.STOP)) > dtime) ):
            self._write("GTIs do not match data.\n")
            self._write("Using %i sec time bins:\n" % dtime)

            self._write("  Integrated time of non-zero counts = %i\n" 
                        % self.data_integral)
            self._write("  Integral of GTIs = %i\n" % self.gti_integral)
            self.gtiTestPassed = False
        else:
            self.gtiTestPassed = True
            self._passed += 1
            self._write('.')
    def _write(self, output):
        if not self.quiet:
            sys.stdout.write(output)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "usage: Ft1Validator.py <FT1 file> "
        sys.exit(1)

    validator = Ft1Validator(sys.argv[1])
    validator.runTests()
