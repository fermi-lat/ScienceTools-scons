#  setup for pointlike source finder
# $Header$

from pointlike_defaults import *

#specify data
pixelfile = r'F:\glast\data\SC2\obssim\allsky_noGRBs.fits'

# choose region to search
SourceFinder.ra, SourceFinder.dec = 248.79, -17.86 # 3EGJ1635m1751 
SourceFinder.examine_radius   =  180 # 180 for all sky


# file to write a table of results to
outfile='../output/all_sky04e.txt'

print 'will write to file %s '% outfile
