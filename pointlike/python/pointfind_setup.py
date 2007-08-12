#  setup for pointlike source finder
# $Header$
print 'setup for pointfind'

# data source 

pixelfile = "F:/glast/data/SC2/obssim/sc2_obssim_map.fits"

# direction and cone or radius about it to examine
l,b = 0, -90

radius   = 60 # 180 for all sky

count_threshold=346  

TSmin   = 10   # minimum TS for candidates

# parameter for pruning: radius in degrees
prune_radius = 0.25

# file to write a table of results to
outfile='south_sources.txt'
        
