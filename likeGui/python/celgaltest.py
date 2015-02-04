"""
Unit tests for celgal module.  Tests consistency between coordinate
conversions for both J2000 and B1950 epochs.  The data for these tests
were generated using the coordinate conversions available from the NED
web page: <http://nedwww.ipac.caltech.edu/>

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#$Header$

import unittest
import celgal as celgal

class CelgalTestCases(unittest.TestCase):
    def testJ2000Conversions(self):
        converter = celgal.celgal(1)
        sources = {}
        sources["Crab"] = ( (83.5665, 22.0078), (184.52996368, -5.83998396) )
        sources["Geminga"] = ( (98.4909, 17.8605), (195.06003431, 4.31998806) )
        sources["Vela"] = ( (128.7346, -45.2023), (263.52989939, -2.86004251) )
        sources["3C_279"] = ( (193.9797, -5.8239), (304.97995250, 57.02996622) )
        sources["GalCenter"] = ( (266.40508920, -28.93617470), (0, 0) )
        dist = celgal.dist
        tol = 2e-4
        for key in sources.keys():
            assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
            assert dist(converter.gal(sources[key][0]), sources[key][1]) <tol
    def testB1950Conversions(self):
        converter = celgal.celgal(0)
        sources = {}
        sources["Crab"] = ( (82.8143, 21.9748), (184.52995465, -5.83998847) )
        sources["Geminga"] = ( (97.7614, 17.8998), (195.06003244, 4.31995548) )
        sources["Vela"] = ( (128.3128, -45.0290), (263.53005171, -2.85999746) )
        sources["3C_279"] = ( (193.3324, -5.5534), (304.97991590, 57.02999541) )
        sources["GalCenter"] = ( (265.61084403, -28.91679035), (0, 0) )
        dist = celgal.dist
        tol = 1e-4
        for key in sources.keys():
            assert dist(sources[key][0], converter.cel(sources[key][1])) < tol
            assert dist(converter.gal(sources[key][0]), sources[key][1]) < tol

if __name__ == "__main__":
    unittest.main()
