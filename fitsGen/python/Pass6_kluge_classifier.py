"""
@brief Pass 6 event classes.  See
http://www-glast.stanford.edu/protected/mail/irf/0157.html

These definitions partition the events into three classes.
"""
#
# $Header$
#
from EventClassifier import EventClassifier

meritVariables = """
CTBClassLevel
CTBCORE
CTBBestEnergyProb
Tkr1FirstLayer
""".split()

#
# Note that the prefilter cuts applied by ROOT (the TCuts par file
# option), should contain the cuts that are common to all event
# classes.
#
# In Pass6_kluge, these event class defs will go into CTBCLASSLEVEL
#
eventClassCuts = ["(CTBClassLevel==1) || ((CTBCORE<0.1) || (CTBBestEnergyProb<0.1))",
                  "(CTBClassLevel==2) && (CTBCORE>0.1) && (CTBBestEnergyProb>0.1)",
                  "(CTBClassLevel==3) && (CTBCORE>0.1) && (CTBBestEnergyProb>0.1)"]

##
## The current P6_v1 IRFs *do not* partition the data, i.e., we are still relying on
## CTBClassLevel in FT1 to get Bill's "analysis classes".  In this case, we need
## partition the data into two event classes: just front vs back.
##
#
#eventClassCuts = ["17 - Tkr1FirstLayer < 11.5",
#                  "17 - Tkr1FirstLayer > 11.5"]

eventClassifier = EventClassifier(eventClassCuts)

if __name__ == '__main__':
#    rows = [{'CTBClassLevel' : 1},
#            {'CTBClassLevel' : 2,
#             'CTBCORE' : 0.05,
#             'CTBBestEnergyProb' : 0.05},
#            {'CTBClassLevel' : 3,
#             'CTBCORE' : 0.05,
#             'CTBBestEnergyProb' : 0.11},
#            {'CTBClassLevel' : 2,
#             'CTBCORE' : 0.11,
#             'CTBBestEnergyProb' : 0.11},
#            {'CTBClassLevel' : 3,
#             'CTBCORE' : 0.11,
#             'CTBBestEnergyProb' : 0.11}]
#    classes = (0, 0, 0, 1, 2)
#
    rows = [{'Tkr1FirstLayer' : 10}, 
            {'Tkr1FirstLayer' : 12}]
    classes = (0, 1)
    for row, id in zip(rows, classes):
        assert(id == eventClassifier(row))
