#--------------------------------------------------------------------------------------------
# Source class: Pulsars
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
# catid:            Catalogue Identifier (prefixes results and determines result name)
# catname:          Catalogue FITS name
# new_quantity:     List of new quantities
# selection:        List of selection criteria
# prob_method:      Probability method
# prob_prior:       Prior probability
# prob_thres:       Probability threshold
# max_counterparts: Maximum number of counterparts
#--------------------------------------------------------------------------------------------
"""
Pulsar Source Class (ATNF catalogue).
"""

# Catalogue identifier
catid = "PUL"

# Catalogue FITS filename
catname = "obj-pulsar.fits"

# Probability method and threshold
prob_method = "PROB_POST"
prob_prior  = 0.01
prob_thres  = 0.10

# Maximum number of counterparts
max_counterparts = 10

# Selection criteria
selection = ['@PUL_EDOTD2 > 10e+32']

#chatter=1
