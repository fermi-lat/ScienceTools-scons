#  setup for pointlike source finder
# $Header$
print 'runing setup for pointfind'

# data source 

def test():
  " define the pixelfile for a quick test, running the pixel file created by the test program"
  import os
  path = os.environ['POINTLIKEROOT']
  return os.path.join(path, 'src', 'test', 'pointlike_test.fits')

pixelfile = test()

# direction and cone or radius about it to examine
l,b = 0, 0

radius   = 180 # 180 for all sky

count_threshold=346  # this number is a bit arbitray

TSmin   = 10   # minimum TS for candidates

# parameter for pruning: radius in degrees (also needs to be tuned)
prune_radius = 0.25

# file to write a table of results to
outfile='pointfind_test.txt'
        
