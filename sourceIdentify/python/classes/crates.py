#--------------------------------------------------------------------------------------------
# Source class: CRATES radio sources
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
CRATES radio sources.
Isotropic MC sky calibrated.
"""

# Catalogue identifier
catid = "CRATES"

# Catalogue FITS filename
catname = "obj-blazar-crates.fits"

# Probability method and threshold
prob_method      = "PROB_POST"        # Use posterior probability
prob_prior       = 0.022              # Formula used to calculate prior
prob_thres       = 0.50               # Probability threshold
figure_of_merit  = ""                 # No FoM used

# Maximum number of counterparts per LAT source
max_counterparts = 10

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "" , "" ]

# List of source selection criteria (setup up to 9 criteria in the list)
selection = [ "" , "" ]

