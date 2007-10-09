/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
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


/* Private Prototypes _______________________________________________________ */
int set_fits_col_format(catalogAccess::Quantity *desc, std::string *format);
int fits_tform_binary(int typecode, long repeat, long width, 
                      std::string *format);


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
    char        **cptr;

    // Debug mode: Entry
    if (par->logDebug())
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_add");

    // Initialise temporary memory pointers
    dptr  = NULL;
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
      cptr = new char*[nrows];
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
        for (unsigned int i = 0; i < m_cc[row].prob_add.size(); ++i)
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
            strcpy(cptr[row], SValue.c_str());
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
            strcpy(cptr[row], SValue.c_str());
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
      Log(Log_0, " ==> ENTRY: Catalogue::cfits_eval");

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

      // Dump header (optionally)
      if (verbose) {
        Log(Log_2, "");
        Log(Log_2, "Add new output catalogue quantities:");
        Log(Log_2, "====================================");
      }

      // Add all new output catalogue quantities
      for (iQty = 0; iQty < numQty; iQty++) {

        // Test expression to determine the format of the new column
        fstatus = fits_test_expr(fptr,
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
        fstatus = fits_calculator(fptr, 
                                  (char*)par->m_outCatQtyFormula[iQty].c_str(),
                                  fptr,
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

        // Dump new output catalogue quantities information (optionally)
        if (verbose) {
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
      Log(Log_0, " <== EXIT: Catalogue::cfits_eval"
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
