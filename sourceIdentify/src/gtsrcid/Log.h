/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.3  2007/09/20 14:22:06  jurgen
Use application name from sourceIdentify.h file

Revision 1.2  2006/02/01 13:33:36  jurgen
Tried to fix Win32 compilation bugs.
Change revision number to 1.3.2.
Replace header information with CVS typeset information.

------------------------------------------------------------------------------*/
#ifndef LOG_H
#define LOG_H

/* Includes _________________________________________________________________ */
#include "sourceIdentify.h"


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Defintions _______________________________________________________________ */
#define DEFAULT_LOG_FILENAME  TOOL_LOGFILE
#define DEFAULT_TASK_NAME     TOOL_NAME


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
