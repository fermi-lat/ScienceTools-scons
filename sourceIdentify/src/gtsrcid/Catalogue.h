/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.25  2008/03/26 13:37:53  jurgen
Remove unused members

Revision 1.24  2008/03/26 13:37:10  jurgen
Generalize probability calculation and implement Bayesian method

Revision 1.23  2008/03/21 16:42:56  jurgen
Update documentation

Revision 1.22  2008/03/21 15:27:03  jurgen
Estimate number of false associations

Revision 1.21  2008/03/21 09:10:12  jurgen
Enhance code documentation.

Revision 1.20  2008/03/20 21:56:26  jurgen
implement local counterpart density

Revision 1.19  2008/02/23 08:14:23  jurgen
correct catalogAccess header file path after catalogAccess modifications

Revision 1.18  2007/11/08 14:42:11  jurgen
Handle error circles (e.g. 3EG catalogue)

Revision 1.17  2007/10/11 13:20:54  jurgen
Correctly remove FITS special function columns

Revision 1.16  2007/10/10 15:39:12  jurgen
Introduce handling of special functions 'gammln', 'erf', and 'erfc'

Revision 1.15  2007/10/09 16:46:23  jurgen
Write counterpart catalogue reference (row) to output catalogue

Revision 1.14  2007/10/09 08:17:40  jurgen
Correctly interpret positional errors and correctly evaluate PROB_POS
as likelihood

Revision 1.13  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

Revision 1.12  2007/10/03 09:06:08  jurgen
Add chance coincidence probability PROB_CHANCE

Revision 1.11  2007/10/02 21:48:45  jurgen
Add PROB_ANGSEP, PROB_ADD and ANGSEP generic columns to FITS output file

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
/**
 * @file Catalogue.h
 * @brief Catalogue class interface definition.
 * @author J. Knodlseder
 */

#ifndef CATALOGUE_H
#define CATALOGUE_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"
#include "Parameters.h"
#include "catalogAccess/catalog.h"
#include "catalogAccess/quantity.h"
#include "fitsio.h"

/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {

/* Definitions ______________________________________________________________ */
#define OUTCAT_MAX_STRING_LEN      256
#define OUTCAT_MAX_KEY_LEN         256
#define OUTCAT_EXT_NAME            "GLAST_CAT"
//
#define OUTCAT_NUM_GENERIC            20
//
#define OUTCAT_COL_ID_COLNUM          1
#define OUTCAT_COL_ID_NAME            "ID"
#define OUTCAT_COL_ID_FORM            "20A"
#define OUTCAT_COL_ID_UCD             "ID_MAIN"
//
#define OUTCAT_COL_RA_COLNUM          2
#define OUTCAT_COL_RA_NAME            "RAJ2000"
#define OUTCAT_COL_RA_FORM            "1E"
#define OUTCAT_COL_RA_UNIT            "deg"
#define OUTCAT_COL_RA_UCD             "POS_EQ_RA_MAIN"
//
#define OUTCAT_COL_DEC_COLNUM         3
#define OUTCAT_COL_DEC_NAME           "DEJ2000"
#define OUTCAT_COL_DEC_FORM           "1E"
#define OUTCAT_COL_DEC_UNIT           "deg"
#define OUTCAT_COL_DEC_UCD            "POS_EQ_DEC_MAIN"
//
#define OUTCAT_COL_MAJERR_COLNUM      4
#define OUTCAT_COL_MAJERR_NAME        "POS_ERR_MAJ"
#define OUTCAT_COL_MAJERR_FORM        "1E"
#define OUTCAT_COL_MAJERR_UNIT        "deg"
#define OUTCAT_COL_MAJERR_UCD         "ERROR"
//
#define OUTCAT_COL_MINERR_COLNUM      5
#define OUTCAT_COL_MINERR_NAME        "POS_ERR_MIN"
#define OUTCAT_COL_MINERR_FORM        "1E"
#define OUTCAT_COL_MINERR_UNIT        "deg"
#define OUTCAT_COL_MINERR_UCD         "ERROR"
//
#define OUTCAT_COL_POSANGLE_COLNUM    6
#define OUTCAT_COL_POSANGLE_NAME      "POS_ERR_ANG"
#define OUTCAT_COL_POSANGLE_FORM      "1E"
#define OUTCAT_COL_POSANGLE_UNIT      "deg"
#define OUTCAT_COL_POSANGLE_UCD       "ERROR"
//
#define OUTCAT_COL_PROB_COLNUM        7
#define OUTCAT_COL_PROB_NAME          "PROB"
#define OUTCAT_COL_PROB_FORM          "1D"
#define OUTCAT_COL_PROB_UNIT          "probability"
#define OUTCAT_COL_PROB_UCD           ""
//
#define OUTCAT_COL_PROB_POS_COLNUM    8
#define OUTCAT_COL_PROB_POS_NAME      "PROB_POS"
#define OUTCAT_COL_PROB_POS_FORM      "1D"
#define OUTCAT_COL_PROB_POS_UNIT      "probability"
#define OUTCAT_COL_PROB_POS_UCD       ""
//
#define OUTCAT_COL_PDF_POS_COLNUM     9
#define OUTCAT_COL_PDF_POS_NAME       "PDF_POS"
#define OUTCAT_COL_PDF_POS_FORM       "1D"
#define OUTCAT_COL_PDF_POS_UNIT       "dp/dr"
#define OUTCAT_COL_PDF_POS_UCD        ""
//
#define OUTCAT_COL_PROB_CHANCE_COLNUM 10
#define OUTCAT_COL_PROB_CHANCE_NAME   "PROB_CHANCE"
#define OUTCAT_COL_PROB_CHANCE_FORM   "1D"
#define OUTCAT_COL_PROB_CHANCE_UNIT   "probability"
#define OUTCAT_COL_PROB_CHANCE_UCD    ""
//
#define OUTCAT_COL_PDF_CHANCE_COLNUM  11
#define OUTCAT_COL_PDF_CHANCE_NAME    "PDF_CHANCE"
#define OUTCAT_COL_PDF_CHANCE_FORM    "1D"
#define OUTCAT_COL_PDF_CHANCE_UNIT    "dp/dr"
#define OUTCAT_COL_PDF_CHANCE_UCD     ""
//
#define OUTCAT_COL_PROB_PRIOR_COLNUM  12
#define OUTCAT_COL_PROB_PRIOR_NAME    "PROB_PRIOR"
#define OUTCAT_COL_PROB_PRIOR_FORM    "1D"
#define OUTCAT_COL_PROB_PRIOR_UNIT    "probability"
#define OUTCAT_COL_PROB_PRIOR_UCD     ""
//
#define OUTCAT_COL_PROB_POST_COLNUM   13
#define OUTCAT_COL_PROB_POST_NAME     "PROB_POST"
#define OUTCAT_COL_PROB_POST_FORM     "1D"
#define OUTCAT_COL_PROB_POST_UNIT     "probability"
#define OUTCAT_COL_PROB_POST_UCD      ""
//
#define OUTCAT_COL_LR_COLNUM          14
#define OUTCAT_COL_LR_NAME            "LIKRAT"
#define OUTCAT_COL_LR_FORM            "1D"
#define OUTCAT_COL_LR_UNIT            ""
#define OUTCAT_COL_LR_UCD             ""
//
#define OUTCAT_COL_ANGSEP_COLNUM      15
#define OUTCAT_COL_ANGSEP_NAME        "ANGSEP"
#define OUTCAT_COL_ANGSEP_FORM        "1E"
#define OUTCAT_COL_ANGSEP_UNIT        "deg"
#define OUTCAT_COL_ANGSEP_UCD         ""
//
#define OUTCAT_COL_PSI_COLNUM         16
#define OUTCAT_COL_PSI_NAME           "PSI"
#define OUTCAT_COL_PSI_FORM           "1E"
#define OUTCAT_COL_PSI_UNIT           "deg"
#define OUTCAT_COL_PSI_UCD            "ERROR"
//
#define OUTCAT_COL_POSANG_COLNUM      17
#define OUTCAT_COL_POSANG_NAME        "POSANG"
#define OUTCAT_COL_POSANG_FORM        "1E"
#define OUTCAT_COL_POSANG_UNIT        "deg"
#define OUTCAT_COL_POSANG_UCD         ""
//
#define OUTCAT_COL_RHO_COLNUM         18
#define OUTCAT_COL_RHO_NAME           "RHO"
#define OUTCAT_COL_RHO_FORM           "1E"
#define OUTCAT_COL_RHO_UNIT           "src/deg^2"
#define OUTCAT_COL_RHO_UCD            ""
//
#define OUTCAT_COL_LAMBDA_COLNUM      19
#define OUTCAT_COL_LAMBDA_NAME        "LAMBDA"
#define OUTCAT_COL_LAMBDA_FORM        "1E"
#define OUTCAT_COL_LAMBDA_UNIT        "src"
#define OUTCAT_COL_LAMBDA_UCD         ""
//
#define OUTCAT_COL_REF_COLNUM         20
#define OUTCAT_COL_REF_NAME           "REF"
#define OUTCAT_COL_REF_FORM           "1J"
#define OUTCAT_COL_REF_UNIT           ""
#define OUTCAT_COL_REF_UCD            ""
//
#define SRC_FORMAT "  RA=%8.4f  DE=%8.4f  e_maj=%7.4f  e_min=%7.4f  e_ang=%6.2f"

/* Class constants __________________________________________________________ */
const long   c_maxCptLoad    = 10000000000L; //!< Max. # of sources to load at once
const double c_filter_maxsep = 4.0;          //!< Minimum filter radius

/* Mathematical constants ___________________________________________________ */
const double pi          =  3.1415926535897931159979635;
const double twopi       =  6.2831853071795862319959269;
const double sqrt2pi     =  2.5066282746310002416123552;
const double twosqrt2ln2 =  2.3548200450309493270140138;
const double deg2rad     =  0.0174532925199432954743717;
const double rad2deg     = 57.295779513082322864647722;
const double dnorm       =  2.9957230;
const double twodnorm    =  2.0 * dnorm;

/* Probability scaling constants ____________________________________________ */
const double e_norm_1s = 1.0 / sqrt(1.1478742);  //!< 1 sigma = 68.269%, 2 dof
const double e_norm_2s = 1.0 / sqrt(3.0900358);  //!< 2 sigma = 95.450%, 2 dof
const double e_norm_3s = 1.0 / sqrt(5.9145778);  //!< 3 sigma = 99.730%, 2 dof
const double e_norm_68 = 1.0 / sqrt(1.1394375);  //!< 68.000%, 2 dof
const double e_norm_95 = 1.0 / sqrt(2.9957230);  //!< 95.000%, 2 dof
const double e_norm_99 = 1.0 / sqrt(4.6051713);  //!< 99.000%, 2 dof

/* Search strings (need "stop" as last string !!!) __________________________ */
const std::string search_id[] = {"NAME", "ID", "stop"};

/* Special function strings (need "stop" as last string !!!) ________________ */
const std::string fct_names[] = {"gammln", "erf", "erfc", "stop"};

/* Type defintions __________________________________________________________ */
typedef enum {                        // Position error type
  NoError = 1,                          //!< No error
  Radius,                               //!< Error radius
  Ellipse,                              //!< Error ellipse
  RaDec                                 //!< Errors on RA and Dec
} PosErrorType;

typedef enum {                        // Position error probability
  Sigma_1 = 1,                          //!< 1 sigma standard deviations
  Sigma_2,                              //!< 2 sigma standard deviations
  Sigma_3,                              //!< 3 sigma standard deviations
  Prob_68,                              //!< 68% probability ellipse
  Prob_95,                              //!< 95% probability ellipse
  Prob_99                               //!< 99% probability ellipse
} PosErrorProb;

typedef struct {                      // Counterpart candidate
  std::string             id;           //!< Unique identifier
  double                  pos_eq_ra;    //!< Right Ascension (deg)
  double                  pos_eq_dec;   //!< Declination (deg)
  double                  pos_err_maj;  //!< 95% uncertainty ellipse major axis (deg)
  double                  pos_err_min;  //!< 95% uncertainty ellipse minor axis (deg)
  double                  pos_err_ang;  //!< 95% uncertainty ellipse PA (deg)
  double                  prob;         //!< Counterpart probability
  //
  long                    index;        //!< Index of CCs in CPT catalogue
  double                  angsep;       //!< Angular separation of CPT from source
  double                  psi;          //!< Eff. radius of 95% error ellipse (deg)
  double                  posang;       //!< Position angle of CPT w/r to source
  double                  lambda;       //!< Expected number of false counterparts
  double                  prob_pos;     //!< Counterpart probability
  double                  prob_chance;  //!< Chance coincidence probability
  double                  prob_prior;   //!< Counterpart prior probability
  double                  prob_post;    //!< Counterpart posterior probability
  double                  pdf_pos;      //!< Counterpart PDF
  double                  pdf_chance;   //!< Chance coincidence PDF
  double                  likrat;       //!< Likelihood ratio
} CCElement;

typedef struct {                      // Catalogue object information
  std::string             name;         //!< Object name
  int                     pos_valid;    //!< Position validity (1=valid)
  double                  pos_eq_ra;    //!< Right Ascension (deg)
  double                  pos_eq_dec;   //!< Declination (deg)
  double                  pos_err_maj;  //!< Position error major axis
  double                  pos_err_min;  //!< Position error minor axis
  double                  pos_err_ang;  //!< Position error angle
} ObjectInfo;

typedef struct {                      // Input catalogue
  std::string             inName;       //!< Input name
  std::string             catCode;      //!< Catalogue code
  std::string             catURL;       //!< Catalogue URL
  std::string             catName;      //!< Catalogue name
  std::string             catRef;       //!< Catalogue Reference
  std::string             tableName;    //!< Table name
  std::string             tableRef;     //!< Table reference
  catalogAccess::Catalog  cat;          //!< Catalogue
  long                    numLoad;      //!< Number of loaded objects in catalogue
  long                    numTotal;     //!< Total number of objects in catalogue
  std::string             col_id;       //!< Source ID column name
  std::string             col_ra;       //!< Right Ascension column name
  std::string             col_dec;      //!< Declination column name
  std::string             col_e_ra;     //!< Right Ascension error column name
  std::string             col_e_dec;    //!< Declination error column name
  std::string             col_e_maj;    //!< Error ellipse Semimajor axis or radius
  std::string             col_e_min;    //!< Error ellipse Semi-minor axis
  std::string             col_e_posang; //!< Error ellipse Position angle
  PosErrorType            col_e_type;   //!< Position error type
  PosErrorProb            col_e_prob;   //!< Position error probability
  double                  e_pos_scale;  //!< Position error scaling
  ObjectInfo             *object;       //!< Object information
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
                             Status status);
  Status dump_descriptor(Parameters *par, InCatalogue *in, Status status);
  Status dump_results(Parameters *par, Status status);
  //
  // Low-level source identification methods
  // ---------------------------------------
  Status      cid_get(Parameters *par, int iSrc, Status status);
  Status      cid_filter(Parameters *par, int iSrc, Status status);
  Status      cid_refine(Parameters *par, int iSrc, Status status);
  Status      cid_select(Parameters *par, int iSrc, Status status);
  Status      cid_prob_pos(Parameters *par, int iSrc, Status status);
  Status      cid_prob_chance(Parameters *par, int iSrc, Status status);
  Status      cid_prob_prior(Parameters *par, Status status);
  Status      cid_prob_post(Parameters *par, Status status);
  Status      cid_prob(Parameters *par, Status status);
  Status      cid_local_density(Parameters *par, int iSrc, Status status);
  Status      cid_global_density(Parameters *par, Status status);
  Status      cid_sort(Parameters *par, Status status);
  Status      cid_dump(Parameters *par, Status status);
  std::string cid_assign_src_name(std::string name, int row);
  //
  // Low-level FITS catalogue handling methods
  // -----------------------------------------
  Status cfits_create(fitsfile **fptr, char *filename, Parameters *par, 
                      Status status);
  Status cfits_clear(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_add(fitsfile *fptr, int iSrc, Parameters *par, Status status);
  Status cfits_eval(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_eval_column(fitsfile *fptr, Parameters *par, std::string column,
                           std::string formula, Status status);
  Status cfits_eval_regular_expression(fitsfile *fptr, Parameters *par,
                                       std::string column, std::string formula,
                                       Status status);
  Status cfits_eval_special_expression(fitsfile *fptr, Parameters *par,
                                       std::string column, std::string &formula,
                                       Status status);
  Status cfits_eval_special_function(fitsfile *fptr, Parameters *par,
                                     std::string fct,
                                     std::string column_res, 
                                     std::string column_arg,
                                     Status status);
  Status cfits_eval_clear(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_select(fitsfile *fptr, int iSrc, Parameters *par, Status status);
  Status cfits_collect(fitsfile *fptr, Parameters *par, std::vector<int> &stat,
                       Status status);
  Status cfits_get_col(fitsfile *fptr, Parameters *par, std::string colname,
                       std::vector<double> &col, Status status);
  Status cfits_get_col_str(fitsfile *fptr, Parameters *par, std::string colname,
                           std::vector<std::string> &col, Status status);
  Status cfits_set_col(fitsfile *fptr, Parameters *par, std::string colname,
                       std::vector<double> &col, Status status);
  Status cfits_set_pars(fitsfile *fptr, Parameters *par, Status status);
  Status cfits_save(fitsfile *fptr, Parameters *par, Status status);
private:
  //
  // Input catalogues
  InCatalogue              m_src;            //!< Source catalogue
  InCatalogue              m_cpt;            //!< Counterpart catalogue
  //
  // Catalogue building parameters
  int                      m_maxCptLoad;     //!< Maximum # of cpts to be loaded
  int                      m_fCptLoaded;     //!< Loaded counterparts fully
  fitsfile                *m_memFile;        //!< Memory catalogue FITS file pointer
  fitsfile                *m_outFile;        //!< Output catalogue FITS file pointer
  //
  // Counterpart candidate (CC) working arrays
  int                      m_numCC;          //!< Number of counterpart candidates
  CCElement               *m_cc;             //!< List of counterpart candidates
  double                   m_filter_rad;     //!< Filter step radius
  double                   m_ring_rad_min;   //!< Density ring minimum
  double                   m_ring_rad_max;   //!< Density ring maximum
  double                   m_omega;          //!< Solid angle of error ellipse
  double                   m_rho;            //!< Local counterpart density
  //
  // Counterpart statistics
  int                      m_num_Sel;        //!< Number of selection criteria
  int                     *m_cpt_stat;       //!< Counterpart statistics
  std::vector<int>         m_src_cpts;       //!< Number of initial counterparts
  std::vector<std::string> m_cpt_names;      //!< Counterpart names for each source
  int                      m_num_assoc;      //!< Number of associations
  //
  // Output cataloge: source catalogue quantities
  int                      m_num_src_Qty;    //!< Number of src. cat. quantities
  std::vector<int>         m_src_Qty_colnum; //!< Vector of column numbers
  std::vector<std::string> m_src_Qty_ttype;  //!< Vector of column types
  std::vector<std::string> m_src_Qty_tform;  //!< Vector of column formats
  std::vector<std::string> m_src_Qty_tunit;  //!< Vector of column units
  std::vector<std::string> m_src_Qty_tbucd;  //!< Vector of column UCDs
  //
  // Output cataloge: counterpart catalogue quantities
  int                      m_num_cpt_Qty;    //!< Number of cpt. cat. quantities
  std::vector<int>         m_cpt_Qty_colnum; //!< Vector of column numbers
  std::vector<std::string> m_cpt_Qty_ttype;  //!< Vector of column types
  std::vector<std::string> m_cpt_Qty_tform;  //!< Vector of column formats
  std::vector<std::string> m_cpt_Qty_tunit;  //!< Vector of column units
  std::vector<std::string> m_cpt_Qty_tbucd;  //!< Vector of column UCDs
};
inline Catalogue::Catalogue(void) { init_memory(); }
inline Catalogue::~Catalogue(void) { free_memory(); }


/* Prototypes _______________________________________________________________ */
std::string upper(std::string arg);
double      nr_gammln(double arg);
double      nr_gammp(double a, double x);
double      nr_gammq(double a, double x);
void        nr_gser(double *gamser, double a, double x, double *gln);
void        nr_gcf(double *gammcf, double a, double x, double *gln);

/* Globals __________________________________________________________________ */

/* Namespace ends ___________________________________________________________ */
}
#endif // CATALOGUE_H
