#!/usr/bin/env python

import math

try:
    from numpy import *
except ImportError:
    from math import *
    arcsin = asin
    arccos = acos
    arctan2 = atan2

#"""Angular separation in degrees between two sky coordinates"""
#    
# PURPOSE:
# Calculates the angular distance between two sky positions (RA1,Dec)
# and (RA2,Dec2) along a great circle arc.
#  
# NOTES:
# - Ported and adapted from IDL function GCIRC.pro by rchaves
# - Essentially just the Haversine formula applied
# - The latter two input arguments can be scalars or vectors
# - Assumes Cartesian projection
# - Modified by J.Ballet <Jean.Ballet@cea.fr>,
#            and  D. Landriu <David.Landriu@cea.fr>:
#       - merging dist() with MyGCIRC()
#       - writing subroutine computing only sinus of the angle                       
#           to improve processing speed
#
# INPUT:
# RA1   Right ascension or longitude of position 1 [scalar; deg]
# Dec1  Declination or latitude of position 1 [scalar; deg]
# RA2   Right ascension or longitude of position 2 [vector; deg]
# Dec2  Declination or latitude of position 2 [vector; deg]
#
# OUTPUT:
# dis   Distance(s) between sky positions 1 and 2 [vector; deg]
# @author R. Chaves <Ryan.Chaves@cea.fr>

def sindist2(a, b):

  """Angular separation in degrees between two sky coordinates"""
  (ra1, dec1) = a
  (ra2, dec2) = b
  
  ra1rad = ra1*pi/180.
  dec1rad = dec1*pi/180.
  ra2rad = ra2*pi/180.
  dec2rad = dec2*pi/180.
  
  deldec2 = (dec2rad - dec1rad) / 2.0
  delra2  = (ra2rad  - ra1rad) / 2.0

  sin2dis = sin(deldec2) * sin(deldec2) +\
            cos(dec1rad) * cos(dec2rad) * sin(delra2)  * sin(delra2)   

  return dec1rad, ra1rad, dec2rad, ra2rad, sin2dis  

def dist( a, b ):

    """ Spherical distance """
    
    dec1rad, ra1rad, dec2rad, ra2rad, sin2dis = sindist2(a, b)

    if sin2dis < 0.05 :
#     Use Haversine formula (introduced by Ryan) to get better precision : 
      dis = arcsin(sqrt(sin2dis)) * 2.0 # radians

    else:

      mu = (cos(dec1rad)*cos(ra1rad)*cos(dec2rad)*cos(ra2rad) +\
            cos(dec1rad)*sin(ra1rad)*cos(dec2rad)*sin(ra2rad) +\
            sin(dec1rad)*sin(dec2rad))
        
      dis = Angdist(mu)

    dis *= 180./pi # radians -> deg
      
    return dis

def SphCoords(u):
   """ Spherical coordinates in radians for a normalised 3Dvector u """     
   if abs(u[2])< 1 :
      theta_rad = math.asin(u[2])
      if abs(u[0])>.00001:     
         phi_rad = math.atan(u[1]/u[0])+pi*(1-u[0]/abs(u[0]))/2.
      else:
         phi_rad = ( pi /2. - u[1]/cos(theta_rad)) * u[1]/abs(u[1])  
   else:
      theta_rad = pi /2. * int(u[2])
      phi_rad = 0  

   return(phi_rad,theta_rad)

def Angdist(x):
   """ Angular distance in radians corresponding to a cosinus  """ 
   if abs(x) < 1:
      angdist = arccos(x)
   elif abs(x) < 1.00001:
      angdist = pi/2 * (1-int(x))
   else:   
      print "Error: x must be smaller than 1"

   return angdist 
