/*----------------------------------------------------------------------------*/
/*                                Catalogue.h                                 */
/* -------------------------------------------------------------------------- */
/* Task            : Catalogue interface header file.                         */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0.0                                                    */
/* Date of version : 20-May-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/*----------------------------------------------------------------------------*/
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
#define OUTCAT_NUM_GENERIC         7
//
#define OUTCAT_COL_ID_COLNUM       1
#define OUTCAT_COL_ID_NAME         "ID"
#define OUTCAT_COL_ID_FORM         "20A"
//
#define OUTCAT_COL_RA_COLNUM       2
#define OUTCAT_COL_RA_NAME         "POS_EQ_RA"
#define OUTCAT_COL_RA_FORM         "1E"
#define OUTCAT_COL_RA_UNIT         "deg"
//
#define OUTCAT_COL_DEC_COLNUM      3
#define OUTCAT_COL_DEC_NAME        "POS_EQ_DEC"
#define OUTCAT_COL_DEC_FORM        "1E"
#define OUTCAT_COL_DEC_UNIT        "deg"
//
#define OUTCAT_COL_MAJERR_COLNUM   4
#define OUTCAT_COL_MAJERR_NAME     "POS_ERR_MAJ"
#define OUTCAT_COL_MAJERR_FORM     "1E"
#define OUTCAT_COL_MAJERR_UNIT     "deg"
//
#define OUTCAT_COL_MINERR_COLNUM   5
#define OUTCAT_COL_MINERR_NAME     "POS_ERR_MIN"
#define OUTCAT_COL_MINERR_FORM     "1E"
#define OUTCAT_COL_MINERR_UNIT     "deg"
//
#define OUTCAT_COL_POSANGLE_COLNUM 6
#define OUTCAT_COL_POSANGLE_NAME   "POS_ERR_ANG"
#define OUTCAT_COL_POSANGLE_FORM   "1E"
#define OUTCAT_COL_POSANGLE_UNIT   "deg"
//
#define OUTCAT_COL_PROB_COLNUM     7
#define OUTCAT_COL_PROB_NAME       "PROB"
#define OUTCAT_COL_PROB_FORM       "1E"
#define OUTCAT_COL_PROB_UNIT       "probability"

/* Constants ________________________________________________________________ */
const double pi          = 3.1415926535897931159979635;
const double twopi       = 6.2831853071795862319959269;
const double sqrt2pi     = 2.5066282746310002416123552;
const double twosqrt2ln2 = 2.3548200450309493270140138;
const double deg2rad     = 0.0174532925199432954743717;
const double rad2deg     = 57.295779513082322864647722;

/* Type defintions __________________________________________________________ */
typedef struct {                  // Counterpart candidate
  std::string id;                   // Unique identifier
  double      pos_eq_ra;            // Right Ascension (deg)
  double      pos_eq_dec;           // Declination (deg)
  double      pos_err_maj;          // Uncertainty ellipse major axis (deg)
  double      pos_err_min;          // Uncertainty ellipse minor axis (deg)
  double      pos_err_ang;          // Uncertainty ellipse positron angle (deg)
  double      prob;                 // Counterpart probability
  //
  long        index;                // Index of CCs in CPT catalogue
  double      angsep;               // Angular separation of CCs from source
  double      prob_angsep;          // Probability from angular separation
} CCElement;

class Catalogue {
public:

  // Constructor & destructor
  Catalogue(void);                          // Inline
 ~Catalogue(void);                          // Inline

  // Public methods
  Status build(Parameters *par, Status status);
  Status save(Parameters *par, Status status);
              
  // Private methods
private:
  void   init_memory(void);
  void   free_memory(void);
  Status get_input_descriptor(Parameters *par, std::string catName, 
                              catalogAccess::Catalog *cat, Status status);
  Status get_input_catalogue(Parameters *par, std::string catName, 
                             catalogAccess::Catalog *cat, Status status);
  Status get_counterpart_candidates(Parameters *par, long iSrc, Status status);
  Status get_counterparts(Parameters *par, double *ra, double *dec,
                          double *pos_err, Status status);
  Status get_probability(Parameters *par, long iSrc, Status status);
  Status get_probability_angsep(Parameters *par, long iSrc, Status status);
  Status create_output_catalogue(Parameters *par, Status status);
  Status add_counterpart_candidates(Parameters *par, long iSrc, Status status);
  Status eval_output_catalogue_quantities(Parameters *par, Status status);
  Status select_output_catalogue(Parameters *par, Status status);
  Status cc_sort(Parameters *par, Status status);
  Status dump_descriptor(catalogAccess::Catalog *cat, Status status);
  Status dump_counterpart_candidates(Parameters *par, Status status);

private:
  long                    numSrc;     // Number of sources in source catalogue
  long                    numCpt;     // Number of sources in counterpart cat.
  long                    maxCptLoad; // Maximum number of counterparts to be loaded
  long                    fCptLoaded; // Loaded counterparts fully
  catalogAccess::Catalog  src;        // Source catalogue
  catalogAccess::Catalog  cpt;        // Counterpart catalogue
  fitsfile               *outFile;    // Output catalogue FITS file pointer
  //
  // Counterpart candidate (CC) working arrays
  long                    numCC;      // Number of CCs
  CCElement              *cc;         // CCs
  //
  // Output cataloge: source catalogue quantities
  long                     num_src_Qty;
  std::vector<int>         src_Qty_colnum;
  std::vector<std::string> src_Qty_ttype;
  std::vector<std::string> src_Qty_tform;
  std::vector<std::string> src_Qty_tunit;
  //
  // Output cataloge: counterpart catalogue quantities
  long                     num_cpt_Qty;
  std::vector<int>         cpt_Qty_colnum;
  std::vector<std::string> cpt_Qty_ttype;
  std::vector<std::string> cpt_Qty_tform;
  std::vector<std::string> cpt_Qty_tunit;
};
inline Catalogue::Catalogue(void) { init_memory(); }
inline Catalogue::~Catalogue(void) { free_memory(); }


/* Prototypes _______________________________________________________________ */


/* Namespace ends ___________________________________________________________ */
}
#endif // CATALOGUE_H
