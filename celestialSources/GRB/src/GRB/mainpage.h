 // $Header$
 // Mainpage for doxygen

 /*! \mainpage GRB Package: simulating a transient source

   This source description is an example of time dependent spectrum.
   <br>
   The GRB model that we have adopted is based on the \em fireball model of Gamma Ray Burst.
   We adopted this model because it is able to well reproduce the fast time variability observed in the GRB signal. 
   <br>
   The GRB simulator can be use with several test program:
   
   - test_GRB.exe\n
   This executable tests the GRB algorithm. It inizializes the GRB simulation,
   and it extracts photons from the spectrum.
   To launch it tipe test_GRB.exe. 
   The option are contained in the joboptions.txt file.
   
   - test_GRBROOT.exe
   This test program makes use of the ROOT graphical envirorment to display
   some plots regarding the simulated GRB. It shows the evolution of the flux \
   with respect to the time, and it plots the integrated spectrum and the 
   light curves.
   
   <br>
   <hr>
   \section Howto How to use the GRB spectrum in Gleam
   <br>
   GRB is an indipendent package, it is anexternal service and it has to be 
   declared in the joboption file of FluxSvc. For example here there is the 
   two lines that add GRBSvc in the external services available and add the 
   GRB shared library. 
   They could be added, for example, to the "defaultOptions.txt" of FluxSvc
   <br>
   \verbatim 
   //================================================== 
   ApplicationMgr.ExtSvc += { "GRBSvc" };
   ApplicationMgr.DLLs   += { "GRB" };
   //==================================================
   \endverbatim
   
   <br>
   At this point GRB is available from an external application (as Gleam). 
   To have the item "GRB" in the sources menu of the gui one can just edit 
   the xml file containing the source definition adding the following lines:  
   <br>
   \verbatim 
   <source name="GRB" >
   <spectrum> <SpectrumClass name="GRBSpectrum"/> <use_spectrum/> </spectrum>
   </source>
   <hr>
   \section parameters GRB Parameters
   \endverbatim
   <br>
   The file /src/test/GRBParam.txt holds information used for the description of the GRB. It can be changed without recompiling the entire package.
   <br>
   It MUST be in /src/test directory of the GRB package. 
   <br>
The File GRBParam.txt contains information about:

   -# Number of shells emitted by the engine (nshell).
   -# The redshift of the source.
   -# the total energy available in erg.
   -# The initial separation from one shell to the subsequent.
   -# The initial thickness of the shells.
   -# the minimum Lorentz factor of the shells
   -# the maximum lorentz factor of the shells
   \include GRBParam.txt
   The /src/test/jobOptions.txt file holds information used for the
   implementation of GRB algorithm. It doesn`t contain any information 
   regarding the physics of the GRB (that are all included in the GRBParam 
   file), but it manages some options available for the GRB Algorithm.
   
   \include jobOptions.txt
   
   \section requirements requirements
   \include requirements


 */


