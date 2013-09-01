#!/usr/bin/env python

import sys, string
from math import pow

def read_3EG_catalog():
    true_name = []
    name = []
    ra = []
    dec = []
    F100 = []
    gamma = []
    other_names = []
    file = '3EG_catalog.txt'
    catalog = open(file)
    lines = catalog.readlines()
    names = []
    for line in lines:
        data = line.split(',')
        if data[0][:3] == '3EG':
            true_name.append(data[0])
            if (data[0][9] == '+'):
                name.append('_' + data[0][:3] + '_' + data[0][4:9] + 'p' + data[0][10:])
            else:
                name.append('_' + data[0][:3] + '_' + data[0][4:9] + 'm' + data[0][10:])
            ra.append(string.atof(data[1]))
            dec.append(string.atof(data[2]))
            if (string.strip(data[8]) != '---'):
                gamma.append(string.atof(string.strip(data[8])))
            else:
                # use default index
                gamma.append(2.1)
            # use the first F100 flux that's listed...
            flux = string.atof(data[6])*1e-4
        if len(data) == 17 and string.strip(data[11]) == 'P1234':
            # but replace with the P1234 average value if it's available
            flux = string.atof(data[6])*1e-4
        if len(data) == 17 and string.strip(data[13]) != '':
            names.append(string.strip(data[13]))
        if len(data) == 1 and string.strip(data[0]) == '':
            F100.append(flux)
            other_names.append(names)
            names = []
    return (true_name, name, ra, dec, F100, gamma, other_names)

(true_name, name, ra, dec, F100, gamma, other_names) = read_3EG_catalog()

if len(sys.argv) == 2:
    emin = string.atoi(sys.argv[1])
    sys.stderr.write("emin = %s\n" % emin)
else:
    emin = 100.

sys.stdout.write('<source_library title="The_3EG_catalog">\n\n')
for i in range(len(name)):
    sys.stdout.write("<!-- %s" % true_name[i])
    if (len(other_names[i]) > 0):
        sys.stdout.write(", also known as\n     ")
        for names in other_names[i]:
            sys.stdout.write("%s " % names)
        sys.stdout.write("-->\n")
    else:
        sys.stdout.write(" -->\n")
    Femin = F100[i]*pow(emin, 1. - gamma[i])/pow(100., 1. - gamma[i])
    sys.stdout.write('<source name="%s" flux="%s">\n' % (name[i], Femin))
    sys.stdout.write('   <spectrum escale="MeV">\n')
    sys.stdout.write('       <particle name="gamma">\n')
    sys.stdout.write('           <power_law emin="%s" emax="100000." gamma="%s"/>\n' % (emin, gamma[i]))
    sys.stdout.write('       </particle>\n')
    sys.stdout.write('       <celestial_dir ra="%s" dec="%s"/>\n' % (ra[i], dec[i]))
    sys.stdout.write('   </spectrum>\n')
    sys.stdout.write('</source>\n\n')

sys.stdout.write('<!-- All the 3EG sources -->\n')
sys.stdout.write('<source name = "all_3EG_sources">\n')
for i in range(len(name)):
    sys.stdout.write('   <nestedSource sourceRef="%s" />\n' % name[i])
sys.stdout.write('</source>\n\n')

sys.stdout.write('</source_library>\n')
