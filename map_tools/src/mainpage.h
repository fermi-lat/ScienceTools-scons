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

