/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.15  2008/03/20 21:56:26  jurgen
implement local counterpart density

Revision 1.14  2008/02/23 10:36:57  jurgen
remove redundant catalogAccess header inclusion

Revision 1.13  2007/11/08 11:18:31  jurgen
Correctly handle missing name column

Revision 1.12  2007/10/11 13:20:54  jurgen
Correctly remove FITS special function columns

Revision 1.11  2007/10/09 16:46:23  jurgen
Write counterpart catalogue reference (row) to output catalogue

Revision 1.10  2007/10/09 08:17:40  jurgen
Correctly interpret positional errors and correctly evaluate PROB_POS
as likelihood

Revision 1.9  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

Revision 1.8  2007/10/03 09:06:08  jurgen
Add chance coincidence probability PROB_CHANCE

Revision 1.7  2007/10/02 22:01:16  jurgen
Change parameter name maxNumCtp to maxNumCpt

Revision 1.6  2007/09/21 20:27:14  jurgen
Correct cfits_collect bug (unstable row selection)

Revision 1.5  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

Revision 1.4  2007/09/21 12:49:10  jurgen
Enhance log-file output and chatter level

Revision 1.3  2007/09/20 16:28:21  jurgen
Enhance catalogue interface for column recognition

Revision 1.2  2006/02/07 16:05:04  jurgen
Use ObjectInfo structure to hold catalogue object information

Revision 1.1  2006/02/03 12:11:37  jurgen
New file that contains routines that have formerly been found in
Catalogue.cxx. The routines have also been renamed and preceeded
by "cid_". The routines handle source identification at the low
level.

------------------------------------------------------------------------------*/
/**
 * @file Catalogue_id.cxx
 * @brief Implements source identification methods of Catalogue class.
 * @author J. Knodlseder
 */

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"
#include "Catalogue.h"
#include "Log.h"


/* Definitions ______________________________________________________________ */
#define CATALOGUE_TIMING   0                     // Enables timing measurements


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {
using namespace catalogAccess;


/* Type defintions __________________________________________________________ */


/* Private Prototypes _______________________________________________________ */


/**************************************************************************//**
 * @brief Get counterpart candidates for a source
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 *
 * This is the main counterpart association driving routine. Counterpart
 * association is done in a two step process:
 * (1) a filter step, and
 * (2) a refine step.
 *
 * The filter step gathers all counterparts within a rectangular bounding box
 * (in Right Ascension and Declination) that encloses a circular region around
 * the source position. The radius of the circular region is given by
 * Catalogue::m_filter_maxsep.
 *
 * The refine step calculates the association probabilities for all filtered
 * sources.
 *
 * Finally, all candidates are added to the output catalogue.
 ******************************************************************************/
Status Catalogue::cid_get(Parameters *par, long iSrc, Status status) {

    // Declare local variables
    ObjectInfo *src;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_get");

    // Single loop for common exit point
    do {

      // Get pointer to source object
      src = &(m_src.object[iSrc]);

      // Dump source information (optionally)
      if (par->logNormal()) {
        if (src->pos_valid) {
          Log(Log_2, " Source %5d .....................: %20s"SRC_FORMAT,
              iSrc+1, src->name.c_str(),
              src->pos_eq_ra, src->pos_eq_dec,
              src->pos_err_maj, src->pos_err_min, src->pos_err_ang);
        }
        else {
          Log(Log_2, " Source %5d .....................: %20s"
              " No position information found.",
              iSrc+1, src->name.c_str());
        }
      }

      // Fall through if no position information has been found
      if (!src->pos_valid)
        continue;

      // Filter step: Get counterparts near the source position
      status = cid_filter(par, iSrc, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to perform filter step for source %d.",
              (Status)status, iSrc+1);
        continue;
      }

      // Refine step: Assign probability for each counterpart
      status = cid_refine(par, iSrc, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to perform refine step for source %d.",
              (Status)status, iSrc+1);
        continue;
      }

      // Add all counterpart candidates to output catalogue
      status = cfits_add(m_outFile, iSrc, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to add counterpart candidates for source"
              " %d to FITS output catalogue '%s'.", 
              (Status)status, iSrc+1, par->m_outCatName.c_str());
        continue;
      }

      // Optionally dump counterpart candidats
      if (par->logNormal()) {
        cid_dump(par, status);
        Log(Log_2, "");
      }

     } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_get (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Filter step of counterpart identification
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 *
 * @todo Load counterpart catalogue only partially if number of objects is large.
 *
 * The filter step gets all counterpart candidates from the catalogue for a
 * given source that are sufficiently close to the source.
 * This is done by defining a rectangular bounding box around the source
 * that extends from -filter_rad to +filter_rad in Declination
 * and from -filter_rad/cos(dec) to +filter_rad/cos(dec) in Right
 * Ascension.
 ******************************************************************************/
Status Catalogue::cid_filter(Parameters *par, long iSrc, Status status) {

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
    ObjectInfo *src;
    ObjectInfo *cpt;
    CCElement  *cpt_ptr;

    // Timing measurements
    #if CATALOGUE_TIMING
    clock_t t_start_loop;
    clock_t t_start_tot   = clock();
    float   t_elapse      = 0.0;
    float   t_elapse_loop = 0.0;
    #endif

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_filter");

    // Single loop for common exit point
    do {

      // Reset statistics
      m_numCC  = 0;
      numNoPos = 0;
      numRA    = 0;
      numDec   = 0;

      // Get pointer to source object
      src = &(m_src.object[iSrc]);

      // Calculate sin and cos of source latitude
      src_dec_sin = sin(src->pos_eq_dec * deg2rad);
      src_dec_cos = cos(src->pos_eq_dec * deg2rad);

      // Calculate the size of the bounding box
      double filter_rad = c_filter_maxsep;

      // Define bounding box around source position. The declination
      // range of the bounding box is constrained to [-90,90] deg, the
      // Right Ascension boundaries are put into the interval [0,360[ deg.
      cpt_dec_min = src->pos_eq_dec - filter_rad;
      cpt_dec_max = src->pos_eq_dec + filter_rad;
      if (cpt_dec_min < -90.0) cpt_dec_min = -90.0;
      if (cpt_dec_max >  90.0) cpt_dec_max =  90.0;
      if (src_dec_cos > 0.0) {
        filter_maxsep = filter_rad / src_dec_cos;
        if (filter_maxsep > 180.0)
          filter_maxsep = 180.0;
      }
      else
        filter_maxsep = 180.0;
      cpt_ra_min  = src->pos_eq_ra - filter_maxsep;
      cpt_ra_max  = src->pos_eq_ra + filter_maxsep;
      cpt_ra_min = cpt_ra_min - double(long(cpt_ra_min / 360.0) * 360.0);
      if (cpt_ra_min < 0.0) cpt_ra_min += 360.0;
      cpt_ra_max = cpt_ra_max - double(long(cpt_ra_max / 360.0) * 360.0);
      if (cpt_ra_max < 0.0) cpt_ra_max += 360.0;

      // If the counterpart catalogue is big then load only sources from a
      // region around the specified position ...
      if (m_cpt.numTotal > m_maxCptLoad) {
      }

      // ... otherwise load the entire counterpart catalogue
      else {

        // Load only if noy yet done ...
        if (!m_fCptLoaded) {

          // Load counterpart catalogue
          status = get_input_catalogue(par, &m_cpt, par->m_cptPosError, status);
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

          // Stop if the counterpart catalogue is empty
          if (m_cpt.numLoad < 1) {
            status = STATUS_CAT_EMPTY;
            if (par->logTerse())
              Log(Error_2, "%d : Counterpart catalogue is empty. Stop.", 
                  (Status)status);
            continue;
          }
          else {
            if (par->logVerbose())
              Log(Log_2, " Counterpart catalogue contains %d sources.", 
                  m_cpt.numLoad);
          }

          // Free memory if it has already been allocated
          if (m_cc != NULL) delete [] m_cc;
          m_cc = NULL;

          // Allocate memory for counterpart candidate search
          m_cc = new CCElement[m_cpt.numLoad];
          if (m_cc == NULL) {
            status = STATUS_MEM_ALLOC;
            if (par->logTerse())
              Log(Error_2, "%d : Memory allocation failure.", (Status)status);
            continue;
          }

          // Bookkeep catalogue loading
          m_fCptLoaded = 1;

        } // endif: catalogue was not yet loaded
      } // endelse: entire catalogue requested

      // Initialise counterpart pointers
      cpt     = m_cpt.object;
      cpt_ptr = m_cc;

      // Loop over counterparts
      #if CATALOGUE_TIMING
      t_start_loop = clock();
      #endif
      for (iCpt = 0; iCpt < m_cpt.numLoad; iCpt++, cpt++) {

        // Filter counterparts that have no positional information
        if (!cpt->pos_valid) {
          numNoPos++;
          continue;
        }

        // Filter counterpart if it falls outside the declination range.
        if (cpt->pos_eq_dec < cpt_dec_min || 
            cpt->pos_eq_dec > cpt_dec_max) {
          numDec++;
          continue;
        }

        // Filter source if it falls outside the Right Ascension range. The
        // first case handles no R.A. wrap around ...
        if (cpt_ra_min < cpt_ra_max) {
          if (cpt->pos_eq_ra < cpt_ra_min || cpt->pos_eq_ra > cpt_ra_max) {
            numRA++;
            continue;
          }
        }
        // ... and this one R.A wrap around
        else {
          if (cpt->pos_eq_ra < cpt_ra_min && cpt->pos_eq_ra > cpt_ra_max) {
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
        cpt_ptr->posang      = 0.0;
        cpt_ptr->filter_rad  = filter_rad;
        cpt_ptr->rho_rad_min = 0.0;
        cpt_ptr->rho_rad_max = filter_rad;
        cpt_ptr->rho_omega   = twopi *
                               (cos(cpt_ptr->rho_rad_min * deg2rad) -
                                cos(cpt_ptr->rho_rad_max * deg2rad)) *
                               rad2deg * rad2deg;
        cpt_ptr->rho         = 0.0;
        cpt_ptr->lambda      = 0.0;
        cpt_ptr->prob_pos    = 0.0;
        cpt_ptr->prob_chance = 0.0;
        cpt_ptr++;
        m_numCC++;

      } // endfor: looped over all counterpart candidates

      // Optionally dump counterpart filter statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Filter step candidates ..........: %5d", m_numCC);
        if (par->logVerbose()) {
          Log(Log_2, "    Filter bounding box radius ....: %7.3f deg",
              filter_rad);
          Log(Log_2, "    Outside declination range .....: %5d [%7.3f - %7.3f]",
              numDec, cpt_dec_min, cpt_dec_max);
          Log(Log_2, "    Outside Right Ascension range .: %5d [%7.3f - %7.3f[",
              numRA, cpt_ra_min, cpt_ra_max);
        }
        if (numNoPos > 0)
          Log(Warning_2, "    No positions ..................: %5d", numNoPos);
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
      Log(Log_0, " <== EXIT: Catalogue::cid_filter (status=%d)", 
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Refine step of counterpart identification
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 *
 * Calculates the counterpart probability, sorts all counterpart candidates
 * by decreasing probability and eliminates all candidtates with a too low
 * probability.
 ******************************************************************************/
Status Catalogue::cid_refine(Parameters *par, long iSrc, Status status) {

    // Declare local variables
    long                           iCC;
    long                           numUseCC;
    double                         prob;
    char                           cid[OUTCAT_MAX_STRING_LEN];
    std::vector<double>            prob_add;
    std::vector<double>            prob_val;
    std::vector<long>              prob_too_small;
    std::vector<long>              prob_too_large;
    std::vector<double>::size_type i_add;
    std::vector<double>::size_type num_add;
    int                            num_sel;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_refine (%d candidates)", 
          m_numCC);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Store number of counterpart candidates before selection
      m_cpt_stat[iSrc*(m_num_Sel+1)] = m_numCC;

      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Initialise cumulative additional probabilities,
      // set unique counterpart candidate identifier and
      // clear counterpart vectors
      prob_add.clear();
      for (iCC = 0; iCC < m_numCC; iCC++) {
        prob_add.push_back(1.0);
        sprintf(cid, "CC_%5.5ld_%5.5ld", iSrc+1, iCC+1);
        m_cc[iCC].id = cid;
        m_cc[iCC].prob_add.clear();
      }

      // Determine counterpart probabilities based on position
      status = cid_prob_pos(par, iSrc, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine counterpart probabilities"
              " for source %d based on position.", 
              (Status)status, iSrc+1);
        continue;
      }

      // Determine number of additional probabilites
      num_add = par->m_probColNames.size();

      // Determine number of output catalogue selection strings
      num_sel = par->m_select.size();

      // If additional probabilities are requested or a selection should
      // be performed then we setup an in-memory catalogue
      if (num_add > 0 || num_sel > 0) {

        // Clear in memory catalogue
        status = cfits_clear(m_memFile, par, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to clear in-memory FITS catalogue.", 
            (Status)status);
          continue;
        }

        // Add quantities to in memory catalogue
        status = cfits_add(m_memFile, iSrc, par, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to add counterpart candidates to"
                " in-memory FITS catalogue.", (Status)status);
          continue;
        }

      } // endif: in-memory catalogue has been requested

      // If we need additional probability information then calculate it now
      if (num_add > 0) {

        // Initialise probability boundary violation counters
        prob_too_small.clear();
        prob_too_large.clear();
        for (i_add = 0; i_add < num_add; i_add++) {
          prob_too_small.push_back(0);
          prob_too_large.push_back(0);
        }

        // Evaluate in memory catalogue quantities (no verbosity)
        status = cfits_eval(m_memFile, par, 0, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate new quantities for in-memory"
                " FITS catalogue.", (Status)status);
          continue;
        }

        // Loop over the additional probability columns
        for (i_add = 0; i_add < num_add; i_add++) {
          // Extract probability information from column
          status = cfits_colval(m_memFile, 
                                (char*)par->m_probColNames[i_add].c_str(),
                                par, &prob_val, status);
          if (status != STATUS_OK) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to determine probability from column"
                  " '%s' of in-memory FITS catalogue.",
                  (Status)status, par->m_probColNames[i_add].c_str());
            break;
          }

          // Push probabilities in vector and cumulate additional probabilities.
          // Make sure that the probabilities are composed between 0 and 1 ...
          for (iCC = 0; iCC < m_numCC; iCC++) {

            // Set probability in the range [0,1]
            prob = prob_val[iCC];
            if (prob < 0.0) {
              prob = 0.0;
              prob_too_small[i_add]++;
            }
            else if (prob > 1.0) {
              prob = 1.0;
              prob_too_large[i_add]++;
            }

            // Cumulate additional probabilities
            prob_add[iCC] *= prob;

            // Save probability for each counterpart candidate (for logging)
            m_cc[iCC].prob_add.push_back(prob);
          } // endfor: looped over all counterpart candidates

        } // endfor: looped over the additional probability columns
        if (status != STATUS_OK)
          continue;

      } // endif: calculated additional quantities for probability estimation

      // Select counterparts if requested
      if (num_sel > 0) {
        status = cid_select(par, iSrc, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to select counterparts for source %d.",
                (Status)status, iSrc+1);
          continue;
        }
      }

      // Determine local chance coincidence probability
      status = cid_prob_chance(par, iSrc, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine chance coincidence probability"
              " for source %d.",
              (Status)status, iSrc+1);
        continue;
      }

      // Assign the counterpart probabilities
      for (iCC = 0; iCC < m_numCC; iCC++) {
        m_cc[iCC].prob = m_cc[iCC].prob_pos *
                         prob_add[iCC] *
                         (1.0 - m_cc[iCC].prob_chance);
      }

      // Sort counterpart candidates by decreasing probability
      status = cid_sort(par, status);
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
      if (numUseCC > par->m_maxNumCpt)
        numUseCC = par->m_maxNumCpt;

      // Eliminate counterpart candidates below threshold. Fall through if no
      // counterparts are left
      m_numCC = numUseCC;
      if (m_numCC < 1) {
        if (par->logExplicit()) {
          Log(Log_2, "  Refine step candidates ..........: no");
          for (i_add = 0; i_add < num_add; i_add++) {
            if (prob_too_small[i_add] > 1)
              Log(Warning_2, 
                  "  Probabilities < 0.0 .............: %5d (Quantity='%s')",
                  prob_too_small[i_add],
                  par->m_probColNames[i_add].c_str());
            if (prob_too_large[i_add] > 1)
              Log(Warning_2, 
                  "  Probabilities > 1.0 .............: %5d (Quantity='%s')",
                  prob_too_large[i_add],
                  par->m_probColNames[i_add].c_str());
          }
        }
        continue;
      }

      // Set unique counterpart candidate identifier (overwrite former ID)
      for (iCC = 0; iCC < m_numCC; iCC++) {
        sprintf(cid, "CC_%5.5ld_%5.5ld", iSrc+1, iCC+1);
        m_cc[iCC].id = cid;
      }

      // Optionally dump counterpart refine statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Refine step candidates ..........: %5d", m_numCC);
        for (i_add = 0; i_add < num_add; i_add++) {
          if (prob_too_small[i_add] > 1)
            Log(Warning_2, 
                "  Probabilities < 0.0 .............: %5d (Quantity='%s')",
                prob_too_small[i_add],
                par->m_probColNames[i_add].c_str());
          if (prob_too_large[i_add] > 1)
            Log(Warning_2, 
                "  Probabilities > 1.0 .............: %5d (Quantity='%s')",
                prob_too_large[i_add],
                par->m_probColNames[i_add].c_str());
        }
     }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_refine (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Select counterparts
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 *
 * Only the counterparts are retained that satisfy the specified selection
 * criteria. Selection is performed using the CFITSIO  row selection
 * function.
 ******************************************************************************/
Status Catalogue::cid_select(Parameters *par, long iSrc, Status status) {

    // Declare local variables
    std::vector<std::string> col_id;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_select (%d candidates)", 
          m_numCC);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if no selection strings are specified.
      int m_num_Sel = par->m_select.size();
      if (m_num_Sel < 1)
        continue;

      // Select counterparts in memory
      status = cfits_select(m_memFile, iSrc, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to select catalogue counterparts.",
              (Status)status);
        continue;
      }

      // Get list of counterpart IDs that survived
      status = cfits_get_col_str(m_memFile, par, OUTCAT_COL_ID_NAME, col_id,
                                 status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read counterpart IDs from memory.",
              (Status)status);
        continue;
      }

      // If list is empty then stop now
      int nSelected = (int)col_id.size();
      if (nSelected < 1) {
        m_numCC = 0;
        continue;
      }

      // Collect all counterparts that survived
      int inx = 0;
      for (int iCC = 0; iCC < m_numCC; ++iCC) {
        for (int i = 0; i < nSelected; ++i) {
          if (m_cc[iCC].id == col_id[i]) {
            m_cc[inx] = m_cc[iCC];
            inx++;
          }
        }
      }

      // Check that we found everybody
      if (inx != nSelected) {
        status = STATUS_CAT_SEL_FAILED;
        if (par->logTerse())
          Log(Error_2, "%d : In-memory counterpart selection error (%d/%d).",
              (Status)status, inx, nSelected);
        continue;
      }

      // Set number of remaining counterparts
      m_numCC = nSelected;

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_select (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Calculate the counterpart probability based on position
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 *
 * The following members of the counterpart candidates are set:
 * pos_eq_ra    Right Ascension of counterpart candidate (deg)
 * pos_eq_dec   Declination of counterpart candidate (deg)
 * pos_err_maj  Uncertainty ellipse major axis (deg)
 * pos_err_min  Uncertainty ellipse minor axis (deg)
 * pos_err_ang  Uncertainty ellipse positron angle (deg)
 * angsep       Angular separation of counterpart candidate from source
 * posang       Position angle of counterpart candidate w/r to source
 * prob_pos     Probability of counterpart candidate based on position 
 ******************************************************************************/
Status Catalogue::cid_prob_pos(Parameters *par, long iSrc, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_pos (%d candidates)",
          m_numCC);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Get pointer to source object
      ObjectInfo *src = &(m_src.object[iSrc]);

      // Fall through if no source position is available
      if (!src->pos_valid)
        continue;

      // Calculate sin and cos of source declination
      double dec         = src->pos_eq_dec * deg2rad;
      double src_dec_sin = sin(dec);
      double src_dec_cos = cos(dec);

      // Loop over counterpart candidates
      for (int iCC = 0; iCC < m_numCC; iCC++) {

        // Get index of candidate in counterpart catalogue
        int iCpt = m_cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Fall through if no counterpart position is available
        if (!cpt->pos_valid)
          continue;

        // Perform trigonometric operations to prepare angular separation
        // and position angle calculations
        double ra_diff     = (cpt->pos_eq_ra - src->pos_eq_ra) * deg2rad;
        double dec         = cpt->pos_eq_dec * deg2rad;
        double cpt_dec_sin = sin(dec);
        double cpt_dec_cos = cos(dec);
        double cpt_dec_tan = tan(dec);
        double arg         = src_dec_sin * cpt_dec_sin +
                             src_dec_cos * cpt_dec_cos * cos(ra_diff);

        // Calculate angular separation between source and counterpart in
        // degrees. Make sure that the separation is always comprised between
        // [0,180] (out of range arguments lead to a floating exception).
        if (arg <= -1.0)
          m_cc[iCC].angsep = 180.0;
        else if (arg >= 1.0)
          m_cc[iCC].angsep = 0.0;
        else
          m_cc[iCC].angsep = acos(arg) * rad2deg;

        // Calculate position angle, counterclockwise from celestial north
        m_cc[iCC].posang = atan2(sin(ra_diff), src_dec_cos*cpt_dec_tan -
                                 src_dec_sin*cos(ra_diff)) * rad2deg;

        // Calculate 95% source error ellipse
        double angle     = (m_cc[iCC].posang - src->pos_err_ang) * deg2rad;
        double cos_angle = cos(angle);
        double sin_angle = sin(angle);
        double a         = (src->pos_err_maj > 0.0) ? (cos_angle*cos_angle) /
                           (src->pos_err_maj*src->pos_err_maj) : 0.0;
        double b         = (src->pos_err_min > 0.0) ? (sin_angle*sin_angle) /
                           (src->pos_err_min*src->pos_err_min) : 0.0;
        arg              = a + b;
        double error     = (arg > 0.0) ? 1.0/sqrt(arg) : 0.0;

        // Calculate counterpart probability from angular separation
        switch (par->m_posProbType) {
        case Exponential:         // Initial formula (obsolete)
          if (error > 0.0)
            m_cc[iCC].prob_pos = exp(-m_cc[iCC].angsep / error);
          else
            m_cc[iCC].prob_pos = 0.0;
          break;

        case Gaussian:            // Parabolic log-likelihood function
        default:
          double error2 = error * error;
          if (error2 > 0.0) {
            arg                = m_cc[iCC].angsep * m_cc[iCC].angsep / error2;
            m_cc[iCC].prob_pos = exp(-2.9957230 * arg);
          }
          else
            m_cc[iCC].prob_pos = 0.0;
          break;
        }

        // Assign position and error ellipse
        // DUMMY: we assign the position and error ellipse of the partner
        //        that has the smaller positional uncertainty
        if (cpt->pos_err_maj < src->pos_err_maj) {
          m_cc[iCC].pos_eq_ra   = cpt->pos_eq_ra;
          m_cc[iCC].pos_eq_dec  = cpt->pos_eq_dec;
          m_cc[iCC].pos_err_maj = cpt->pos_err_maj;
          m_cc[iCC].pos_err_min = cpt->pos_err_min;
          m_cc[iCC].pos_err_ang = cpt->pos_err_ang;
        }
        else {
          m_cc[iCC].pos_eq_ra   = src->pos_eq_ra;
          m_cc[iCC].pos_eq_dec  = src->pos_eq_dec;
          m_cc[iCC].pos_err_maj = src->pos_err_maj;
          m_cc[iCC].pos_err_min = src->pos_err_min;
          m_cc[iCC].pos_err_ang = src->pos_err_ang;
        }

      } // endfor: looped over counterpart candidates

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob_pos (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute chance coincidence probability
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] iSrc Index of source in catalogue (starting from 0).
 * @param[in] status Error status.
 ******************************************************************************/
Status Catalogue::cid_prob_chance(Parameters *par, long iSrc, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_chance (%d candidates)",
          m_numCC);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Get pointer to source object
      ObjectInfo *src = &(m_src.object[iSrc]);

      // Fall through if no source position is available
      if (!src->pos_valid)
        continue;

      // Compute number of counterparts within acceptance ring
      int num = 0;
      for (int iCC = 0; iCC < m_numCC; iCC++) {

        // Get index of candidate in counterpart catalogue
        int iCpt = m_cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Fall through if no counterpart position is available
        if (!cpt->pos_valid)
          continue;

        // Collect all sources in circular region
        if (m_cc[iCC].angsep >= m_cc[iCC].rho_rad_min &&
            m_cc[iCC].angsep <= m_cc[iCC].rho_rad_max)
          num++;

      } // endfor: looped over all counterpart candidates

      // Compute chance coincidence probabilities for all sources
      for (int iCC = 0; iCC < m_numCC; iCC++) {

        // Get index of candidate in counterpart catalogue
        int iCpt = m_cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Fall through if no counterpart position is available
        if (!cpt->pos_valid)
          continue;

        // Compute local counterpart density (counterparts / deg2)
        m_cc[iCC].rho = double(num) / m_cc[iCC].rho_omega;

        // Compute the expected number of sources within the area given
        // by the angular separation between source and counterpart
        m_cc[iCC].lambda = pi * m_cc[iCC].angsep * m_cc[iCC].angsep *
                           m_cc[iCC].rho;

        // Compute chance coincidence probability
        if (par->m_chanceProbType == Local)
          m_cc[iCC].prob_chance = 1.0 - exp(-m_cc[iCC].lambda);

      } // endfor: looped over all counterpart candidates

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob_chance (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Sort counterpart candidates
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] status Error status.
 *
 * Sort by increasing probability and in case of equal probability) by 
 * decreasing angular separation
 ******************************************************************************/
Status Catalogue::cid_sort(Parameters *par, Status status) {

    // Declare local variables
    long      iCC;
    long      jCC;
    long      imax;
    double    max_prob;
    double    min_sep;
    CCElement swap;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_sort");

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
        max_prob =   0.0;
        min_sep  = 180.0;
        imax     = iCC;
        for (jCC = iCC; jCC < m_numCC; jCC++) {

          // Check if we found a better candidate
          if (m_cc[jCC].prob > max_prob) {        // Increasing probability
            imax     = jCC;
            max_prob = m_cc[jCC].prob;
            min_sep  = m_cc[jCC].angsep;
          }
          else if (m_cc[jCC].prob == max_prob) {  // Equal probability &
            if (m_cc[jCC].angsep < min_sep) {     // decreasing separation
              imax     = jCC;
              max_prob = m_cc[jCC].prob;
              min_sep  = m_cc[jCC].angsep;
            }
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
      Log(Log_0, " <== EXIT: Catalogue::cid_sort (status=%d)", status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Dump counterpart candidates that are actually in buffer
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] status Error status.
 ******************************************************************************/
Status Catalogue::cid_dump(Parameters *par, Status status) {

    // Declare local variables
    long                            iCC;
    long                            iCpt;
    std::vector<double>::size_type  i_add;
    std::vector<double>::size_type  num_add;
    ObjectInfo                     *cpt;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_dump");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (m_numCC < 1)
        continue;

      // Loop over counterpart candidates
      for (iCC = 0; iCC < m_numCC; iCC++) {

        // Get index of candidate in counterpart catalogue
        iCpt = m_cc[iCC].index;

        // Get pointer to counterpart object
        cpt = &(m_cpt.object[iCpt]);

        // Determine number of additional probabilites
        num_add = m_cc[iCC].prob_add.size();

        // Normal log level
        if (par->logNormal()) {
          if (cpt->pos_valid) {
            Log(Log_2, "  Cpt%5d P=%3.0f%% S=%6.2f' PA=%4.0f: %20s"SRC_FORMAT,
                iCC+1, 
                m_cc[iCC].prob*100.0, m_cc[iCC].angsep*60.0, m_cc[iCC].posang,
                cpt->name.c_str(),
                cpt->pos_eq_ra, cpt->pos_eq_dec,
                cpt->pos_err_maj, cpt->pos_err_min, cpt->pos_err_ang);
          }
          else {
            Log(Log_2, "  Cpt%5d P=%3.0f%% .................: %20s"
                " No position information found",
                iCC+1, 
                m_cc[iCC].prob*100.0,
                cpt->name.c_str());
          }
        }

        // Explicit log level
        if (par->logVerbose()) {
          if (cpt->pos_valid) {
            if (num_add > 0) {
              Log(Log_2, "   Angular separation probability .: %7.3f %%",
                  m_cc[iCC].prob_pos*100.0);
              for (i_add = 0; i_add < num_add; i_add++) {
                Log(Log_2, 
                    "   Additional probability .........: %7.3f %%"
                    "  (Quantity='%s')",
                    m_cc[iCC].prob_add[i_add]*100.0,
                    par->m_probColNames[i_add].c_str());
              }
              Log(Log_2, "   Chance coincidence probability .: %7.3f %%",
                  m_cc[iCC].prob_chance*100.0);
            }
          }
          Log(Log_2, "    Local counterpart density .....: %10.5f src/deg^2",
              m_cc[iCC].rho);
          Log(Log_2, "    Expected number of chance coinc: %10.5f src",
              m_cc[iCC].lambda);
          Log(Log_2, "    Local density ring ............: %7.3f - %7.3f deg"
              " (%7.3f deg^2)",
              m_cc[iCC].rho_rad_min, m_cc[iCC].rho_rad_max, m_cc[iCC].rho_omega);
        }

      } // endfor: looped over counterpart candidats

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_dump (status=%d)", status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Assign source name
 *
 * @param[in] name Source name.
 * @param[in] row Catalogue row (starting from 0).
 *
 * Assigns source name for output catalogue. Leading and trailing whitespace
 * are stripped from the source name provided on input. If the source name
 * string is empty, a dummy name will be constructed from the catalogue row
 * number (CAT_ROW_xxx, where xxx is the row number starting from 1).
 ******************************************************************************/

/*----------------------------------------------------------------------------*/
/*                      Catalogue::cid_assign_src_name                        */
/* -------------------------------------------------------------------------- */
/* Private method: assign source name if string                               */
/*----------------------------------------------------------------------------*/
std::string Catalogue::cid_assign_src_name(std::string name, int row) {

    // If string is empty then assign a default name
    std::string empty = name;
    std::remove(empty.begin(), empty.end(), ' ');
    if (empty == " ") {
      std::ostringstream number;
      number << (row+1);
      name = "CAT_ROW_" + number.str();
    }

    // Return name
    return name;

}


/* Namespace ends ___________________________________________________________ */
}
