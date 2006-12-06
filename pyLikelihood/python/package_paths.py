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

extra_paths = [('HANDOFF_RESPONSEROOT',
                os.path.join(inst_dir, 'irfs',
                             '%s' %packageroot('handoff_response'))),
               ('CALDB',
                os.path.join(inst_dir, 'irfs',
                             '%s' % packageroot('caldb'), 'CALDB')), 
               ('CALDBCONFIG',
                os.path.join(caldb, 'software', 'tools', 'caldb.config')),
               ('CALDBALIAS',
                os.path.join(caldb, 'software', 'tools', 'alias_caldb.fits'))]
