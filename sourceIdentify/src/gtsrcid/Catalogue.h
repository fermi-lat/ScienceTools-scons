/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.10  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

Revision 1.9  2007/09/21 12:49:10  jurgen
Enhance log-file output and chatter level

Revision 1.8  2006/02/09 22:49:52  jurgen
Add 'L' to integer long constant

Revision 1.7  2006/02/09 13:06:18  jurgen
Put maximum number of source to load at once in a constant and change
value to a large value (since the loading logic has not yet been
implemented).

Revision 1.6  2006/02/07 16:05:04  jurgen
Use ObjectInfo structure to hold catalogue object information

Revision 1.5  2006/02/07 11:10:50  jurgen
Suppress catalogAccess verbosity

Revision 1.4  2006/02/03 12:14:52  jurgen
New version that allows additional probabilities to be taken
into account. The code has been considerably reorganised. Also
catalogue column prefixes are now handled differently.

Revision 1.3  2006/02/01 13:33:36  jurgen
Tried to fix Win32 compilation bugs.
Change revision number to 1.3.2.
Replace header information with CVS typeset information.

------------------------------------------------------------------------------*/
#ifndef CATALOGUE_H
#define CATALOGUE_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"
#include "Parameters.h"
#include "src/catalog.h"
#include "src/quantity.h"
#include "fitsio.h"

/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {

/* Definitions ______________________________________________________________ */
#define OUTCAT_MAX_STRING_LEN      256
#define OUTCAT_MAX_KEY_LEN         80
#define OUTCAT_EXT_NAME            "GLAST_CAT"
//
#define OUTCAT_NUM_GENERIC         10
//
#define OUTCAT_COL_ID_COLNUM       1
#define OUTCAT_COL_ID_NAME         "ID"
#define OUTCAT_COL_ID_FORM         "20A"
#define OUTCAT_COL_ID_UCD          "ID_MAIN"
//
#define OUTCAT_COL_RA_COLNUM       2
#define OUTCAT_COL_RA_NAME         "RA_J2000"
#define OUTCAT_COL_RA_FORM         "1E"
#define OUTCAT_COL_RA_UNIT         "deg"
#define OUTCAT_COL_RA_UCD          "POS_EQ_RA_MAIN"
//
#define OUTCAT_COL_DEC_COLNUM      3
#define OUTCAT_COL_DEC_NAME        "DEC_J2000"
#define OUTCAT_COL_DEC_FORM        "1E"
#define OUTCAT_COL_DEC_UNIT        "deg"
#define OUTCAT_COL_DEC_UCD         "POS_EQ_DEC_MAIN"
//
#define OUTCAT_COL_MAJERR_COLNUM   4
#define OUTCAT_COL_MAJERR_NAME     "POS_ERR_MAX"
#define OUTCAT_COL_MAJERR_FORM     "1E"
#define OUTCAT_COL_MAJERR_UNIT     "deg"
#define OUTCAT_COL_MAJERR_UCD      "ERROR"
//
#define OUTCAT_COL_MINERR_COLNUM   5
#define OUTCAT_COL_MINERR_NAME     "POS_ERR_MIN"
#define OUTCAT_COL_MINERR_FORM     "1E"
#define OUTCAT_COL_MINERR_UNIT     "deg"
#define OUTCAT_COL_MINERR_UCD      "ERROR"
//
#define OUTCAT_COL_POSANGLE_COLNUM 6
#define OUTCAT_COL_POSANGLE_NAME   "POS_ERR_ANG"
#define OUTCAT_COL_POSANGLE_FORM   "1E"
#define OUTCAT_COL_POSANGLE_UNIT   "deg"
#define OUTCAT_COL_POSANGLE_UCD    "ERROR"
//
#define OUTCAT_COL_PROB_COLNUM     7
#define OUTCAT_COL_PROB_NAME       "PROB"
#define OUTCAT_COL_PROB_FORM       "1E"
#define OUTCAT_COL_PROB_UNIT       "probability"
#define OUTCAT_COL_PROB_UCD        ""
//
#define OUTCAT_COL_PROB1_COLNUM    8
#define OUTCAT_COL_PROB1_NAME      "PROB_ANGSEP"
#define OUTCAT_COL_PROB1_FORM      "1E"
#define OUTCAT_COL_PROB1_UNIT      "probability"
#define OUTCAT_COL_PROB1_UCD       ""
//
#define OUTCAT_COL_PROB2_COLNUM    9
#define OUTCAT_COL_PROB2_NAME      "PROB_ADD"
#define OUTCAT_COL_PROB2_FORM      "1E"
#define OUTCAT_COL_PROB2_UNIT      "probability"
#define OUTCAT_COL_PROB2_UCD       ""
//
#define OUTCAT_COL_ANGSEP_COLNUM   10
#define OUTCAT_COL_ANGSEP_NAME     "ANGSEP"
#define OUTCAT_COL_ANGSEP_FORM     "1E"
#define OUTCAT_COL_ANGSEP_UNIT     "deg"
#define OUTCAT_COL_ANGSEP_UCD      ""
//
#define SRC_FORMAT "  RA=%8.4f  DE=%8.4f  e_maj=%7.4f  e_min=%7.4f  e_ang=%6.2f"

/* Class constants __________________________________________________________ */
const long   c_maxCptLoad    = 10000000000L; // Maximum # of sources to load at once
const double c_filter_maxsep = 4.0;          // Filter all sources more distant than 2 deg

/* Mathematical constants ___________________________________________________ */
const double pi          = 3.1415926535897931159979635;
const double twopi       = 6.2831853071795862319959269;
const double sqrt2pi     = 2.5066282746310002416123552;
const double twosqrt2ln2 = 2.3548200450309493270140138;
const double deg2rad     = 0.0174532925199432954743717;
const double rad2deg     = 57.295779513082322864647722;

/* Type defintions __________________________________________________________ */
typedef struct {                      // Counterpart candidate
  std::string             id;           // Unique identifier
  double                  pos_eq_ra;    // Right Ascension (deg)
  double                  pos_eq_dec;   // Declination (deg)
  double                  pos_err_maj;  // Uncertainty ellipse major axis (deg)
  double                  pos_err_min;  // Uncertainty ellipse minor axis (deg)
  double                  pos_err_ang;  // Uncertainty ellipse positron angle (deg)
  double                  prob;         // Counterpart probability
  //
  long                    index;        // Index of CCs in CPT catalogue
  double                  angsep;       // Angular separation of CCs from source
  double                  prob_angsep;  // Probability from angular separation
  std::vector<double>     prob_add;     // Additional probabilities
} CCElement;

typedef struct {                      // Catalogue object information
  std::string             name;         // Object name
  int                     pos_valid;    // Position validity (1=valid)
  double                  pos_eq_ra;    // Right Ascension (deg)
  double                  pos_eq_dec;   // Declination (deg)
  double                  pos_err_maj;  // Position error major axis
  double                  pos_err_min;  // Position error minor axis
  double                  pos_err_ang;  // Position error angle
} ObjectInfo;

typedef struct {                      // Input catalogue
  std::string             inName;       // Input name
  std::string             catCode;      // Catalogue code
  std::string             catURL;       // Catalogue URL
  std::string             catName;      // Catalogue name
  std::string             catRef;       // Catalogue Reference
  std::string             tableName;    // Table name
  std::string             tableRef;     // Table reference
  catalogAccess::Catalog  cat;          // Catalogue
  long                    numLoad;      // Number of loaded objects in catalogue
  long                    numTotal;     // Total number of objects in catalogue
  ObjectInfo             *object;       // Object information
} InCatalogue;

class Catalogue {
public:

  // Constructor & destructor
  Catalogue(void);                          // Inline
 ~Catalogue(void);                          // Inline

  // Public methods
  Status build(Parameters *par, Status status);

  // Private methods
private:
  void   init_memory(void);
  void   free_memory(void);
  Status get_input_descriptor(Parameters *par, std::string catName, 
                              InCatalogue *in,  Status status);
  Status get_input_catalogue(Parameters *par, InCatalogue *in, double posErr,
                             std::string &obj_name, Status status);
  Status dump_descriptor(InCatalogue *in, Status status);
  Status dump_results(Parameters *par, Status status);
  //
  // Low-level source identification methods
  // ---------------------------------------
  Status      cid_get(Parameters *par, long iSrc, Status status);
  Status      cid_filter(Parameters *par, long iSrc, Status status);
  Status      cid_refine(Parameters *par, long iSrc, Status status);
  Status      cid_prob_angsep(Parameters *par, long iSrc, Status status);
  Status      cid_sort(Parameters *par, Status status);
  Status      cid_dump(Parameters *par, Status status);
  std::string cid_assign_src_name(std::string name, int row);
  //
  // Low-level FITS catalogue handling methods
  // -----------------------------------------
  Status cfits_create(fitsfile **fptr, char *filename, Parameters *par, 
                      Status status);
  Status cfits_clear(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_add(fitsfile *fptr, long iSrc, Parameters *par, Status status);
  Status cfits_eval(fitsfile *fptr, Parameters *par, int verbose, Status status);
  Status cfits_colval(fitsfile *fptr, char *colname, Parameters *par, 
                      std::vector<double> *val, Status status);
  Status cfits_select(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_collect(fitsfile *fptr, Parameters *par, std::vector<int> &stat,
                       Status status);
  Status cfits_get_col(fitsfile *fptr, Parameters *par, std::string colname,
                       std::vector<double> &col, Status status);
  Status cfits_get_col_str(fitsfile *fptr, Parameters *par, std::string colname,
                           std::vector<std::string> &col, Status status);
  Status cfits_save(fitsfile *fptr, Parameters *par, Status status);
private:
  //
  // Input catalogues
  InCatalogue              m_src;           // Source catalogue
  InCatalogue              m_cpt;           // Counterpart catalogue
  std::string              m_src_name;      // Source names
  std::string              m_cpt_name;      // Counterpart names
  //
  // Catalogue building parameters
  long                     m_maxCptLoad;    // Maximum number of counterparts to be loaded
  long                     m_fCptLoaded;    // Loaded counterparts fully
  double                   m_filter_maxsep; // Maximum counterpart separation (in deg)
  fitsfile                *m_memFile;       // Memory catalogue FITS file pointer
  fitsfile                *m_outFile;       // Output catalogue FITS file pointer
  //
  // Counterpart candidate (CC) working arrays
  long                     m_numCC;         // Number of CCs
  CCElement               *m_cc;            // CCs
  //
  // Counterpart statistics
  int                      m_num_Sel;       // Number of quantity selection criteria
  int                     *m_cpt_stat;      // Counterpart statistics for each source
  std::vector<int>         m_src_cpts;      // Number of initial counterparts
  std::vector<std::string> m_cpt_names;     // Counterpart names for each source
  //
  // Output cataloge: source catalogue quantities
  long                     m_num_src_Qty;
  std::vector<int>         m_src_Qty_colnum;
  std::vector<std::string> m_src_Qty_ttype;
  std::vector<std::string> m_src_Qty_tform;
  std::vector<std::string> m_src_Qty_tunit;
  std::vector<std::string> m_src_Qty_tbucd;
  //
  // Output cataloge: counterpart catalogue quantities
  long                     m_num_cpt_Qty;
  std::vector<int>         m_cpt_Qty_colnum;
  std::vector<std::string> m_cpt_Qty_ttype;
  std::vector<std::string> m_cpt_Qty_tform;
  std::vector<std::string> m_cpt_Qty_tunit;
  std::vector<std::string> m_cpt_Qty_tbucd;
};
inline Catalogue::Catalogue(void) { init_memory(); }
inline Catalogue::~Catalogue(void) { free_memory(); }


/* Prototypes _______________________________________________________________ */


/* Globals __________________________________________________________________ */


/* Namespace ends ___________________________________________________________ */
}
#endif // CATALOGUE_H
