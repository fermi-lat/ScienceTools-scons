/*----------------------------------------------------------------------------*/
/*                                Parameters.h                                */
/* -------------------------------------------------------------------------- */
/* Task            : Task parameter interface header file.                    */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0                                                      */
/* Date of version : 25-Feb-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0   25-Feb-2005  first version                                           */
/*----------------------------------------------------------------------------*/
#ifndef PARAMETERS_H
#define PARAMETERS_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify/sourceIdentify.h"


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Defintions _______________________________________________________________ */
#define MAX_CHAR        1000
#define MIN_OUTCAT_QTY  1
#define MAX_OUTCAT_QTY  9
#define MIN_OUTCAT_SEL  1
#define MAX_OUTCAT_SEL  9

/* Type defintions __________________________________________________________ */
class Parameters {
public:

  // Friend classes
  friend class Catalogue;

  // Constructor & destructor
  Parameters(void);                         // Inline
 ~Parameters(void);                         // Inline

  // Public methods
  #if USE_ST_APP
  Status load(st_app::AppParGroup &pars, Status status);
  #else
  Status load(Status status);
  #endif
  Status dump(Status status);
  int    logError(void);                    // Inline
  int    logAction(void);                   // Inline
  int    logDetail(void);                   // Inline
  int    logDebug(void);                    // Inline
              
  // Private methods
private:
  void   init_memory(void);
  void   free_memory(void);

private:
  std::string              m_srcCatName;          // Source catalogue name
  std::string              m_cptCatName;          // Counterpart catalogue name
  std::string              m_outCatName;          // Output catalogue name
  std::string              m_srcCatQty;           // Source catalogue quantities
  std::string              m_cptCatQty;           // Counterpart catalogue quantities
  std::vector<std::string> m_outCatQtyName;       // New output catalogue quantities
  std::vector<std::string> m_outCatQtyFormula;    // New output catalogue quantities
  std::vector<std::string> m_select;              // Selections
  std::string              m_probMethod;          // Probability method
  double                   probThres;             // Probability threshold
  long                     maxNumCtp;             // Maximum # of counterparts
  int                      clobber;               // Clobber flag
  int                      verbose;               // Verbose level
};
inline Parameters::Parameters(void) { init_memory(); }
inline Parameters::~Parameters(void) { free_memory(); }
inline int Parameters::logError(void) { return (verbose > 0); }
inline int Parameters::logAction(void) { return (verbose > 1); }
inline int Parameters::logDetail(void) { return (verbose > 2); }
inline int Parameters::logDebug(void) { return (verbose > 3); }

/* Prototypes _______________________________________________________________ */


/* Namespace ends ___________________________________________________________ */
}
#endif // PARAMETERS_H
