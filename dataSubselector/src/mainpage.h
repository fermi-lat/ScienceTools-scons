/**
   @mainpage dataSubselector Tool

   @author J. Chiang (based on original implementation of T. Stephens)

   @section intro Introduction

   The dataSubselector tool allows the user to make more detailed cuts
   on the data extracted from the D1 Database by the U1 Tool.<br>
 
   Cuts can be made on the following columns in the data
   - RA & Dec (J2000 degrees)
   - time (MET seconds)
   - Energy (MeV)
   - conversion layer (0-15)
   - theta & phi (<it>apparent</it> photon direction in instrument
     coordinates) (h)
   - IMGAMMAPROB column (h)
   - Zenith Angle (h)
   - reconstruction flags:
       - background (h)
       - PSF (h)
       - energy resolution (h)

   (h) indicates a hidden parameter in the par file.

   <hr>
   @section notes Release Notes
   release.notes

   <hr>
   @section requirements Requirements
   @verbinclude requirements
 
*/
