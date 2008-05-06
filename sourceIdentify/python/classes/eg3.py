#--------------------------------------------------------------------------------------------
# Source class: 3EG EGRET Catalogue
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
3EG EGRET Catalogue Source Class.
"""

# Catalogue identifier
catid = "3EG"

# Catalogue FITS filename
catname = "gamma-3eg.fits"

# Probability method and threshold
prob_method = "PROB_POST"
prob_prior  = "catch22"
prob_thres  = 0.05

# Maximum number of counterparts
max_counterparts = 10
