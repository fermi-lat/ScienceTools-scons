/*----------------------------------------------------------------------------*/
/*                                   Log.h                                    */
/* -------------------------------------------------------------------------- */
/* Task            : Task logging interface header file.                      */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0                                                      */
/* Date of version : 25-Feb-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0   25-Feb-2005  first version                                           */
/*----------------------------------------------------------------------------*/
#ifndef LOG_H
#define LOG_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify/sourceIdentify.h"


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Defintions _______________________________________________________________ */
#define DEFAULT_LOG_FILENAME  "sourceIdentify.log"
#define DEFAULT_TASK_NAME     "sourceIdentify"


/* Type defintions __________________________________________________________ */
typedef enum {
  Error_0 = 1,
  Error_1,
  Error_2,
  Error_3,
  Warning_0,   
  Warning_1, 
  Warning_2, 
  Warning_3,
  Alert_0,     
  Alert_1,   
  Alert_2,   
  Alert_3,
  Log_0,       
  Log_1,     
  Log_2,     
  Log_3
} MessageType;


/* Classes __________________________________________________________________ */


/* Prototypes _______________________________________________________________ */
Status LogInit(const char *logName, const char *taskName, Status status);
Status LogClose(Status status);
Status Log(MessageType msgType, const char *msgFormat, ...);
//           __attribute__ ((format (printf, 3, 4)));


/* Namespace ends ___________________________________________________________ */
}
#endif // LOG_H
