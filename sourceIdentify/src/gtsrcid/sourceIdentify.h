/*----------------------------------------------------------------------------*/
/*                              sourceIdentify.h                              */
/* -------------------------------------------------------------------------- */
/* Task            : Source identification header file.                       */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.3.1                                                    */
/* Date of version : 20-Dec-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/* 1.1.0  21-Jul-2005  add debug information                                  */
/* 1.2.0  26-Sep-2005  - adapted generic quantity names to U9 (v0r2p3)        */
/*                     - add UCD keywords to output FITS file                 */
/* 1.3.0  19-Dec-2005  - prefix class members by "m_"                         */
/*                     - introduce maximum acceptance angle for filter step   */
/*                     - extract counterpart locations only once              */
/* 1.3.1  20-Dec-2005  - treat R.A. wrap around correctly                     */
/*----------------------------------------------------------------------------*/
#ifndef SOURCEIDENTIFY_H
#define SOURCEIDENTIFY_H

/* Definitions ______________________________________________________________ */
#define TOOL_NAME     "gtsrcid"
#define TOOL_VERSION  "gtsrcid-1.3.1"
#define TOOL_LOGFILE  "gtsrcid.log"
#define USE_ST_APP 1

/* Includes _________________________________________________________________ */
#include <string>
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

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
  STATUS_CAT_EMPTY         = -100301              // Catalogue empty
} Status;

/* Prototypes _______________________________________________________________ */

/* Globals __________________________________________________________________ */

/* Namespace ends ___________________________________________________________ */
}
#endif // SOURCEIDENTIFY_H
