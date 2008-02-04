"""
@brief Pass 5 event classes.  See
http://confluence.slac.stanford.edu/display/DC2/Summary+of+response+function+sets

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
from EventClassifier import EventClassifier

meritVariables = """
EvtRun    EvtEnergyCorr 
GltWord   FilterStatus_HI 
Tkr1FirstLayer  
CTBBestEnergyProb CTBCORE CTBBestEnergy CTBBestEnergyRatio CTBClassLevel
""".split()

#
# Note that the prefilter cuts applied by ROOT (the TCuts par file
# option), should contain the cuts that are common to all event
# classes.  For this version of Pass5, these are just those associated
# with the P5_v0_transient class.  Alternative analysis classes can be
# obtained via judicious use of fcopy and selecting on CTBCLASSLEVEL.
#

commonCuts = '&&'.join(('1',))

#
# These divide into front vs back
#
eventClassCuts = ['&&'.join((commonCuts, '(Tkr1FirstLayer>5.5)')),
                  '&&'.join((commonCuts, '(Tkr1FirstLayer<5.5)'))]
#eventClassCuts = ['&&'.join(('1',))]

eventClassifier = EventClassifier(eventClassCuts)
