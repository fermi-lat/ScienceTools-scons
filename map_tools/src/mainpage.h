// $Header$
// Mainpage for doxygen

/*! \mainpage package map_tools

   \authors Toby Burnett

   \section introduction Introduction

   This package contains utility classes for managing GLAST images for count and exposure maps, 
   and the following applications.

   \section applications Applications
    
    - count_map, defined in count_map.cxx, much of analysis in class
	SkyImage, with parameters from class CountMapParameters, data selection from class SkyMap
    - exposure_cube, defined in exposure_cube.cxx. Also uses  CountMapParameters, Exposure. Create a hypercube 
    in ra, dec, costheta, save as a multi-image FITS file.
    - exposure_map, defined in exposure_map.cxx. Also uses SkyImage, MapParameters, Exposure. 
    Read in a hypercube, make a sky map using a basic function to describe the effective area vs. angle.
<br>
    Each application has an example  .par file in the pfiles folder.
    <hr>
 \section fitsstuff FITS Stuff

 A selection of classes for FITS I/O, copied from Goodi,  currently resides in the folder @table, in the namespace table.
 When adequtate replacement exists, these will be removed.


  \section notes release notes
  release.notes
  \section requirements requirements
  \include requirements

*/


/**
 @page userGuide User's Guide

@section par par files
 The format of the entries in a .par file is given in the <a
 href="http://www-glast.slac.stanford.edu/sciencetools/userInterface/doc/pil.pdf">PIL user
 manual</a>.  The ordering of the fields in each line is
 - variable name
 - variable type (s=string, r=real, i=integer, b=boolean)
 - query mode (q=ask, h=hidden, l=learn)
 - default value
 - lower bound or a list of allowed selections delimited by "|"
 - upper bound
 - prompt string

 @section env Environment variables
If a file name does not start with a a "/" character, it is assumed to be in a folder given by the appropriate
environment variable

- INFILES  input files
- PFILES   parameter fiels
- OUTFILES output files
- TPLDIR   template fiels


 @section countmap count_map

This application reads (ra,dec) pairs from a tuple, from either FITS or ROOT, and makes a FITS image file.
The user can specify a selection string, but this can be slow, and for large input files it is better to use
another tool, such a ftselect (FITS) or the root command line, to prepare a new input file.  

 @verbinclude count_map.par

 @section exposurecube exposure_cube

This applicaton creates a FITS "hypercube" image file. Each layer is a bin in cos(theta). 

 @verbinclude exposure_cube.par

 @section exposuremap exposure_map

 Here we create exposure maps for a specific effective area cos theta function. A linear function is wired into this 
 application, see Aeff,  but is is quite representative of the function for most energies. A map for a specific energy, 
 or integrated over energy with a power-law weighting, could be done easily by replacing the Aeff functor.


 @verbinclude exposure_map.par


*/