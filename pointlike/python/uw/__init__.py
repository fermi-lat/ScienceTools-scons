"""
THe UW system for performing spectral analysis 

at this top level define a simple "factory" function giving a simple user access to the system

$Header$
Authors:  T. Burnett, M. Kerr, E. Wallace, M. Roth
"""

def factory(**kwargs):

    from thb_roi import roi_factory, config
    import os

#    f = roi_factory.ROIfactory(config.AE(**kwargs))
    f = roi_factory.ROIfactory(**kwargs)
       
    if os.name=='nt':
        os.system('title %s' %os.getcwd())
    return f
    
