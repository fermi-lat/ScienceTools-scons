/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.13  2007/09/20 16:28:21  jurgen
Enhance catalogue interface for column recognition

Revision 1.12  2006/02/09 13:06:18  jurgen
Put maximum number of source to load at once in a constant and change
value to a large value (since the loading logic has not yet been
implemented).

Revision 1.11  2006/02/07 16:05:04  jurgen
Use ObjectInfo structure to hold catalogue object information

Revision 1.10  2006/02/07 11:10:50  jurgen
Suppress catalogAccess verbosity

Revision 1.9  2006/02/03 22:10:30  jurgen
Remove comments to correctly catch an error in the determination
of the number of catalogue entries (these comments have been
introduced due to a bug in catalogAccess - which has been fixed in
version v0r2p6).

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


/* Globals __________________________________________________________________ */


/* Constants ________________________________________________________________ */


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
      m_maxCptLoad    = c_maxCptLoad;
      m_fCptLoaded    = 0;
      m_filter_maxsep = c_filter_maxsep; 
      m_memFile       = NULL;
      m_outFile       = NULL;

      // Initialise counterpart candidate working arrays
      m_numCC         = 0;
      m_cc            = NULL;

      // Initialise counterpart statistics
//      m_src_cpts      = NULL;
      m_cpt_stat      = NULL;
      m_num_Sel       = 0;

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
//      if (m_src_cpts   != NULL) delete [] m_src_cpts;
      if (m_cpt_stat   != NULL) delete [] m_cpt_stat;

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

      // Set catalogAccess verbosity
      catalogAccess::verbosity = g_u9_verbosity;
    
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
                                      double posErr, std::string &obj_name,
                                      Status status) {

    // Declare local variables
    int          caterr;
    long         i;
    double       err;
    double       err_max;
    double       err_min;
    double       err_ang;
    double       e_RA;
    double       e_DE;
    ObjectInfo  *ptr;
    
    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_input_catalogue");

    // Single loop for common exit point
    do {

      // Set catalogAccess verbosity
      catalogAccess::verbosity = g_u9_verbosity;

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
      ptr      = in->object;
      obj_name = in->cat.getNameObjName();
      for (i = 0; i < in->numLoad; i++, ptr++) {
      
        // Get object name using firt catalogAccess, then "NAME" and then "ID"
        if (in->cat.getSValue(obj_name, i, &(ptr->name)) != IS_OK) {
          obj_name = "NAME";
          if (in->cat.getSValue(obj_name, i, &(ptr->name)) != IS_OK) {
            obj_name = "ID";
            if (in->cat.getSValue(obj_name, i, &(ptr->name)) != IS_OK) {
              obj_name = "no-name";
            }
          }
        }

        // Assign source name
        ptr->name = cid_assign_src_name(ptr->name, i);

        // Initalise position, error and validity flag
        ptr->pos_valid   = 0;        // Invalid position
        ptr->pos_eq_ra   = 0.0;
        ptr->pos_eq_dec  = 0.0;
        ptr->pos_err_maj = posErr;  // Task parameter error as default
        ptr->pos_err_min = posErr;  // Task parameter error as default
        ptr->pos_err_ang = 0.0;

        // Get Right Ascension. Go to next object if no Right Ascension was
        // found
        if (in->cat.ra_deg(i,  &(ptr->pos_eq_ra)) != IS_OK) {
          if (in->cat.getNValue("RAJ2000", i, &(ptr->pos_eq_ra)) != IS_OK) {
            if (in->cat.getNValue("_RAJ2000", i, &(ptr->pos_eq_ra)) != IS_OK) {
              continue;
            }
          }
        }

        // Get Declination. Go to next object if no Right Ascension was
        // found
        if (in->cat.dec_deg(i,  &(ptr->pos_eq_dec))  != IS_OK) {
          if (in->cat.getNValue("DEJ2000", i, &(ptr->pos_eq_dec)) != IS_OK) {
            if (in->cat.getNValue("_DEJ2000", i, &(ptr->pos_eq_dec)) != IS_OK) {
              continue;
            }
          }
        }

        // Signal that we have a valid position
        ptr->pos_valid = 1;

        // Put Right Ascension in interval [0,2pi[
        ptr->pos_eq_ra = ptr->pos_eq_ra - 
                         double(long(ptr->pos_eq_ra / 360.0) * 360.0);
        if (ptr->pos_eq_ra < 0.0) 
          ptr->pos_eq_ra += 360.0;

        // Try getting position error using catalogAccess
        if (in->cat.posError_deg(i, &err) == IS_OK) {
          ptr->pos_err_maj = err;
          ptr->pos_err_min = err;
          ptr->pos_err_ang = 0.0;
        }

        // Try getting position error using POS_ERR keywords
        else if ((in->cat.getNValue("POS_ERR_MAX", i, &err_max) == IS_OK) &&
                 (in->cat.getNValue("POS_ERR_MIN", i, &err_min) == IS_OK) &&
                 (in->cat.getNValue("POS_ERR_ANG", i, &err_ang) == IS_OK)) {
          ptr->pos_err_maj = err_max;
          ptr->pos_err_min = err_min;
          ptr->pos_err_ang = err_ang;
        }

        // Try getting position error using standard keywords
        else if ((in->cat.getNValue("e_RAJ2000", i, &e_RA) == IS_OK) &&
                 (in->cat.getNValue("e_DEJ2000", i, &e_DE) == IS_OK)) {
          // Correct Right Ascension error by declination
          e_RA *= cos(ptr->pos_eq_dec*deg2rad);
          if (e_RA > e_DE) {           // Error ellipse along RA axis
            ptr->pos_err_maj = e_RA;
            ptr->pos_err_min = e_DE;
            ptr->pos_err_ang = 0.0;
          }
          else {                       // Error ellipse along DE axis
            ptr->pos_err_maj = e_DE;
            ptr->pos_err_min = e_RA;
            ptr->pos_err_ang = 90.0;
          }
        }

        // Use task parameter error in case that a position error is 0.0
        if ((ptr->pos_err_maj <= 0.0) || (ptr->pos_err_min <= 0.0)) {
          ptr->pos_err_maj = posErr;
          ptr->pos_err_min = posErr;
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
/*                            Catalogue::dump_results                          */
/* -------------------------------------------------------------------------- */
/* Private method: dump counterpart identification results                    */
/*----------------------------------------------------------------------------*/
Status Catalogue::dump_results(Parameters *par, Status status) {

    // Declare local variables
    ObjectInfo *src;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::dump_results");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Dump header
      Log(Log_2, "");
      Log(Log_2, "Counterpart identification results:");
      Log(Log_2, "===================================");
      
      // Build header
      char add[256];
      char select[256] = "";
      for (int iSel = 0; iSel < m_num_Sel; ++iSel) {
        sprintf(add, " Sel%2.2d", iSel+1);
        strcat(select, add);
      }

      // Dump header
      Log(Log_2, "                                      #Cpts%s", select);
 
      // Loop over all sources
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc) {

        // Get pointer to source object
        src = &(m_src.object[iSrc]);

        // Build selection string
        sprintf(select, " %5d", m_src_cpts[iSrc]);
        for (int iSel = 0; iSel < m_num_Sel; ++iSel) {
          sprintf(add, " %5d", m_cpt_stat[iSrc*m_num_Sel + iSel]);
          strcat(select, add);
        }

        // Dump information
        Log(Log_2, " Source %5d %18s ..: %s %s",
            iSrc+1, src->name.c_str(), select, m_cpt_names[iSrc].c_str());

      } // endfor: looped over all sources

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::dump_results (status=%d)", status);
    
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
/*   +-- cfits_create (create output catalogue)                               */
/*   |                                                                        */
/*   +-- get_input_catalogue (get source catalogue)                           */
/*   |                                                                        */
/*   N-- cid_get (get counterpart candidates for each source)                 */
/*   |   |                                                                    */
/*   |   +-- cid_filter (filter step)                                         */
/*   |   |   |                                                                */
/*   |   |   +-- get_input_catalogue (get counterpart catalogue)              */
/*   |   |                                                                    */
/*   |   +-- cid_refine (refine step)                                         */
/*   |   |   |                                                                */
/*   |   |   +-- cid_prob_angsep (get probability from angular separation)    */
/*   |   |   |                                                                */
/*   |   |   +-- cfits_clear (clear in-memory catalogue)                      */
/*   |   |   |                                                                */
/*   |   |   +-- cfits_add (add quantities to in-memory catalogue)            */
/*   |   |   |                                                                */
/*   |   |   +-- cfits_eval (evaluate quantities in in-memory catalogue)      */
/*   |   |   |                                                                */
/*   |   |   +-- cfits_colval (extract probability info from in-memory cat.)  */
/*   |   |   |                                                                */
/*   |   |   +-- cid_sort (sort counterpart candidates)                       */
/*   |   |                                                                    */
/*   |   +-- cfits_add (add candidates to output catalogue)                   */
/*   |                                                                        */
/*   +-- cfits_collect (collect counterpart results)                          */
/*   |                                                                        */
/*   +-- cfits_eval (evaluate output catalogue quantities)                    */
/*   |                                                                        */
/*   +-- cfits_select (select output catalogue entries)                       */
/*   |                                                                        */
/*   +-- cfits_save (save output catalogue)                                   */
/*   |                                                                        */
/*   +-- dump_results (dump results)                                          */
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
      status = cfits_create(&m_memFile, "mem://gtsrcid", par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create FITS memory catalogue"
              " 'mem://gtsrcid'.", (Status)status);
        continue;
      }
    
      // Create FITS output catalogue on disk
      status = cfits_create(&m_outFile, (char*)par->m_outCatName.c_str(), par, 
                            status);
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
      status = get_input_catalogue(par, &m_src, par->m_srcPosError, m_src_name,
                                   status);
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

      // Set vectors dimensions
      m_src_cpts  = std::vector<int>(m_src.numLoad);
      m_cpt_names = std::vector<std::string>(m_src.numLoad);
      
      // Loop over all sources
      for (iSrc = 0; iSrc < m_src.numLoad; iSrc++) {
      
        // Get counterpart candidates for the source
        status = cid_get(par, iSrc, status);
        if (status != STATUS_OK)
          break;

        // Collect counterpart statistics (before selection!)
        m_src_cpts[iSrc] = m_numCC;
     
      } // endfor: looped over all sources
      if (status != STATUS_OK)
        continue;

      // Collect statistics (used to build counterpart names)
      std::vector<int> stat;
      status = cfits_collect(m_outFile, par, stat, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to collect statistics from catalogue.",
                        (Status)status);
        continue;
      }

      // Evaluate output catalogue quantities
      status = cfits_eval(m_outFile, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to evaluate output catalogue quantities .",
              (Status)status);
        continue;
      }

      // Select output catalogue counterparts
      status = cfits_select(m_outFile, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to select output catalogue counterparts.",
              (Status)status);
        continue;
      }

      // Save output catalogue counterparts
      status = cfits_save(m_outFile, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to save output catalogue.",
              (Status)status);
        continue;
      }

      // Dump counterpart results
     if (par->logTerse()) {
       status = dump_results(par, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to dump counterpart results.",
                (Status)status);
          continue;
        }
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
