/*----------------------------------------------------------------------------*/
/*                                Parameters.h                                */
/* -------------------------------------------------------------------------- */
/* Task            : Task parameter interface header file.                    */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0.0                                                    */
/* Date of version : 20-May-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/*----------------------------------------------------------------------------*/
#ifndef PARAMETERS_H
#define PARAMETERS_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"


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
  Status load(st_app::AppParGroup &pars, Status status);
  Status dump(Status status);
  int    logTerse(void);                    // Inline
  int    logNormal(void);                   // Inline
  int    logExplicit(void);                 // Inline
  int    logVerbose(void);                  // Inline
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
  double                   m_probThres;           // Probability threshold
  long                     m_maxNumCtp;           // Maximum # of counterparts
  int                      m_chatter;             // Chatter level
  int                      m_clobber;             // Clobber flag
  int                      m_debug;               // Debugging mode activated
  std::string              m_mode;                // Automatic parameter mode
};
inline Parameters::Parameters(void) { init_memory(); }
inline Parameters::~Parameters(void) { free_memory(); }
inline int Parameters::logTerse(void) { return (m_chatter > 0); }
inline int Parameters::logNormal(void) { return (m_chatter > 1); }
inline int Parameters::logExplicit(void) { return (m_chatter > 2); }
inline int Parameters::logVerbose(void) { return (m_chatter > 3); }
inline int Parameters::logDebug(void) { return (m_debug); }

/* Prototypes _______________________________________________________________ */


/* Namespace ends ___________________________________________________________ */
}
#endif // PARAMETERS_H
