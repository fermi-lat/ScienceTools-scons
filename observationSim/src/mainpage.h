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
 IRAF-style parameter file, called <a
 href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/data/obsSim.par?cvsroot=CVS_SLAC">obsSim.par</a>,
 prepared first.  This file must must sit in a directory specified by
 the PFILES environment variable, or if (and only if) that variable is
 not set, the file must be in the current working directory. \n\n
 Here's an example obsSim.par file:

 @verbinclude obsSim.par

 The format of the entries in a .par file is given in the <a
 href="http://isdc.unige.ch/Soft/AstroRoot/pil.ps">PIL user
 manual</a>.  A cursory description is also available at the
 Likelihood User's Guide page.  We describe each parameter entry in
 turn.

 - @b XML_source_file This file contains flux-style definitions of
   sources to be available for simulation.  A description of the
   format for the flux-style source model entries can be found in the
   Doxygen documentation for the <a
   href="http://www.slac.stanford.edu/exp/glast/ground/software/RM/documentation/GlastRelease/GlastRelease-v3r3p7/flux/v8r2/">flux
   package</a>.
   \n\n 
   Two xml source files are provide by default, <a
   href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/xml/source_library.xml?cvsroot=CVS_SLAC">source_library.xml</a>
   and <a
   href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/xml/3EG_catalog_30MeV.xml?cvsroot=CVS_SLAC">3EG_catalog_30MeV.xml</a>.
   The former contains a variety of diagnostic sources, including some
   that should only be to be used by Gleam such as cosmic ray sources.
   It also contains sources that are useful for @b obsSim such as
   "galdiffusemap30" which simulates the Galactic diffuse emission
   using the EGRET diffuse model, extrapolated to 30 MeV.  The latter
   file contains sources from the Third EGRET catalog, using the Phase
   1-3/Cycle 4 fluxes (when available), extrapolated to 30 MeV.  For
   reference, this file was generated from the <a
   href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/xml/3EG_catalog.txt?cvsroot=CVS_SLAC">ascii
   version</a> of the catalog using <a
   href="http://glast.stanford.edu/cgi-bin/cvsweb/observationSim/xml/3EG_to_xml.py?cvsroot=CVS_SLAC">this
   Python script</a>.

 - @b Source_list This file contains the list of sources to be used in
   the simulation.  This list must comprise sources appearing in the
   default XML source files or in the user-provided XML file.  If a
   name appears more than once, either explicitly or because it is
   included as a nested source in a composite, its flux contribution
   will be multiplied by the number of occurrences.
   \n\n
   For example, @b Source_list might contain
   @verbatim
   all_3EG_sources
   galdiffusemap30
   extragalactic
   @endverbatim
   The first entry will produce events from all 271 3EG sources; the
   second source simulates the Galactic diffuse emission model used by
   EGRET; and the third source provides isotropic emission that models
   the extragalactic diffuse as reported by <a
   href="http://adsabs.harvard.edu/cgi-bin/nph-bib_query?bibcode=1998ApJ...494..523S&amp;db_key=AST&amp;high=3e6fc354ed13604">Sreekumar
   et al. 1998</a>.

 - @b Number_of_events This is either the number of events to be created
   by the simulation or the simulation time in seconds.

 - @b Use_as_sim_time A flag to determine whether or not to treat @b
   Number_of_events as the simulation time.

 - @b Response_functions "FRONT/BACK" and "COMBINED" are the Glast25
   parameterizations, which do not include energy dispersion.
   "FRONT/BACK_10" and "COMBINED_10" are those same PSF and effective
   area parameterizations, including an ad hoc 10\% energy
   resolution. When additional LAT IRFs are available, they will be
   included in the list of valid options.

 - @b Output_file_prefix The root name for the FITS files to be created.

 - @b Maximum_effective_area This value is used by the 
   <a
   href="http://www.slac.stanford.edu/exp/glast/ground/software/RM/documentation/GlastRelease/GlastRelease-v3r3p7/flux/v8r2/">flux package</a> as
   the cross-sectional area in \f$m^2\f$ of a sphere containing the
   LAT.  Larger area values will produce more incident photons, and
   since photons are selected on the basis of the effective area,
   those larger values will result in a less efficient simulation.
   This value will depend on the various response functions that are
   used.  The Glast25 parameterizations have a maximum value effective
   area of 1.21\f$m^2\f$.

 - @b Start_time This is the start time for the simulation in mission
   elapsed time (in seconds).

 - @b Pointing_history_file This is the ascii file from which flux reads
   the orbit and attitude information about the spacecraft.  If set to
   "none", then the orbit is computed using the astro::EarthOrbit class
   with \f$\pm 35^\circ\f$ once-per-orbit step-rocking .

 - @b Maximum_number_of_rows This is the maximum number of rows that
   the output FITS files are to contain.  If multiple files are
   required, they are labeled as "<prefix>_events_0000.fits",
   "<prefix>_events_0001.fits", etc..

 Here's a sample session using the above obsSim.par file:

 @verbatim
glast-guess1[jchiang] obsSim.exe
File containing source names [source_names.dat] : 
Number of events (or simulation time in seconds) <1 - 4e7> [5000] : 
Use number of events as simulation time? [no] : 
Response functions to use <FRONT/BACK|COMBINED|FRONT/BACK_10|COMBINED_10> [FRONT/BACK] : 
Prefix for output files [test] : 
Pointing history file [pointing_history.txt] : none
Generating 5000 events....
Done.
glast-guess1[jchiang] ls test*fits
test_events_0000.fits  test_scData_0000.fits
glast-guess1[jchiang] 
 @endverbatim

*/
