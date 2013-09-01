/*----------------------------------------------------------------------------*/
/*                             sourceIdentify.cxx                             */
/* -------------------------------------------------------------------------- */
/* Task            : Source identification.                                   */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0                                                      */
/* Date of version : 25-Feb-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0   25-Feb-2005  first version                                           */
/* -------------------------------------------------------------------------- */

/* Includes _________________________________________________________________ */
#include <time.h>    // for "clock_t" type
#include "sourceIdentify/sourceIdentify.h"
#include "sourceIdentify/Parameters.h"
#include "sourceIdentify/Log.h"
#include "sourceIdentify/Catalogue.h"


/* Namespace usage __________________________________________________________ */
using namespace sourceIdentify;


/* Globals __________________________________________________________________ */


/* Constants ________________________________________________________________ */


/* Type defintions __________________________________________________________ */


/* Prototypes _______________________________________________________________ */


#if USE_ST_APP
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
      status = LogInit("sourceIdentify.log", "sourceIdentify-1.0", status);
      if (status != STATUS_OK)
        continue;

      // Dump header
      Log(Log_1, 
         "************************************************************");
      Log(Log_1, 
         "*                      sourceIdentify                      *");
      Log(Log_1, 
         "* -------------------------------------------------------- *");
      Log(Log_1, 
         "* Revision : 1.0                                           *");
      Log(Log_1, 
         "* Date     : 25 Februar 2005                               *");
      Log(Log_1, 
         "* Author   : Jurgen Knodlseder (CESR)                      *");
      Log(Log_1, 
         "************************************************************");

      st_app::AppParGroup &pars(getParGroup("sourceIdentify"));

      // Load task parameters
      status = par.load(pars, status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while loading task parameters.", status);      
        continue;
      }

      // Dump task parameters
      status = par.dump(status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while dumping task parameters.", status);      
        continue;
      }
       
      // Build counterpart catalogue
      status = cat.build(&par, status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while building counterpart candidate"
                       " catalogue.", status);      
        continue;
      }

      // Save counterpart candidate catalogue
      status = cat.save(&par, status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while saving counterpart candidate"
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
st_app::StAppFactory<application> g_factory("sourceIdentify");
#else
/*----------------------------------------------------------------------------*/
/*                                   main                                     */
/* -------------------------------------------------------------------------- */
/* Task: Main function call.                                                  */
/*----------------------------------------------------------------------------*/
int main (int argc, char *argv[]) {

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
      status = LogInit("sourceIdentify.log", "sourceIdentify-1.0", status);
      if (status != STATUS_OK)
        continue;

      // Dump header
      Log(Log_1, 
         "************************************************************");
      Log(Log_1, 
         "*                      sourceIdentify                      *");
      Log(Log_1, 
         "* -------------------------------------------------------- *");
      Log(Log_1, 
         "* Revision : 1.0                                           *");
      Log(Log_1, 
         "* Date     : 21 November 2004                              *");
      Log(Log_1, 
         "* Author   : Jurgen Knodlseder (CESR)                      *");
      Log(Log_1, 
         "************************************************************");

      // Load task parameters
      status = par.load(status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while loading task parameters.", status);      
        continue;
      }

      // Dump task parameters
      status = par.dump(status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while dumping task parameters.", status);      
        continue;
      }
       
      // Build counterpart catalogue
      status = cat.build(&par, status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while building counterpart candidate"
                       " catalogue.", status);      
        continue;
      }

      // Save counterpart candidate catalogue
      status = cat.save(&par, status);
      if (status != STATUS_OK) {
        if (par.logError())
          Log(Error_3, "%d : Error while saving counterpart candidate"
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
    return 0;

}
#endif
/**
 * @file   sourceIdentify.cxx
 * @brief  Source identification main program.
 * @author J. Knodlseder
 *
 * $Header$
 */
