/*----------------------------------------------------------------------------*/
/*                               Catalogue.cxx                                */
/* -------------------------------------------------------------------------- */
/* Task            : Catalogue interface file.                                */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.3.1                                                    */
/* Date of version : 20-Dec-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/* 1.1.0  21-Jul-2005  add debug information                                  */
/* 1.2.0  26-Sep-2005  - adapted generic quantity names to U9 (v0r2p3)        */
/*                     - add UCD keywords to output FITS file                 */
/* 1.3.0  19-Dec-2005  - prefix class members by "m_"                         */
/*                     - introduce maximum acceptance angle for filter step   */
/*                     - extract counterpart locations only once              */
/* 1.3.1  20-Dec-2005  - treat R.A. wrap around correctly                     */
/*----------------------------------------------------------------------------*/

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


/* Prototypes _______________________________________________________________ */
int set_fits_col_format(catalogAccess::Quantity *desc, std::string *format);
int fits_tform_binary(int typecode, long repeat, long width, 
                      std::string *format);


/*----------------------------------------------------------------------------*/
/*                             set_fits_col_format                            */
/* -------------------------------------------------------------------------- */
/* Private function: set FITS column format from catalogue description        */
/*----------------------------------------------------------------------------*/
int set_fits_col_format(catalogAccess::Quantity *desc, std::string *format) {

    // Declare local variables
    std::string            col_format;
    std::string::size_type len;
    std::string::size_type pos;

    // Single loop for common exit point
    do {

      // Set quantity format
      switch (desc->m_type) {
      case Quantity::VECTOR:
        *format = "1E";
        break;
      case Quantity::NUM:
        *format = "1E";
        break;
      case Quantity::STRING:
        col_format = desc->m_format;
        len        = col_format.length();
        pos        = col_format.find("A",0);
        if (pos != std::string::npos) {
          if (pos < (len-1)) {
            pos++;
            len -= pos;
            *format = col_format.substr(pos,len) + "A";
          }
          else
            *format = col_format;
        }
        else
          *format = "10A";
        break;
      default:
        *format = "1E";
        break;
      }

    } while (0); // End of main do-loop
    
    // Return
    return STATUS_OK;

}


/*----------------------------------------------------------------------------*/
/*                               fits_tform_binary                            */
/* -------------------------------------------------------------------------- */
/* Private function: set FITS column format from catalogue description        */
/*----------------------------------------------------------------------------*/
int fits_tform_binary(int typecode, long repeat, long width, 
                      std::string *format) {

    // Declare local variables
    char add[256];

    // Single loop for common exit point
    do {

      // Set column format
      switch (typecode) {
      case TBIT:
        *format = "X";
        break;
      case TBYTE:
        *format = "B";
        break;
      case TLOGICAL:
        *format = "L";
        break;
      case TSTRING:
        *format = "A";
        break;
      case TSHORT:
        *format = "I";
        break;
      case TINT32BIT:
        *format = "J";
        break;
      case TLONGLONG:
        *format = "K";
        break;
      case TFLOAT:
        *format = "E";
        break;
      case TDOUBLE:
        *format = "D";
        break;
      case TCOMPLEX:
        *format = "C";
        break;
      case TDBLCOMPLEX:
        *format = "M";
        break;
      default:
        format->clear();
        break;
      }

      // Add repeat string
      if (repeat > 0) {
        sprintf(add, "%ld", repeat);
        *format = add + *format;
      }

      // Add width string
      if (width > 0) {
        sprintf(add, "%ld", width);
        *format = *format + add;
      }

    } while (0); // End of main do-loop
    
    // Return
    return STATUS_OK;

}


/*----------------------------------------------------------------------------*/
/*                            Catalogue::init_memory                          */
/* -------------------------------------------------------------------------- */
/* Private method: init memory                                                */
/*----------------------------------------------------------------------------*/
void Catalogue::init_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {

      // Intialise private members
      m_numSrc        = 0;
      m_numCpt        = 0;
      m_maxCptLoad    = 100000; // load maximum of 100000 sources
      m_fCptLoaded    = 0;
      m_filter_maxsep = 4.0; // filter all sources more distant than 2 deg
      m_cpt_loc       = NULL;
      m_outFile       = NULL;
      m_numCC         = 0;
      m_cc            = NULL;
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
      if (m_cc != NULL) delete [] m_cc;

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
                                       catalogAccess::Catalog *cat,
                                       Status status) {

    // Declare local variables
    int caterr;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_input_descriptor");

    // Single loop for common exit point
    do {
    
      // First interpret the input string as filename and load the catalogue
      // from the file. If this fails then interpret input string as
      // catalogue name and load from WEB.
      caterr = cat->importDescription(catName);
      if (caterr < 0) {
        if (par->logVerbose())
          Log(Warning_2, "%d : Unable to load catalogue '%s' descriptor from"
              " file.", caterr, catName.c_str());
        caterr = m_src.importDescriptionWeb(catName);
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
      
      // Debug mode: dump catalogue descriptor
      if (par->logVerbose()) {
        status = dump_descriptor(cat, status);
        if (status != STATUS_OK)
          continue;
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
Status Catalogue::get_input_catalogue(Parameters *par, std::string catName, 
                                      catalogAccess::Catalog *cat,
                                      Status status) {

    // Declare local variables
    int caterr;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_input_catalogue");

    // Single loop for common exit point
    do {

      // First interpret the input string as filename and load the catalogue
      // from the file. If this fails then interpret input string as
      // catalogue name and load from WEB.
      caterr = cat->import(catName);
      if (caterr < 0) {
        if (par->logVerbose())
          Log(Warning_2, "%d : Unable to load catalogue '%s' from file.",
              caterr, catName.c_str());
        caterr = m_src.importWeb(catName);
        if (caterr < 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to load catalogue '%s' from file or web.",
                caterr, catName.c_str());
          status = STATUS_CAT_NOT_FOUND;
          continue;
        }
        else {
          if (par->logVerbose())
            Log(Log_2, " Loaded catalogue '%s' from web.", catName.c_str());
        }
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Loaded catalogue '%s' from file.", catName.c_str());
      }
      
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_input_catalogue (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                    Catalogue::get_counterpart_candidates                   */
/* -------------------------------------------------------------------------- */
/* Private method: get  counterpart candidates for source 'iSrc'              */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_counterpart_candidates(Parameters *par, long iSrc, 
                                             Status status) {

    // Declare local variables
    double      ra;
    double      dec;
    double      pos_err;
    std::string src_name;
    std::string obj_name;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_counterpart_candidates");

    // Single loop for common exit point
    do {

      // Determine the name of the generic quantity "object name"
      obj_name = m_src.getNameObjName();

      // Get source coordinates
      m_src.ra_deg(iSrc,  &ra);
      m_src.dec_deg(iSrc, &dec);
      m_src.posError_deg(iSrc, &pos_err);
      m_src.getSValue(obj_name, iSrc, &src_name);

      // Optionally dump source information
      if (par->logExplicit()) {
        Log(Log_2, "");
        Log(Log_2, " Source %5d .....................: %s"
            " (%8.3f,%8.3f) +/- %8.3f",
            iSrc, src_name.c_str(), ra, dec, pos_err);
      }
        
      // Get counterparts near the source position
      status = get_counterparts(par, &ra, &dec, status);
      if (status != STATUS_OK)
        continue;

      // Assign probability for each counterpart
      status = get_probability(par, iSrc, status);
      if (status != STATUS_OK)
        continue;

      // Add all counterpart candidates to output catalogue
      status = add_counterpart_candidates(par, iSrc, status);
      if (status != STATUS_OK)
        continue;

      // Optionally dump counterpart candidats
      if (par->logExplicit())
        dump_counterpart_candidates(par, status);

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_counterpart_candidates (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                         Catalogue::get_counterparts                        */
/* -------------------------------------------------------------------------- */
/* Private method: get data from counterpart catalogue                        */
/* -------------------------------------------------------------------------- */
/*                  Filter step of counterpart identification                 */
/*                                                                            */
/* If the counterpart catalogue is big we load here only the part of the      */
/* catalogue that spatially overlaps with the specified source position.      */
/* Otherwise we load the entire catalogue (only on the first call).           */
/* For all counterpart candidates that were loaded we calculate the angular   */
/* separation to the source of interest and keep only those candidates that   */
/* are spatially sufficient close to the source.                              */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_counterparts(Parameters *par, double *ra, double *dec,
                                   Status status) {

    // Declare local variables
    long        iCpt;
    long        numNoPos;
    long        numRA;
    long        numDec;
    double      src_dec_sin;
    double      src_dec_cos;
    double      cpt_dec_min;
    double      cpt_dec_max;
    double      cpt_ra_min;
    double      cpt_ra_max;
    double      filter_maxsep;
    ObjectInfo *cpt_info;
    CCElement  *cpt_ptr;

    // Timing measurements
    #if CATALOGUE_TIMING
    clock_t t_start_loop;
    clock_t t_start_tot   = clock();
    float   t_elapse        = 0.0;
    float   t_elapse_loop   = 0.0;
    #endif

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_counterparts");

    // Single loop for common exit point
    do {

      // Reset statistics
      m_numCC  = 0;
      numNoPos = 0;
      numRA    = 0;
      numDec   = 0;

      // Calculate sin and cos of source latitude
      src_dec_sin = sin(*dec * deg2rad);
      src_dec_cos = cos(*dec * deg2rad);
      
      // Define bounding box around source position. The declination
      // range of the bounding box is constrained to [-90,90] deg, the
      // Right Ascension boundaries are put into the interval [0,360[ deg.
      cpt_dec_min = *dec - m_filter_maxsep;
      cpt_dec_max = *dec + m_filter_maxsep;
      if (cpt_dec_min < -90.0) cpt_dec_min = -90.0;
      if (cpt_dec_max >  90.0) cpt_dec_max =  90.0;
      if (src_dec_cos > 0.0) {
        filter_maxsep = m_filter_maxsep / src_dec_cos;
        if (filter_maxsep > 180.0)
          filter_maxsep = 180.0;
      }
      else
        filter_maxsep = 180.0;
      cpt_ra_min  = *ra - filter_maxsep;
      cpt_ra_max  = *ra + filter_maxsep;
      cpt_ra_min = cpt_ra_min - double(long(cpt_ra_min / 360.0) * 360.0);
      if (cpt_ra_min < 0.0) cpt_ra_min += 360.0;
      cpt_ra_max = cpt_ra_max - double(long(cpt_ra_max / 360.0) * 360.0);
      if (cpt_ra_max < 0.0) cpt_ra_max += 360.0;
    
      // If the counterpart catalogue is big then load only sources from a
      // region around the specified position ...
      if (m_numCpt > m_maxCptLoad) {
      }
      
      // ... otherwise load the entire counterpart catalogue
      else {

        // Load only if noy yet done ...
        if (!m_fCptLoaded) {

          // Load counterpart catalogue
          status = get_input_catalogue(par, par->m_cptCatName, &m_cpt, status);
          if (status != STATUS_OK) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to load counterpart catalogue '%s'"
                  " data.", (Status)status, par->m_cptCatName.c_str());
            continue;
          }
          else {
            if (par->logVerbose())
              Log(Log_2, " Counterpart catalogue loaded.");
          }

          // Determine the number of sources in counterpart catalogue. Stop if
          // the catalogue is empty
          m_cpt.getNumRows(&m_numCpt);
          if (m_numCpt < 1) {
            status = STATUS_CAT_EMPTY;
            if (par->logTerse())
              Log(Error_2, "%d : Counterpart catalogue is empty. Stop.", 
                  (Status)status);
            continue;
          }
          else {
            if (par->logVerbose())
              Log(Log_2, " Counterpart catalogue contains %d sources.", 
                  m_numCpt);
          }
          
          // Allocate memory for counterpart candidate search
          m_cc      = new CCElement[m_numCpt];
          m_cpt_loc = new ObjectInfo[m_numCpt];
          if (m_cc == NULL || m_cpt_loc == NULL) {
            status = STATUS_MEM_ALLOC;
            if (par->logTerse())
              Log(Error_2, "%d : Memory allocation failure.", (Status)status);
            continue;
          }
          
          // Extract counterpart location information
          cpt_info = m_cpt_loc;
          for (iCpt = 0; iCpt < m_numCpt; iCpt++, cpt_info++) {
          
            // Get location. If no location is available then set it to 9999.
            // Put counterpart Right Ascension in interval [0,2pi[
            if ((m_cpt.ra_deg(iCpt,  &(cpt_info->ra))  != IS_OK) ||
                (m_cpt.dec_deg(iCpt, &(cpt_info->dec)) != IS_OK)) {
              cpt_info->ra  = 9999;
              cpt_info->dec = 9999;
            }
            else {
              cpt_info->ra = cpt_info->ra - 
                             double(long(cpt_info->ra / 360.0) * 360.0);
              if (cpt_info->ra < 0.0) cpt_info->ra += 360.0;
            }
            
          } // endfor: looped over all counterparts
 
          // Bookkeep catalogue loading
          m_fCptLoaded = 1;
          
        } // endif: catalogue was not yet loaded
      } // endelse: entire catalogue requested

      // Initialise counterpart pointers
      cpt_info = m_cpt_loc;
      cpt_ptr  = m_cc;

      // Loop over counterparts
      #if CATALOGUE_TIMING
      t_start_loop = clock();
      #endif
      for (iCpt = 0; iCpt < m_numCpt; iCpt++, cpt_info++) {

        // Filter counterpart if it falls outside the declination range. If
        // the declination is larger than 9000 than the counterpart has no
        // location information and should be skipped ...
        if (cpt_info->dec < cpt_dec_min || cpt_info->dec > cpt_dec_max) {
          if (cpt_info->dec > 9000.0) {
            numNoPos++;
            continue;
          }
          else {
            numDec++;
            continue;
          }
        }

        // Filter source if it falls outside the Right Ascension range. The
        // first case handles no R.A. wrap around ...
        if (cpt_ra_min < cpt_ra_max) {
          if (cpt_info->ra < cpt_ra_min || cpt_info->ra > cpt_ra_max) {
            numRA++;
            continue;
          }
        }
        // ... and this one R.A wrap around
        else {
          if (cpt_info->ra < cpt_ra_min && cpt_info->ra > cpt_ra_max) {
            numRA++;
            continue;
          }
        }
        
        // If we are still alive then we keep the counterpart as candidat
        cpt_ptr->id          = "NULL";
        cpt_ptr->pos_eq_ra   = 0.0;
        cpt_ptr->pos_eq_dec  = 0.0;
        cpt_ptr->pos_err_maj = 0.0;
        cpt_ptr->pos_err_min = 0.0;
        cpt_ptr->pos_err_ang = 0.0;
        cpt_ptr->prob        = 0.0;
        cpt_ptr->index       = iCpt;
        cpt_ptr->angsep      = 0.0;
        cpt_ptr->prob_angsep = 0.0;
        cpt_ptr++;
        m_numCC++;
        
      } // endfor: looped over all counterpart candidates

      // Optionally dump counterpart filter statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Filter step candidates ..........: %d", m_numCC);
        if (par->logVerbose()) {
          Log(Log_2, "    Outside declination range .....: %d [%.3f - %.3f]", 
              numDec, cpt_dec_min, cpt_dec_max);
          Log(Log_2, "    Outside Right Ascension range .: %d [%.3f - %.3f[", 
              numRA, cpt_ra_min, cpt_ra_max);
        }
        if (numNoPos > 0)
          Log(Warning_2, "    No positions ..................: %d", numNoPos);
      }
      #if CATALOGUE_TIMING
      t_elapse_loop += (float)(clock() - t_start_loop) / (float)CLOCKS_PER_SEC;
      #endif

    } while (0); // End of main do-loop

    // Timing measurements
    #if CATALOGUE_TIMING
    t_elapse += (float)(clock() - t_start_tot) / (float)CLOCKS_PER_SEC;
    Log(Log_0, "  Filter step timing ..............:");
    Log(Log_0, "    Total CPU sec used ............: %.5f", t_elapse);
    Log(Log_0, "    CPU sec spent in loop .........: %.5f", t_elapse_loop);
    #endif

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_counterparts (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                          Catalogue::get_probability                        */
/* -------------------------------------------------------------------------- */
/* Private method: calculate the counterpart probability                      */
/* -------------------------------------------------------------------------- */
/*                  Refine step of counterpart identification                 */
/*                                                                            */
/* Calculates the counterpart probability, sorts all counterpart candidates   */
/* by decreasing probability and eliminates all candidtates with a too low    */
/* probability.                                                               */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_probability(Parameters *par, long iSrc, Status status) {

    // Declare local variables
    long iCC;
    long numUseCC;
    char cid[OUTCAT_MAX_STRING_LEN];

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_probability (%d candidates)", 
          m_numCC);

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;
        
      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Determine counterpart probabilities based on angular separation
      status = get_probability_angsep(par, iSrc, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine counterpart probabilities"
              " based on angular separation.", (Status)status);
        continue;
      }
    
      // Assign the counterpart probabilities
      // DUMMY: we only use the probability based on angular separation
      for (iCC = 0; iCC < m_numCC; iCC++)
        m_cc[iCC].prob = m_cc[iCC].prob_angsep;              

      // Sort counterpart candidates by decreasing probability
      status = cc_sort(par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to sort counterpart candidates.", 
              (Status)status);
        continue;
      }

      // Determine the number of counterpart candidates above the probability 
      // threshold
      numUseCC = 0;
      for (iCC = 0; iCC < m_numCC; iCC++) {
        if (m_cc[iCC].prob >= par->m_probThres)
          numUseCC++;
        else
          break;
      }

      // Apply the maximum number of counterpart threshold
      if (numUseCC > par->m_maxNumCtp)
        numUseCC = par->m_maxNumCtp;

      // Eliminate counterpart candidates below threshold. Fall through if no 
      // counterparts are left
      m_numCC = numUseCC;
      if (m_numCC < 1) {
        if (par->logExplicit())
          Log(Log_2, "  Refine step candidates ..........: no");
        continue;
      }

      // Set unique counterpart candidate identifier
      // DUMMY: Build string from indices
      for (iCC = 0; iCC < m_numCC; iCC++) {
        sprintf(cid, "CC_%5.5ld_%5.5ld", iSrc+1, iCC+1);
        m_cc[iCC].id = cid;
      }

      // Optionally dump counterpart refine statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Refine step candidates ..........: %d", m_numCC);
      }
    
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_probability (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                       Catalogue::get_probability_angsep                    */
/* -------------------------------------------------------------------------- */
/* Private method: calculate the counterpart probability based on angular     */
/*                 separation for all candidates                              */
/* -------------------------------------------------------------------------- */
/* TBW                                                                        */
/* -------------------------------------------------------------------------- */
/* The following members of the counterpart candidates are set:               */
/* pos_eq_ra    Right Ascension of counterpart candidate (deg)                */
/* pos_eq_dec   Declination of counterpart candidate (deg)                    */
/* pos_err_maj  Uncertainty ellipse major axis (deg)                          */
/* pos_err_min  Uncertainty ellipse minor axis (deg)                          */
/* pos_err_ang  Uncertainty ellipse positron angle (deg)                      */
/* angsep       Angular separation of counterpart candidate from source       */
/* prob_angsep  Probability of counterpart candidate based on position        */
/*----------------------------------------------------------------------------*/
Status Catalogue::get_probability_angsep(Parameters *par, long iSrc, 
                                         Status status) {

    // Declare local variables
    long   iCC;
    long   iCpt;
    double src_ra;
    double src_dec;
    double src_err_maj;
    double src_err_min;
    double src_err_ang;
    double src_dec_sin;
    double src_dec_cos;
    double cpt_ra;
    double cpt_dec;
    double cpt_err_maj;
    double cpt_err_min;
    double cpt_err_ang;
    double cpt_dec_sin;
    double cpt_dec_cos;
    double arg;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::get_probability_angsep"
          " (%d candidates)", m_numCC);

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;
        
      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Get position for source. Fall through if information is not available
      if ((m_src.ra_deg(iSrc,  &src_ra)  != IS_OK) ||
          (m_src.dec_deg(iSrc, &src_dec) != IS_OK))
        continue;

      // Get error ellipse for source. Fall through if information is not 
      // available
      src_err_maj = 0.0;
      src_err_min = 0.0;
      src_err_ang = 0.0;
      if ((m_src.posError_deg(iSrc, &src_err_maj) != IS_OK) ||
          (m_src.posError_deg(iSrc, &src_err_min) != IS_OK)) {
        if (par->m_srcPosError > 0.0) {
          src_err_maj = par->m_srcPosError;
          src_err_min = par->m_srcPosError;
        }
        else
          continue;
      }

      // Calculate sin and cos of source declination
      src_dec_sin = sin(src_dec * deg2rad);
      src_dec_cos = cos(src_dec * deg2rad);
    
      // Loop over counterpart candidates
      for (iCC = 0; iCC < m_numCC; iCC++) {

        // Get index of candidate in counterpart catalogue
        iCpt = m_cc[iCC].index;

        // Get position for counterpart candidate. Fall through if position is 
        // not available
        if ((m_cpt.ra_deg(iCpt, &cpt_ra)   != IS_OK) ||
            (m_cpt.dec_deg(iCpt, &cpt_dec) != IS_OK))
          continue;

        // Get error ellipse for counterpart candidate. Assume no error if error
        // ellipse is not available
        cpt_err_ang = 0.0;
        if ((m_cpt.posError_deg(iCpt, &cpt_err_maj) != IS_OK) ||
            (m_cpt.posError_deg(iCpt, &cpt_err_min) != IS_OK)) {
          cpt_err_maj = par->m_cptPosError;
          cpt_err_min = par->m_cptPosError;	  
          cpt_err_ang = 0.0;
        }

        // Calculate angular separation between source and counterpart in
        // degrees. Make sure that the separation is always comprised between 
        // [0,180] (out of range arguments lead to a floating exception)
        cpt_dec_sin = sin(cpt_dec * deg2rad);
        cpt_dec_cos = cos(cpt_dec * deg2rad);
        arg         = src_dec_sin * cpt_dec_sin +
                      src_dec_cos * cpt_dec_cos * cos((src_ra - cpt_ra)*deg2rad);
        if (arg <= -1.0)
          m_cc[iCC].angsep = 180.0;
        else if (arg >= 1.0)
          m_cc[iCC].angsep = 0.0;
        else
          m_cc[iCC].angsep = acos(arg) * rad2deg;

        // Calculate counterpart probability from angular separation
        // DUMMY: we use an exponential law for the probability
        m_cc[iCC].prob_angsep = 
          exp(-m_cc[iCC].angsep / sqrt(cpt_err_maj*cpt_err_maj + 
                                       src_err_maj*src_err_maj));

        // Assign position and error ellipse
        // DUMMY: we assign the position and error ellipse of the partner
        //        that has the smaller positional uncertainty
        if (cpt_err_maj < src_err_maj) {
          m_cc[iCC].pos_eq_ra   = cpt_ra;
          m_cc[iCC].pos_eq_dec  = cpt_dec;
          m_cc[iCC].pos_err_maj = cpt_err_maj;
          m_cc[iCC].pos_err_min = cpt_err_min;
          m_cc[iCC].pos_err_ang = cpt_err_ang;
        }
        else {
          m_cc[iCC].pos_eq_ra   = src_ra;
          m_cc[iCC].pos_eq_dec  = src_dec;
          m_cc[iCC].pos_err_maj = src_err_maj;
          m_cc[iCC].pos_err_min = src_err_min;
          m_cc[iCC].pos_err_ang = src_err_ang;
        }
              
      } // endfor: looped over counterpart candidates

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_probability_angsep (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                      Catalogue::create_output_catalogue                    */
/* -------------------------------------------------------------------------- */
/* Private method: create an empty FITS output catalogue                      */
/*----------------------------------------------------------------------------*/
Status Catalogue::create_output_catalogue(Parameters *par, Status status) {

    // Declare local variables
    int                                    fstatus;
    int                                    col;
    int                                    num_col;
    long                                   iQty;
    long                                   numQty;
    std::string                            form;
    std::vector<std::string>               qtyNames;
    std::vector<std::string>               qtyUnits;
    std::vector<std::string>               qtyUCDs;
    std::vector<catalogAccess::Quantity>   qtyDesc;
    char                                   keyname[20];
    char                                   comment[80];
    char                                 **ttype;
    char                                 **tform;
    char                                 **tunit;
    char                                 **tbucd;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::create_output_catalogue");

    // Initialise temporary memory pointers
    ttype = NULL;
    tform = NULL;
    tunit = NULL;
    tbucd = NULL;

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // If clobber=1 we make sure that the output catalogue does not yet exist
      if (par->m_clobber)
        remove(par->m_outCatName.c_str());
        
      // Create empty FITS file.
      fstatus = fits_create_file(&m_outFile, par->m_outCatName.c_str(), 
                                 &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create output catalogue '%s'.", 
              fstatus, par->m_outCatName.c_str());
        continue;
      }

      // Initialise column index to the OUTCAT_NUM_GENERIC generic columns
      num_col       = OUTCAT_NUM_GENERIC;
      m_num_src_Qty = 0;
      m_num_cpt_Qty = 0;

      // Add source catalogue columns
      numQty = m_src.getQuantityNames(&qtyNames);
      numQty = m_src.getQuantityUnits(&qtyUnits);
      numQty = m_src.getQuantityUCDs(&qtyUCDs);
      numQty = m_src.getQuantityDescription(&qtyDesc);
      for (iQty = 0; iQty < numQty; iQty++) {
        if ((par->m_srcCatQty.find("*", 0)            != std::string::npos) ||
            (par->m_srcCatQty.find(qtyNames[iQty], 0) != std::string::npos)) {

          // Increment number of source quantities and FITS file columns
          m_num_src_Qty++;
          num_col++;

          // Set quantity FITS column format
          set_fits_col_format(&qtyDesc[iQty], &form);
        
          // Add quantity information to source quantity string
          m_src_Qty_ttype.push_back(qtyNames[iQty]);
          m_src_Qty_tform.push_back(form);
          m_src_Qty_tunit.push_back(qtyUnits[iQty]);
          m_src_Qty_tbucd.push_back(qtyUCDs[iQty]);
        }
      }

      // Add counterpart catalogue columns
      numQty = m_cpt.getQuantityNames(&qtyNames);
      numQty = m_cpt.getQuantityUnits(&qtyUnits);
      numQty = m_cpt.getQuantityUCDs(&qtyUCDs);
      numQty = m_cpt.getQuantityDescription(&qtyDesc);
      for (iQty = 0; iQty < numQty; iQty++) {
        if ((par->m_cptCatQty.find("*", 0)            != std::string::npos) ||
            (par->m_cptCatQty.find(qtyNames[iQty], 0) != std::string::npos)) {

          // Increment number of source quantities and FITS file columns
          m_num_cpt_Qty++;
          num_col++;

          // Set quantity FITS column format
          set_fits_col_format(&qtyDesc[iQty], &form);
        
          // Add quantity information to source quantity string
          m_cpt_Qty_ttype.push_back(qtyNames[iQty]);
          m_cpt_Qty_tform.push_back(form);
          m_cpt_Qty_tunit.push_back(qtyUnits[iQty]);
          m_cpt_Qty_tbucd.push_back(qtyUCDs[iQty]);
        }
      }

      // Allocate temporary memory to hold column information
      ttype = new (char*)[num_col];
      tform = new (char*)[num_col];
      tunit = new (char*)[num_col];
      tbucd = new (char*)[num_col];
      if (ttype == NULL ||
          tform == NULL ||
          tunit == NULL ||
          tbucd == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }
      for (col = 0; col < num_col; col++) {
        ttype[col] = new char[OUTCAT_MAX_KEY_LEN];
        tform[col] = new char[OUTCAT_MAX_KEY_LEN];
        tunit[col] = new char[OUTCAT_MAX_KEY_LEN];
        tbucd[col] = new char[OUTCAT_MAX_KEY_LEN];
        if (ttype[col] == NULL ||
            tform[col] == NULL ||
            tunit[col] == NULL ||
            tbucd[col] == NULL) {
          status = STATUS_MEM_ALLOC;
          if (par->logTerse())
            Log(Error_2, "%d : Memory allocation failure.", (Status)status);
          break;
        }
        sprintf(ttype[col], "%s", "");
        sprintf(tform[col], "%s", "");
        sprintf(tunit[col], "%s", "");
        sprintf(tbucd[col], "%s", "");
      }
      if (status != STATUS_OK)
        continue;

      // Add generic columns
      col = OUTCAT_COL_ID_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_ID_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_ID_FORM);
      sprintf(tbucd[col], "%s", OUTCAT_COL_ID_UCD);
      col = OUTCAT_COL_RA_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_RA_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_RA_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_RA_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_RA_UCD);
      col = OUTCAT_COL_DEC_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_DEC_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_DEC_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_DEC_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_DEC_UCD);
      col = OUTCAT_COL_MAJERR_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_MAJERR_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_MAJERR_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_MAJERR_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_MAJERR_UCD);
      col = OUTCAT_COL_MINERR_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_MINERR_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_MINERR_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_MINERR_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_MINERR_UCD);
      col = OUTCAT_COL_POSANGLE_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_POSANGLE_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_POSANGLE_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_POSANGLE_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_POSANGLE_UCD);
      col = OUTCAT_COL_PROB_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_PROB_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_PROB_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_PROB_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_PROB_UCD);

      // Initialise column counter for additional columns
      col = OUTCAT_NUM_GENERIC;

      // Add source catalogue quantities
      for (iQty = 0; iQty < m_num_src_Qty; iQty++) {
        m_src_Qty_colnum.push_back(col+1);
        sprintf(ttype[col], "SRC_%s", m_src_Qty_ttype[iQty].c_str());
        sprintf(tform[col], m_src_Qty_tform[iQty].c_str());
        sprintf(tunit[col], m_src_Qty_tunit[iQty].c_str());
        sprintf(tbucd[col], m_src_Qty_tbucd[iQty].c_str());
        col++;
      }

      // Add counterpart catalogue quantities
      for (iQty = 0; iQty < m_num_cpt_Qty; iQty++) {
        m_cpt_Qty_colnum.push_back(col+1);
        sprintf(ttype[col], "CPT_%s", m_cpt_Qty_ttype[iQty].c_str());
        sprintf(tform[col], m_cpt_Qty_tform[iQty].c_str());
        sprintf(tunit[col], m_cpt_Qty_tunit[iQty].c_str());
        sprintf(tbucd[col], m_cpt_Qty_tbucd[iQty].c_str());
        col++;
      }

      // Create binary table
      fstatus = fits_create_tbl(m_outFile, BINARY_TBL, 0, num_col, 
                                ttype, tform, tunit,
                                OUTCAT_EXT_NAME, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create empty catalogue '%s'.", 
              fstatus, par->m_outCatName.c_str());
        continue;
      }
      
      // Write UCD keywords
      for (col = 0; col < num_col; col++) {
        sprintf(keyname, "TBUCD%d", col+1);
        sprintf(comment, "UCD for field %3d", col+1);
        fstatus = fits_write_key(m_outFile, TSTRING, keyname, tbucd[col], 
                                 comment, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to write keyword '%s'.", 
                fstatus, keyname);
          break;
        }
      }
      if (fstatus != 0)
        continue;
         
    } while (0); // End of main do-loop

    // Free temporary memory
    if (ttype != NULL) {
      for (col = 0; col < num_col; col++) {
        if (ttype[col] != NULL) delete [] ttype[col];
      }
      delete [] ttype;
    }
    if (tform != NULL) {
      for (col = 0; col < num_col; col++) {
        if (tform[col] != NULL) delete [] tform[col];
      }
      delete [] tform;
    }
    if (tunit != NULL) {
      for (col = 0; col < num_col; col++) {
        if (tunit[col] != NULL) delete [] tunit[col];
      }
      delete [] tunit;
    }
    if (tbucd != NULL) {
      for (col = 0; col < num_col; col++) {
        if (tbucd[col] != NULL) delete [] tbucd[col];
      }
      delete [] tbucd;
    }

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::create_output_catalogue (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                     Catalogue::add_counterpart_candidates                  */
/* -------------------------------------------------------------------------- */
/* Private method: add counterpart candidates to internal FITS file           */
/*----------------------------------------------------------------------------*/
Status Catalogue::add_counterpart_candidates(Parameters *par, long iSrc, 
                                             Status status) {

    // Declare local variables
    int           fstatus;
    int           colnum;
    long          nactrows;
    long          firstrow;
    long          row;
    long          frow;
    long          nrows;
    long          iQty;
    long          iCpt;
    double        NValue;
    std::string   SValue;
    std::string   form;
    std::string   name;
    double       *dptr;
    char        **cptr;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::add_counterpart_candidates");

    // Initialise temporary memory pointers
    dptr = NULL;
    cptr = NULL;    

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;
        
      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Determine number of rows in actual table
      fstatus = fits_get_num_rows(m_outFile, &nactrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in output"
              " catalogue.", fstatus);
        continue;
      }

      // Define the rows that should be inserted
      firstrow = (long)nactrows;
      frow     = firstrow + 1;   
      nrows    = (long)m_numCC;

      // Insert rows for the new counterpart candidates
      fstatus = fits_insert_rows(m_outFile, firstrow, nrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to add %d rows to output catalogue.", 
              fstatus, nrows);
        continue;
      }

      // Allocate memory to hold quantities
      dptr = new double[nrows];
      cptr = new (char*)[nrows];
      if (dptr == NULL ||
          cptr == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }
      for (row = 0; row < nrows; row++) {
        cptr[row] = new char[OUTCAT_MAX_STRING_LEN];
        if (cptr[row] == NULL) {
          status = STATUS_MEM_ALLOC;
          if (par->logTerse())
            Log(Error_2, "%d : Memory allocation failure.", (Status)status);
          break;
        }
      }
      if (status != STATUS_OK)
        continue;


      // Add unique counterpart identifier
      for (row = 0; row < nrows; row++)
        strcpy(cptr[row], m_cc[row].id.c_str());
      fstatus = fits_write_col_str(m_outFile, OUTCAT_COL_ID_COLNUM, 
                                   frow, 1, nrows, cptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart identifier to output"
              " catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Right Ascention
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_eq_ra;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_RA_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart Right Ascension to"
              " output catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Declination
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_eq_dec;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_DEC_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart Declination to"
              " output catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Major Axis
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_maj;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_MAJERR_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse major"
              " axis to output catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Minor Axis
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_min;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_MINERR_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse minor"
              " axis to output catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Position Angle
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_ang;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_POSANGLE_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse position"
              " angle to output catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Probability
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].prob;
      fstatus = fits_write_col(m_outFile, TDOUBLE, OUTCAT_COL_PROB_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart probability to output"
              " catalogue.", fstatus);
        continue;
      }

      // Add source catalogue columns
      for (iQty = 0; iQty < m_num_src_Qty; iQty++) {

        // Get column information
        colnum = m_src_Qty_colnum[iQty];
        form   = m_src_Qty_tform[iQty];
        name   = m_src_Qty_ttype[iQty];

        // Add numerical quantities
        if (form.find("E", 0) != std::string::npos) {
          m_src.getNValue(name, iSrc, &NValue);
          for (row = 0; row < nrows; row++)
            dptr[row] = NValue;
          fstatus = fits_write_col(m_outFile, TDOUBLE, colnum, frow, 1, nrows, 
                                   dptr, &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write source catalogue data <%s>"
                  " (column %d) to output catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

        // Add string quantities
        if (form.find("A", 0) != std::string::npos) {
          m_src.getSValue(name, iSrc, &SValue);
          for (row = 0; row < nrows; row++)
            strcpy(cptr[row], SValue.c_str());
          fstatus = fits_write_col_str(m_outFile, colnum, frow, 1, nrows, cptr, 
                                       &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write source catalogue data <%s>"
                  " (column %d) to output catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

      }
      if (fstatus != 0)
        continue;

      // Add counterpart catalogue columns
      for (iQty = 0; iQty < m_num_cpt_Qty; iQty++) {

        // Get column information
        colnum = m_cpt_Qty_colnum[iQty];
        form   = m_cpt_Qty_tform[iQty];
        name   = m_cpt_Qty_ttype[iQty];

        // Add numerical quantities
        if (form.find("E", 0) != std::string::npos) {
          for (row = 0; row < nrows; row++) {
            iCpt = m_cc[row].index;
            m_cpt.getNValue(name, iCpt, &NValue);
            dptr[row] = NValue;
          }
          fstatus = fits_write_col(m_outFile, TDOUBLE, colnum, frow, 1, nrows, 
                                   dptr, &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write counterpart catalogue data <%s>"
                  " (column %d) to output catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

        // Add string quantities
        if (form.find("A", 0) != std::string::npos) {
          for (row = 0; row < nrows; row++) {
            iCpt = m_cc[row].index;
            m_cpt.getSValue(name, iCpt, &SValue);
            strcpy(cptr[row], SValue.c_str());
          }
          fstatus = fits_write_col_str(m_outFile, colnum, frow, 1, nrows, cptr, 
                                       &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write counterpart catalogue data"
                  " <%s> (column %d) to output catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

      }
      if (fstatus != 0)
        continue;
   
    } while (0); // End of main do-loop

    // Delete temporary memory
    if (dptr != NULL) delete [] dptr;
    if (cptr != NULL) {
      for (row = 0; row < nrows; row++) {
        if (cptr[row] != NULL) delete [] cptr[row];
      }
      delete [] cptr;
    }

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::add_counterpart_candidates (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                Catalogue::eval_output_catalogue_quantities                 */
/* -------------------------------------------------------------------------- */
/* Private method: evaluate output catalogue quantities                       */
/*----------------------------------------------------------------------------*/
Status Catalogue::eval_output_catalogue_quantities(Parameters *par, 
                                                   Status status) {

    // Declare local variables
    int                    fstatus;
    int                    datatype;
    int                    naxis;
    long                   nelements;
    unsigned long          iQty;
    std::string::size_type numQty;
    std::string            tform;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::eval_output_catalogue_quantities");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of new output catalogue quantities. Fall through if
      // there are no new output catalogue quantities
      numQty = par->m_outCatQtyName.size();
      if (numQty < 1)
        continue;

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Add new output catalogue quantities:");
        Log(Log_2, "====================================");
      }

      // Add all new output catalogue quantities
      for (iQty = 0; iQty < numQty; iQty++) {

        // Test expression to determine the format of the new column
        fstatus = fits_test_expr(m_outFile,
                                 (char*)par->m_outCatQtyFormula[iQty].c_str(),
                                 0, &datatype, &nelements, &naxis, NULL,
                                 &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Warning_2, " Unable to evaluate expression <%s='%s'> for"
                " creating new output catalogue quantity (status=%d).",
                par->m_outCatQtyName[iQty].c_str(), 
                par->m_outCatQtyFormula[iQty].c_str(),
                fstatus);
          fstatus = 0;
          continue;
        }

        // Set column format
        if (nelements < 1) 
          nelements = 1;
        fits_tform_binary(datatype, nelements, 0, &tform);

        // Create new FITS column
        fstatus = fits_calculator(m_outFile, 
                                  (char*)par->m_outCatQtyFormula[iQty].c_str(),
                                  m_outFile,
                                  (char*)par->m_outCatQtyName[iQty].c_str(),
                                  (char*)tform.c_str(), 
                                  &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate quantity <%s='%s'> in output"
                " catalogue.", 
                fstatus, 
                par->m_outCatQtyName[iQty].c_str(), 
                par->m_outCatQtyFormula[iQty].c_str());
          break;
        }

        // Dump new output catalogue quantities information
        if (par->logNormal()) {
          Log(Log_2, " New quantity .....................: %s = %s"
              " (format='%s')",
              par->m_outCatQtyName[iQty].c_str(),
              par->m_outCatQtyFormula[iQty].c_str(),
              tform.c_str());
        }

      }
      if (fstatus != 0)
        continue;

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::eval_output_catalogue_quantities"
          " (status=%d)", status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                      Catalogue::select_output_catalogue                    */
/* -------------------------------------------------------------------------- */
/* Private method: select output catalogue entries                            */
/*----------------------------------------------------------------------------*/
Status Catalogue::select_output_catalogue(Parameters *par, Status status) {

    // Declare local variables
    int                    fstatus;
    long                   numBefore;
    long                   numAfter;
    std::string::size_type iSel;
    std::string::size_type numSel;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::select_output_catalogue");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of output catalogue selection strings. Fall through
      // if there are no such strings
      numSel = par->m_select.size();
      if (numSel < 1)
        continue;

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Select output catalogue counterparts:");
        Log(Log_2, "=====================================");
      }

      // Select output catalogue entries
      for (iSel = 0; iSel < numSel; iSel++) {

        // Determine number of rows in table before selection
        fstatus = fits_get_num_rows(m_outFile, &numBefore, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of rows in output"
                " catalogue.", fstatus);
          break;
        }

        // Perform selection
        fstatus = fits_select_rows(m_outFile, m_outFile,
                                   (char*)par->m_select[iSel].c_str(),
                                   &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Warning_2, " Unable to perform selection <%s> on the output"
                " catalogue (status=%d).",
                par->m_select[iSel].c_str(), 
                fstatus);
          fstatus = 0;
          continue;
        }

        // Determine number of rows in table after selection
        fstatus = fits_get_num_rows(m_outFile, &numAfter, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of rows in output"
                " catalogue.", fstatus);
          break;
        }

        // Dump selection information
        if (par->logNormal()) {
          Log(Log_2, " Selection ........................: %s", 
              par->m_select[iSel].c_str());
          Log(Log_2, "   Number of deleted counterparts .: %d (%d => %d)", 
              numBefore-numAfter, numBefore, numAfter);
        }

      }
      if (fstatus != 0)
        continue;

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::select_output_catalogue (status=%d)", 
          status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                             Catalogue::cc_sort                             */
/* -------------------------------------------------------------------------- */
/* Private method: sort counterpart candidates by increasing probability      */
/*----------------------------------------------------------------------------*/
Status Catalogue::cc_sort(Parameters *par, Status status) {

    // Declare local variables
    long      iCC;
    long      jCC;
    long      imax;
    double    max;
    CCElement swap;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cc_sort");

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;
        
      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;
    
      // Sort counterpart candidats (dirty brute force method, to be replaced
      // by more efficient method if needed ...)
      for (iCC = 0; iCC < m_numCC; iCC++) {
        max  = 0.0;
        imax = iCC;
        for (jCC = iCC; jCC < m_numCC; jCC++) {
          if (m_cc[jCC].prob > max) {
            imax = jCC;
            max  = m_cc[jCC].prob;
          }
        }
        if (iCC != imax) {
          swap       = m_cc[iCC];
          m_cc[iCC]  = m_cc[imax];
          m_cc[imax] = swap;
        } 
      }
    
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cc_sort (status=%d)", status);
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                          Catalogue::dump_descriptor                        */
/* -------------------------------------------------------------------------- */
/* Private method: dump catalogue descriptor                                  */
/*----------------------------------------------------------------------------*/
Status Catalogue::dump_descriptor(catalogAccess::Catalog *cat, Status status) {

    // Declare local variables
    long                                               iQty;
    long                                               numQty;
    long                                               numRows;
    long                                               len;
    long                                               maxLenNames;
    long                                               maxLenUnits;
    long                                               maxLenUCDs;
    std::string                                        qtyType;
    std::vector<std::string>                           titles;
    std::vector<std::string>                           qtyNames;
    std::vector<std::string>                           qtyUnits;
    std::vector<std::string>                           qtyUCDs;
    std::vector<catalogAccess::Quantity>               qtyDesc;
    std::vector<catalogAccess::Quantity::QuantityType> qtyTypes;

    // Single loop for common exit point
    do {
      
      // Extract information from catalogue
      cat->getCatalogTitles(&titles);
      numQty = cat->getQuantityNames(&qtyNames);
      numQty = cat->getQuantityUnits(&qtyUnits);
      numQty = cat->getQuantityUCDs(&qtyUCDs);
      numQty = cat->getQuantityDescription(&qtyDesc);
      numQty = cat->getQuantityTypes(&qtyTypes);
      cat->getNumRows(&numRows);
      
      // Get string lengths
      maxLenNames = 0;
      maxLenUnits = 0;
      maxLenUCDs  = 0;
      for (iQty = 0; iQty < numQty; iQty++) {
        if ((len = strlen(qtyNames[iQty].c_str())) > maxLenNames)
          maxLenNames = len;
        if ((len = strlen(qtyUnits[iQty].c_str())) > maxLenUnits)
          maxLenUnits = len;
        if ((len = strlen(qtyUCDs[iQty].c_str())) > maxLenUCDs)
          maxLenUCDs = len;
      }

      // Dump catalogue information
      Log(Log_2, " Catalogue ........................: %s", titles[0].c_str());
      Log(Log_2, " Number of entries (rows) .........: %d", numRows);
      Log(Log_2, " Number of quantities (columns) ...: %d", numQty);

      // Dump information about catalogue quantitites
      for (iQty = 0; iQty < numQty; iQty++) {
      
        // Set quantity type
        switch (qtyTypes[iQty]) {
        case 0:
          qtyType = "vector (" + qtyDesc[iQty].m_format + ")";
          break;
        case 1:
          qtyType = "numerical (" + qtyDesc[iQty].m_format + ")";
          break;
        case 2:
          qtyType = "string (" + qtyDesc[iQty].m_format + ")";
          break;
        default:
          qtyType = "unknown";
          break;
        }
        
        // Dump quantity
        Log(Log_2, "  Quantity %3d ....................: %*s %*s %*s (%s)", 
                 iQty+1, 
                 maxLenNames, qtyNames[iQty].c_str(),
                 maxLenUnits, qtyUnits[iQty].c_str(),
                 maxLenUCDs, qtyUCDs[iQty].c_str(),
                 qtyType.c_str());

      } // endfor: looped over quantities
    
    } while (0); // End of main do-loop
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                    Catalogue::dump_counterpart_candidates                  */
/* -------------------------------------------------------------------------- */
/* Private method: dump counterpart candidates that are actually in buffer    */
/*----------------------------------------------------------------------------*/
Status Catalogue::dump_counterpart_candidates(Parameters *par, Status status) {

    // Declare local variables
    long        iCC;
    long        iCpt;
    double      cpt_ra;
    double      cpt_dec;
    double      cpt_error;
    std::string cpt_name;
    std::string obj_name;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::dump_counterpart_candidates");

    // Single loop for common exit point
    do {
    
      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;
        
      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Determine the name of the generic quantity "object name"
      obj_name = m_cpt.getNameObjName();
    
      // Loop over counterpart candidates
      for (iCC = 0; iCC < m_numCC; iCC++) {
      
        // Get index of candidate in counterpart catalogue
        iCpt = m_cc[iCC].index;

        // Extract information for counterpart candidate
        m_cpt.ra_deg(iCpt,  &cpt_ra);
        m_cpt.dec_deg(iCpt, &cpt_dec);
        m_cpt.posError_deg(iCpt, &cpt_error);
        m_cpt.getSValue(obj_name, iCpt, &cpt_name);

        // Dump counterpart candidate information
        if (!par->logVerbose()) {
          Log(Log_2, "  Counterpart candidate %5d .....: %s Prob=%7.3f %%",
              iCC+1, cpt_name.c_str(), m_cc[iCC].prob*100.0);
        }
        else {
          Log(Log_2, "  Counterpart candidate %5d .....: %s Prob=%7.3f %%"
              " (RA,DEC)=(%8.3f,%8.3f) +/- %8.3f  Sep=%8.3f deg",
              iCC+1, cpt_name.c_str(), m_cc[iCC].prob*100.0,
              cpt_ra, cpt_dec, cpt_error, m_cc[iCC].angsep);
        }

      } // endfor: looped over counterpart candidats
    
    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::dump_counterpart_candidates"
          " (status=%d)", status);
    
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
        Log(Log_2, "Build counterpart candidate catalogue:");
        Log(Log_2, "======================================");
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

      // Create output catalogue
      status = create_output_catalogue(par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create output catalogue.",
              (Status)status);
        continue;
      }
      
      // Load source catalogue
      status = get_input_catalogue(par, par->m_srcCatName, &m_src, status);
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
      
      // Determine the number of sources in source catalogue. Stop of the
      // catalogue is empty
      m_src.getNumRows(&m_numSrc);
      if (m_numSrc < 1) {
        status = STATUS_CAT_EMPTY;
        if (par->logTerse())
          Log(Error_2, "%d : Source catalogue is empty. Stop", (Status)status);
        continue;
      }
      else {
        if (par->logVerbose())
          Log(Log_2, " Source catalogue contains %d sources.", m_numSrc);
      }
      
      // Loop over all sources
      for (iSrc = 0; iSrc < m_numSrc; iSrc++) {
      
        // Get counterpart candidates for the source
        status = get_counterpart_candidates(par, iSrc, status);
        if (status != STATUS_OK)
          break;
      
      } // endfor: looped over all sources
      if (status != STATUS_OK)
        continue;

      // Evaluate output catalogue quantities
      status = eval_output_catalogue_quantities(par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to evaluate output catalogue quantities.",
              (Status)status);
        continue;
      }

      // Select output catalogue counterparts
      status = select_output_catalogue(par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to select output catalogue counterparts.",
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


/*----------------------------------------------------------------------------*/
/*                               Catalogue::save                              */
/* -------------------------------------------------------------------------- */
/* Save counterpart catalogue.                                                */
/*----------------------------------------------------------------------------*/
Status Catalogue::save(Parameters *par, Status status) {

    // Declare local variables
    int fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::save");
        
    // Initialise FITSIO status
    fstatus = (int)status;
        
    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Save counterpart candidate catalogue:");
        Log(Log_2, "=====================================");
      }

      // Close FITS file
      fstatus = fits_close_file(m_outFile, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to close output catalogue.", fstatus);
        continue;
      }
    
    } while (0); // End of main do-loop
    
    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::save (status=%d)", status);
    
    // Return status
    return status;

}

/* Namespace ends ___________________________________________________________ */
}
