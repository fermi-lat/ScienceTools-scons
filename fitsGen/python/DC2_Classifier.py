"""
@brief DC2 version of Python code for parsing ROOT TCuts and
partitioning Gleam events into event classes. 

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

from EventClassifier import EventClassifier

meritVariables = """
Tkr1FirstLayer  CTBCORE  CTBGAM  CTBBestEnergyProb
""".split()

#
# DC2 Event class cuts.  Note that the ordering ensures that class A
# events are assigned first, so that class B events need only be
# defined by their looser lower limits.  Defining the cuts using an
# order from more to less restrictive also helps ensure that the
# events are properly partitioned.
#
eventClassCuts = ["CTBCORE > 0.35 && CTBBestEnergyProb > 0.35 && "
                  + " CTBGAM > 0.50 && Tkr1FirstLayer > 5.5",
                  "CTBCORE > 0.35 && CTBBestEnergyProb > 0.35 && "
                  + " CTBGAM > 0.50 && Tkr1FirstLayer < 5.5",
                  "CTBCORE > 0.1 && CTBBestEnergyProb > 0.3 && "
                  + " CTBGAM > 0.35 && Tkr1FirstLayer > 5.5",
                  "CTBCORE > 0.1 && CTBBestEnergyProb > 0.3 && "
                  + " CTBGAM > 0.35 && Tkr1FirstLayer < 5.5"]

eventClassifier = EventClassifier(eventClassCuts)
