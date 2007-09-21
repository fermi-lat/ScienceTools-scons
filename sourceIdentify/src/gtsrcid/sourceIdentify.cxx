/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.10  2007/09/20 14:16:18  jurgen
Improve st_app handling (dump version)

Revision 1.9  2006/03/02 02:01:54  jurgen
Set hidden parameters to meaningful values

Revision 1.8  2006/02/07 16:05:05  jurgen
Use ObjectInfo structure to hold catalogue object information

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
#include <time.h>            // for "clock_t" type
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
/*                                  gtsrcid                                   */
/* -------------------------------------------------------------------------- */
/* Task: Base class for gtsrcid application.                                  */
/*----------------------------------------------------------------------------*/
class gtsrcid : public st_app::StApp {
public:

    // Construct gtsrcid application and specify name and version
    gtsrcid(): st_app::StApp() {
      setName(TOOL_NAME);
      setVersion(TOOL_VERSION);
    }

    // Destructor
    virtual ~gtsrcid() throw() { }

    // Standard main for gtsrcid application
    virtual void run() {

      // Declare (and initialise) variables
      Status     status = STATUS_OK;
      clock_t    t_start;
      Parameters par;
      Catalogue  cat;

      // Main do-loop to fall through in case of an error
      do {

        // Save the execution start time
        t_start = clock();
      
        // Initialise log file
        status = LogInit(TOOL_LOGFILE, TOOL_VERSION, status);
        if (status != STATUS_OK)
          continue;

        // Get parameter file object
        st_app::AppParGroup &pars(getParGroup(TOOL_NAME));

        // Load task parameters
        status = par.load(pars, status);
        if (status != STATUS_OK) {
          if (par.logTerse())
            Log(Error_3, "%d : Error while loading task parameters.", status);      
          continue;
        }

        // Dump header into log file
        if (par.logTerse()) {
          Log(Log_1, HD_BORDER);
          Log(Log_1, HD_NAME);
          Log(Log_1, HD_SEP);
          Log(Log_1, HD_VERSION);
          Log(Log_1, HD_DATE);
          Log(Log_1, HD_AUTHOR);
          Log(Log_1, HD_BORDER);
        }

        // Dump task parameters
        if (par.logTerse()) {
          status = par.dump(status);
          if (status != STATUS_OK) {
            if (par.logTerse())
              Log(Error_3, "%d : Error while dumping task parameters.", status);      
            continue;
          }
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
      clock_t t_stop   = clock();
      double  t_elapse = (double)(t_stop - t_start) / (double)CLOCKS_PER_SEC;

      // Dump termination message
      if (par.logTerse())
        Log(Log_1, "Task terminated using %.3f sec CPU time.", t_elapse);
   
      // Finish log file
      status = LogClose(status);
    
      // Return from task
//    return 0;
    }

private:
    std::string m_app_name;
};

// Create factory object which can create the application
st_app::StAppFactory<gtsrcid> g_app_factory("gtsrcid");

/**
 * @file   sourceIdentify.cxx
 * @brief  Source identification main program.
 * @author J. Knodlseder
 *
 * $Header$
 */
