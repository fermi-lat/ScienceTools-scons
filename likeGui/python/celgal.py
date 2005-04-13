#!/usr/bin/env python
"""
Class for transforming between Equatorial and Galactic coordinates.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""

try:
    from numarray import *
except ImportError:
    import math
    from math import *
    arcsin = asin
    arccos = acos
    arctan2 = atan2

class celgal:
    def __init__(self, J2000=1):
        #
        # Rotation angles for the two most common epochs
        #
        if J2000:
            self.zrot1 = 282.8592
            self.xrot = 62.8717
            self.zrot2 = 32.93224
        else: # use B1950 values
            self.zrot1 = 282.25
            self.xrot = 62.6
            self.zrot2 = 33.

        self.cos_xrot = cos(self.xrot*pi/180.)
        self.sin_xrot = sin(self.xrot*pi/180.)

    def gal(self, radec):
        (ra, dec) = radec
        return (self.glon(ra, dec), self.glat(ra, dec))

    def cel(self, lb):
        (glon, glat) = lb
        return (self.RA(glon, glat), self.DEC(glon, glat))

    def glon(self, ra, dec) :
        """Galactic longitude as a function of Equatorial coordinates"""
        sdec = sin(dec*pi/180.)
        cdec = cos(dec*pi/180.)
        sdra = sin((ra-self.zrot1)*pi/180.)
        cdra = cos((ra-self.zrot1)*pi/180.)
        glon = self.zrot2 + arctan2(cdec*sdra*self.cos_xrot+sdec*self.sin_xrot,
                                    cdec*cdra)*180./pi
        try:
            if glon < 0. : glon += 360.
            if glon > 360. : glon -= 360.
        except RuntimeError:
            for i in xrange(len(glon)):
                if glon[i] < 0.: glon[i] += 360.
                if glon[i] > 360.: glon[i] -= 360.
        return glon

    def glat(self, ra, dec) :
        """Galactic latitude as a function of Equatorial coordinates"""
        sdec = sin(dec*pi/180.)
        cdec = cos(dec*pi/180.)
        sdra = sin((ra-self.zrot1)*pi/180.)
        return arcsin(sdec*self.cos_xrot-cdec*sdra*self.sin_xrot)*180./pi
            
    def RA(self, longitude, latitude) :
        """Right ascension as a function of Galactic coordinates"""
        clat = cos(latitude*pi/180.)
        slat = sin(latitude*pi/180.)
        cdlon = cos((longitude-self.zrot2)*pi/180.)
        sdlon = sin((longitude-self.zrot2)*pi/180.)
        ra = self.zrot1 + arctan2(clat*sdlon*self.cos_xrot-slat*self.sin_xrot,
                                  clat*cdlon)*180./pi
        try:
            if ra < 0. : ra = ra + 360.
            if ra > 360. : ra = ra - 360.
        except RuntimeError:
            for i in xrange(len(ra)):
                if ra[i] < 0.: ra[i] += 360.
                if ra[i] > 360.: ra[i] -= 360.
        return ra

    def DEC(self, longitude, latitude) :
        """Declination as a function of Galactic coordinates"""
        clat = cos(latitude*pi/180.)
        slat = sin(latitude*pi/180.)
        sdlon = sin((longitude-self.zrot2)*pi/180.)
        return arcsin(clat*sdlon*self.sin_xrot+slat*self.cos_xrot)*180./pi
    
def dist(a, b):
    """Angular separation in degrees between two sky coordinates"""
    ra1, dec1 = a
    ra2, dec2 = b
    ra1 = ra1*pi/180.
    dec1 = dec1*pi/180.
    ra2 = ra2*pi/180.
    dec2 = dec2*pi/180.
    mu = (cos(dec1)*cos(ra1)*cos(dec2)*cos(ra2)
          + cos(dec1)*sin(ra1)*cos(dec2)*sin(ra2) + sin(dec1)*sin(dec2))
    return Angdist(mu)*180./pi

def SphCoords(u):
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
    sources["Crab"] = ( (83.5665, 22.0078), (184.53, -5.84) )
    sources["Geminga"] = ( (98.4909, 17.8605), (195.06, 4.32) )
    sources["Vela"] = ( (128.7346, -45.2023), (263.53, -2.86) )
    sources["3C_279"] = ( (193.9797, -5.8239), (304.98, 57.03) )
    sources["GalCenter"] = ( (266.4051, -28.9362), (0, 0) )
    tol = 1e-3
    for key in sources.keys():
        #print sources[key][0], converter.cel(sources[key][1])
        assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
        #print converter.gal(sources[key][0]), sources[key][1]
        assert dist(converter.gal(sources[key][0]), sources[key][1]) <tol
    #
    # check epoch B1950 conversions
    #
    converter = celgal(0)
    sources["Crab"] = ( (82.8143, 21.9748), (184.53, -5.84) )
    sources["Geminga"] = ( (97.7614, 17.8998), (195.06, 4.32) )
    sources["Vela"] = ( (128.3128, -45.0290), (263.53, -2.86) )
    sources["3C_279"] = ( (193.3324, -5.5534), (304.98, 57.03) )
    sources["GalCenter"] = ( (265.6108, -28.9168), (0, 0) )
    tol = 1e-4
    for key in sources.keys():
        #print sources[key][0], converter.cel(sources[key][1])
        assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
        #print converter.gal(sources[key][0]), sources[key][1]
        assert dist(converter.gal(sources[key][0]), sources[key][1]) < tol
