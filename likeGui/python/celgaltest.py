"""
Unit tests for celgal module.  Tests consistency between coordinate
conversions for both J2000 and B1950 epochs.  The data for these tests
were generated using the coordinate conversions available from the NED
web page: <http://nedwww.ipac.caltech.edu/>

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#$Header$

import unittest
import celgal_new as celgal

class CelgalTestCases(unittest.TestCase):
    def testJ2000Conversions(self):
        converter = celgal.celgal
        sources = {}
        sources["Crab"] = ( (83.5665, 22.0078), (184.53, -5.84) )
        sources["Geminga"] = ( (98.4909, 17.8605), (195.06, 4.32) )
        sources["Vela"] = ( (128.7346, -45.2023), (263.53, -2.86) )
        sources["3C_279"] = ( (193.9797, -5.8239), (304.98, 57.03) )
        sources["GalCenter"] = ( (266.4051, -28.9362), (0, 0) )
        dist = celgal.dist
        tol = 6e-4
        for key in sources.keys():
            assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
            assert dist(converter.gal(sources[key][0]), sources[key][1]) <tol
    def testB1950Conversions(self):
        converter = celgal.Celgal(0)
        sources = {}
        sources["Crab"] = ( (82.8143, 21.9748), (184.53, -5.84) )
        sources["Geminga"] = ( (97.7614, 17.8998), (195.06, 4.32) )
        sources["Vela"] = ( (128.3128, -45.0290), (263.53, -2.86) )
        sources["3C_279"] = ( (193.3324, -5.5534), (304.98, 57.03) )
        sources["GalCenter"] = ( (265.6108, -28.9168), (0, 0) )
        dist = celgal.dist
        tol = 1e-4
        for key in sources.keys():
            assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
            assert dist(converter.gal(sources[key][0]), sources[key][1]) < tol

if __name__ == "__main__":
    unittest.main()
