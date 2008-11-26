#--------------------------------------------------------------------------------------------
# Source class: ATNF Pulsars with low EdotD2
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
Pulsar Source Class with low EdotD2 (ATNF catalogue).
"""

# Catalogue identifier
catid = "PULLOW"

# Catalogue FITS filename
catname = "obj-pulsar.fits"

# Probability method and threshold
prob_method      = "PROB_POST"        # Use posterior probability
prob_prior       = 0.044              # Formula used to calculate prior
prob_thres       = 0.50               # Probability threshold
figure_of_merit  = ""                 # Formula used to calculate FoM

# Maximum number of counterparts per LAT source
max_counterparts = 10

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "" , "" ]

# List of source selection criteria (setup up to 9 criteria in the list)
selection = [ 'DEFNULL(@PULLOW_EDOTD2,0.0) <= 5e+33' ]
