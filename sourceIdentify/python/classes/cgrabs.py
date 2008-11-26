#--------------------------------------------------------------------------------------------
# Source class: Blazars
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
Blazar Source Class (CGRaBS).
Isotropic MC sky calibrated.
"""

# Catalogue identifier
catid = "CGRABS"

# Catalogue FITS filename
catname = "obj-blazar-cgrabs.fits"

# Probability method and threshold
prob_method      = "PROB_POST"        # Use posterior probability
prob_prior       = 0.14               # Formula used to calculate prior
prob_thres       = 0.50               # Probability threshold
figure_of_merit  = ""                 # No FoM used

# Maximum number of counterparts per LAT source
max_counterparts = 10

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "" , "" ]

# List of source selection criteria (setup up to 9 criteria in the list)
selection = [ "" , "" ]

