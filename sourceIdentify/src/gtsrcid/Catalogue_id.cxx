/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.27  2008/04/18 20:50:33  jurgen
Implement catch-22 scheme for prior probability calculation and compute log likelihood-ratio instead of likelihood ratio (avoid numerical problems)

Revision 1.26  2008/04/18 16:14:16  jurgen
Add LR statistics to log file

Revision 1.25  2008/04/18 10:43:20  jurgen
Allow for divergent LRs (flag them)

Revision 1.24  2008/04/16 22:00:34  jurgen
Compute unique posterior probabilities

Revision 1.23  2008/04/15 22:30:54  jurgen
Cleanup counterpart statistics

Revision 1.22  2008/04/15 21:24:12  jurgen
Introduce sparse matrix for source catalogue probability computation.

Revision 1.21  2008/04/04 14:55:52  jurgen
Remove counterpart candidate working memory and introduce permanent counterpart candidate memory

Revision 1.20  2008/03/26 16:57:30  jurgen
implement global counterpart density evaluation

Revision 1.19  2008/03/26 13:37:10  jurgen
Generalize probability calculation and implement Bayesian method

Revision 1.18  2008/03/21 16:42:56  jurgen
Update documentation

Revision 1.17  2008/03/21 15:27:03  jurgen
Estimate number of false associations

Revision 1.16  2008/03/21 09:10:12  jurgen
Enhance code documentation.

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
#include <iostream>
#include <stdexcept>
#include "sourceIdentify.h"
#include "Catalogue.h"
#include "Log.h"

/* Definitions ______________________________________________________________ */
#define CATALOGUE_TIMING     0               // Enables timing measurements
#define ALLOW_LR_DIVERGENCE  1               // Allows for divergent LR values

/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {
using namespace catalogAccess;


/* Type defintions __________________________________________________________ */


/* Private Prototypes _______________________________________________________ */


/**************************************************************************//**
 * @brief Perform single source association
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * This is the main counterpart association driving routine. Counterpart
 * association is done in a two step process:\n
 * (1) a filter step, and \n
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
Status Catalogue::cid_source(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_source");

    // Single loop for common exit point
    do {

      // Dump source information (optionally)
      if (par->logNormal()) {
        if (src->info->pos_valid) {
          Log(Log_2, " Source %5d .....................: %20s"SRC_FORMAT,
              src->iSrc+1, src->info->name.c_str(),
              src->info->pos_eq_ra, src->info->pos_eq_dec,
              src->info->pos_err_maj, src->info->pos_err_min,
              src->info->pos_err_ang);
        }
        else {
          Log(Log_2, " Source %5d .....................: %20s"
              " No position information found.",
              src->iSrc+1, src->info->name.c_str());
        }
      }

      // Fall through if no position information has been found
      if (!src->info->pos_valid)
        continue;

      // Filter step: Get counterparts near the source position
      status = cid_filter(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to perform filter step for source %d.",
              (Status)status, src->iSrc+1);
        continue;
      }

      // Selection step: Select only relevant counterparts
      status = cid_select(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to perform selection step for source %d.",
              (Status)status, src->iSrc+1);
        continue;
      }

      // Refine step: Assign probability for each counterpart
      status = cid_refine(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to perform refine step for source %d.",
              (Status)status, src->iSrc+1);
        continue;
      }

      // Optionally dump counterpart candidats
      if (par->logNormal())
        cid_dump(par, src, status);

     } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_source (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Filter step of counterpart identification
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * @todo Load counterpart catalogue only partially if number of objects is large.
 * @todo Proper prior assignment
 *
 * The filter step gets all counterpart candidates from the catalogue for a
 * given source that are sufficiently close to the source.
 * This is done by defining a rectangular bounding box around the source
 * that extends from -m_filter_rad to +m_filter_rad in Declination
 * and from -m_filter_rad/cos(dec) to +m_filter_rad/cos(dec) in Right
 * Ascension.
 *
 * This method sets the number of filter step candidates in src->numFilter.
 ******************************************************************************/
Status Catalogue::cid_filter(Parameters *par, SourceInfo *src, Status status) {

    // Declare local variables
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
    ObjectInfo *cpt;

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
      numNoPos = 0;
      numRA    = 0;
      numDec   = 0;

      // Calculate sin and cos of source latitude
      src_dec_sin = sin(src->info->pos_eq_dec * deg2rad);
      src_dec_cos = cos(src->info->pos_eq_dec * deg2rad);

      // Set bounding box enclosing radius
      double radius   = src->info->pos_err_maj * 5.0 / 2.0; // 5 sigma radius
      src->filter_rad = (radius > c_filter_maxsep) ? radius : c_filter_maxsep;

      // Set local counterpart density ring radii
      src->ring_rad_min = 0.0;
      src->ring_rad_max = src->filter_rad;

      // Compute solid angle of error ellipse
      src->omega = pi * src->info->pos_err_maj * src->info->pos_err_min;

      // Define bounding box around source position. The declination
      // range of the bounding box is constrained to [-90,90] deg, the
      // Right Ascension boundaries are put into the interval [0,360[ deg.
      cpt_dec_min = src->info->pos_eq_dec - src->filter_rad;
      cpt_dec_max = src->info->pos_eq_dec + src->filter_rad;
      if (cpt_dec_min < -90.0) cpt_dec_min = -90.0;
      if (cpt_dec_max >  90.0) cpt_dec_max =  90.0;
      if (src_dec_cos > 0.0) {
        filter_maxsep = src->filter_rad / src_dec_cos;
        if (filter_maxsep > 180.0)
          filter_maxsep = 180.0;
      }
      else
        filter_maxsep = 180.0;
      cpt_ra_min  = src->info->pos_eq_ra - filter_maxsep;
      cpt_ra_max  = src->info->pos_eq_ra + filter_maxsep;
      cpt_ra_min = cpt_ra_min - double(long(cpt_ra_min / 360.0) * 360.0);
      if (cpt_ra_min < 0.0) cpt_ra_min += 360.0;
      cpt_ra_max = cpt_ra_max - double(long(cpt_ra_max / 360.0) * 360.0);
      if (cpt_ra_max < 0.0) cpt_ra_max += 360.0;

      // Start timing
      #if CATALOGUE_TIMING
      t_start_loop = clock();
      #endif

      // Determine number of counterpart candidates that fall in the
      // bounding box and that have a valid position
      src->numFilter = 0;
      cpt            = m_cpt.object;
      for (int iCpt = 0; iCpt < m_cpt.numLoad; iCpt++, cpt++) {

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

        // If we are still alive then keep this counterpart
        m_cpt_sel[src->numFilter] = iCpt;

        // Increment number of counterparts
        src->numFilter++;

      } // endfor: looped over all counterpart candidates

      // Collect all counterpart candidates
      if (src->numFilter > 0) {

        // Allocate memory for counterpart candidates
        src->cc = new CCElement[src->numFilter];
        if (src->cc == NULL) {
          status = STATUS_MEM_ALLOC;
          if (par->logTerse())
            Log(Error_2, "%d : Memory allocation failure.", (Status)status);
          continue;
        }

        // Initialise all counterpart candidates
        for (int i = 0; i < src->numFilter; ++i) {
          src->cc[i].id               = "NULL";
          src->cc[i].pos_eq_ra        = 0.0;
          src->cc[i].pos_eq_dec       = 0.0;
          src->cc[i].pos_err_maj      = 0.0;
          src->cc[i].pos_err_min      = 0.0;
          src->cc[i].pos_err_ang      = 0.0;
          src->cc[i].prob             = 0.0;
          src->cc[i].index            = m_cpt_sel[i];
          src->cc[i].angsep           = 0.0;
          src->cc[i].psi              = 0.0;
          src->cc[i].posang           = 0.0;
          src->cc[i].mu               = 0.0;
          src->cc[i].prob_pos         = 0.0;
          src->cc[i].prob_chance      = 0.0;
          src->cc[i].prob_prior       = 0.0;
          src->cc[i].prob_post        = 0.0;
          src->cc[i].prob_post_single = 0.0;
          src->cc[i].prob_post_cat    = 0.0;
          src->cc[i].pdf_pos          = 0.0;
          src->cc[i].pdf_chance       = 0.0;
          src->cc[i].likrat           = 0.0;
          src->cc[i].rho              = 0.0;
          src->cc[i].fom              = 0.0;
          src->cc[i].prob_prod1       = 0.0;
          src->cc[i].prob_prod2       = 0.0;
          src->cc[i].prob_norm        = 0.0;
          src->cc[i].likrat_div       = 0;

        } // endfor: looped over all counterpart candidates
      } // endif: there were counterpart candidates

      // Optionally dump counterpart filter statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Filter step candidates ..........: %5d", src->numFilter);
        if (par->logVerbose()) {
          Log(Log_2, "    Filter bounding box radius ....: %7.3f deg",
              src->filter_rad);
          Log(Log_2, "    Error ellipse solid angle .....: %7.3f deg^2",
              src->omega);
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
 * @brief Selection step of counterpart identification
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Selects only relevant counterparts. Non-selected counterparts are considered
 * to be deleted.
 *
 * This method expects src->numFilter counterparts. It sets the number of
 * selected candidates in src->numSelect.
 ******************************************************************************/
Status Catalogue::cid_select(Parameters *par, SourceInfo *src, Status status) {

    // Declare local variables
    std::vector<std::string> col_id;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_select (%d candidates)",
          src->numFilter);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Initialise number of selected sources
      src->numSelect = src->numFilter;

      // Store number of counterpart candidates before selection
      m_cpt_stat[src->iSrc*(m_num_Sel+1)] = src->numFilter;

      // Fall through if there are no counterpart candidates
      if (src->numFilter < 1)
        continue;

      // Set unique counterpart candidate identifier for in-memory catalogue
      char cid[OUTCAT_MAX_STRING_LEN];
      for (int iCC = 0; iCC < src->numFilter; ++iCC) {
        sprintf(cid, "CC_%5.5d_%5.5d", src->iSrc+1, iCC+1);
        src->cc[iCC].id = cid;
      }

      // Update in-memory catalogue
      status = cfits_update(m_memFile, par, src, src->numFilter, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update counterpart candidates"
                       " in-memory FITS catalogue.", (Status)status);
        continue;
      }

      // Fall through if no selection strings are specified.
      int m_num_Sel = par->m_select.size();
      if (m_num_Sel < 1)
        continue;

      // Select counterparts in memory
      status = cfits_select(m_memFile, par, src, status);
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
        src->numSelect = 0;
        continue;
      }

      // Collect all counterparts that survived
      int inx = 0;
      for (int iCC = 0; iCC < src->numFilter; ++iCC) {
        for (int i = 0; i < nSelected; ++i) {
          if (src->cc[iCC].id == col_id[i]) {
            src->cc[inx] = src->cc[iCC];
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
      src->numSelect = nSelected;

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_select (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Refine step of counterpart identification
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * @todo Develop other than local counterpart density methods.
 *
 * Calculates the counterpart probability, sorts all counterpart candidates
 * by decreasing probability and eliminates all candidtates with a too low
 * probability.
 *
 * This method expects src->numSelect counterparts. It sets the number of refine
 * step candidates in src->numRefine.
 ******************************************************************************/
Status Catalogue::cid_refine(Parameters *par, SourceInfo *src, Status status) {

    // Declare local variables

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_refine (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Initialise number of refine step candidates
      src->numRefine = src->numSelect;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Compute figures of merit
      status = cid_fom(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to compute FOM.", (Status)status);
        continue;
      }

      // Compute PROB_POS and PDF_POS.
      status = cid_prob_pos(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to compute PROB_POS.", (Status)status);
        continue;
      }

      // Determine counterpart density (requires information computed in
      // cid_prob_pos)
      status = cid_local_density(par, src, status);
//      status = cid_global_density(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine counterpart density.",
              (Status)status);
        continue;
      }

      // Compute PROB_CHANCE and PDF_CHANCE
      status = cid_prob_chance(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine chance coincidence probability.",
              (Status)status);
        continue;
      }

      // Compute PROB_PRIOR
      status = cid_prob_prior(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to compute PROB_PRIOR.", (Status)status);
        continue;
      }

      // Compute PROB_POST_SINGLE
      status = cid_prob_post_single(par, src, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine posterior probability.",
              (Status)status);
        continue;
      }

      // Store PROB_POST_SINGLE in PROB for sorting
      for (int iCC = 0; iCC < src->numSelect; ++iCC)
         src->cc[iCC].prob = src->cc[iCC].prob_post_single;

      // Sort counterpart candidates by decreasing probability
      status = cid_sort(par, src, src->numSelect, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to sort counterpart candidates.",
              (Status)status);
        continue;
      }

      // Neglect counterparts with too low probability
      src->numRefine = 0;
      for (int iCC = 0; iCC < src->numSelect; ++iCC) {
        if (src->cc[iCC].prob_post_single <= c_prob_min)
          break;
        src->numRefine++;
      }

      // Fall through if no counterparts are left
      if (src->numRefine < 1) {
        if (par->logExplicit())
          Log(Log_2, "  Refine step candidates ..........: no");
        continue;
      }

      // Optionally dump counterpart refine statistics
      if (par->logExplicit()) {
        Log(Log_2, "  Refine step candidates ..........: %5d", src->numRefine);
//        Log(Log_2, "    Local counterpart density .....: %.5f src/deg^2", src->rho);
        Log(Log_2, "    Local density ring ............: %.3f - %.3f deg",
            src->ring_rad_min, src->ring_rad_max);
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
 * @brief Compute FoM for all counterpart candidates
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Requires catalogue information in FITS memory file.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_fom(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_fom (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Compute FoM only if a formula has been provided
      if (par->m_FoM.length() > 0) {

        // Set column name and allocate FoM vector
        std::string         column  = "FOM";
        std::vector<double> fom;

        // Evaluate FoM column
        status = cfits_eval_column(m_memFile, par, column, par->m_FoM, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate expression <%s='%s'> in"
                         " formula.",
                         (Status)status, column.c_str(), par->m_FoM.c_str());
          continue;
        }

        // Extract FoM vector
        status = cfits_get_col(m_memFile, par, column, fom, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to extract column <%s> from"
                         "in-memory FITS catalogue.",
                         (Status)status, column.c_str());
          continue;
        }

        // Get FoM
        for (int iCC = 0; iCC < src->numSelect; ++iCC)
          src->cc[iCC].fom = fom[iCC];

      }

      // ... otherwise reset FoM to zero
      else {
        for (int iCC = 0; iCC < src->numSelect; ++iCC)
          src->cc[iCC].fom = 0.0;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_fom (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Calculate the counterpart probability based on position
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer of source information.
 * @param[in] status Error status.
 *
 * Computes the probability \b PROB_POS that the true source position is
 * located at an angular distance greater than \f$ r \f$ from the position
 * measured by LAT.
 * The computation is done using
 * \f[ {\tt PROB\_POS} = \exp(-5.991 r^2 / \Psi^2) \f]
 * where \f$ \Psi \f$ is the effective 95% error radius of the LAT source.
 * \f$ \Psi \f$ is computed using
 * \f[ \Psi = \left[ \frac{\cos^2(\theta-\phi)}{a^2} + 
 *                   \frac{\sin^2(\theta-\phi)}{b^2}\right]^{-1/2} \f]
 * where
 * \f$ a \f$ and \f$ b \f$ are the semimajor and semiminor axes of the
 * LAT error ellipse,
 * \f$ \phi \f$ is the position angle of the LAT error ellipse (measured
 * eastwards from north in celestial coordinates) and
 * \f$ \theta \f$ is the position angle of the counterpart source with respect
 * to the LAT source.
 *
 * Further computes \b PDF_POS, which is the probability density of
 * 1-PROB_POS.
 * The probability density is computed using
 * \f[ {\tt PDF\_POS} = 5.991 \frac{r}{\Psi^2} exp(-2.996 r^2 / \Psi^2) \f]
 *
 * @todo Define more intelligent scheme to attribute counterpart position errors.
 *
 * This method updates the following fields \n
 * CCElement::angsep (angular separation of counterpart candidate from source) \n
 * CCElement::posang (position angle of counterpart candidate w/r to source) \n
 * CCElement::psi (effective 95% error ellipse radius) \n
 * CCElement::prob_pos (position association probability) \n
 * CCElement::pdf_pos (position association probability density) \n
 * CCElement::pos_eq_ra (Right Ascension of counterpart candidate) \n
 * CCElement::pos_eq_dec (Declination of counterpart candidate) \n
 * CCElement::pos_err_maj (uncertainty ellipse major axis) \n
 * CCElement::pos_err_min (uncertainty ellipse minor axis) \n
 * CCElement::pos_err_ang (uncertainty ellipse positron angle) \n
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_prob_pos(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_pos (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Fall through if no source position is available
      if (!src->info->pos_valid)
        continue;

      // Calculate sin and cos of source declination
      double dec         = src->info->pos_eq_dec * deg2rad;
      double src_dec_sin = sin(dec);
      double src_dec_cos = cos(dec);

      // Loop over counterpart candidates
      for (int iCC = 0; iCC < src->numSelect; ++iCC) {

        // Get index of candidate in counterpart catalogue
        int iCpt = src->cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Fall through if no counterpart position is available
        if (!cpt->pos_valid)
          continue;

        // Perform trigonometric operations to prepare angular separation
        // and position angle calculations
        double ra_diff     = (cpt->pos_eq_ra - src->info->pos_eq_ra) * deg2rad;
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
          src->cc[iCC].angsep = 180.0;
        else if (arg >= 1.0)
          src->cc[iCC].angsep = 0.0;
        else
          src->cc[iCC].angsep = acos(arg) * rad2deg;

        // Calculate position angle, counterclockwise from celestial north
        src->cc[iCC].posang = atan2(sin(ra_diff), src_dec_cos*cpt_dec_tan -
                                    src_dec_sin*cos(ra_diff)) * rad2deg;

        // Calculate 95% source error ellipse
        double angle     = (src->cc[iCC].posang - src->info->pos_err_ang) * deg2rad;
        double cos_angle = cos(angle);
        double sin_angle = sin(angle);
        double a         = (src->info->pos_err_maj > 0.0) ? (cos_angle*cos_angle) /
                           (src->info->pos_err_maj*src->info->pos_err_maj) : 0.0;
        double b         = (src->info->pos_err_min > 0.0) ? (sin_angle*sin_angle) /
                           (src->info->pos_err_min*src->info->pos_err_min) : 0.0;
        arg              = a + b;
        double error2    = (arg > 0.0) ? 1.0/arg : 0.0;

        // Calculate counterpart probability from angular separation
        if (error2 > 0.0) {
          double arg1           = src->cc[iCC].angsep / error2;
          double arg2           = src->cc[iCC].angsep * arg1;
          double expval         = exp(-dnorm * arg2);
          src->cc[iCC].psi      = sqrt(error2);
          src->cc[iCC].pdf_pos  = twodnorm * arg1 * expval;
          src->cc[iCC].prob_pos = expval;
        }
        else {
          src->cc[iCC].psi      = 0.0;
          src->cc[iCC].pdf_pos  = 0.0;
          src->cc[iCC].prob_pos = 0.0;
        }

        // Assign position and error ellipse
        // DUMMY: we assign the position and error ellipse of the partner
        //        that has the smaller positional uncertainty
        if (cpt->pos_err_maj < src->info->pos_err_maj) {
          src->cc[iCC].pos_eq_ra   = cpt->pos_eq_ra;
          src->cc[iCC].pos_eq_dec  = cpt->pos_eq_dec;
          src->cc[iCC].pos_err_maj = cpt->pos_err_maj;
          src->cc[iCC].pos_err_min = cpt->pos_err_min;
          src->cc[iCC].pos_err_ang = cpt->pos_err_ang;
        }
        else {
          src->cc[iCC].pos_eq_ra   = src->info->pos_eq_ra;
          src->cc[iCC].pos_eq_dec  = src->info->pos_eq_dec;
          src->cc[iCC].pos_err_maj = src->info->pos_err_maj;
          src->cc[iCC].pos_err_min = src->info->pos_err_min;
          src->cc[iCC].pos_err_ang = src->info->pos_err_ang;
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
 * @brief Computes chance coincidence probability and probability density
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Computes the chance coincidence probability \b PROB_CHANCE and the
 * chance coincidence probability density \b PDF_CHANCE for a given LAT
 * source.
 * The chance coincidence probability is computed using
 * \f[ {\tt PROB\_CHANCE} = 1 - \exp(-r^2 / r_0^2) \f]
 * where \f$ r_0 \f$ is the characteristic angle between confusing sources
 * and \f$ r \f$ is the angular separation between the LAT source and the
 * counterpart candidate.
 * The chance coincidence probability density is computed using
 * \f[ {\tt PDF\_CHANCE} = 2 \frac{r}{r_0^2} \exp(-r^2 / r_0^2) \f]
 *
 * This method updates the following fields \n
 * CCElement::mu (expected number of confusing sources)\n
 * CCElement::prob_chance (chance coincidence probability)\n
 * CCElement::pdf_chance (chance coincidence probability density)\n
 * The method also updates the corresponding columns in the in-memory FITS file.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_prob_chance(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_chance (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Fall through if no source position is available
      if (!src->info->pos_valid)
        continue;

      // Allocate vector columns for in-memory FITS file update
      std::vector<double> col_mu;
      std::vector<double> col_prob_chance;
      std::vector<double> col_pdf_chance;

      // Compute chance coincidence probabilities for all sources
      for (int iCC = 0; iCC < src->numSelect; iCC++) {

        // Get index of candidate in counterpart catalogue
        int iCpt = src->cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Fall through if no counterpart position is available
        if (!cpt->pos_valid)
          continue;

        // Compute the expected number of sources within the area given
        // by the angular separation between source and counterpart
        double r_div_lambda2 = src->cc[iCC].angsep * pi * src->cc[iCC].rho;
        src->cc[iCC].mu      = r_div_lambda2 * src->cc[iCC].angsep;

        // Compute chance coincidence probability
        double exp_mu            = exp(-src->cc[iCC].mu);
        src->cc[iCC].prob_chance = 1.0 - exp_mu;
        src->cc[iCC].pdf_chance  = 2.0 * r_div_lambda2 * exp_mu;

        // Add result to column vectors
        col_mu.push_back(src->cc[iCC].mu);
        col_prob_chance.push_back(src->cc[iCC].prob_chance);
        col_pdf_chance.push_back(src->cc[iCC].pdf_chance);

      } // endfor: looped over all counterpart candidates

      // Update in-memory columns
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_MU_NAME,
                             col_mu, status);
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_PROB_CHANCE_NAME,
                             col_prob_chance, status);
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_PDF_CHANCE_NAME,
                             col_pdf_chance, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update columns of in-memory FITS file.",
                       (Status)status);
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob_chance (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute prior probabilities for all counterpart candidates
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Computes \n
 * CCElement::prob_prior (prior association probability)
 *
 * The method updates the corresponding column in the in-memory FITS file.
 *
 * The prior probability is constrained to the interval [0,1].
 *
 * Requires catalogue information in FITS memory file.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_prob_prior(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_prior (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Set column name and allocate prior vector
      std::string         column  = "PROB_PRIOR";
      std::vector<double> prob_prior;

      // In case of catch-22, use just the initial value now
      if (par->m_catch22) {
        for (int iCC = 0; iCC < src->numSelect; ++iCC)
          prob_prior.push_back(m_prior);
      }

      // ... otherwise evaluate prior following the formula
      else {

        // Evaluate PROB_PRIOR column
        status = cfits_eval_column(m_memFile, par, column, par->m_probPrior, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate expression <%s='%s'> in"
                         " formula.",
                         (Status)status, column.c_str(), par->m_probPrior.c_str());
          continue;
        }

        // Extract PROB_PRIOR vector
        status = cfits_get_col(m_memFile, par, column, prob_prior, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to extract column <%s> from"
                         "in-memory FITS catalogue.",
                         (Status)status, column.c_str());
          continue;
        }

      } // endelse: evaluated prior

      // Allocate vector column for in-memory FITS file update
      std::vector<double> col_prob_prior;

      // Set PROB_PRIOR information
      for (int iCC = 0; iCC < src->numSelect; ++iCC) {

        // Get probability in the range [0,1]
        double p = prob_prior[iCC];
        if (p < 0.0)      p = 0.0;
        else if (p > 1.0) p = 1.0;

        // Save probability for each counterpart candidate
        src->cc[iCC].prob_prior = p;

        // Add result to column vector
        col_prob_prior.push_back(src->cc[iCC].prob_prior);

      } // endfor: looped over all counterpart candidates

      // Update in-memory column
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_PROB_PRIOR_NAME,
                             col_prob_prior, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update column of in-memory FITS file.",
                       (Status)status);
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob_prior (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute posterior probabilities
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * @todo Improve zero counterpart density handling.
 *
 * Requires \n
 * CCElement::prob_prior (prior probabilities) \n
 * CCElement::psi (effective radius of 95% error ellipse) \n
 * Catalogue::m_r0 (average confusing source distance)
 *
 * Computes \n
 * CCElement::likrat (likelihood ratio) \n
 * CCElement::prob_post_single (single source posterior probabilities)
 *
 * The method updates the corresponding column in the in-memory FITS file.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_prob_post_single(Parameters *par, SourceInfo *src,
                                       Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob_post_single (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Allocate vector columns for in-memory FITS file update
      std::vector<double> col_likrat;
      std::vector<double> col_prob_post_single;

      // Loop over all counterpart candidates
      for (int iCC = 0; iCC < src->numSelect; ++iCC) {

        // Initialise results
        src->cc[iCC].likrat           = 0.0;
        src->cc[iCC].likrat_div       = 0;
        src->cc[iCC].prob_post_single = 0.0;

        // Compute log-likelihood ratio.
        // If Psi^2 > 2.996 lambda2^2 then the likelihood ratio diverges.
        // Depending on the compile option we either calculate the divergent
        // likelihood ratio or we set LR = 0.
        double psi2    = src->cc[iCC].psi * src->cc[iCC].psi;
        double lambda2 = (src->cc[iCC].rho > 0.0) ? 1.0 / (pi * src->cc[iCC].rho) :
                         src->ring_rad_max;
        if (psi2 > 0.0 && lambda2 > 0.0) {
          double beta  = 2.996 * lambda2 / psi2;
          double scale = 2.996 / psi2 - 1.0 / lambda2;
          double arg   = scale * src->cc[iCC].angsep * src->cc[iCC].angsep;
          #if ALLOW_LR_DIVERGENCE
          if (scale < 0.0)
            src->cc[iCC].likrat_div = 1;
          src->cc[iCC].likrat = log(beta) - arg;
          #else
          if (scale >= 0.0)
            src->cc[iCC].likrat = log(beta) - arg;
          #endif
        }

        // Compute posterior probability. Make this computation overflow
        // safe!
        //There are some special cases:
        // PROB_PRIOR >= 1 => PROB_POST = 1
        // PROB_PRIOR <= 0 => PROB_POST = 0
        // LR = 0          => PROB_POST = 0
        if (src->cc[iCC].prob_prior >= 1.0)
          src->cc[iCC].prob_post_single = 1.0;
        else if (src->cc[iCC].prob_prior <= 0.0)
          src->cc[iCC].prob_post_single = 0.0;
        else {
          double log_eta = log(src->cc[iCC].prob_prior) -
                           log(1.0 - src->cc[iCC].prob_prior);
          double log_arg = src->cc[iCC].likrat + log_eta;
          if (log_arg < 100.0) {
            double arg = exp(log_arg);
            src->cc[iCC].prob_post_single = (arg > 0.0) ? 1.0 / (1.0 + 1.0 / arg) :
                                            0.0;
          }
          else
            src->cc[iCC].prob_post_single = 1.0;
        }

        // Add results to column vectors
        col_likrat.push_back(src->cc[iCC].likrat);
        col_prob_post_single.push_back(src->cc[iCC].prob_post_single);

      } // endfor: looped over all counterpart candidates

      // Update in-memory columns
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_LR_NAME,
                             col_likrat, status);
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_PROB_POST_S_NAME,
                             col_prob_post_single, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update columns of in-memory FITS file.",
                       (Status)status);
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob_post_single (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute association probability
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * This method re-allocated the in memory catalogue.
 *
 * This method expects src->Refine counterparts.
 ******************************************************************************/
Status Catalogue::cid_prob(Parameters *par, SourceInfo *src, Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_prob (%d candidates)",
          src->numRefine);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numRefine < 1)
        continue;

      // Get column and formula
      std::string column  = "PROB";

      // Update in-memory catalogue
      status = cfits_update(m_memFile, par, src, src->numRefine, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update counterpart candidates"
                       " in-memory FITS catalogue.", (Status)status);
        continue;
      }

      // Evaluate PROB column
      status = cfits_eval_column(m_memFile, par, column, par->m_probMethod, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to evaluate expression <%s='%s'> in"
                       " formula.",
                       (Status)status, column.c_str(), par->m_probMethod.c_str());
        continue;
      }

      // Extract PROB vector
      std::vector<double> prob;
      status = cfits_get_col(m_memFile, par, column, prob, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to extract column <%s> from"
                       "in-memory FITS catalogue.",
                       (Status)status, column.c_str());
        continue;
      }

      // Allocate vector column for in-memory FITS file update
      std::vector<double> col_prob;

      // Set PROB information
      for (int iCC = 0; iCC < src->numRefine; ++iCC) {

        // Get probability in the range [0,1]
        double p = prob[iCC];
        if (p < 0.0)      p = 0.0;
        else if (p > 1.0) p = 1.0;

        // Save probability for each counterpart candidate
        src->cc[iCC].prob = p;

        // Add results to column vectors
        col_prob.push_back(src->cc[iCC].prob);

      } // endfor: looped over all counterpart candidates

      // Update in-memory column
      status = cfits_set_col(m_memFile, par, OUTCAT_COL_PROB_NAME,
                             col_prob, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to update column of in-memory FITS file.",
                       (Status)status);
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_prob (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute local counterpart density at the position of a given source
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Computes the local counterpart density at the position of a given source
 * by collecting the number of counterparts within a ring around the source
 * position. The ring is defined by a minimum and maximum radius, stored in
 * the class members Catalogue::m_ring_rad_min and Catalogue::m_ring_rad_max.
 *
 * The density is calculated by dividing the number of counterpart sources in
 * the ring by the solid angle of the ring. The density is stored in the class
 * member Catalogue::m_rho.
 *
 * If no counterpart was found in the ring the number of counterparts is set to
 * one. In this case the local counterpart density is to be considered as an
 * upper limit to the true counterpart density.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_local_density(Parameters *par, SourceInfo *src,
                                    Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_density_local (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numSelect < 1)
        continue;

      // Fall through if no source position is available
      if (!src->info->pos_valid)
        continue;

      // Compute solid angle of ring
      double omega = twopi * (cos(src->ring_rad_min * deg2rad) -
                              cos(src->ring_rad_max * deg2rad)) * rad2deg * rad2deg;

      // Case A: We consider FoMs
      if (par->m_FoM.length() > 0) {

        // Initialise number of counterparts in acceptance ring
        for (int iCC = 0; iCC < src->numSelect; ++iCC)
          src->cc[iCC].rho = 0.0;

        // Collect all counterparts that are within ring
        for (int iCC = 0; iCC < src->numSelect; ++iCC) {
          if (src->cc[iCC].angsep >= src->ring_rad_min &&
              src->cc[iCC].angsep <= src->ring_rad_max) {

            // Get FoM for actual counterpart
            double fom0 = src->cc[iCC].fom;

            // Increment numbers for all counterpart candidates that have smaller
            // FoM, i.e. FoM <= FoM_0. For all these counterparts the present
            // candidate has a larger FoM.
            for (int i = 0; i < src->numSelect; ++i) {
              if (src->cc[i].fom <= fom0)
                src->cc[i].rho += 1.0;
            }

          } // endif: counterpart candidate was in ring
        } // endfor: collected all counterparts

        // Assign local counterpart densities. Make sure that we have at least
        // one source. This provides an upper limit for the counterpart density
        // in case that we have found no source in the acceptance ring.
        if (omega > 0.0) {
          for (int iCC = 0; iCC < src->numSelect; ++iCC) {
            if (src->cc[iCC].rho < 1.0) src->cc[iCC].rho = 1.0;
            src->cc[iCC].rho /= omega;
          }
        }
        else {
          for (int iCC = 0; iCC < src->numSelect; ++iCC)
            src->cc[iCC].rho = 0.0;
        }

      } // endif: Case A

      // Case B: We do not consider FoMs
      else {

        // Compute number of counterparts within acceptance ring
        int num = 0;
        for (int iCC = 0; iCC < src->numSelect; ++iCC) {
          if (src->cc[iCC].angsep >= src->ring_rad_min &&
              src->cc[iCC].angsep <= src->ring_rad_max)
            num++;
        }

        // Make sure that we have at least one source. This provides an upper limit
        // for the counterpart density in case that we have found no source in the
        // acceptance ring
        if (num < 1) num = 1;

        // Compute local counterpart density
        double rho = (omega > 0.0) ? double(num) / omega : 0.0;

        // Assign local counterpart densities
        for (int iCC = 0; iCC < src->numSelect; ++iCC)
          src->cc[iCC].rho = rho;

      } // endelse: Case B

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_density_local (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Compute global counterpart density
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * This method expects src->numSelect counterparts.
 ******************************************************************************/
Status Catalogue::cid_global_density(Parameters *par, SourceInfo *src,
                                     Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_global_density (%d candidates)",
          src->numSelect);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Compute global allsky density
      double rho = double(m_cpt.numTotal) / 41252.961;

      // Assign density
      for (int iCC = 0; iCC < src->numSelect; ++iCC)
        src->cc[iCC].rho = rho;

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cid_global_density (status=%d)",
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Sort counterpart candidates
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * Sort by increasing probability and in case of equal probability) by 
 * decreasing angular separation
 ******************************************************************************/
Status Catalogue::cid_sort(Parameters *par, SourceInfo *src, int num,
                           Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_sort (%d candidates)", num);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (num < 1)
        continue;

      // Sort counterpart candidats (dirty brute force method, to be replaced
      // by more efficient method if needed ...)
      for (int iCC = 0; iCC < num; ++iCC) {
        double max_prob =   0.0;
        double min_sep  = 180.0;
        int    imax     = iCC;
        for (int jCC = iCC; jCC < num; ++jCC) {

          // Check if we found a better candidate
          if (src->cc[jCC].prob > max_prob) {        // Increasing probability
            imax     = jCC;
            max_prob = src->cc[jCC].prob;
            min_sep  = src->cc[jCC].angsep;
          }
          else if (src->cc[jCC].prob == max_prob) {  // Equal probability &
            if (src->cc[jCC].angsep < min_sep) {     // decreasing separation
              imax     = jCC;
              max_prob = src->cc[jCC].prob;
              min_sep  = src->cc[jCC].angsep;
            }
          }

        }
        if (iCC != imax) {
          CCElement swap = src->cc[iCC];
          src->cc[iCC]   = src->cc[imax];
          src->cc[imax]  = swap;
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
 * @brief Dump refine step counterpart candidates for source
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] src Pointer to source information.
 * @param[in] status Error status.
 *
 * This method expects src->numRefine counterparts.
 ******************************************************************************/
Status Catalogue::cid_dump(Parameters *par, SourceInfo *src, Status status) {

    // Declare local variables

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cid_dump (%d candidates)",
          src->numRefine);

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no counterpart candidates
      if (src->numRefine < 1)
        continue;

      // Loop over counterpart candidates
      for (int iCC = 0; iCC < src->numRefine; ++iCC) {

        // Get index of candidate in counterpart catalogue
        int iCpt = src->cc[iCC].index;

        // Get pointer to counterpart object
        ObjectInfo *cpt = &(m_cpt.object[iCpt]);

        // Normal log level
        if (par->logNormal()) {
          if (cpt->pos_valid) {
            if (src->cc[iCC].likrat_div) {
              Log(Log_2, "  Cpt%5d[P=%3.0f%%]S=%6.2f' PA=%4.0f: %20s"SRC_FORMAT,
                  iCC+1,
                  src->cc[iCC].prob_post_single*100.0,
                  src->cc[iCC].angsep*60.0,
                  src->cc[iCC].posang,
                  cpt->name.c_str(),
                  cpt->pos_eq_ra, cpt->pos_eq_dec,
                  cpt->pos_err_maj, cpt->pos_err_min, cpt->pos_err_ang);
            }
            else {
              Log(Log_2, "  Cpt%5d P=%3.0f%% S=%6.2f' PA=%4.0f: %20s"SRC_FORMAT,
                  iCC+1,
                  src->cc[iCC].prob_post_single*100.0,
                  src->cc[iCC].angsep*60.0,
                  src->cc[iCC].posang,
                  cpt->name.c_str(),
                  cpt->pos_eq_ra, cpt->pos_eq_dec,
                  cpt->pos_err_maj, cpt->pos_err_min, cpt->pos_err_ang);
            }
          }
          else {
            Log(Log_2, "  Cpt%5d P=%3.0f%% .................: %20s"
                " No position information found",
                iCC+1,
                src->cc[iCC].prob*100.0,
                cpt->name.c_str());
          }
        }

        // Verbose log level
        if (par->logVerbose()) {
          if (cpt->pos_valid) {
            Log(Log_2, "    Angular separation ............: %7.3f arcmin",
                src->cc[iCC].angsep*60.0);
            Log(Log_2, "    Effective 95%% error radius ....: %7.3f arcmin",
                src->cc[iCC].psi*60.0);
            Log(Log_2, "    Angular separation probability : %7.3f %%"
                " (dP/dr=%11.4e)",
                src->cc[iCC].prob_pos*100.0, src->cc[iCC].pdf_pos);
            Log(Log_2, "    Chance coincidence probability : %7.3f %%"
                " (dP/dr=%11.4e, mu=%.3f)",
                src->cc[iCC].prob_chance*100.0, src->cc[iCC].pdf_chance,
                src->cc[iCC].mu);
            Log(Log_2, "    Prior association probability .: %7.3f %%",
                src->cc[iCC].prob_prior*100.0);
            Log(Log_2, "    Posterior association prob. ...: %7.3f %%",
                src->cc[iCC].prob_post_single*100.0);
            if (src->cc[iCC].likrat_div) {
              Log(Log_2, "    Likelihood ratio ..............: %.3e (diverged)",
                  src->cc[iCC].likrat);
            }
            else {
              Log(Log_2, "    Likelihood ratio ..............: %.3f",
                  src->cc[iCC].likrat);
            }
          }
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
