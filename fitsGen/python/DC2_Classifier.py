"""
@brief DC2 version of Python code for parsing ROOT TCuts and
partitioning Gleam events into event classes.  The function
eventClassifier should be called via embed_python and the
EventClassifier class from makeFT1.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
meritVariables = """
Tkr1FirstLayer  CTBCORE  CTBGAM  CTBBestEnergyProb
""".split()

#
# Example event class cuts.
#
eventClassCuts = ["CTBCORE > 0.35 && CTBBestEnergyProb > 0.35 && "
                  + " CTBGAM > 0.50 && Tkr1FirstLayer > 5.5",
                  "CTBCORE > 0.35 && CTBBestEnergyProb > 0.35 && "
                  + " CTBGAM > 0.50 && Tkr1FirstLayer < 5.5",
                  "CTBCORE > 0.1 && CTBBestEnergyProb > 0.3 && "
                  + " CTBGAM > 0.35 && Tkr1FirstLayer > 5.5",
                  "CTBCORE > 0.1 && CTBBestEnergyProb > 0.3 && "
                  + " CTBGAM > 0.35 && Tkr1FirstLayer < 5.5"]

event_classes = [cut.replace('&&', 'and') for cut in eventClassCuts]

def eventClassifier(row):
    for key in row:
        exec("%s = row['%s']" % (key, key))
    for i, cut in enumerate(event_classes):
        if eval(cut):
            return i
    return -1
