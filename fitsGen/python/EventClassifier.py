"""
@brief Python code for parsing ROOT TCuts and partitioning Gleam
events into event classes.  A function object eventClassifier should
be created as an instance of EventClassifier with the desired TCuts
defining the various classes. The eventClassifier functor will be
called from makeFT1 via embed_python and the fitsGen::EventClassifier
class.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

class EventClassifier(object):
    def __init__(self, eventClassCuts):
        self.event_classes = [cut.replace('&&', 'and').replace('||', 'or')
                              for cut in eventClassCuts]
    def __call__(self, row):
        for key in row:
            exec("%s = row['%s']" % (key, key))
        for i, cut in enumerate(self.event_classes):
            if eval(cut):
                return i
        return -1

if __name__ == '__main__':
    TCuts = ['CTBSummedCTBGAM>=0.5 && CTBCORE>=0.8',
             'CTBSummedCTBGAM>=0.5 && CTBCORE>=0.5 && CTBCORE<0.8',
             'CTBSummedCTBGAM>=0.5 && CTBCORE<0.5',
             'CTBSummedCTBGAM>=0.1 && CTBSummedCTBGAM<0.5',
             'CTBSummedCTBGAM<0.1']
    eventClassifier = EventClassifier(TCuts)
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
    for i, row in enumerate(rows):
        assert(i == eventClassifier(row))
