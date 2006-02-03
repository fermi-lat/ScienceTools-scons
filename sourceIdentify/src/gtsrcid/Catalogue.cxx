/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.8  2006/02/03 12:14:51  jurgen
New version that allows additional probabilities to be taken
into account. The code has been considerably reorganised. Also
catalogue column prefixes are now handled differently.

Revision 1.7  2006/02/02 09:31:55  jurgen
fix last Win32 bug !!!

Revision 1.6  2006/02/02 09:29:29  jurgen
correct Win32 bug

Revision 1.5  2006/02/02 09:26:14  jurgen
correct Win32 compile bugs

Revision 1.4  2006/02/01 15:17:10  jurgen
correct g++-3.4.3 compile error related to parentheses around (char*)

Revision 1.3  2006/02/01 13:33:36  jurgen
Tried to fix Win32 compilation bugs.
Change revision number to 1.3.2.
Replace header information with CVS typeset information.

------------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"
#include "Catalogue.h"
#include "Log.h"
#include "src/quantity.h"


/* Definitions ______________________________________________________________ */
#define CATALOGUE_TIMING   0                     // Enables timing measurements


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {
using namespace catalogAccess;


/* Type defintions __________________________________________________________ */


/* Private Prototypes _______________________________________________________ */


/*----------------------------------------------------------------------------*/
/*                            Catalogue::init_memory                          */
/* -------------------------------------------------------------------------- */
/* Private method: init memory                                                */
/*----------------------------------------------------------------------------*/
void Catalogue::init_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {
    
      // Initialise source catalogue private members
      m_src.numLoad  = 0;
      m_src.numTotal = 0;
      m_src.object   = NULL;

      // Initialise counterpart catalogue private members
      m_cpt.numLoad  = 0;
      m_cpt.numTotal = 0;
      m_cpt.object   = NULL;

      // Intialise catalogue building parameters
      m_maxCptLoad    = 100000; // load maximum of 100000 sources
      m_fCptLoaded    = 0;
      m_filter_maxsep = 4.0; // filter all sources more distant than 2 deg
      m_memFile       = NULL;
      m_outFile       = NULL;

      // Initialise counterpart candidate working arrays
      m_numCC         = 0;
      m_cc            = NULL;

      // Initialise output catalogue quantities
      m_num_src_Qty   = 0;
      m_num_cpt_Qty   = 0;

    } while (0); // End of main do-loop
    
    // Return
    return;

}


/*----------------------------------------------------------------------------*/
/*                             Catalogue::free_memory                         */
/* -------------------------------------------------------------------------- */
/* Private method: free memory                                                */
/*----------------------------------------------------------------------------*/
void Catalogue::free_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {

      // Free temporary memory
      if (m_src.object != NULL) delete [] m_src.object;
      if (m_cpt.object != NULL) delete [] m_cpt.object;
      if (m_cc         != NULL) delete [] m_cc;

      // Initialise memory
      init_memory();

    } while (0); // End of main do-loop
    
    // Return
    return;

}


/*----------------------------------------------------------------------------*/
/*                       Catalogue::get_input_descriptor                      */
/* -------------------------------------------------------------------------- */
/* Private method: get descriptor for input catalogue                         */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_input_descriptor(Parameters *par, std::string catName, 
                                       InCatalogue *in, Status status) {

    // Declare local variables
    int                      caterr;
    std::vector<std::string> titles;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_input_descriptor");

    // Single loop for common exit point
    do {
    
      // Determine the number of objects in the catalogue. First we try to
      // access the catalogue on disk, then on the Web ...
      caterr = in->cat.getMaxNumRows(&in->numTotal, catName);
      if (caterr < 0) {
        if (par->logVerbose())
          Log(Warning_2, "%d : Unable to determine catalogue '%s' size from"
              " file. Try on Web now.", caterr, catName.c_str());
        caterr = in->cat.getMaxNumRowsWeb(&in->numTotal, catName);
        if (caterr < 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of objects in"
                " catalogue '%s'.", caterr, catName.c_str());
          status = STATUS_CAT_NOT_FOUND;
          continue;
        }
      }
          
      // Import the catalogue descriptor. First we try to access the catalogue 
      // on disk, then on the Web ...
      caterr = in->cat.importDescription(catName);
      if (caterr < 0) {
        if (par->logVerbose())
          Log(Warning_2, "%d : Unable to load catalogue '%s' descriptor from"
              " file. Try on Web now.", caterr, catName.c_str());
        caterr = in->cat.importDescriptionWeb(catName);
        if (caterr < 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to load catalogue '%s' descriptor from"
                " file or web.", caterr, catName.c_str());
          status = STATUS_CAT_NOT_FOUND;
          continue;
        }
        else {
          if (par->logVerbose())
            Log(Log_2, " Loaded catalogue '%s' descriptor from web.", 
                catName.c_str());
        }
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Loaded catalogue '%s' descriptor from file.", 
              catName.c_str());
      }
      
      // Store input name
      in->inName = catName;

      // Set title vector to 6 elements
      titles.clear();
      titles.push_back(" ");
      titles.push_back(" ");
      titles.push_back(" ");
      titles.push_back(" ");
      titles.push_back(" ");
      titles.push_back(" ");
      
      // Extract titles from catalogue
      in->cat.getCatalogTitles(&titles);

      // Store catalogue title information
      in->catCode   = titles[0];
      in->catURL    = titles[1];
      in->catName   = titles[2];
      in->catRef    = titles[3];
      in->tableName = titles[4];
      in->tableRef  = titles[5];

      // Determine the number of loaded objects in catalogue (should be 0)
      in->cat.getNumRows(&in->numLoad);
           
      // Dump catalogue descriptor (optionally)
      if (par->logVerbose()) {
        status = dump_descriptor(in, status);
        if (status != STATUS_OK)
          continue;
        Log(Log_2, " ");
      }
    
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_input_descriptor (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                        Catalogue::get_input_catalogue                      */
/* -------------------------------------------------------------------------- */
/* Private method: get data for input catalogue                               */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_input_catalogue(Parameters *par, InCatalogue *in,
                                      Status status) {

    // Declare local variables
    int         caterr;
    long        i;
    ObjectInfo *ptr;
    
    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_input_catalogue");

    // Single loop for common exit point
    do {

      // First interpret the input string as filename and load the catalogue
      // from the file. If this fails then interpret input string as
      // catalogue name and load from WEB.
      caterr = in->cat.import(in->inName);
      if (caterr < 0) {
        if (par->logVerbose())
          Log(Warning_2, "%d : Unable to load catalogue '%s' from file.",
              caterr, in->inName.c_str());
        caterr = in->cat.importWeb(in->inName);
        if (caterr < 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to load catalogue '%s' from file or web.",
                caterr, in->inName.c_str());
          status = STATUS_CAT_NOT_FOUND;
          continue;
        }
        else {
          if (par->logVerbose())
            Log(Log_2, " Loaded catalogue '%s' from web.", in->inName.c_str());
        }
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Loaded catalogue '%s' from file.", in->inName.c_str());
      }
      
      // Determine the number of loaded objects in catalogue. Fall throgh if
      // there are no objects loaded
      in->cat.getNumRows(&in->numLoad);
      if (in->numLoad < 1)
        continue;

      // Allocate memory for object information
      if (in->object != NULL) delete [] in->object;
      in->object = new ObjectInfo[in->numLoad];
      if (in->object == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }
          
      // Extract object information
      ptr = in->object;
      for (i = 0; i < in->numLoad; i++, ptr++) {

        // Get location. If no location is available then set it to 9999.
        // Put counterpart Right Ascension in interval [0,2pi[
        if ((in->cat.ra_deg(i,  &(ptr->ra))  != IS_OK) ||
            (in->cat.dec_deg(i, &(ptr->dec)) != IS_OK)) {
          ptr->ra  = 9999;
          ptr->dec = 9999;
        }
        else {
          ptr->ra = ptr->ra - double(long(ptr->ra / 360.0) * 360.0);
          if (ptr->ra < 0.0) ptr->ra += 360.0;
        }
        
      } // endfor: looped over all objects

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_input_catalogue (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                          Catalogue::dump_descriptor                        */
/* -------------------------------------------------------------------------- */
/* Private method: dump catalogue descriptor                                  */
/*----------------------------------------------------------------------------*/
Status Catalogue::dump_descriptor(InCatalogue *in, Status status) {

    // Declare local variables
    long                                               iQty;
    long                                               numQty;
    long                                               len;
    long                                               maxLenNames;
    long                                               maxLenUnits;
    long                                               maxLenForms;
    long                                               maxLenUCDs;
    std::string                                        qtyType;
    std::vector<std::string>                           qtyNames;
    std::vector<std::string>                           qtyUnits;
    std::vector<std::string>                           qtyUCDs;
    std::vector<catalogAccess::Quantity>               qtyDesc;
    std::vector<catalogAccess::Quantity::QuantityType> qtyTypes;

    // Single loop for common exit point
    do {
          
      // Extract information from catalogue
      numQty = in->cat.getQuantityNames(&qtyNames);
      numQty = in->cat.getQuantityUnits(&qtyUnits);
      numQty = in->cat.getQuantityUCDs(&qtyUCDs);
      numQty = in->cat.getQuantityDescription(&qtyDesc);
      numQty = in->cat.getQuantityTypes(&qtyTypes);
      
      // Get string lengths
      maxLenNames = 0;
      maxLenUnits = 0;
      maxLenForms = 0;
      maxLenUCDs  = 0;
      for (iQty = 0; iQty < numQty; iQty++) {
        if ((len = strlen(qtyNames[iQty].c_str())) > maxLenNames)
          maxLenNames = len;
        if ((len = strlen(qtyUnits[iQty].c_str())) > maxLenUnits)
          maxLenUnits = len;
        if ((len = strlen(qtyDesc[iQty].m_format.c_str())) > maxLenForms)
          maxLenForms = len;
        if ((len = strlen(qtyUCDs[iQty].c_str())) > maxLenUCDs)
          maxLenUCDs = len;
      }

      // Dump catalogue information
      Log(Log_2, " Catalogue input name .............: %s", 
          in->inName.c_str());
      Log(Log_2, " Catalogue code ...................: %s", 
          in->catCode.c_str());
      Log(Log_2, " Catalogue URL ....................: %s", 
          in->catURL.c_str());
      Log(Log_2, " Catalogue name ...................: %s", 
          in->catName.c_str());
      Log(Log_2, " Catalogue reference ..............: %s", 
          in->catRef.c_str());
      Log(Log_2, " Catalogue table name .............: %s", 
          in->tableName.c_str());
      Log(Log_2, " Catalogue table reference ........: %s", 
          in->tableRef.c_str());
      Log(Log_2, " Number of objects in catalogue ...: %d", 
          in->numTotal);
      Log(Log_2, " Number of loaded objects .........: %d", 
          in->numLoad);
      Log(Log_2, " Number of quantities (columns) ...: %d", 
          numQty);

      // Dump information about catalogue quantitites
      for (iQty = 0; iQty < numQty; iQty++) {
      
        // Set quantity type
        switch (qtyTypes[iQty]) {
        case 0:
          qtyType = "vector";
          break;
        case 1:
          qtyType = "numerical";
          break;
        case 2:
          qtyType = "string";
          break;
        default:
          qtyType = "unknown";
          break;
        }
        
        // Dump quantity
        Log(Log_2, 
            "  Quantity %3d ....................: %*s [%*s] (%*s) <%*s> (%s)", 
            iQty+1, 
            maxLenNames, qtyNames[iQty].c_str(),
            maxLenUnits, qtyUnits[iQty].c_str(),
            maxLenForms, qtyDesc[iQty].m_format.c_str(),
            maxLenUCDs,  qtyUCDs[iQty].c_str(),
            qtyType.c_str());

      } // endfor: looped over quantities
    
    } while (0); // End of main do-loop
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                              Catalogue::build                              */
/* -------------------------------------------------------------------------- */
/* Build counterpart catalogue.                                               */
/* -------------------------------------------------------------------------- */
/* build                                                                      */
/*   |                                                                        */
/*   +-- get_input_descriptor (get source catalogue input descriptior)        */
/*   |                                                                        */
/*   +-- get_input_descriptor (get counterpart catalogue input descriptior)   */
/*   |                                                                        */
/*   +-- create_output_catalogue (create output catalogue)                    */
/*   |                                                                        */
/*   +-- get_input_catalogue (get source catalogue)                           */
/*   |                                                                        */
/*   N-- get_counterpart_candidates (get counterpart candidates for each src) */
/*   |   |                                                                    */
/*   |   +-- get_counterparts (filter step)                                   */
/*   |   |   |                                                                */
/*   |   |   +-- get_input_catalogue (get counterpart catalogue)              */
/*   |   |                                                                    */
/*   |   +-- get_probability (refine step)                                    */
/*   |   |   |                                                                */
/*   |   |   +-- get_probability_angsep (get probability from angular sep.)   */
/*   |   |   |                                                                */
/*   |   |   +-- cc_sort (sort counterpart candidates by decreasing prob.)    */
/*   |   |                                                                    */
/*   |   +-- add_counterpart_candidates (add candidates to output catalogue)  */
/*   |   |                                                                    */
/*   |   +-- dump_counterpart_candidates (dump candidates to output catalogue)*/
/*   |                                                                        */
/*   +-- eval_output_catalogue_quantities (evaluate output catalogue qties)   */
/*   |                                                                        */
/*   +-- select_output_catalogue (select output catalogue entries)            */
/*----------------------------------------------------------------------------*/
Status Catalogue::build(Parameters *par, Status status) {

    // Declare local variables
    long iSrc;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::build");
        
    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Prepare catalogues:");
        Log(Log_2, "===================");
      }
      
      // Get input catalogue descriptors
      status = get_input_descriptor(par, par->m_srcCatName, &m_src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to load source catalogue '%s' descriptor.",
              (Status)status, par->m_srcCatName.c_str());
        continue;
      }
      status = get_input_descriptor(par, par->m_cptCatName, &m_cpt, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to load counterpart catalogue '%s'"
              " descriptor.", (Status)status, par->m_cptCatName.c_str());
        continue;
      }

      // Create FITS catalogue in memory
      status = cfits_create(&m_memFile, "mem://gtsrcid", par, par->logNormal(),
                            status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create FITS memory catalogue"
              " 'mem://gtsrcid'.", (Status)status);
        continue;
      }
    
      // Create FITS output catalogue on disk
      status = cfits_create(&m_outFile, (char*)par->m_outCatName.c_str(), par, 
                            par->logNormal(), status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create FITS output catalogue '%s'.", 
              (Status)status, par->m_outCatName.c_str());
        continue;
      }

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Build counterpart candidate catalogue:");
        Log(Log_2, "======================================");
      }
      
      // Load source catalogue
      status = get_input_catalogue(par, &m_src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to load source catalogue '%s' data.",
              (Status)status, par->m_srcCatName.c_str());
        continue;
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Source catalogue loaded.");
      }
      
      // Stop of the source catalogue is empty
      if (m_src.numLoad < 1) {
        status = STATUS_CAT_EMPTY;
        if (par->logTerse())
          Log(Error_2, "%d : Source catalogue is empty. Stop", (Status)status);
        continue;
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Source catalogue contains %d sources.", m_src.numLoad);
      }
      
      // Loop over all sources
      for (iSrc = 0; iSrc < m_src.numLoad; iSrc++) {
      
        // Get counterpart candidates for the source
        status = cid_get(par, iSrc, status);
        if (status != STATUS_OK)
          break;
      
      } // endfor: looped over all sources
      if (status != STATUS_OK)
        continue;

      // Evaluate output catalogue quantities
      status = cfits_eval(m_outFile, par, par->logNormal(), status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to evaluate output catalogue quantities .",
              (Status)status);
        continue;
      }

      // Select output catalogue counterparts
      status = cfits_select(m_outFile, par, par->logNormal(), status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to select output catalogue counterparts.",
              (Status)status);
        continue;
      }

      // Save output catalogue counterparts
      status = cfits_save(m_outFile, par, par->logNormal(), status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to save output catalogue.",
              (Status)status);
        continue;
      }
      
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::build (status=%d)", status);
    
    // Return status
    return status;

}


/* Namespace ends ___________________________________________________________ */
}
