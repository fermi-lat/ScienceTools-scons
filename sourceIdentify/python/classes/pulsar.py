#--------------------------------------------------------------------------------------------
# Source class: Pulsars
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
Pulsar Source Class (ATNF catalogue).
"""

# Catalogue identifier
catid = "PUL"

# Catalogue FITS filename
catname = "obj-pulsar.fits"

# Probability method and threshold
prob_method      = "PROB_POST"        # Use posterior probability
prob_prior       = "nsrc() / ncpt()"  # Bayesian frequency prior
prob_thres       = 0.40               # Probability threshold
figure_of_merit  = ""                 # Formula used to calculate FoM

# Maximum number of counterparts per LAT source
max_counterparts = 100

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "" , "" ]

# List of source selection criteria (setup up to 9 criteria in the list)
selection = [ '@PUL_EDOTD2 > 5e+33' ]
