#--------------------------------------------------------------------------------------------
# Source class: Pulsar Wind Nebulae
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
Pulsar Wind Nebula Source Class
"""

# Catalogue identifier
catid = "PWN"

# Catalogue FITS filename
catname = "obj-pwn.fits"

# Probability method and threshold
prob_method      = "PROB_POST"        # Use posterior probability
prob_prior       = "0.01"  # Bayesian frequency prior
prob_thres       = 0.40               # Probability threshold
figure_of_merit  = ""                 # Formula used to calculate FoM

# Maximum number of counterparts per LAT source
max_counterparts = 1

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "" , "" ]

# List of source selection criteria (setup up to 9 criteria in the list)
selection = [ "" , "" ]
