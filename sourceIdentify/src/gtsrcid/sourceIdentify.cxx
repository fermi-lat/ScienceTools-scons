/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.7  2006/02/07 11:10:51  jurgen
Suppress catalogAccess verbosity

Revision 1.6  2006/02/03 12:14:52  jurgen
New version that allows additional probabilities to be taken
into account. The code has been considerably reorganised. Also
catalogue column prefixes are now handled differently.

Revision 1.5  2006/02/02 09:44:43  jurgen
Remove doxygen documentation and set revision number to v0r5

Revision 1.4  2006/02/01 15:59:35  jurgen
Don't devide by CLOCKS_PER_SEC

Revision 1.3  2006/02/01 13:33:37  jurgen
Tried to fix Win32 compilation bugs.
Change revision number to 1.3.2.
Replace header information with CVS typeset information.

------------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include <time.h>    // for "clock_t" type
#include "sourceIdentify.h"
#include "Parameters.h"
#include "Log.h"
#include "Catalogue.h"


/* Namespace usage __________________________________________________________ */
using namespace sourceIdentify;


/* Globals __________________________________________________________________ */



/* Constants ________________________________________________________________ */


/* Type defintions __________________________________________________________ */


/* Prototypes _______________________________________________________________ */


/*----------------------------------------------------------------------------*/
/*                                application                                 */
/* -------------------------------------------------------------------------- */
/* Task: Main application class.                                              */
/*----------------------------------------------------------------------------*/
class application : public st_app::StApp {
public:

    // Constructor
    application(): m_f("application", "", 2) {}

    // Action
    virtual void run() {

    // Declare (and initialise) variables
    Status      status = STATUS_OK;
    clock_t     t_start;
    clock_t     t_stop;
    float       t_elapse;
    Parameters  par;
    Catalogue   cat;

    // Main do-loop to fall through in case of an error
    do {

      // Save the execution start time
      t_start = clock();
      
      // Initialise log file
      status = LogInit(TOOL_LOGFILE, TOOL_VERSION, status);
      if (status != STATUS_OK)
        continue;

      // Dump header
      Log(Log_1, 
         "************************************************************");
      Log(Log_1, 
         "*                sourceIdentify (gtsrcid)                  *");
      Log(Log_1, 
         "* -------------------------------------------------------- *");
      Log(Log_1, 
         "* Revision : v1r0p2                                        *");
      Log(Log_1, 
         "* Date     : 7 February 2006                               *");
      Log(Log_1, 
         "* Author   : Jurgen Knodlseder (CESR)                      *");
      Log(Log_1, 
         "************************************************************");

      // ...
      st_app::AppParGroup &pars(getParGroup(TOOL_NAME));

      // Load task parameters
      status = par.load(pars, status);
      if (status != STATUS_OK) {
        if (par.logTerse())
          Log(Error_3, "%d : Error while loading task parameters.", status);      
        continue;
      }

      // Dump task parameters
      status = par.dump(status);
      if (status != STATUS_OK) {
        if (par.logTerse())
          Log(Error_3, "%d : Error while dumping task parameters.", status);      
        continue;
      }
       
      // Build counterpart catalogue
      status = cat.build(&par, status);
      if (status != STATUS_OK) {
        if (par.logTerse())
          Log(Error_3, "%d : Error while building counterpart candidate"
                       " catalogue.", status);      
        continue;
      }

    } while (0); // End of main do-loop
 
    // Save the execution stop time and calculate elapsed time
    t_stop = clock();
    t_elapse = (float)(t_stop - t_start) / (float)CLOCKS_PER_SEC;

    // Dump termination message
    Log(Log_1, "Task terminated using %.3f sec CPU time.", t_elapse);
   
    // Finish log file
    status = LogClose(status);
    
    // Return from task
//    return 0;
}
private:
    st_stream::StreamFormatter m_f;
};

// Create instance of tool
st_app::StAppFactory<application> g_factory(TOOL_NAME);

/**
 * @file   sourceIdentify.cxx
 * @brief  Source identification main program.
 * @author J. Knodlseder
 *
 * $Header$
 */
