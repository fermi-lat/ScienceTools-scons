/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.32  2008/03/26 13:37:10  jurgen
Generalize probability calculation and implement Bayesian method

Revision 1.31  2008/03/21 16:46:14  jurgen
Remove double log

Revision 1.30  2008/03/21 16:42:56  jurgen
Update documentation

Revision 1.29  2008/03/21 15:27:03  jurgen
Estimate number of false associations

Revision 1.28  2008/03/21 09:10:12  jurgen
Enhance code documentation.

Revision 1.27  2008/03/20 21:56:26  jurgen
implement local counterpart density

Revision 1.26  2008/03/20 12:17:44  jurgen
Invert _RA/_DE and RA/DE column name search

Revision 1.25  2008/02/23 10:36:57  jurgen
remove redundant catalogAccess header inclusion

Revision 1.24  2007/12/06 16:42:15  jurgen
Add RA/DEC and PosErr generic names

Revision 1.23  2007/11/30 16:19:26  jurgen
Correct version number and add RAdeg/DEdeg columns

Revision 1.22  2007/11/08 14:42:11  jurgen
Handle error circles (e.g. 3EG catalogue)

Revision 1.21  2007/10/11 13:20:54  jurgen
Correctly remove FITS special function columns

Revision 1.20  2007/10/10 15:39:12  jurgen
Introduce handling of special functions 'gammln', 'erf', and 'erfc'

Revision 1.19  2007/10/09 16:46:23  jurgen
Write counterpart catalogue reference (row) to output catalogue

Revision 1.18  2007/10/09 08:17:40  jurgen
Correctly interpret positional errors and correctly evaluate PROB_POS
as likelihood

Revision 1.17  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

Revision 1.16  2007/09/21 20:27:14  jurgen
Correct cfits_collect bug (unstable row selection)

Revision 1.15  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

Revision 1.14  2007/09/21 12:49:10  jurgen
Enhance log-file output and chatter level

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
/**
 * @file Catalogue.cxx
 * @brief Implements methods of Catalogue class.
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


/* Globals __________________________________________________________________ */


/* Constants ________________________________________________________________ */


/* Type defintions __________________________________________________________ */


/* Private Prototypes _______________________________________________________ */
std::string find(std::vector <std::string> &arg, std::string match);
Status      get_info(Parameters *par, InCatalogue *in, Status status);
Status      get_id_info(Parameters *par, InCatalogue *in, 
                        std::vector <std::string> &qtyNames,
                        std::vector <std::string> &qtyUCDs,
                        Status status);
Status      get_pos_info(Parameters *par, InCatalogue *in,
                         std::vector <std::string> &qtyNames,
                         std::vector <std::string> &qtyUCDs,
                         Status status);
Status      get_pos_error_info(Parameters *par, InCatalogue *in,
                               std::vector <std::string> &qtyNames,
                               Status status);
void        set_info(Parameters *par, InCatalogue *in, int &i, ObjectInfo *ptr,
                     double &posErr);


/*============================================================================*/
/*                              Private functions                             */
/*============================================================================*/

/**************************************************************************//**
 * @brief Convert string to upper case
 *
 * @param[in] arg String to convert into upper case.
 ******************************************************************************/
std::string upper(std::string arg) {

  // Copy argument
  std::string result = arg;

  // Convert to upper case
  std::transform(result.begin(), result.end(), result.begin(),
                 (int(*)(int)) std::toupper);

  // Return result
  return result;

}


/**************************************************************************//**
 * @brief Returns the shortest string that matches string
 *
 * @param[in] arg Vector of strings to be compated to match.
 * @param[in] match String to be matched.
 *
 * From a list of strings, returns the string that matches the pattern specified
 * by 'match'. If more than a single match exists, the shortest of all matches
 * is returned.
 ******************************************************************************/
std::string find(std::vector <std::string> &arg, std::string match) {

  // Initialise result
  std::string result;
  int         length = 0;

  // Convert match to upper case
  match = upper(match);

  // Loop over all vector elements
  for (int i = 0; i < (int)arg.size(); ++i) {
    if (upper(arg[i]).find(match, 0) != std::string::npos) {
      int this_length = arg[i].length();
      if (length == 0) {
        length = this_length;
        result = arg[i];
      }
      else if (this_length < length) {
        length = this_length;
        result = arg[i];
      }
    }
  }

  // Make sure that result string is empty if we found nothing
  if (length == 0)
    result.clear();

  // Return result
  return result;

}


/**************************************************************************//**
 * @brief Get column information
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 ******************************************************************************/
Status get_info(Parameters *par, InCatalogue *in, Status status) {

    // Declare local variables
    int                       numKeys;
    int                       numUCDs;
    std::vector <std::string> qtyNames;
    std::vector <std::string> qtyUCDs;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: get_info");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Extract keys and UCDs from catalogue. Stop if their number does not
      // correspond
      numKeys = in->cat.getQuantityNames(&qtyNames);
      numUCDs = in->cat.getQuantityUCDs(&qtyUCDs);
      if (numKeys != numUCDs) {
        if (par->logTerse())
          Log(Error_2, "%d : Mismatch between number of keys (%d) and UCDs (%d)",
              status, numKeys, numUCDs);
        continue;
      }

      // Get source ID column
      status = get_id_info(par, in, qtyNames, qtyUCDs, status);
      if (status == STATUS_CAT_NO_ID) {
        status = STATUS_OK;
        in->col_id.clear();
      }

      // Get position columns
      status = get_pos_info(par, in, qtyNames, qtyUCDs, status);
      if (status == STATUS_CAT_NO_POS) {
        status = STATUS_OK;
        in->col_ra.clear();
        in->col_dec.clear();
      }

      // Get position error columns
      status = get_pos_error_info(par, in, qtyNames, status);
      if (status == STATUS_CAT_NO_POS_ERROR) {
        status         = STATUS_OK;
        in->col_e_type = NoError;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: get_info (status=%d)", 
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Get source name information
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 *
 * The following ranked criteria are applied to determine the name of the
 * catalogue column that contains the source name:
 * 1) Search for column with UCD 'ID_MAIN'
 * 2) Search for column with names contained in search string (NAME, ID)
 ******************************************************************************/
Status get_id_info(Parameters *par, InCatalogue *in,
                   std::vector <std::string> &qtyNames,
                   std::vector <std::string> &qtyUCDs,
                   Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: get_id_info");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Initialise status to 'not found'
      status = STATUS_CAT_NO_ID;

      // Search for first ID in UCDs
      for (int i = 0; i < (int)qtyUCDs.size(); ++i) {
        if (qtyUCDs[i].find("ID_MAIN", 0) != std::string::npos) {
          in->col_id = qtyNames[i];
          status     = STATUS_OK;
          break;
        }
      }
      if (status == STATUS_OK)
        continue;

      // Search for ID in keys
      for (int k = 0; search_id[k] != "stop"; ++k) {
        std::string match  = find(qtyNames, upper(search_id[k]));
        if (match.length() > 0) {
          in->col_id = match;
          status     = STATUS_OK;
          break;
        }
      }
      if (status == STATUS_OK)
        continue;

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: get_id_info (status=%d)", 
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Get source position information
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 *
 * The following ranked criteria are applied to determine the name of the
 * catalogue columns that contain the source position:
 * 1) Search for columns with UCDs 'POS_EQ_RA_MAIN' & 'POS_EQ_DEC_MAIN'
 * 2) Search for columns with names 'RAdeg' & 'DEdeg'
 * 3) Search for columns with names '_RAJ2000' & '_DEJ2000'
 * 4) Search for columns with names 'RAJ2000' & 'DEJ2000'
 * 5) Search for columns with names 'RA' & 'DEC'
 ******************************************************************************/
Status get_pos_info(Parameters *par, InCatalogue *in,
                    std::vector <std::string> &qtyNames,
                    std::vector <std::string> &qtyUCDs,
                    Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: get_pos_info");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Initialise status to 'not found'
      status = STATUS_CAT_NO_POS;
      in->col_ra.clear();
      in->col_dec.clear();

      // Search for first RA/Dec position in UCDs
      for (int i = 0; i < (int)qtyUCDs.size(); ++i) {
        if (qtyUCDs[i].find("POS_EQ_RA_MAIN", 0) != std::string::npos) {
          in->col_ra = qtyNames[i];
          break;
        }
      }
      for (int i = 0; i < (int)qtyUCDs.size(); ++i) {
        if (qtyUCDs[i].find("POS_EQ_DEC_MAIN", 0) != std::string::npos) {
          in->col_dec = qtyNames[i];
          break;
        }
      }
      if ((in->col_ra.length() > 0) && (in->col_dec.length() > 0)) {
        status = STATUS_OK;
        continue;
      }

      // Search for RAdeg/DEdeg columns
      if ((find(qtyNames, "RAdeg").length() > 0) &&
          (find(qtyNames, "DEdeg").length() > 0)) {
        in->col_ra  = "RAdeg";
        in->col_dec = "DEdeg";
        status      = STATUS_OK;
        continue;
      }

      // Search for _RAJ2000/_DEJ2000 columns
      if ((find(qtyNames, "_RAJ2000").length() > 0) &&
          (find(qtyNames, "_DEJ2000").length() > 0)) {
        in->col_ra  = "_RAJ2000";
        in->col_dec = "_DEJ2000";
        status      = STATUS_OK;
        continue;
      }

      // Search for RAJ2000/DEJ2000 columns
      if ((find(qtyNames, "RAJ2000").length() > 0) &&
          (find(qtyNames, "DEJ2000").length() > 0)) {
        in->col_ra  = "RAJ2000";
        in->col_dec = "DEJ2000";
        status      = STATUS_OK;
        continue;
      }

      // Search for RA/DEC columns
      if ((find(qtyNames, "RA").length() > 0) &&
          (find(qtyNames, "DEC").length() > 0)) {
        in->col_ra  = "RA";
        in->col_dec = "DEC";
        status      = STATUS_OK;
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: get_pos_info (status=%d)", status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Get source position error information
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 *
 * The following ranked criteria are applied to determine the name of the
 * catalogue columns that contain the source position:
 * 1) Search for columns with names 'Conf_95_SemiMajor', 'Conf_95_SemiMinor' &
 *    'Conf_95_PosAng'
 * 2) Search for columns with names 'Conf_68_SemiMajor', 'Conf_68_SemiMinor' &
 *    'Conf_68_PosAng'
 * 3) Search for columns with names OUTCAT_COL_MAJERR_NAME, 
 *    OUTCAT_COL_MINERR_NAME & OUTCAT_COL_POSANGLE_NAME
 * 4) Search for columns with names 'e_RAdeg' & 'e_DEdeg'
 * 5) Search for columns with names 'e_RAJ2000' & 'e_DEJ2000'
 * 6) Search for columns with name 'theta95'
 * 7) Search for columns with name 'PosErr'
 ******************************************************************************/
Status get_pos_error_info(Parameters *par, InCatalogue *in,
                          std::vector <std::string> &qtyNames,
                          Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: get_pos_error_info");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Initialise status to 'not found'
      status = STATUS_CAT_NO_POS_ERROR;

      // Initialise error type to 'no error' and error scaling to unity
      in->col_e_type  = NoError;
      in->e_pos_scale = 1.0;

      // Search loop
      do {

        // Search for LAT catalogue names (95%)
        if ((find(qtyNames, "Conf_95_SemiMajor").length() > 0) &&
            (find(qtyNames, "Conf_95_SemiMinor").length() > 0) &&
            (find(qtyNames, "Conf_95_PosAng").length() > 0)) {
          in->col_e_maj    = "Conf_95_SemiMajor";
          in->col_e_min    = "Conf_95_SemiMinor";
          in->col_e_posang = "Conf_95_PosAng";
          in->col_e_type   = Ellipse;
          in->col_e_prob   = Prob_95;
          status           = STATUS_OK;
          continue;
        }

        // Search for LAT catalogue names (68%)
        if ((find(qtyNames, "Conf_68_SemiMajor").length() > 0) &&
            (find(qtyNames, "Conf_68_SemiMinor").length() > 0) &&
            (find(qtyNames, "Conf_68_PosAng").length() > 0)) {
          in->col_e_maj    = "Conf_68_SemiMajor";
          in->col_e_min    = "Conf_68_SemiMinor";
          in->col_e_posang = "Conf_68_PosAng";
          in->col_e_type   = Ellipse;
          in->col_e_prob   = Prob_68;
          status           = STATUS_OK;
          continue;
        }

        // Search for output catalogue columns
        if ((find(qtyNames, OUTCAT_COL_MAJERR_NAME).length() > 0) &&
            (find(qtyNames, OUTCAT_COL_MINERR_NAME).length() > 0) &&
            (find(qtyNames, OUTCAT_COL_POSANGLE_NAME).length() > 0)) {
          in->col_e_maj    = OUTCAT_COL_MAJERR_NAME;
          in->col_e_min    = OUTCAT_COL_MINERR_NAME;
          in->col_e_posang = OUTCAT_COL_POSANGLE_NAME;
          in->col_e_type   = Ellipse;
          in->col_e_prob   = Prob_95;
          status           = STATUS_OK;
          continue;
        }

        // Search for e_RAdeg/e_DEdeg columns
        if ((find(qtyNames, "e_RAdeg").length() > 0) &&
            (find(qtyNames, "e_DEdeg").length() > 0)) {
          in->col_e_ra   = "e_RAdeg";
          in->col_e_dec  = "e_DEdeg";
          in->col_e_type = RaDec;
          in->col_e_prob = Sigma_1;
          status         = STATUS_OK;
          continue;
        }

        // Search for e_RAJ2000/e_DEJ2000 columns
        if ((find(qtyNames, "e_RAJ2000").length() > 0) &&
            (find(qtyNames, "e_DEJ2000").length() > 0)) {
          in->col_e_ra   = "e_RAJ2000";
          in->col_e_dec  = "e_DEJ2000";
          in->col_e_type = RaDec;
          in->col_e_prob = Sigma_1;
          status         = STATUS_OK;
          continue;
        }

        // Search for theta95 column (3EG catalogue)
        if (find(qtyNames, "theta95").length() > 0) {
          in->col_e_maj  = "theta95";
          in->col_e_type = Radius;
          in->col_e_prob = Prob_95;
          status         = STATUS_OK;
          continue;
        }

        // Search for PosErr column
        if (find(qtyNames, "PosErr").length() > 0) {
          in->col_e_maj  = "PosErr";
          in->col_e_type = Radius;
          in->col_e_prob = Sigma_1;
          status         = STATUS_OK;
          continue;
        }

      } while (0); // End of search loop

      // Get error scaling (returned errors are 95% confidence errors)
      if (in->col_e_type != NoError) {
        switch (in->col_e_prob) {
        case Sigma_1:
          in->e_pos_scale = e_norm_1s;
          break;
        case Sigma_2:
          in->e_pos_scale = e_norm_2s;
          break;
        case Sigma_3:
          in->e_pos_scale = e_norm_3s;
          break;
        case Prob_68:
          in->e_pos_scale = e_norm_68;
          break;
        case Prob_95:
          in->e_pos_scale = e_norm_95;
          break;
        case Prob_99:
          in->e_pos_scale = e_norm_99;
          break;
        default:
          in->e_pos_scale = e_norm_95;
          break;
        }
        in->e_pos_scale /= e_norm_95;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: get_pos_error_info (status=%d)", status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Set information for source from catalogue
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] i Source number (starting from 0).
 * @param[in] ptr Pointer to source information structure.
 * @param[in] posErr Error radius if no error is found in catalogue.
 ******************************************************************************/
void set_info(Parameters *par, InCatalogue *in, int &i, ObjectInfo *ptr,
              double &posErr) {

    // Declare local variables
    double err_maj;
    double err_min;
    double err_ang;
    double e_RA;
    double e_DE;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: set_info");

    // Single loop for common exit point
    do {

      // Initialise source information
      ptr->pos_valid   = 0;        // Invalid position
      ptr->name.clear();
      ptr->pos_eq_ra   = 0.0;
      ptr->pos_eq_dec  = 0.0;
      ptr->pos_err_maj = posErr;
      ptr->pos_err_min = posErr;
      ptr->pos_err_ang = 0.0;

      // Set source name
      if (in->cat.getSValue(in->col_id, i, &(ptr->name)) != IS_OK)
        ptr->name = "no-name";

      // Set source position
      if ((in->cat.getNValue(in->col_ra,  i, &(ptr->pos_eq_ra))  == IS_OK) &&
          (in->cat.getNValue(in->col_dec, i, &(ptr->pos_eq_dec)) == IS_OK)) {

        // Set position validity flag
        ptr->pos_valid = 1;

        // Put Right Ascension in interval [0,2pi[
        ptr->pos_eq_ra = ptr->pos_eq_ra -
                         double(long(ptr->pos_eq_ra / 360.0) * 360.0);
        if (ptr->pos_eq_ra < 0.0)
          ptr->pos_eq_ra += 360.0;

      } // endif: source position found

      // Set source position error (type dependent)
      switch (in->col_e_type) {
      case NoError:
        ptr->pos_err_maj = posErr;
        ptr->pos_err_min = posErr;
        ptr->pos_err_ang = 0.0;
        break;
      case Radius:
        if (in->cat.getNValue(in->col_e_maj, i, &err_maj) == IS_OK) {
          ptr->pos_err_maj = err_maj * in->e_pos_scale;
          ptr->pos_err_min = err_maj * in->e_pos_scale;
          ptr->pos_err_ang = 0.0;
        }
        break;
      case Ellipse:
        if ((in->cat.getNValue(in->col_e_maj,    i, &err_maj) == IS_OK) &&
            (in->cat.getNValue(in->col_e_min,    i, &err_min) == IS_OK) &&
            (in->cat.getNValue(in->col_e_posang, i, &err_ang) == IS_OK)) {
          ptr->pos_err_maj = err_maj * in->e_pos_scale;
          ptr->pos_err_min = err_min * in->e_pos_scale;
          ptr->pos_err_ang = err_ang;
        }
        break;
      case RaDec:
        if ((in->cat.getNValue(in->col_e_ra, i, &e_RA) == IS_OK) &&
            (in->cat.getNValue(in->col_e_dec, i, &e_DE) == IS_OK)) {
          e_RA *= cos(ptr->pos_eq_dec*deg2rad);
          if (e_RA > e_DE) {           // Error ellipse along RA axis
            ptr->pos_err_maj = e_RA * in->e_pos_scale;
            ptr->pos_err_min = e_DE * in->e_pos_scale;
            ptr->pos_err_ang = 90.0;   // P.A. = 90.0 deg
          }
          else {                       // Error ellipse along DE axis
            ptr->pos_err_maj = e_DE * in->e_pos_scale;
            ptr->pos_err_min = e_RA * in->e_pos_scale;
            ptr->pos_err_ang = 0.0;    // P.A. = 0.0 deg
          }
        }
        break;
      }


     } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: set_info");

    // Return
    return;

}


/*============================================================================*/
/*                          Low-level catalogue methods                       */
/*============================================================================*/

/**************************************************************************//**
 * @brief Initialise class memory.
 *
 * The maximum number of counterparts is set by the constant c_maxCptLoad.
 * The filter step bounding box size is set by the constant c_filter_maxsep.
 ******************************************************************************/
void Catalogue::init_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {

      // Initialise source catalogue private members
      m_src.numLoad     = 0;
      m_src.numTotal    = 0;
      m_src.object      = NULL;
      m_src.col_e_type  = NoError;
      m_src.e_pos_scale = 1.0;
      m_src.inName.clear();
      m_src.catCode.clear();
      m_src.catURL.clear();
      m_src.catName.clear();
      m_src.catRef.clear();
      m_src.tableName.clear();
      m_src.tableRef.clear();
      m_src.col_id.clear();
      m_src.col_ra.clear();
      m_src.col_dec.clear();
      m_src.col_e_ra.clear();
      m_src.col_e_dec.clear();
      m_src.col_e_maj.clear();
      m_src.col_e_min.clear();

      // Initialise counterpart catalogue private members
      m_cpt.numLoad     = 0;
      m_cpt.numTotal    = 0;
      m_cpt.object      = NULL;
      m_cpt.col_e_type  = NoError;
      m_cpt.e_pos_scale = 1.0;
      m_cpt.inName.clear();
      m_cpt.catCode.clear();
      m_cpt.catURL.clear();
      m_cpt.catName.clear();
      m_cpt.catRef.clear();
      m_cpt.tableName.clear();
      m_cpt.tableRef.clear();
      m_cpt.col_id.clear();
      m_cpt.col_ra.clear();
      m_cpt.col_dec.clear();
      m_cpt.col_e_ra.clear();
      m_cpt.col_e_dec.clear();
      m_cpt.col_e_maj.clear();
      m_cpt.col_e_min.clear();

      // Intialise catalogue building parameters
      m_maxCptLoad    = c_maxCptLoad;
      m_fCptLoaded    = 0;
      m_memFile       = NULL;
      m_outFile       = NULL;

      // Initialise counterpart candidate working arrays
      m_numCC         = 0;
      m_cc            = NULL;
      m_filter_rad    = 0.0;
      m_ring_rad_min  = 0.0;
      m_ring_rad_max  = 0.0;
      m_omega         = 0.0;
      m_rho           = 0.0;

      // Initialise counterpart statistics
      m_num_Sel   = 0;
      m_cpt_stat  = NULL;
      m_num_assoc = 0;

      // Initialise output catalogue quantities
      m_num_src_Qty   = 0;
      m_num_cpt_Qty   = 0;

    } while (0); // End of main do-loop

    // Return
    return;

}


/**************************************************************************//**
 * @brief Free class memory.
 ******************************************************************************/
void Catalogue::free_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {

      // Free temporary memory
      if (m_src.object != NULL) delete [] m_src.object;
      if (m_cpt.object != NULL) delete [] m_cpt.object;
      if (m_cc         != NULL) delete [] m_cc;
      if (m_cpt_stat   != NULL) delete [] m_cpt_stat;

      // Initialise memory
      init_memory();

    } while (0); // End of main do-loop

    // Return
    return;

}


/*============================================================================*/
/*                         High-level catalogue methods                       */
/*============================================================================*/

/**************************************************************************//**
 * @brief Get descriptor for input catalogue
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] catName Catalogue name.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 ******************************************************************************/
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

      // Get information
      status = get_info(par, in, status);
      if (status != STATUS_OK)
        continue;

      // Dump catalogue descriptor (optionally)
      if (par->logTerse()) {
        status = dump_descriptor(par, in, status);
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


/**************************************************************************//**
 * @brief Get data for input catalogue
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] posErr Error radius in case that information is missing in catalogue.
 * @param[in] status Error status.
 ******************************************************************************/
Status Catalogue::get_input_catalogue(Parameters *par, InCatalogue *in,
                                      double posErr, Status status) {

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
      int caterr = in->cat.import(in->inName);
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
      ObjectInfo *ptr = in->object;
      for (int i = 0; i < in->numLoad; i++, ptr++) {

        // Set source information
        set_info(par, in, i, ptr, posErr);

        // Assign source name
        ptr->name = cid_assign_src_name(ptr->name, i);

      } // endfor: looped over all objects

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::get_input_catalogue (status=%d)", 
          status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Dump catalogue descriptor
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] in Pointer to input catalogue.
 * @param[in] status Error status.
 ******************************************************************************/
Status Catalogue::dump_descriptor(Parameters *par, InCatalogue *in, 
                                  Status status) {

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

      // Dump header
      Log(Log_2, "");
      Log(Log_2, "Catalogue descriptor:");
      Log(Log_2, "=====================");

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
      Log(Log_2, " Source ID column key .............: <%s>",
          in->col_id.c_str());
      Log(Log_2, " Position column keys .............: <%s> <%s>",
          in->col_ra.c_str(), in->col_dec.c_str());
      switch (in->col_e_type) {
      case Radius:
        Log(Log_2, " Position error column keys .......: <%s>",
            in->col_e_maj.c_str());
        break;
      case Ellipse:
        Log(Log_2, " Position error column keys .......: <%s> <%s> <%s>",
            in->col_e_maj.c_str(), in->col_e_min.c_str(), in->col_e_posang.c_str());
        break;
      case RaDec:
        Log(Log_2, " Position error column keys .......: <%s> <%s>",
            in->col_e_ra.c_str(), in->col_e_dec.c_str());
        break;
      case NoError:
      default:
        Log(Log_2, " Position error column keys .......: "
                   "no error information found");
        break;
      }
      switch (in->col_e_prob) {
      case Sigma_1:
        Log(Log_2, " Position error unit ..............: "
                   "1 sigma (68.269%%) (scale=%7.5f)", in->e_pos_scale);
        break;
      case Sigma_2:
        Log(Log_2, " Position error unit ..............: "
                   "2 sigma (95.450%%) (scale=%7.5f)", in->e_pos_scale);
        break;
      case Sigma_3:
        Log(Log_2, " Position error unit ..............: "
                   "3 sigma (99.730%%) (scale=%7.5f)", in->e_pos_scale);
        break;
      case Prob_68:
        Log(Log_2, " Position error unit ..............: 68%% (scale=%7.5f)",
            in->e_pos_scale);
        break;
      case Prob_95:
        Log(Log_2, " Position error unit ..............: 95%% (scale=%7.5f)",
            in->e_pos_scale);
        break;
      case Prob_99:
        Log(Log_2, " Position error unit ..............: 99%% (scale=%7.5f)",
            in->e_pos_scale);
        break;
      default:
        Log(Log_2, " Position error unit (default) ....: 95%% (scale=%7.5f)",
            in->e_pos_scale);
        break;
      }

      // Dump information about catalogue quantitites
      if (par->logVerbose()) {
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
      } // endif: verbose level

    } while (0); // End of main do-loop

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Dump counterpart identification results
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] status Error status.
 *
 * The results are stored in
 * the array m_cpt_stat[m_src.numLoad * (m_num_sel+1)]
 * the vector m_src_cpts[m_src.numLoad] and
 * the vector m_cpt_names[m_src.numLoad].
 * m_cpt_stat contains for each source a vector that traces the number of
 * counterparts that survive a given step. The first elements is the number
 * of counterpart candidates that came out of the filter step. The following
 * elements are the number of counterpart that survived the various selection
 * criteria. m_src_cpts contains for each source the number of counterparts
 * that survived the refine step. The names of these counterparts and their
 * associated probabilities are stored in the vector m_cpt_names.
 ******************************************************************************/
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
      Log(Log_2, "Counterpart association results:");
      Log(Log_2, "================================");

      // Build header
      char add[256];
      char select[256] = "";
      for (int iSel = 0; iSel < m_num_Sel; ++iSel) {
        sprintf(add, " Sel%2.2d", iSel+1);
        strcat(select, add);
      }
      sprintf(add, " Refine", iSel+1);
      strcat(select, add);

      // Dump header
      Log(Log_2, "                                      Filter%s", select);

      // Loop over all sources
      int n_src_assoc = 0;
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc) {

        // Get pointer to source object
        src = &(m_src.object[iSrc]);

        // Build selection string
        sprintf(select, " %6d", m_cpt_stat[iSrc*(m_num_Sel+1)]);
        for (int iSel = 0; iSel < m_num_Sel; ++iSel) {
          sprintf(add, " %5d", m_cpt_stat[iSrc*(m_num_Sel+1) + iSel+1]);
          strcat(select, add);
        }
        sprintf(add, " %6d", m_src_cpts[iSrc]);
        strcat(select, add);

        // Dump information
        Log(Log_2, " Source %5d %18s ..: %s %s",
            iSrc+1, src->name.c_str(), select, m_cpt_names[iSrc].c_str());

        // Collect number of associated sources
        if (m_src_cpts[iSrc] > 0)
          n_src_assoc++;

      } // endfor: looped over all sources


      // Dump summary
      Log(Log_2, "");
      Log(Log_2, "Counterpart association summary:");
      Log(Log_2, "================================");
      Log(Log_2, " Number of sources ................: %10d", m_src.numLoad);
      Log(Log_2, " Number of associated sources .....: %10d", n_src_assoc);
      Log(Log_2, " Number of associations ...........: %10d", m_num_assoc);
      //Log(Log_2, " Estimated number false assoc's ...: %10d", n_false);
      //Log(Log_2, " Counterparts within 95%% ellipses .: %10d", m_num_ellipse);
      //Log(Log_2, "   Expected chance counterparts ...: %10.3f"
      //           " (%.2f%% of all counterparts within 95%% ellipses)",
      //           m_tot_lambda, f_false*100.0);

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::dump_results (status=%d)", status);

    // Return status
    return status;

}


/**************************************************************************//**
 * @brief Build counterpart catalogue
 *
 * @param[in] par Pointer to gtsrcid parameters.
 * @param[in] status Error status.
 *
 * Main driver method that performs counterpart associations.
 *
 * \verbatim
 * build
 *   |
 *   +-- get_input_descriptor (get source catalogue input descriptior)
 *   |
 *   +-- get_input_descriptor (get counterpart catalogue input descriptior)
 *   |
 *   +-- cfits_create (create output catalogue)
 *   |
 *   +-- get_input_catalogue (get source catalogue)
 *   |
 *   N-- cid_get (get counterpart candidates for each source)
 *   |   |
 *   |   +-- cid_filter (filter step)
 *   |   |   |
 *   |   |   +-- get_input_catalogue (get counterpart catalogue)
 *   |   |
 *   |   +-- cid_refine (refine step)
 *   |   |   |
 *   |   |   +-- cfits_clear (clear in-memory catalogue)
 *   |   |   |
 *   |   |   +-- cfits_add (add quantities to in-memory catalogue)
 *   |   |   |
 *   |   |   +-- cfits_eval (evaluate quantities in in-memory catalogue)
 *   |   |   |
 *   |   |   +-- cid_prob_pos (compute PROB_POS & PDF_POS)
 *   |   |   |
 *   |   |   +-- cid_prob_prior (compute PROB_PRIOR)
 *   |   |   |
 *   |   |   +-- cid_select (select counterparts)
 *   |   |   |   |
 *   |   |   |   +-- cfits_select (select output catalogue entries)
 *   |   |   |
 *   |   |   +-- cid_prob_chance (compute PROB_CHANCE & PDF_CHANCE)
 *   |   |   |
 *   |   |   +-- cid_prob_post (compute PROB_POST)
 *   |   |   |
 *   |   |   +-- cid_prob (compute PROB)
 *   |   |   |
 *   |   |   +-- cid_sort (sort counterpart candidates)
 *   |   |
 *   |   +-- cfits_add (add candidates to output catalogue)
 *   |
 *   +-- cfits_collect (collect counterpart results)
 *   |
 *   +-- cfits_eval (evaluate output catalogue quantities)
 *   |
 *   +-- cfits_set_pars (set run parameter keywords)
 *   |
 *   +-- cfits_save (save output catalogue)
 *   |
 *   +-- dump_results (dump results)
 * \endverbatim
 ******************************************************************************/
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
      status = get_input_catalogue(par, &m_src, par->m_srcPosError, status);
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

      // Stop if the source catalogue is empty
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

      // Determine number of quantity selection criteria
      m_num_Sel = par->m_select.size();

      // Set vectors dimensions
      m_src_cpts  = std::vector<int>(m_src.numLoad);
      m_cpt_names = std::vector<std::string>(m_src.numLoad);

      // Allocate selection statistics
      m_cpt_stat = new int[m_src.numLoad*(m_num_Sel+1)];
      if (m_cpt_stat == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc) {
        for (int iSel = 0; iSel <= m_num_Sel; ++iSel)
          m_cpt_stat[iSrc*(m_num_Sel+1) + iSel] = 0;
      }

      // Loop over all sources
      for (iSrc = 0; iSrc < m_src.numLoad; iSrc++) {

        // Get counterpart candidates for the source
        status = cid_get(par, iSrc, status);
        if (status != STATUS_OK)
          break;

        // Store the number of counterpart candidates
        m_src_cpts[iSrc] = m_numCC;

        // Sum the total number of associations
        m_num_assoc += m_numCC;

      } // endfor: looped over all sources
      if (status != STATUS_OK)
        continue;

      // Close in-memory catalogue
      status = cfits_save(m_memFile, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to close in-memory catalogue.",
              (Status)status);
        continue;
      }

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

      // Write parameters as keywords to catalogue
      status = cfits_set_pars(m_outFile, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write parameters to output catalogue.",
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
