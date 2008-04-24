/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.12  2008/04/18 20:50:33  jurgen
Implement catch-22 scheme for prior probability calculation and compute log likelihood-ratio instead of likelihood ratio (avoid numerical problems)

Revision 1.11  2008/03/26 13:37:10  jurgen
Generalize probability calculation and implement Bayesian method

Revision 1.10  2008/03/21 09:10:12  jurgen
Enhance code documentation.

Revision 1.9  2008/03/20 21:56:26  jurgen
implement local counterpart density

Revision 1.8  2007/10/09 08:17:40  jurgen
Correctly interpret positional errors and correctly evaluate PROB_POS
as likelihood

Revision 1.7  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

Revision 1.6  2007/10/02 22:01:16  jurgen
Change parameter name maxNumCtp to maxNumCpt

Revision 1.5  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

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
 * @file Parameters.h
 * @brief Parameters class interface definition.
 * @author J. Knodlseder
 */

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


/* Classes __________________________________________________________________ */
class Parameters {
public:

  // Friend classes
  friend class Catalogue;

  // Constructor & destructor
  Parameters(void);                            // Inline
 ~Parameters(void);                            // Inline

  // Public methods
  Status load(st_app::AppParGroup &pars, Status status);
  Status dump(Status status);
  int    logTerse(void);                       // Inline
  int    logNormal(void);                      // Inline
  int    logExplicit(void);                    // Inline
  int    logVerbose(void);                     // Inline
  int    logDebug(void);                       // Inline

  // Private methods
private:
  void   init_memory(void);
  void   free_memory(void);

private:
  std::string              m_srcCatName;       //!< Source catalogue name
  std::string              m_srcCatPrefix;     //!< Source catalogue prefix
  std::string              m_srcCatQty;        //!< Source catalogue quantities
  double                   m_srcPosError;      //!< Source catalogue default error
  std::string              m_cptCatName;       //!< Counterpart catalogue name
  std::string              m_cptCatPrefix;     //!< Counterpart catalogue prefix
  std::string              m_cptCatQty;        //!< Counterpart catalogue quantities
  double                   m_cptPosError;      //!< Counterpart catalogue def. error
  std::string              m_cptDensFile;      //!< Counterpart catalogue density file
  std::string              m_outCatName;       //!< Output catalogue name
  std::string              m_probMethod;       //!< Association probability formula
  std::string              m_probPrior;        //!< Prior probability formula
  double                   m_probThres;        //!< Probability threshold
  long                     m_maxNumCpt;        //!< Maximum # of counterparts
  std::string              m_FoM;              //!< Figure of merit
  int                      m_catch22;          //!< Perform catch-22 iterations
  std::vector<std::string> m_outCatQtyName;    //!< New output catalogue quantities
  std::vector<std::string> m_outCatQtyFormula; //!< New output catalogue formulae
  std::vector<std::string> m_select;           //!< Selections
  int                      m_chatter;          //!< Chatter level
  int                      m_clobber;          //!< Clobber flag
  int                      m_debug;            //!< Debugging mode activated
  std::string              m_mode;             //!< Automatic parameter mode
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
