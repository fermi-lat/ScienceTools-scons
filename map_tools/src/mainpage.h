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
    - read_map, example executable showing how to copy an image from one file to another, redefining the 
    image parameters
    - map_stats Prints out statistics for a map.
    - HealpixArray, defined in HealpixArray.h  Template used to associate an object with
      each healpix pixel.
    - CosineBinner, defined in CosineBinner.h.  Bins for storing values corresponding
      to different values of cos(theta).
    - BasicExposure, defined in Exposure.cxx.  A generic exposure class.
    - SkyBinner, defined in Exposure.cxx.  A HealpixArray of CosineBinner objects.
    - SkyExposure, defined in Exposure.cxx.  A BasicExposure constructed from a SkyBinner
      and a CosineBinner.
    - Exposure, defined in Exposure.cxx.  Derived from SkyExposure.
      Adds I/O and other functions.
    - HealpixArrayIO, Defined in HealpixArrayIO.cxx. A singleton used to manage I/O of
      a HealpixArray object to/from a FITS file.
      
    <br>
    Each application has an example  .par file in the pfiles folder.
    <hr>

  
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

 The package requirements file defines the following environment variables
- INFILES  input files, subfolder "data"
- PFILES   parameter files, subfolder "pfiles" - PIL will expect the parameter file to be here
- OUTFILES output files, subfolder "output"

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

 @section readmap read_map

 A simple application that reads a value from a map.
 @verbinclude read_map.par

 @section statmap map_stats

 A simple application that generates a new map from a SkyFunction object, a FITS image in this case.
 @verbinclude map_stats.par

 @section HealpixArray HealpixArray

 Used to associate a particular type of object with each pixel in a Healpix tesselation.
 This is a convenient way to data with pixels.

 @section CosineBinner CosineBinner

 A set of bins that can be used to store numbers which correspond to different values of
 cos(theta).  Use the setBinning function member to set static binning options, including
 number of bins, min value for cos(theta), and whether to use sqrt weighting for the bins.

 @section BasicExposure BasicExposure

 A differential exposure template.  A sky pixelization class (such as HealpixArray or HTM)
 and an angular binner class (such as CosineBinner) must be provided to create a
 specific instance of BasicExposure.

 @section SkyBinner SkyBinner

 A HealpixArray of CosineBinner objects.

 @section SkyExposure SkyExposure

 A BasicExposure which uses HealpixArray<CosineBinner> as the sky pixelization and
 CosineBinner as the angular binner class.

 @section Exposure Exposure

 A SkyExposure with methods for input and output to/from a FITS file.  (Uses 
 HealpixArrayIO.)

 @section HealpixArrayIO HealpixArrayIO

 A singleton class used to manage I/O of a HealpixArray object to/from a FITS file.
*/
*/
/** @page devguide Developer's Guide

[in preparation]
*/