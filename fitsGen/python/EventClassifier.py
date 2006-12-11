"""
@brief Python code for parsing ROOT TCuts and partitioning Gleam
events into event classes.  The function eventClassifier should be
called via embed_python and the EventClassifier class from makeFT1.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

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

event_classes = [cut.replace('&&', 'and') for cut in eventClassCuts]

def eventClassifier(row):
    for key in row:
        exec("%s = row['%s']" % (key, key))
    for i, cut in enumerate(event_classes):
        if eval(cut):
            return i
    return -1

if __name__ == '__main__':
    rows = [{'CTBSummedCTBGAM' : 0.5,
             'CTBCORE' : 0.8},
            {'CTBSummedCTBGAM' : 0.5,
             'CTBCORE' : 0.7},
            {'CTBSummedCTBGAM' : 0.5,
             'CTBCORE' : 0.3},
            {'CTBSummedCTBGAM' : 0.3,
             'CTBCORE' : 0.8},
            {'CTBSummedCTBGAM' : 0.05,
             'CTBCORE' : 0.8}]
    for row in rows:
        print eventClassifier(row)
