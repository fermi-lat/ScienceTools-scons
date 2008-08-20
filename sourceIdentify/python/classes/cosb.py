#--------------------------------------------------------------------------------------------
# Source class: COS-B Catalogue
#--------------------------------------------------------------------------------------------
#
# $Author$
# $Revision$
# $Date$
#--------------------------------------------------------------------------------------------
"""
COS-B Catalogue Source Class.

This is a special class since the COS-B errox boxes are in general larger than the LAT
error boxes. Therefore the formal probabilities are not used (the probability threshold
is set to 0) but a selection criterion is used that considers all counterparts within
an angular separation of 2.0 deg or within the 3 sigma error box.
"""

# Catalogue identifier
catid = "2CG"

# Catalogue FITS filename
catname = "gamma-cosb.fits"

# Probability method and threshold
prob_method      = "PROB_POST"    # Formula used to calculate probability
prob_prior       = "0.70"         # Formula used to calculate prior
prob_thres       = 0.0            # Probability threshold
figure_of_merit  = ""             # Formula used to calculate FoM

# Maximum number of counterparts per LAT source
max_counterparts = 1

# List of new catalogue quantities (setup up to 9 formulae in the list)
new_quantity = [ "ANGSEP_SDEV = 2.0 * ANGSEP / @2CG_PosErr90" ]

# Limits angular separation to < 2.0 deg to 3 sigma COS-B error box
selection = [ "ANGSEP < 2.0 || ANGSEP_SDEV < 3.0" ]
