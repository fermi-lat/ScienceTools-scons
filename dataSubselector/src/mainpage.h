/**
 * @mainpage dataSubselection Tool
 *
 * @author  Tom Stephens
 *
 * @section intro Introduction
 * The dataSubselection tool allows the user to make more detailed cuts on the data extracted
 * from the D1 Database by the U1 Tool.<br>
 * 
 * The dataSubselection tool works by using the cut parameters to build a cfitsio row filtering
 * command.  This command is then used to filter the data when the file is opened.
 * The filtered data is then written to the output file. <br>
 * 
 * Currently cuts can be made on the following columns in the data
 * <ul>
 * <li>RA & Dec
 * <li>Time
 * <li>Energy
 * <li>Theta & Phi (instrument coordinates)
 * <li>Zenith Angle
 * <li>IMGAMMAPROB column
 * <li>various data cuts (background, PSF and energy resolution) from the reconstruction
 * </ul>
 * 
 * <hr>
 * @section jobOptions jobOptions
 * @subsection usage Usage
 * @verbinclude dataSubselector.usage
 * <hr>
 * @section notes Release Notes
 * Prototypes:
 * <ul>
 * <li>v1r0 - This is the initial version to get the Goodi functionality working to read
 * and write the files.
 * <li>v1r0p2 - Inital version for DC1 - works on linux but not check out on Windows.
 * </ul>
 * <hr>
 * @section requirements Requirements
 * <ul>
 * <li>Goodi - v2r4p9 or later
 * </ul>
 * <hr>
 * 
 */
