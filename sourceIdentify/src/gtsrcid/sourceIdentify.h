/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.11  2007/09/21 14:29:03  jurgen
Correct memory bug and updated test script

Revision 1.10  2007/09/20 14:16:18  jurgen
Improve st_app handling (dump version)

Revision 1.9  2006/03/02 02:01:54  jurgen
Set hidden parameters to meaningful values

Revision 1.8  2006/02/14 15:38:54  jurgen
Set the version string to "v1r0p3"

Revision 1.7  2006/02/07 16:05:05  jurgen
Use ObjectInfo structure to hold catalogue object information

Revision 1.6  2006/02/07 11:10:51  jurgen
Suppress catalogAccess verbosity

Revision 1.5  2006/02/03 12:14:52  jurgen
New version that allows additional probabilities to be taken
into account. The code has been considerably reorganised. Also
catalogue column prefixes are now handled differently.

Revision 1.4  2006/02/02 09:44:43  jurgen
Remove doxygen documentation and set revision number to v0r5

Revision 1.3  2006/02/01 13:33:37  jurgen
Tried to fix Win32 compilation bugs.
Change revision number to 1.3.2.
Replace header information with CVS typeset information.

------------------------------------------------------------------------------*/
#ifndef SOURCEIDENTIFY_H
#define SOURCEIDENTIFY_H

/* Definitions ______________________________________________________________ */
#define TOOL_NAME     "gtsrcid"
#define TOOL_VERSION  "v1r3p0"
#define TOOL_LOGFILE  "gtsrcid.log"
#define TOOL_DATE     " 7-Oct-2007"
#define HD_BORDER     "************************************************************"
#define HD_SEP        "* -------------------------------------------------------- *"
#define HD_NAME       "*                          gtsrcid                         *"
#define HD_VERSION    "* Revision : "TOOL_VERSION"                                        *"
#define HD_DATE       "* Date     : "TOOL_DATE"                                   *"
#define HD_AUTHOR     "* Author   : Jurgen Knodlseder (CESR)                      *"
#define USE_ST_APP 1

/* Includes _________________________________________________________________ */
#include <string>
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

/* Architecture specific definitions ________________________________________ */
#ifndef EXPSYM
#ifdef WIN32
#define EXPSYM __declspec(dllexport)
#else
#define EXPSYM
#endif
#endif


/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Type defintions __________________________________________________________ */
typedef enum  {                                  // Function status code
  STATUS_OK                =       0,             // Function status ok
  STATUS_MEM_ALLOC         = -100000,             // Memory allocation failure
  STATUS_LOG_OPEN_FAILED   = -100100,             // Log file open error
  STATUS_LOG_CLOSE_FAILED  = -100101,             // Log file close error
  STATUS_LOG_WRITE_FAILED  = -100102,             // Log file write error
  STATUS_PAR_BAD_PARAMETER = -100200,             // Bad task parameter
  STATUS_CAT_NOT_FOUND     = -100300,             // Catalogue not found
  STATUS_CAT_EMPTY         = -100301,             // Catalogue empty
  STATUS_CAT_BAD_PROB_COL  = -100302,             // Bad probability coltype
  STATUS_CAT_NO_ID         = -100400,             // No ID column
  STATUS_CAT_NO_POS        = -100401,             // No position columns
  STATUS_CAT_NO_POS_ERROR  = -100402              // No position error columns
} Status;

/* Prototypes _______________________________________________________________ */


/* Globals __________________________________________________________________ */
extern int g_u9_verbosity;


/* Namespace ends ___________________________________________________________ */
}
#endif // SOURCEIDENTIFY_H
