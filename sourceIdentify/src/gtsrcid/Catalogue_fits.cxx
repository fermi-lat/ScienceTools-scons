/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.10  2007/10/10 15:39:12  jurgen
Introduce handling of special functions 'gammln', 'erf', and 'erfc'

Revision 1.9  2007/10/09 16:46:23  jurgen
Write counterpart catalogue reference (row) to output catalogue

Revision 1.8  2007/10/09 08:17:40  jurgen
Correctly interpret positional errors and correctly evaluate PROB_POS
as likelihood

Revision 1.7  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

Revision 1.6  2007/10/03 09:06:08  jurgen
Add chance coincidence probability PROB_CHANCE

Revision 1.5  2007/10/02 21:48:45  jurgen
Add PROB_ANGSEP, PROB_ADD and ANGSEP generic columns to FITS output file

Revision 1.4  2007/09/21 20:27:14  jurgen
Correct cfits_collect bug (unstable row selection)

Revision 1.3  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

Revision 1.2  2007/09/21 12:49:10  jurgen
Enhance log-file output and chatter level

Revision 1.1  2006/02/03 12:09:53  jurgen
New file that contains routines that formerly existed in the file
Catalogue.cxx. The routines have also been renamed and preceeded
by "cfits_". These routines are handling the output catalogue
creation and allow in memory catalogues and FITS disk catalogues.

------------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"
#include "Catalogue.h"
#include "Log.h"
#include "src/quantity.h"


/* Definitions ______________________________________________________________ */


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {
using namespace catalogAccess;


/* Type defintions __________________________________________________________ */
typedef struct {                          // Special functions
  std::vector <std::string> fct;            // Function names
  std::vector <std::string> arg;            // Function arguments
  std::vector <std::string> colname_res;    // Result column names
  std::vector <std::string> colname_arg;    // Argument column names
} SpecialFcts;


/* Globals __________________________________________________________________ */
int g_col_special = 1;


/* Special function prototypes ______________________________________________ */
std::vector<double> funct_gammln(std::vector<double> arg);
std::vector<double> funct_erf(std::vector<double> arg);
std::vector<double> funct_erfc(std::vector<double> arg);

/* Private Prototypes _______________________________________________________ */
int set_fits_col_format(catalogAccess::Quantity *desc, std::string *format);
int fits_tform_binary(int typecode, long repeat, long width, 
                      std::string *format);
Status extract_next_special_function(std::string formula,
                                     std::string &new_formula,
                                     SpecialFcts &fcts);


/*============================================================================*/
/*                              Special functions                             */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                                funct_gammln                                */
/* -------------------------------------------------------------------------- */
/* Private method: logarithm of gamma function                                */
/* -------------------------------------------------------------------------- */
/* Implemented from Numerical Recipes, V2.08                                  */
/*----------------------------------------------------------------------------*/
std::vector<double> funct_gammln(std::vector<double> arg) {

    // Get vector dimension
    int n = (int)arg.size();

    // Allocate result vector
    std::vector<double> res(n);

    // Evaluate function
    for (int i = 0; i < n; ++i) {
      double x   = arg[i];
      double y   = x;
      double tmp = x + 5.5;
      tmp -= (x+0.5)*log(tmp);
      double ser = 1.000000000190015;
      ser += 76.18009172947146      / (++y);
      ser -= 86.50532032941677      / (++y);
      ser += 24.01409824083091      / (++y);
      ser -=  1.231739572450155     / (++y);
      ser +=  0.1208650973866179e-2 / (++y);
      ser -=  0.5395239384953e-5    / (++y);
      res[i] = -tmp+log(2.5066282746310005*ser/x);
    }

    // Return result
    return res;

}


/*----------------------------------------------------------------------------*/
/*                                funct_erf                                   */
/* -------------------------------------------------------------------------- */
/* Private method: error function                                             */
/*----------------------------------------------------------------------------*/
std::vector<double> funct_erf(std::vector<double> arg) {

    // Get vector dimension
    int n = (int)arg.size();

    // Allocate result vector
    std::vector<double> res(n);

    // Evaluate error function
    for (int i = 0; i < n; ++i) {
      res[i] = (arg[i] < 0.0) ? -nr_gammp(0.5, arg[i]*arg[i]) 
                              :  nr_gammp(0.5, arg[i]*arg[i]);
    }

    // Return result
    return res;

}


/*----------------------------------------------------------------------------*/
/*                                funct_erfc                                  */
/* -------------------------------------------------------------------------- */
/* Private method: error function                                             */
/*----------------------------------------------------------------------------*/
std::vector<double> funct_erfc(std::vector<double> arg) {

    // Get vector dimension
    int n = (int)arg.size();

    // Allocate result vector
    std::vector<double> res(n);

    // Evaluate error function
    for (int i = 0; i < n; ++i) {
      res[i] = (arg[i] < 0.0) ? 1.0 + nr_gammp(0.5, arg[i]*arg[i])
                              : nr_gammq(0.5, arg[i]*arg[i]);
    }

    // Return result
    return res;

}


/*============================================================================*/
/*                              Private functions                             */
/*============================================================================*/

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
/*                         extract_next_special_function                      */
/* -------------------------------------------------------------------------- */
/* Private method: extract next special function from formula                 */
/*----------------------------------------------------------------------------*/
Status extract_next_special_function(std::string formula,
                                     std::string &new_formula,
                                     SpecialFcts &fcts) {

    // Initialise return status
    Status status = STATUS_OK;

    // Single loop for common exit point
    do {

      // Initialise function name
      std::string fct = "";

      // Initialise function location
      std::string::size_type loc_min = std::string::npos;

      // Convert formula for search to upper case
      std::string u_formula = upper(formula);

      // Search for the first special function in formula
      for (int i = 0; fct_names[i] != "stop"; ++i) {
        std::string::size_type loc = u_formula.find(upper(fct_names[i])+"(", 0);
        if (loc < loc_min) {
          loc_min  = loc;
          fct      = fct_names[i];
        }
      }

      // Stop if no special function was found
      if (loc_min == std::string::npos) {
        status = STATUS_FCT_NOT_FOUND;
        continue;
      }

      // Search for closing parenthesis. We have to walk through the string
      // to find also additional opening parentheses
      std::string::size_type start = loc_min + fct.length() + 1;
      std::string::size_type stop  = start;
      int                    level = 1;
      while (level > 0 && stop < formula.length()) {

        // If character is an opening parenthesis then increase level
        if (formula[stop] == '(')
          level++;

        // If character is a closing parenthesis then decrease level
        if (formula[stop] == ')')
          level--;

        // If level is >0 then step to next character
        if (level > 0)
          stop++;

      }

      // Signal an error if no closing parentheses were found
      if (level > 0) {
        status = STATUS_FCT_NO_CLOSING;
        continue;
      }

      // Extract function argument
      std::string::size_type length = stop - start;
      std::string            arg    = formula.substr(start, length);

      // Build column name that will hold the result and the argument of the
      // special function and increment column counter
      char buffer[256];
      sprintf(buffer, "_fct_%d_res", g_col_special);
      std::string colname_res = buffer;
      sprintf(buffer, "_fct_%d_arg", g_col_special);
      std::string colname_arg = buffer;
      g_col_special++;

      // Substituing the special function in formula with the name of the 
      // result column
      new_formula = formula;
      length      = stop - loc_min + 1;
      new_formula.erase(loc_min, length);
      new_formula.insert(loc_min, colname_res);

      // Append function to function list
      fcts.fct.push_back(fct);
      fcts.arg.push_back(arg);
      fcts.colname_res.push_back(colname_res);
      fcts.colname_arg.push_back(colname_arg);

    } while (0); // End of main do-loop

    // Return status
    return status;

}


/*============================================================================*/
/*                   Low-level FITS catalogue handling methods                */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                           Catalogue::cfits_create                          */
/* -------------------------------------------------------------------------- */
/* Private method: create an empty FITS catalogue                             */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_create(fitsfile **fptr, char *filename, Parameters *par,
                               Status status) {

    // Declare local variables
    int                                    fstatus;
    int                                    col;
    int                                    num_col;
    long                                   iQty;
    long                                   numQty;
    long                                   len;
    long                                   maxLenType;
    long                                   maxLenUnit;
    long                                   maxLenForm;
    long                                   maxLenUcd;
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
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_create");

    // Initialise temporary memory pointers
    ttype   = NULL;
    tform   = NULL;
    tunit   = NULL;
    tbucd   = NULL;
    num_col = 0;

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if we have no filename
      if (filename == NULL)
        continue;

      // Dump header (optionally)
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Create new FITS catalogue:");
        Log(Log_2, "==========================");
        Log(Log_2, " Catalogue filename ...............: %s", filename);
      }

      // If clobber=1 we make sure that the output catalogue does not yet exist
      if (par->m_clobber)
        remove(filename);

      // Create empty FITS file.
      fstatus = fits_create_file(fptr, filename, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create FITS catalogue '%s'.",
              fstatus, filename);
        continue;
      }

      // Initialise column index to the OUTCAT_NUM_GENERIC generic columns
      num_col       = OUTCAT_NUM_GENERIC;
      m_num_src_Qty = 0;
      m_num_cpt_Qty = 0;

      // Add source catalogue columns
      numQty = m_src.cat.getQuantityNames(&qtyNames);
      numQty = m_src.cat.getQuantityUnits(&qtyUnits);
      numQty = m_src.cat.getQuantityUCDs(&qtyUCDs);
      numQty = m_src.cat.getQuantityDescription(&qtyDesc);
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
      numQty = m_cpt.cat.getQuantityNames(&qtyNames);
      numQty = m_cpt.cat.getQuantityUnits(&qtyUnits);
      numQty = m_cpt.cat.getQuantityUCDs(&qtyUCDs);
      numQty = m_cpt.cat.getQuantityDescription(&qtyDesc);
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
      ttype = new char*[num_col];
      tform = new char*[num_col];
      tunit = new char*[num_col];
      tbucd = new char*[num_col];
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
      col = OUTCAT_COL_PROB_P_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_PROB_P_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_PROB_P_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_PROB_P_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_PROB_P_UCD);
      col = OUTCAT_COL_PROB_A_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_PROB_A_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_PROB_A_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_PROB_A_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_PROB_A_UCD);
      col = OUTCAT_COL_PROB_C_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_PROB_C_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_PROB_C_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_PROB_C_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_PROB_C_UCD);
      col = OUTCAT_COL_ANGSEP_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_ANGSEP_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_ANGSEP_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_ANGSEP_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_ANGSEP_UCD);
      col = OUTCAT_COL_POSANG_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_POSANG_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_POSANG_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_POSANG_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_POSANG_UCD);
      col = OUTCAT_COL_REF_COLNUM - 1;
      sprintf(ttype[col], "%s", OUTCAT_COL_REF_NAME);
      sprintf(tform[col], "%s", OUTCAT_COL_REF_FORM);
      sprintf(tunit[col], "%s", OUTCAT_COL_REF_UNIT);
      sprintf(tbucd[col], "%s", OUTCAT_COL_REF_UCD);

      // Initialise column counter for additional columns
      col = OUTCAT_NUM_GENERIC;

      // Add source catalogue quantities
      for (iQty = 0; iQty < m_num_src_Qty; iQty++) {
        m_src_Qty_colnum.push_back(col+1);
        if ((m_src_Qty_ttype[iQty])[0] == '@')
          sprintf(ttype[col], "%s", m_src_Qty_ttype[iQty].c_str());
        else
          sprintf(ttype[col], "%s%s", par->m_srcCatPrefix.c_str(),
                                      m_src_Qty_ttype[iQty].c_str());
        sprintf(tform[col], m_src_Qty_tform[iQty].c_str());
        sprintf(tunit[col], m_src_Qty_tunit[iQty].c_str());
        sprintf(tbucd[col], m_src_Qty_tbucd[iQty].c_str());
        col++;
      }

      // Add counterpart catalogue quantities
      for (iQty = 0; iQty < m_num_cpt_Qty; iQty++) {
        m_cpt_Qty_colnum.push_back(col+1);
        if ((m_cpt_Qty_ttype[iQty])[0] == '@')
          sprintf(ttype[col], "%s", m_cpt_Qty_ttype[iQty].c_str());
        else
          sprintf(ttype[col], "%s%s", par->m_cptCatPrefix.c_str(),
                                      m_cpt_Qty_ttype[iQty].c_str());
        sprintf(tform[col], m_cpt_Qty_tform[iQty].c_str());
        sprintf(tunit[col], m_cpt_Qty_tunit[iQty].c_str());
        sprintf(tbucd[col], m_cpt_Qty_tbucd[iQty].c_str());
        col++;
      }

      // Dump catalogue information (optionally)
      if (par->logExplicit()) {
        maxLenType = 0;
        maxLenUnit = 0;
        maxLenForm = 0;
        maxLenUcd  = 0;
        for (col = 0; col < num_col; col++) {
          if ((len = strlen(ttype[col])) > maxLenType)
            maxLenType = len;
          if ((len = strlen(tunit[col])) > maxLenUnit)
            maxLenUnit = len;
          if ((len = strlen(tform[col])) > maxLenForm)
            maxLenForm = len;
          if ((len = strlen(tbucd[col])) > maxLenUcd)
            maxLenUcd = len;
        }
        Log(Log_2, " Number of catalogue columns ......: %d", num_col);
        for (col = 0; col < num_col; col++) {
          Log(Log_2, 
              "  Column %4d .....................: %*s [%*s] (%*s) <%*s>",
              col+1, 
              maxLenType, ttype[col], 
              maxLenUnit, tunit[col], 
              maxLenForm, tform[col], 
              maxLenUcd,  tbucd[col]);
        }
      }

      // Create empty binary table
      fstatus = fits_create_tbl(*fptr, BINARY_TBL, 0, num_col,
                                ttype, tform, tunit,
                                OUTCAT_EXT_NAME, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to create catalogue '%s'.",
              fstatus, filename);
        continue;
      }

      // Write UCD keywords
      for (col = 0; col < num_col; col++) {
        sprintf(keyname, "TBUCD%d", col+1);
        sprintf(comment, "UCD for field %3d", col+1);
        fstatus = fits_write_key(*fptr, TSTRING, keyname, tbucd[col],
                                 comment, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to write UCD keyword '%s' to"
                " catalogue '%s'.", fstatus, keyname, filename);
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
      Log(Log_0, " <== EXIT: Catalogue::cfits_create (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                           Catalogue::cfits_clear                           */
/* -------------------------------------------------------------------------- */
/* Private method: clear FITS table                                           */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_clear(fitsfile *fptr, Parameters *par, Status status) {

    // Declare local variables
    int  fstatus;
    long nactrows;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_clear");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of rows in table
      fstatus = fits_get_num_rows(fptr, &nactrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Stop if table is empty
      if (nactrows < 1)
        continue;

      // Delete rows in table
      fstatus = fits_delete_rows(fptr, 1, nactrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to delete %d rows of catalogue.",
              fstatus, nactrows);
        continue;
      }

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_clear (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                            Catalogue::cfits_add                            */
/* -------------------------------------------------------------------------- */
/* Private method: add counterpart candidates to FITS file                    */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_add(fitsfile *fptr, long iSrc, Parameters *par,
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
    long         *lptr;
    char        **cptr;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_add");

    // Initialise temporary memory pointers
    dptr  = NULL;
    lptr  = NULL;
    cptr  = NULL;
    nrows = 0;

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
      fstatus = fits_get_num_rows(fptr, &nactrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Define the rows that should be inserted
      firstrow = (long)nactrows;
      frow     = firstrow + 1;
      nrows    = (long)m_numCC;

      // Insert rows for the new counterpart candidates
      fstatus = fits_insert_rows(fptr, firstrow, nrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to add %d rows to catalogue.",
              fstatus, nrows);
        continue;
      }

      // Allocate memory to hold quantities
      dptr = new double[nrows];
      lptr = new long[nrows];
      cptr = new char*[nrows];
      if (dptr == NULL || lptr == NULL || cptr == NULL) {
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
        sprintf(cptr[row], "%s", m_cc[row].id.c_str());
      fstatus = fits_write_col_str(fptr, OUTCAT_COL_ID_COLNUM, 
                                   frow, 1, nrows, cptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart identifier to"
              " catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Right Ascention
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_eq_ra;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_RA_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart Right Ascension to"
              " catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Declination
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_eq_dec;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_DEC_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart Declination to"
              " catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Major Axis
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_maj;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_MAJERR_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse major"
              " axis to catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Minor Axis
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_min;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_MINERR_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse minor"
              " axis to catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Error Ellipse Position Angle
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].pos_err_ang;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_POSANGLE_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart error ellipse position"
              " angle to catalogue.", fstatus);
        continue;
      }

      // Add Counterpart Probability
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].prob;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_PROB_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write counterpart probability to"
              " catalogue.", fstatus);
        continue;
      }

      // Add angular separation probability
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].prob_pos;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_PROB_P_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write position probability to"
              " catalogue.", fstatus);
        continue;
      }

      // Add additional probability
      for (row = 0; row < nrows; row++) {
        double p = 1.0;
        for (int i = 0; i < (int)m_cc[row].prob_add.size(); ++i)
          p *= m_cc[row].prob_add[i];
        dptr[row] = p;
      }
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_PROB_A_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write additional probability to"
              " catalogue.", fstatus);
        continue;
      }

      // Add chance coincidence probability
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].prob_chance;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_PROB_C_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write chance coincidence probability to"
              " catalogue.", fstatus);
        continue;
      }

      // Add angular separation
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].angsep;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_ANGSEP_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write angular separation to"
              " catalogue.", fstatus);
        continue;
      }

      // Add position angle
      for (row = 0; row < nrows; row++)
        dptr[row] = m_cc[row].posang;
      fstatus = fits_write_col(fptr, TDOUBLE, OUTCAT_COL_POSANG_COLNUM,
                               frow, 1, nrows, dptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write position angle to"
              " catalogue.", fstatus);
        continue;
      }

      // Add reference
      for (row = 0; row < nrows; row++)
        lptr[row] = m_cc[row].index;
      fstatus = fits_write_col(fptr, TLONG, OUTCAT_COL_REF_COLNUM,
                               frow, 1, nrows, lptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write reference to"
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
          m_src.cat.getNValue(name, iSrc, &NValue);
          for (row = 0; row < nrows; row++)
            dptr[row] = NValue;
          fstatus = fits_write_col(fptr, TDOUBLE, colnum, frow, 1, nrows,
                                   dptr, &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write source catalogue data <%s>"
                  " (column %d) to catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

        // Add string quantities
        if (form.find("A", 0) != std::string::npos) {
          m_src.cat.getSValue(name, iSrc, &SValue);
          for (row = 0; row < nrows; row++)
            sprintf(cptr[row], "%s", SValue.c_str());
          fstatus = fits_write_col_str(fptr, colnum, frow, 1, nrows, cptr,
                                       &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write source catalogue data <%s>"
                  " (column %d) to catalogue.", 
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
            m_cpt.cat.getNValue(name, iCpt, &NValue);
            dptr[row] = NValue;
          }
          fstatus = fits_write_col(fptr, TDOUBLE, colnum, frow, 1, nrows,
                                   dptr, &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write counterpart catalogue data"
                  " <%s> (column %d) to catalogue.", 
                  fstatus, name.c_str(), colnum);
            break;
          }
        } // endif: added numerical quantities

        // Add string quantities
        if (form.find("A", 0) != std::string::npos) {
          for (row = 0; row < nrows; row++) {
            iCpt = m_cc[row].index;
            m_cpt.cat.getSValue(name, iCpt, &SValue);
            sprintf(cptr[row], "%s", SValue.c_str());
          }
          fstatus = fits_write_col_str(fptr, colnum, frow, 1, nrows, cptr,
                                       &fstatus);
          if (fstatus != 0) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to write counterpart catalogue data"
                  " <%s> (column %d) to catalogue.", 
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
    if (lptr != NULL) delete [] lptr;
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
      Log(Log_0, " <== EXIT: Catalogue::cfits_add (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                           Catalogue::cfits_eval                            */
/* -------------------------------------------------------------------------- */
/* Private method: evaluate catalogue quantities                              */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_eval(fitsfile *fptr, Parameters *par, int verbose,
                             Status status) {

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of new output catalogue quantities. Fall through if
      // there are no new output catalogue quantities
      int numQty = (int)par->m_outCatQtyName.size();
      if (numQty < 1)
        continue;

      // Dump header (optionally)
      if (verbose) {
        Log(Log_2, "");
        Log(Log_2, "Add new output catalogue quantities:");
        Log(Log_2, "====================================");
      }

      // Reset column counter
//      g_col_special = 1;

      // Add all new output catalogue quantities
      for (int iQty = 0; iQty < numQty; ++iQty) {

        // Get column and formula
        std::string column  = par->m_outCatQtyName[iQty];
        std::string formula = par->m_outCatQtyFormula[iQty];

        // Evaluate special expressions in formula
        status = cfits_eval_special_expression(fptr, par, column, formula, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate special expression <%s='%s'> in"
                " formula.",
                (Status)status, column.c_str(), formula.c_str());
          break;
        }

        // Evaluate regular expression in formula
        status = cfits_eval_regular_expression(fptr, par, column, formula, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate regular expression <%s='%s'> in"
                " formula.",
                (Status)status, column.c_str(), formula.c_str());
          break;
        }

        // Dump new output catalogue quantities information (optionally)
        if (verbose) {
          Log(Log_2, " New quantity .....................: %s = %s",
              column.c_str(), formula.c_str());
        }

      } // endfor: loop over all new output cataloge quantities
      if (status != STATUS_OK)
        continue;

      // Remove special function columns
      status = cfits_eval_clear(fptr, par, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to remove special function columns.",
              (Status)status);
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                         cfits_eval_special_expression                      */
/* -------------------------------------------------------------------------- */
/* Private method: evaluate special expressions in formula                    */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_eval_special_expression(fitsfile *fptr, Parameters *par,
                                                std::string column,
                                                std::string &formula,
                                                Status status) {

    // Declare local variables
    int         fstatus;
    int         n_args_before;
    int         n_args;
    SpecialFcts fcts;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval_special_expression");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Keep old formula for information
      std::string old_formula = formula;

      // Find all special functions in formula
      while (status == STATUS_OK)
        status = extract_next_special_function(formula, formula, fcts);
      if (status == STATUS_FCT_NOT_FOUND)
        status = STATUS_OK;
      if (status != STATUS_OK)
        continue;

      // Find all special functions in arguments
      do {

        // Determine number of arguments before search
        n_args_before = (int)fcts.arg.size();

        // Search for remaining special functions in arguments
        for (int i = 0; i < n_args_before; ++i) {
          status = extract_next_special_function(fcts.arg[i], fcts.arg[i], fcts);
          if (status != STATUS_OK && status != STATUS_FCT_NOT_FOUND)
            break;
        }
        if (status == STATUS_FCT_NOT_FOUND)
          status = STATUS_OK;
        if (status != STATUS_OK)
          break;

        // Determine number of arguments after search
        n_args = (int)fcts.arg.size();

      } while (n_args > n_args_before);
      if (status != STATUS_OK)
        continue;

      // Stop now if there are no special functions
      if ((int)fcts.arg.size() < 1)
        continue;

      // Optionally dump special functions
      if (par->logNormal()) {
        Log(Log_2, " Special function(s) in formula ...: %s = %s",
            column.c_str(),
            old_formula.c_str());
        Log(Log_2, "   Formula replaced by ............: %s = %s",
            column.c_str(),
            formula.c_str());
        for (int i = 0; i < (int)fcts.arg.size(); ++i) {
          Log(Log_2, "   Special function ...............: %s = %s(%s=%s)",
              fcts.colname_res[i].c_str(),
              fcts.fct[i].c_str(),
              fcts.colname_arg[i].c_str(),
              fcts.arg[i].c_str());
        }
      }

      // Evaluate all special functions (from latest to first)
      std::string column;
      std::string formula;
      for (int i = (int)fcts.arg.size()-1; i >= 0; --i) {

        // Evaluate argument
        std::string column  = fcts.colname_arg[i];
        std::string formula = fcts.arg[i];
        status = cfits_eval_regular_expression(fptr, par, column, formula, status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate regular expression <%s='%s'> in"
                " formula.",
                (Status)status, column.c_str(), formula.c_str());
          break;
        }

        // Evaluate function
        std::string fct        = fcts.fct[i];
        std::string column_res = fcts.colname_res[i];
        std::string column_arg = fcts.colname_arg[i];
        status = cfits_eval_special_function(fptr, par, fct, column_res, column_arg,
                                             status);
        if (status != STATUS_OK) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to evaluate special function <%s=%s('%s')> in"
                " formula.",
                (Status)status, column_res.c_str(), fcts.fct[i].c_str(),
                column_arg.c_str());
          break;
        }

      } // endfor: looped over special function evaluations

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval_special_expression"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                        cfits_eval_special_function                         */
/* -------------------------------------------------------------------------- */
/* Private method: evaluate special function                                  */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_eval_special_function(fitsfile *fptr, Parameters *par,
                                              std::string fct,
                                              std::string column_res,
                                              std::string column_arg,
                                              Status status) {

    // Declare local variables
    std::vector<double> arg;
    std::vector<double> res;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval_special_function");

    // Single loop for common exit point
    do {

      // Read argument column
      status = cfits_get_col(fptr, par, column_arg, arg, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read data from column <%s>.",
              (Status)status, column_arg.c_str());
        continue;
      }

      // Convert function name to upper case
      fct = upper(fct);

      // Evaluate special functions
      if (fct == "GAMMLN")              // gammln(arg)
        res = funct_gammln(arg);

      else if (fct == "ERF")            // erf(arg)
        res = funct_erf(arg);

      else if (fct == "ERFC")           // erfc(arg)
        res = funct_erfc(arg);

      else {                            // Invalid function
        status = STATUS_FCT_INVALID;
        if (par->logTerse())
          Log(Error_2, "%d : Unknown special function <%s>.",
              (Status)status, fct.c_str());
        continue;
      }

      // Write result column
      status = cfits_set_col(fptr, par, column_res, res, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write data to column <%s>.",
              (Status)status, column_res.c_str());
        continue;
      }

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval_special_function"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                        cfits_eval_regular_expression                       */
/* -------------------------------------------------------------------------- */
/* Private method: evaluate regular expression using cfitsio interface        */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_eval_regular_expression(fitsfile *fptr, Parameters *par,
                                                std::string column,
                                                std::string formula,
                                                Status status) {

    // Declare local variables
    int         fstatus;
    int         datatype;
    int         naxis;
    long        nelements;
    std::string tform;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval_regular_expression");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Test expression to determine the format of the new column
      fstatus = fits_test_expr(fptr,
                               (char*)formula.c_str(),
                               0, &datatype, &nelements, &naxis, NULL,
                               &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Warning_2, " Unable to evaluate expression <%s='%s'> for"
              " creating new output catalogue quantity (status=%d).",
              column.c_str(), formula.c_str(), fstatus);
        fstatus = 0;
        continue;
      }

      // Set column format
      if (nelements < 1) 
        nelements = 1;
      fits_tform_binary(datatype, nelements, 0, &tform);

      // Create new FITS column
      fstatus = fits_calculator(fptr,
                                (char*)formula.c_str(),
                                fptr,
                                (char*)column.c_str(),
                                (char*)tform.c_str(),
                                &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to evaluate expression <%s='%s'> in"
              " output catalogue.",
              fstatus, column.c_str(), formula.c_str());
        continue;
      }

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval_regular_expression"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                             cfits_eval_clear                               */
/* -------------------------------------------------------------------------- */
/* Private method: remove all columns that have been used for special         */
/*                 function evaluations                                       */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_eval_clear(fitsfile *fptr, Parameters *par, 
                                   Status status) {

    // Declare local variables
    int               fstatus;
    int               colnum;
    std::vector <int> cols;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval_clear");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Remove all function columns
      do {

        // Search for next match
        fstatus = fits_get_colnum(fptr, CASEINSEN, "_fct_#_*", &colnum, 
                                  &fstatus);
        if (fstatus == COL_NOT_FOUND) {
         fstatus = 0;
         break;
        }
        if (fstatus != 0 && fstatus != COL_NOT_UNIQUE) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine column names.", fstatus);
          break;
        }
        fstatus = 0;

        // Remove column
        fstatus = fits_delete_col(fptr, colnum, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to remove column %d.", fstatus, colnum);
          break;
        }

      } while (fstatus != 0);

      // Reset column counter
      g_col_special = 1;

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval_clear"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                          Catalogue::cfits_colval                           */
/* -------------------------------------------------------------------------- */
/* Private method: returns the values of a table column                       */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_colval(fitsfile *fptr, char *colname, Parameters *par,
                               std::vector<double> *val, Status status) {

    // Declare local variables
    int     fstatus;
    int     colnum;
    int     typecode;
    int     anynul;
    long    irow;
    long    icol;
    long    repeat;
    long    width;
    long    nactrows;
    long    nelements;
    double  prob;
    double *ptr;
    double *tmp_val;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_colval");

    // Initialise temporary memory pointers
    tmp_val = NULL;

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Initialise probability vector
      val->clear();

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if we have no column name
      if (colname == NULL)
        continue;

      // Determine number of rows in table
      fstatus = fits_get_num_rows(fptr, &nactrows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Fall through if table is empty
      if (nactrows < 1)
        continue;

      // Determine the column number
      fstatus = fits_get_colnum(fptr, CASEINSEN, colname, &colnum, &fstatus);
      if (fstatus == COL_NOT_UNIQUE) {
        if (par->logTerse())
          Log(Error_2, "%d : Probability column '%s' is not unique. Please"
              " specify unique column name.",
              fstatus, colname);
        continue;
      }
      else if (fstatus == COL_NOT_FOUND) {
        if (par->logTerse())
          Log(Error_2, "%d : Probability column '%s' not found in catalogue.",
              fstatus, colname);
        continue;
      }
      else if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine column number of probability"
              " column '%s'.", fstatus, colname);
        continue;
      }

      // Determine the type of the probability column
      fstatus = fits_get_coltype(fptr, colnum, &typecode, &repeat, &width,
                                 &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine type of probability column"
              " '%s'.", fstatus, colname);
        continue;
      }

      // Abort if column type is not one of the valid types
      if (typecode == TSTRING) {
        status = STATUS_CAT_BAD_PROB_COL;
        if (par->logTerse())
          Log(Error_2, "%d : Probability column '%s' of type 'STRING' is not"
              " allowed.", (Status)status, colname);
        continue;
      }
      else if (typecode == TCOMPLEX) {
        status = STATUS_CAT_BAD_PROB_COL;
        if (par->logTerse())
          Log(Error_2, "%d : Probability column '%s' of type 'COMPLEX' is not"
              " allowed.", (Status)status, colname);
        continue;
      }
      else if (typecode == TDBLCOMPLEX) {
        status = STATUS_CAT_BAD_PROB_COL;
        if (par->logTerse())
          Log(Error_2, "%d : Probability column '%s' of type 'DBLCOMPLEX' is"
              " not allowed.", (Status)status, colname);
        continue;
      }

      // Set the number of elements to be read
      nelements = repeat * nactrows;

      // Allocate temporary memory to hold the column data
      tmp_val = new double[nelements];
      if (tmp_val == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }

      // Read column data
      fstatus = fits_read_col(fptr, TDOUBLE, colnum, 1, 1, nelements,
                              NULL, tmp_val, &anynul, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read data from column %d.",
              fstatus, colnum);
        continue;
      }

      // Setup vector (multiply over vector column if it exists)
      ptr = tmp_val;
      for (irow = 0; irow < nactrows; irow++) {
        prob = 1.0;
        for (icol = 0; icol < repeat; icol++)
          prob *= *ptr++;
        val->push_back(prob);
      }

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Free temporary memory pointers
    if (tmp_val != NULL) delete [] tmp_val;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_colval"
          " (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                           Catalogue::cfits_select                          */
/* -------------------------------------------------------------------------- */
/* Private method: select catalogue entries                                   */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_select(fitsfile *fptr, Parameters *par, Status status) {

    // Declare local variables
    int  fstatus;
    long numBefore;
    long numAfter;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_select");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of output catalogue selection strings. Fall through
      // if there are no such strings
      m_num_Sel = par->m_select.size();
      if (m_num_Sel < 1)
        continue;

      // Dump header
      if (par->logNormal()) {
        Log(Log_2, "");
        Log(Log_2, "Select catalogue counterparts:");
        Log(Log_2, "==============================");
      }

      // Clear counterpart names
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc)
        m_cpt_names[iSrc].clear();

      // Allocate and initialise selection statistics. This is a table of size
      // [m_src.numLoad][numSel]
      m_cpt_stat = new int[m_src.numLoad*m_num_Sel];
      if (m_cpt_stat == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
         continue;
      }
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc) {
        for (int iSel = 0; iSel < m_num_Sel; ++iSel)
          m_cpt_stat[iSrc*(int)m_num_Sel + (int)iSel] = 0;
      }

      // Select catalogue entries
      for (int iSel = 0; iSel < m_num_Sel; ++iSel) {

        // Determine number of rows in table before selection
        fstatus = fits_get_num_rows(fptr, &numBefore, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of rows in"
                " catalogue.", fstatus);
          break;
        }

        // Perform selection
        fstatus = fits_select_rows(fptr, fptr,
                                   (char*)par->m_select[iSel].c_str(),
                                   &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Warning_2, " Unable to perform selection <%s> on the"
                " catalogue (status=%d).",
                par->m_select[iSel].c_str(), 
                fstatus);
          fstatus = 0;
          continue;
        }

        // Determine number of rows in table after selection
        fstatus = fits_get_num_rows(fptr, &numAfter, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of rows in"
                " catalogue.", fstatus);
          break;
        }

        // Determine number of counterparts for each source after selection
        if (numAfter > 0) {

          // Collect statistics
          std::vector<int> stat;
          status = cfits_collect(fptr, par, stat, status);
          if (status != STATUS_OK) {
            if (par->logTerse())
              Log(Error_2, "%d : Unable to collect statistics from catalogue.",
                           (Status)status);
             continue;
          }

          // Save statistics
          for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc)
            m_cpt_stat[iSrc*m_num_Sel + iSel] = stat[iSrc];

        } // endif: there were rows in catalogue

        // Dump selection information
        if (par->logNormal()) {
          Log(Log_2, " Selection ........................: %s",
              par->m_select[iSel].c_str());
          Log(Log_2, "   Number of deleted counterparts .: %d (%d => %d)",
              numBefore-numAfter, numBefore, numAfter);
        }

      } // endfor: looped over selection
      if (fstatus != 0)
        continue;

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_select (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                          Catalogue::cfits_collect                          */
/* -------------------------------------------------------------------------- */
/* Private method: collect counterpart identification statistics from FITS    */
/*                 table                                                      */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_collect(fitsfile *fptr, Parameters *par,
                                std::vector<int> &stat, Status status) {

    // Declare local variables
    std::vector<std::string> col_id;
    std::vector<std::string> col_name;
    std::vector<double>      col_prob;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_collect");

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Fall through if there are no sources
      if (m_src.numLoad < 1)
        continue;

      // Allocate and initialise results
      stat = std::vector<int>(m_src.numLoad);
      for (int iSrc = 0; iSrc < m_src.numLoad; ++iSrc) {
        m_cpt_names[iSrc].clear();
        stat[iSrc] = 0;
      }

      // Read ID column
      status = cfits_get_col_str(fptr, par, OUTCAT_COL_ID_NAME, col_id, status);
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read ID column from catalogue.",
                       (Status)status);
         continue;
      }

      // Read counterpart name column. Don't stop on error
      std::string cpt_name;
      if (m_cpt.col_id[0] == '@')
        cpt_name = m_cpt.col_id;
      else
        cpt_name = par->m_cptCatPrefix + m_cpt.col_id;
      status = cfits_get_col_str(fptr, par, cpt_name, col_name, status);
      if (status != STATUS_OK)
        status = STATUS_OK;

      // Read probability column. Don't stop on error
      status = cfits_get_col(fptr, par, OUTCAT_COL_PROB_NAME, col_prob, status);
      if (status != STATUS_OK)
        status = STATUS_OK;

      // Determine number of counterparts for each source
      for (int i = 0; i < (int)col_id.size(); ++i) {

        // Get source number. Note that we have to subtract 1 since the
        // sources index starts with 1
        std::string src_row = col_id[i].substr(5,3);
        int         iSrc    = atoi(src_row.c_str()) - 1;

        // Fall through if index is invalid
        if (iSrc < 0 || iSrc >= m_src.numLoad)
          continue;

        // Increment statistics vector
        stat[iSrc]++;

        // If there were already names then add a seperator
        if (m_cpt_names[iSrc].length() > 0)
          m_cpt_names[iSrc] += ", ";

        // If we have a name then add it now
        if (col_name.size() == col_id.size())
          m_cpt_names[iSrc] += cid_assign_src_name(col_name[i], iSrc);
        else
          m_cpt_names[iSrc] += "no-name";

        // If we have a probability than attach it now
        if (col_prob.size() == col_id.size()) {
          char buffer[256];
          sprintf(buffer, " (%.1f%%)", col_prob[i]*100.0);
          m_cpt_names[iSrc] += buffer;
        }

      } // endfor: looped over all counterparts

    } while (0); // End of main do-loop

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_collect (status=%d)",
          status);

   // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                        Catalogue::cfits_get_col                            */
/* -------------------------------------------------------------------------- */
/* Private method: get real column from FITS table                            */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_get_col(fitsfile *fptr, Parameters *par,
                                std::string colname,
                                std::vector<double> &col,
                                Status status) {

    // Declare local variables
    int     colnum;
    int     anynul;
    long    numRows;
    int     fstatus;
    double* tmp = NULL;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_get_col (double)");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of rows in table
      fstatus = fits_get_num_rows(fptr, &numRows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Fall through if there are no rows
      if (numRows < 1)
        continue;

      // Allocate result vector
      col = std::vector<double>(numRows);

      // Determine number of requested column
      fstatus = fits_get_colnum(fptr, CASESEN, (char*)colname.c_str(), 
                                &colnum, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to find column %s in catalogue.",
              fstatus, colname.c_str());
        continue;
      }

      // Allocate temporary memory to hold the data
      tmp = new double[numRows];
      if (tmp == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }

      // Read column data
      fstatus = fits_read_col(fptr, TDOUBLE, colnum, 1, 1, numRows,
                              0, tmp, &anynul, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read data from column %s in catalogue.",
              fstatus, colname.c_str());
        continue;
      }

      // Copy information into result vector
      for (int i = 0; i < numRows; ++i)
        col[i] = tmp[i];

    } while (0); // End of main do-loop

    // Free temporary memory
    if (tmp != NULL) delete [] tmp;

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_get_col (double) (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                       Catalogue::cfits_get_col_str                         */
/* -------------------------------------------------------------------------- */
/* Private method: get string column from FITS table                          */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_get_col_str(fitsfile *fptr, Parameters *par, 
                                    std::string colname,
                                    std::vector<std::string> &col,
                                    Status status) {

    // Declare local variables
    int    colnum;
    int    typecode;
    int    anynul;
    long   repeat;
    long   width;
    long   numRows;
    int    fstatus;
    char** tmp_id = NULL;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_get_col_str");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of rows in table
      fstatus = fits_get_num_rows(fptr, &numRows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Fall through if there are no rows
      if (numRows < 1)
        continue;

      // Allocate result vector
      col = std::vector<std::string>(numRows);

      // Determine number of requested column
      fstatus = fits_get_colnum(fptr, CASESEN, (char*)colname.c_str(), 
                                &colnum, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to find column %s in catalogue.",
              fstatus, colname.c_str());
        continue;
      }

      // Determine size of requested column
      fstatus = fits_get_coltype(fptr, colnum, &typecode, &repeat, &width,
                                 &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine width of column %s in catalogue.",
             fstatus, colname.c_str());
        continue;
      }

      // Allocate temporary memory to hold the ID column
      tmp_id = new (char*[numRows]);
      if (tmp_id == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }
      for (int i = 0; i < numRows; ++i) {
        tmp_id[i] = NULL;
        tmp_id[i] = new char[width+1];
        if (tmp_id[i] == NULL) {
          status = STATUS_MEM_ALLOC;
          continue;
        }
      }
      if (status != STATUS_OK) {
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }

      // Read column data
      fstatus = fits_read_col_str(fptr, colnum, 1, 1, numRows,
                                  NULL, tmp_id, &anynul, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to read data from column %s in catalogue.",
              fstatus, colname.c_str());
        continue;
      }

      // Copy information into result vector
      for (int i = 0; i < numRows; ++i)
        col[i].assign(tmp_id[i]);

    } while (0); // End of main do-loop

    // Free temporary memory
    if (tmp_id != NULL) {
      for (int i = 0; i < numRows; ++i) {
        if (tmp_id[i] != NULL) delete [] tmp_id[i];
      }
      delete [] tmp_id;
    }

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_get_col_str (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                        Catalogue::cfits_set_col                            */
/* -------------------------------------------------------------------------- */
/* Private method: write real column into FITS table                          */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_set_col(fitsfile *fptr, Parameters *par,
                                std::string colname,
                                std::vector<double> &col,
                                Status status) {

    // Declare local variables
    int     fstatus;
    int     colnum;
    long    numRows;
    double* tmp = NULL;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_set_col (double)");

    // Initialise FITSIO status
    fstatus = (int)status;

    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Determine number of rows in table
      fstatus = fits_get_num_rows(fptr, &numRows, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to determine number of rows in catalogue.",
              fstatus);
        continue;
      }

      // Fall through if there are no rows
      if (numRows < 1)
        continue;

      // Check if number of rows is compatible with vector dimension
      if (numRows != (long)col.size()) {
        status = STATUS_CAT_INCOMPATIBLE;
        if (par->logTerse())
          Log(Error_2, "%d : Catalogue table incompatible with vector dimension.",
              (Status)status);
        continue;
      }

      // Check if column exists. If not then append column
      fstatus = fits_get_colnum(fptr, CASESEN, (char*)colname.c_str(),
                                &colnum, &fstatus);
      if (fstatus != 0) {

        // Reset error flag
        fstatus = 0;

        // Determine number of existing columns
        fstatus = fits_get_num_cols(fptr, &colnum, &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to determine number of columns in catalogue.",
                fstatus);
          continue;
        }

        // Set requested column number to beyond last column
        colnum++;

        // Append <double> column for result
        fstatus = fits_insert_col(fptr, colnum, (char*)colname.c_str(), "1D",
                                  &fstatus);
        if (fstatus != 0) {
          if (par->logTerse())
            Log(Error_2, "%d : Unable to append column <%s> to catalogue.",
                fstatus, colname.c_str());
          continue;
        }

      } // endif: column did not exist

      // Allocate temporary memory to hold the data
      tmp = new double[numRows];
      if (tmp == NULL) {
        status = STATUS_MEM_ALLOC;
        if (par->logTerse())
          Log(Error_2, "%d : Memory allocation failure.", (Status)status);
        continue;
      }

      // Setup data array
      for (int row = 0; row < numRows; ++row)
        tmp[row] = col[row];

      // Write data into column
      fstatus = fits_write_col(fptr, TDOUBLE, colnum,
                               1, 1, numRows, tmp, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to write data into column <%s> of catalogue.",
              fstatus, colname.c_str());
        continue;
      }

    } while (0); // End of main do-loop

    // Free temporary memory
    if (tmp != NULL) delete [] tmp;

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_set_col (double) (status=%d)",
          status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                           Catalogue::cfits_set_pars                        */
/* -------------------------------------------------------------------------- */
/* Private method: Save run parameters as FITS keywords.                      */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_set_pars(fitsfile *fptr, Parameters *par, Status status) {

    // Declare local variables
    int fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_set_pars");

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
        Log(Log_2, "Save run parameters as FITS keywords:");
        Log(Log_2, "=====================================");
      }

      // Write parameters
      fstatus = fits_update_key_dbl(fptr, "P_THRES", par->m_probThres,
                                    4, "Probability threshold", &fstatus);
      fstatus = fits_update_key_lng(fptr, "MAX_CPT", par->m_maxNumCpt,
                                    "Maximum number of counterparts", &fstatus);
      fstatus = fits_update_key_dbl(fptr, "SRC_ERR", par->m_srcPosError,
                                    4, "Default source position error", &fstatus);
      fstatus = fits_update_key_dbl(fptr, "CPT_ERR", par->m_cptPosError,
                                    4, "Default counterpart position error", &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Warning_2, "%d : Unable to write parameters keywords to"
              " catalogue.", fstatus);
        fstatus = 0;
        continue;
      }
      if (fstatus != 0)
        continue;

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_set_pars (status=%d)", status);

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                              Catalogue::cfits_save                         */
/* -------------------------------------------------------------------------- */
/* Private method: Save catalogue.                                            */
/*----------------------------------------------------------------------------*/
Status Catalogue::cfits_save(fitsfile *fptr, Parameters *par, Status status) {

    // Declare local variables
    int fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_save");

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
      fstatus = fits_close_file(fptr, &fstatus);
      if (fstatus != 0) {
        if (par->logTerse())
          Log(Error_2, "%d : Unable to close catalogue.", fstatus);
        continue;
      }

    } while (0); // End of main do-loop

    // Set FITSIO status
    if (status == STATUS_OK)
      status = (Status)fstatus;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " <== EXIT: Catalogue::cfits_save (status=%d)", status);

    // Return status
    return status;

}

/* Namespace ends ___________________________________________________________ */
}
