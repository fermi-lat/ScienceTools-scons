/*----------------------------------------------------------------------------*/
/*                                  Log.cxx                                   */
/* -------------------------------------------------------------------------- */
/* Task            : Task logging interface file.                             */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0.0                                                    */
/* Date of version : 20-May-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/*----------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include <stdio.h>      // for "FILE" type
#include <string.h>     // for "memcpy" function
#include <stdarg.h>     // for "va_list" type
#include <time.h>       // for time functions
#include "sourceIdentify.h"
#include "Log.h"


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Globals __________________________________________________________________ */
char  gLogTaskName[100];
FILE *gLogFilePtr = NULL;


/* Type defintions __________________________________________________________ */


/* Prototypes _______________________________________________________________ */


/*----------------------------------------------------------------------------*/
/*                                  LogInit                                   */
/* -------------------------------------------------------------------------- */
/* Task: Initialise task logging.                                             */
/*----------------------------------------------------------------------------*/
Status LogInit(const char *logName, const char *taskName, Status status) {

    // Declare (and initialise) variables

    // Main do-loop to fall through in case of an error
    do {
    
      // Clean log task name
      sprintf(gLogTaskName, "");
    
      // If there is already a log file opened then close it first
      if (gLogFilePtr != NULL) {
        status = LogClose(status);
        if (status != STATUS_OK)
          continue;
        else
          gLogFilePtr = NULL;
      }
    
      // Open log file
      if (gLogFilePtr == NULL)
        gLogFilePtr = fopen(logName, "w");
      else {
        status = STATUS_LOG_OPEN_FAILED;
        continue;
      }
      
      // If log file is not opened then signal an error
      if (gLogFilePtr == NULL) {
        status = STATUS_LOG_OPEN_FAILED;
        continue;
      }

      // Store log task name
      sprintf(gLogTaskName, "%s", taskName);

    } while (0); // End of main do-loop

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                                 LogClose                                   */
/* -------------------------------------------------------------------------- */
/* Task: Finish task logging.                                                 */
/*----------------------------------------------------------------------------*/
Status LogClose(Status status) {

    // Declare (and initialise) variables

    // Main do-loop to fall through in case of an error
    do {
    
      // Close log file
      if (gLogFilePtr != NULL)
        fclose(gLogFilePtr);
      else {
        status = STATUS_LOG_CLOSE_FAILED;
        continue;
      }

    } while (0); // End of main do-loop

    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                                    Log                                     */
/* -------------------------------------------------------------------------- */
/* Task: Log message.                                                         */
/*----------------------------------------------------------------------------*/
Status Log(MessageType msgType, const char *msgFormat, ...) {

    // Declare (and initialise) variables
    Status    status = STATUS_OK;
    va_list   vl;
    time_t    now;
    struct tm timeStruct;
    char      type[100];

    // Main do-loop to fall through in case of an error
    do {
    
      // If no log file has been opened then open one now
      if (gLogFilePtr == NULL) {
        status = LogInit(DEFAULT_LOG_FILENAME, DEFAULT_TASK_NAME, status);
        if (status != STATUS_OK)
          continue;
      }

      // Set message type
      switch (msgType) {
      case Error_0:
        sprintf(type, "Error_0");
        break;
      case Error_1:
        sprintf(type, "Error_1");
        break;
      case Error_2:
        sprintf(type, "Error_2");
        break;
      case Error_3:
        sprintf(type, "Error_3");
        break;
      case Warning_0:
        sprintf(type, "Warn_0 ");
        break;
      case Warning_1:
        sprintf(type, "Warn_1 ");
        break;
      case Warning_2:
        sprintf(type, "Warn_2 ");
        break;
      case Warning_3:
        sprintf(type, "Warn_3 ");
        break;
      case Alert_0:
        sprintf(type, "Alert_0");
        break;
      case Alert_1:
        sprintf(type, "Alert_1");
        break;
      case Alert_2:
        sprintf(type, "Alert_2");
        break;
      case Alert_3:
        sprintf(type, "Alert_3");
        break;
      case Log_0:
        sprintf(type, "Log_0  ");
        break;
      case Log_1:
        sprintf(type, "Log_1  ");
        break;
      case Log_2:
        sprintf(type, "Log_2  ");
        break;
      case Log_3:
        sprintf(type, "Log_3  ");
        break;
      default:
        sprintf(type, "Unknown");
        break;
      }
      
      // Get time
      now = time(NULL);
      #ifdef HAVE_GMTIME_R   
        gmtime_r(&now, &timeStruct);
      #else
        memcpy(&timeStruct, gmtime(&now), sizeof(struct tm));      
      #endif
    
      // Write message type, time and task name to log file
      if (fprintf(gLogFilePtr, "%s %04d-%02d-%02dT%02d:%02d:%02d %s: ", 
                  type, 
                  timeStruct.tm_year + 1900,
                  timeStruct.tm_mon + 1,
                  timeStruct.tm_mday,
                  timeStruct.tm_hour,
                  timeStruct.tm_min,
                  timeStruct.tm_sec,
                  gLogTaskName) < 0) {
        status = STATUS_LOG_WRITE_FAILED;
        continue;
      }

      // Write message to log file
      va_start(vl, msgFormat);
      if (vfprintf(gLogFilePtr, msgFormat, vl) < 0) {
        status = STATUS_LOG_WRITE_FAILED;
        continue;
      }
      va_end(vl);
      
      // Write <CR> to log file
      if (fprintf(gLogFilePtr, "\n") != 1) {
        status = STATUS_LOG_WRITE_FAILED;
        continue;
      }

    } while (0); // End of main do-loop

    // Return status
    return status;

}

/* Namespace ends ___________________________________________________________ */
}
/**
 * @file   Log.cxx
 * @brief  Task logging routines.
 * @author J. Knodlseder
 *
 * $Header$
 */
