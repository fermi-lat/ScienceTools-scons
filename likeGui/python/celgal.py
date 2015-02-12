#!/usr/bin/env python
"""
Class for transforming between Equatorial and Galactic coordinates.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""
from numpy import *
import astropy.coordinates as coords
import astropy.units as units
from saclay_celgal import sindist2, SphCoords, Angdist, dist as saclay_dist

class celgal(object):
    def __init__(self, J2000=True):
        if J2000:
            self.celestialCoords = coords.FK5     # J2000
        else:
            self.celestialCoords = coords.FK4     # B1950
    def gal(self, radec):
        cel_coord = coords.SkyCoord(radec[0], radec[1],
                                    self.celestialCoords,
                                    unit=(units.degree, units.degree))
        gal_coord = cel_coord.transform_to(coords.Galactic)
        return gal_coord.l.degree, gal_coord.b.degree
    def cel(self, lb):
        gal_coord = coords.SkyCoord(lb[0], lb[1],
                                    coords.Galactic,
                                    unit=(units.degree, units.degree))
        cel_coord = gal_coord.transform_to(self.celestialCoords)
        return cel_coord.ra.degree, cel_coord.dec.degree
    def glon(self, ra, dec):
        l, b = self.gal((ra, dec))
        return l
    def glat(self, ra, dec):
        l, b = self.gal((ra, dec))
        return b
    def RA(self, l, b):
        lon, lat = self.cel((l, b))
        return lon
    def DEC(self, l, b):
        lon, lat = self.cel((l, b))
        return lat

def dist(a, b):
    a_coord = coords.SkyCoord(a[0], a[1], unit=(units.degree, units.degree))
    b_coord = coords.SkyCoord(b[0], b[1], unit=(units.degree, units.degree))
    return a_coord.separation(b_coord).degree

if __name__ == "__main__":
    import numpy.random as random
    #
    # check epoch J2000 conversions
    #
    converter = celgal()
    sources = {}
    sources["Crab"] = ( (83.5665, 22.0078), (184.52996368, -5.83998396) )
    sources["Geminga"] = ( (98.4909, 17.8605), (195.06003431, 4.31998806) )
    sources["Vela"] = ( (128.7346, -45.2023), (263.52989939, -2.86004251) )
    sources["3C_279"] = ( (193.9797, -5.8239), (304.97995250, 57.02996622) )
    sources["GalCenter"] = ( (266.40508920, -28.93617470), (0, 0) )
    tol = 2e-4
    for key in sources.keys():
        #print sources[key][0], converter.cel(sources[key][1])
        assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
        #print converter.gal(sources[key][0]), sources[key][1]
        assert dist(converter.gal(sources[key][0]), sources[key][1]) <tol
    #
    # check epoch B1950 conversions
    #
    converter = celgal(0)
    sources["Crab"] = ( (82.8143, 21.9748), (184.52995465, -5.83998847) )
    sources["Geminga"] = ( (97.7614, 17.8998), (195.06003244, 4.31995548) )
    sources["Vela"] = ( (128.3128, -45.0290), (263.53005171, -2.85999746) )
    sources["3C_279"] = ( (193.3324, -5.5534), (304.97991590, 57.02999541) )
    sources["GalCenter"] = ( (265.61084403, -28.91679035), (0, 0) )
    tol = 2e-4
    for key in sources.keys():
        #print sources[key][0], converter.cel(sources[key][1])
        assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
        #print converter.gal(sources[key][0]), sources[key][1]
        assert dist(converter.gal(sources[key][0]), sources[key][1]) < tol

    #
    # exercise Saclay angular distance implementation
    #
    npts = 20
    l_vals = random.uniform(size=npts)*360.
    b_vals = random.uniform(size=npts)*180. - 90.
    for i in range(npts/2):
        pt1 = (l_vals[i], b_vals[i])
        pt2 = (l_vals[i+npts/2], b_vals[i+npts/2])
        print dist(pt1, pt2), saclay_dist(pt1, pt2)
        
        
