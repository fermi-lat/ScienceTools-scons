#!/usr/bin/env python
"""
Basic script for steering the observationSim code.  No plotting.

@author J. Chiang
"""
#
# $Header$
#
import os, sys, string, numarray

#
# LAT response and observationSim packages
#
latResponseRoot = os.getenv("LATRESPONSEROOT")
sys.path.append(latResponseRoot + "/python")
import observationSim, latResponse

observationSimRoot = os.getenv("OBSERVATIONSIMROOT")

caldbPath = latResponseRoot + "/data/CALDB"

def run_test(argv):
    """
    The steering function.  argv is a tuple containing the arguments
    (rootname, counts, <source_names>).
    """
    #
    # One needs to provide the full path to the xml files
    #
    xml_files = observationSim.StringVector([observationSimRoot
                                             + "/xml/source_library.xml",
                                             observationSimRoot
                                             + "/xml/3EG_catalog_32MeV.xml",
                                             observationSimRoot
                                             + "/xml/test_sources.xml"])

    if (len(argv) == 2 and argv[1] == "-h"):
        print "usage: test.py rootname counts [source_names]"
        return 0
    
    if (len(argv) > 1):
        root = argv[1]
    else:
        root = "test"

    if (len(argv) > 2):
        count = int(string.atof(argv[2]))
    else:
        count = 1000
        
    source_names = observationSim.StringVector()
    useSimTime = 0                # Generate a number of counts by default
    if (len(argv) > 3):
        for name in argv[3:]:
            if (name == '-t'):    # Detect flag to interpret variable
                useSimTime = 1    # count as seconds of simulation time
            else:
                source_names.append(name)
    print type(source_names)
    if len(source_names) == 0:
        source_names.append("all_3EG_sources")

    my_simulator = observationSim.Simulator(source_names, xml_files)

    aeff = latResponse.AeffGlast25(caldbPath + "/aeff_lat.fits", 4)
    psf = latResponse.PsfGlast25(caldbPath + "/psf_lat.fits", 4)
    edisp = latResponse.EdispGlast25()

    response = latResponse.Irfs(aeff, psf, edisp)

    useA1fmt = 1
    events = observationSim.EventContainer(root + "_events", useA1fmt)
    scData = observationSim.ScDataContainer(root + "_scData", useA1fmt)

    spacecraft = observationSim.LatSc();

    #
    # Break up the count into bite-sized chunks to allow for
    # intermediate plotting of the results by HippoDraw
    #
    if useSimTime:
        elapsed_time = 0.
        time_step = 2.*60.    # Update every two minutes of simulation time
        while (elapsed_time < count - time_step):
            my_simulator.generate_events(time_step, events, scData, 
                                         response, spacecraft)
        my_simulator.generate_events(count-elapsed_time, events, scData, 
                                     response, spacecraft)
    else:
        num_made = 0
        numstep = 30          # Work in chunks of 30 events
        while (num_made < count - numstep):
            sys.stderr.write("%i  " % num_made)
            my_simulator.genNumEvents(num_made + numstep, events, scData,
                                      response, spacecraft)
            num_made += numstep
        my_simulator.genNumEvents(count, events, scData,
                                  response, spacecraft)
    return root

if __name__ == "__main__":
    root = run_test(sys.argv)
    
