#--------------------------------------------------------------------------------------------
# Source class: CEA EGRET Catalogue
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
CEA EGRET Catalogue Source Class.

This is a special class since the EGRET errox boxes are in general larger than the LAT
error boxes. Therefore the formal probabilities are not used (the probability threshold
is set to 0) but a selection criterion is used that considers all counterparts within
an angular separation of 0.5 deg or within the 3 sigma error box.
"""

# Catalogue identifier
catid = "EGR"

# Catalogue FITS filename
catname = "gamma-egr.fits"

# Probability method and threshold
prob_method      = "PROB_POST"    # Formula used to calculate probability
prob_prior       = 0.70           # Formula used to calculate prior
prob_thres       = 0.0            # Probability threshold
figure_of_merit  = ""             # Formula used to calculate FoM

# Maximum number of counterparts per LAT source
max_counterparts = 1

# Derives angular separation in standard deviations
new_quantity = [ "ANGSEP_SDEV = 2.0 * ANGSEP / @EGR_THETA95" ]

# Limits angular separation to < 0.5 deg or to 3 sigma EGRET error box
selection = [ "ANGSEP < 0.5 || ANGSEP_SDEV < 3.0" ]
