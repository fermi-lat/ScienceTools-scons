#!/usr/bin/env python
"""
Class for transforming between Equatorial and Galactic coordinates.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""

from numpy import *
import astropy.coordinates as coords
import astropy.units as units

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

def SphCoords(u):
    import math
    """Spherical coordinates in radians for a normalised 3Dvector u"""
    if abs(u[2]) < 1:
        theta_rad = math.asin(u[2])
        if abs(u[0]) > 0.00001:
            phi_rad = math.atan(u[1]/u[0]) + math.pi*(1 - u[0]/abs(u[0]))/2.
        else:
            phi_rad = (math.pi/2. - u[1]/cos(theta_rad))*u[1]/abs(u[1])  
    else:
        theta_rad = math.pi/2.*int(u[2])
        phi_rad = 0
    return phi_rad, theta_rad

def Angdist(x):
    """Angular distance in radians corresponding to a cosinus""" 
    if abs(x) < 1:
        angdist = arccos(x)
    elif abs(x) < 1.00001:
        angdist = math.pi/2.*(1 - int(x))
    else:
        raise ValueError, "x must be smaller than 1"
    return angdist 

if __name__ == "__main__":
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
