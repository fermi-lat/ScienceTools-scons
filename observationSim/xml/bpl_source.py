#!/usr/bin/env python

import sys, string

if len(sys.argv) < 5:
    print "usage: %s flux gamma1 gamma2 ebreak [emin emax]" % sys.argv[0]
    sys.exit(0)

flux = string.atof(sys.argv[1])
gamma1 = string.atof(sys.argv[2])
gamma2 = string.atof(sys.argv[3])
e0 = string.atof(sys.argv[4])

if len(sys.argv) == 7:
    emin = string.atof(sys.argv[5])
    emax = string.atof(sys.argv[6])
else:
    emin = 30.
    emax = 1e5

ratio = ((gamma2 - 1.)/(gamma1 - 1.)*(e0**(1.-gamma1) - emin**(1. - gamma1))
         /(emax**(1.-gamma2) - e0**(1.-gamma2)))*e0**(gamma1 - gamma2)

flux1 = ratio*flux/(1. + ratio)
flux2 = flux1/ratio

print """
    <!-- Low energy part of a broken power-law source -->
    <source name="low_bpl" flux="%.4f">
        <spectrum escale="MeV">
            <particle name="gamma"> 
                <power_law emin="%.2f" emax="%.2f" gamma="%.2f"/>
            </particle>
            <celestial_dir ra="0." dec="0."/>
        </spectrum>
    </source>

    <!-- High energy part of a broken power-law source -->
    <source name="high_bpl" flux="%.4f">
        <spectrum escale="MeV">
            <particle name="gamma"> 
                <power_law emin="%.2f" emax="%.2f" gamma="%.2f"/>
            </particle>
            <celestial_dir ra="0." dec="0."/>
        </spectrum>
    </source>

    <!-- A strong broken power-law point source. -->
    <source name = "bpl_source">
        <nestedSource sourceRef="low_bpl" />
        <nestedSource sourceRef="high_bpl" />
    </source>
""" % (flux1, emin, e0, gamma1, flux2, e0, emax, gamma2)
