// Mainpage for doxygen

/** @mainpage package observationSim

 @author James Chiang

 @section intro Introduction

 This package uses the flux package to generate photon events from
 astrophysical sources and the latResponse package to process those
 photons according to a specified set of instrument response
 functions.

 <hr>
 @section notes Release Notes
  release.notes

 <hr>
 @section requirements requirements
 @verbinclude requirements
*/

/**  
 @page userGuide The obsSim Application

 @section obsSim Using the obsSim application

 Since all user interaction is handled by HOOPS, one must have the
 IRAF-style parameter file, called <a href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/data/obsSim.par?cvsroot=CVS_SLAC">obsSim.par</a>, prepared first.  This
 file must either be in the current working directory or it must sit
 in a directory specified by the PFILES environment variable.  Here's
 an example obsSim.par file:

 @verbinclude obsSim.par

 Note that each entry in the file referred to by <a href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/data/xml_files.dat?cvsroot=CVS_SLAC">XML_source_filenames</a> must provide the
 complete path:

 @verbinclude xml_files.dat

 Relative paths will not be resolved properly.  One can use
 environment variables, so that a source xml file in the current
 working directory can be specified as "$(PWD)/my_model.xml".  A
 description of the format for the flux-style source model entries can
 be found in the Doxygen documentation for the <a
 href="http://www.slac.stanford.edu/exp/glast/ground/software/RM/documentation/GlastRelease/GlastRelease-v3r3p7/flux/v8r2/">flux
 package</a>.

 The source names in the Source_list refer to sources appearing in the
 files listed in XML_source_filenames.  If a name appears more than
 once, either explicitly or because it is included as a nested source
 in a composite, its flux contribution will be multiplied by the
 number of occurrences.  The file <a href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/data/source_names.dat?cvsroot=CVS_SLAC">source_names.dat</a> might contain

 @verbinclude source_names.dat

 The first entry will produce events from all 271 3EG sources using
 their fluxes averaged over Phases 1--3/Cycle 4, when available.  The
 second source yields Galactic diffuse emission extrapolated down to
 30 MeV using the EGRET model in the flux package.  The third source
 provides isotropic emission that models the extragalactic diffuse as
 reported by <a href="http://adsabs.harvard.edu/cgi-bin/nph-bib_query?bibcode=1998ApJ...494..523S&amp;db_key=AST&amp;high=3e6fc354ed13604">Sreekumar et al.\ 1998</a>.

 Here's a sample session using the above obsSim.par file:

 @verbatim
 glast-guess1[jchiang] ../rh72_gcc2953/obsSim.exe
 File containing XML file names [xml_files.dat] : 
 File containing source names [source_names.dat] : 
 Number of events (or simulation time in seconds) <1 - 4e7> [1000] : 3600
 Use number of events as simulation time? [no] : yes
 Response functions to use <FRONT/BACK|COMBINED> [FRONT/BACK] : 
 Prefix for output files [virgo_region] : all_sky_hour
 Generating events for a simulation time of 3600 seconds....
 Done.
 glast-guess1[jchiang] ls all_sky_hour*
 all_sky_hour_events_0000.fits  all_sky_hour_scData_0000.fits
 glast-guess1[jchiang] 
 @endverbatim

*/
