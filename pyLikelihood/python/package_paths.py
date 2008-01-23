"""
Package-dependent shared libraries and environment variables that will
be added to python_setup.([c]sh, bat) by
SwigPolicy/v*/src/startup_scripts.py.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

import os

if os.name == 'posix':
    inst_dir = '${INST_DIR}'
    caldb = '${CALDB}'
else:
    inst_dir = '%INST_DIR%'
    caldb = '%CALDB%'

from swig_setup import packageroot

caldbroot = os.path.join(inst_dir, 'irfs',
                         '%s' % packageroot('caldb'), 'CALDB')

extra_paths = [('CALDB',
                os.path.join(caldbroot, 'data', 'glast', 'lat')), 
               ('CALDBCONFIG', os.path.join(caldbroot, 'software', 'tools', 
                                            'caldb.config')),
               ('CALDBALIAS', os.path.join(caldbroot, 'software', 'tools', 
                                           'alias_caldb.fits'))]
