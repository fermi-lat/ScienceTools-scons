#--------------------------------------------------------------------------------------------
# Source class: Quasars
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
Quasar Source Class (Veron 2003 catalogue).
"""

# Catalogue identifier
catid = "QSO"

# Catalogue FITS filename
catname = "obj-qso.fits"

# Probability method and threshold
prob_method = "PROB_POST"
prob_prior  = 0.01
prob_thres  = 0.10

# Maximum number of counterparts
max_counterparts = 10
