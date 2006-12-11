"""
@brief Pass 4 event classes based on Toby's straw man proposal.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
from EventClassifier import EventClassifier

meritVariables = """
EvtRun    EvtEnergyCorr 
McEnergy  McXDir  McYDir  McZDir   
McXDirErr   McYDirErr  McZDirErr   
McTkr1DirErr  McDirErr  
GltWord   FilterStatus_HI 
Tkr1FirstLayer  
CTBCORE  CTBSummedCTBGAM  CTBBestEnergyProb
""".split()

#
# Example event class cuts.
#
eventClassCuts = ['CTBSummedCTBGAM>=0.5 && CTBCORE>=0.8',
                  'CTBSummedCTBGAM>=0.5 && CTBCORE>=0.5 && CTBCORE<0.8',
                  'CTBSummedCTBGAM>=0.5 && CTBCORE<0.5',
                  'CTBSummedCTBGAM>=0.1 && CTBSummedCTBGAM<0.5',
                  'CTBSummedCTBGAM<0.1']

eventClassifier = EventClassifier(eventClassCuts)
