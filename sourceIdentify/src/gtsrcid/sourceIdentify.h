/*------------------------------------------------------------------------------
Id ........: $Id$
Author ....: $Author$
Revision ..: $Revision$
Date ......: $Date$
--------------------------------------------------------------------------------
$Log$
Revision 1.32  2010/01/12 08:57:23  jurgen
Correct NULL entries for N_ID=1

Revision 1.31  2009/12/01 13:25:24  jurgen
Correct FoM implementation

Revision 1.30  2009/07/07 21:32:52  jurgen
Correctly read binary double columns

Revision 1.29  2009/03/26 14:15:05  jurgen
Properly handle NULL error radii (by assuming an minimum 1D 1sigma error of 0.005 deg)

Revision 1.28  2009/03/18 10:06:58  jurgen
Update version number

Revision 1.27  2008/11/26 10:34:35  jurgen
Prepare to tag v2r1p0

Revision 1.26  2008/08/20 11:52:21  jurgen
Correct probability computation and resolve STGEN-56

Revision 1.25  2008/04/23 15:09:21  jurgen
Check for catch-22 in upper case

Revision 1.24  2008/04/23 14:12:03  jurgen
Implement zero-argument special functions nsrc(), nlat() and ncpt()

Revision 1.23  2008/04/15 21:24:12  jurgen
Introduce sparse matrix for source catalogue probability computation.

Revision 1.22  2008/03/21 09:10:12  jurgen
Enhance code documentation.

Revision 1.21  2008/03/20 21:56:26  jurgen
implement local counterpart density

Revision 1.20  2008/03/20 12:17:44  jurgen
Invert _RA/_DE and RA/DE column name search

Revision 1.19  2008/03/20 11:03:17  jurgen
Update version number

Revision 1.18  2008/02/23 10:52:16  jurgen
update tag

Revision 1.17  2007/12/06 16:42:15  jurgen
Add RA/DEC and PosErr generic names

Revision 1.16  2007/11/30 16:19:26  jurgen
Correct version number and add RAdeg/DEdeg columns

Revision 1.15  2007/11/08 14:42:11  jurgen
Handle error circles (e.g. 3EG catalogue)

Revision 1.14  2007/10/11 15:39:27  jurgen
Update release date

Revision 1.13  2007/10/10 15:39:12  jurgen
Introduce handling of special functions 'gammln', 'erf', and 'erfc'

Revision 1.12  2007/10/08 11:02:25  jurgen
Implement search for catalogue table information and handle different
position error types

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
/**
 * @file sourceIdentify.h
 * @brief sourceIdentify executable definition.
 * @author J. Knodlseder
 */

#ifndef SOURCEIDENTIFY_H
#define SOURCEIDENTIFY_H

/* Definitions ______________________________________________________________ */
#define TOOL_NAME     "gtsrcid"
#define TOOL_VERSION  "v2r2p4"
#define TOOL_LOGFILE  "gtsrcid.log"
#define TOOL_DATE     "12-Jan-2009"
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
  STATUS_CAT_INCOMPATIBLE  = -100303,             // Incompatible dimensions
  STATUS_CAT_BAD_SPARSE    = -100304,             // Sparse matrix error
  STATUS_CAT_NO_ID         = -100400,             // No ID column
  STATUS_CAT_NO_POS        = -100401,             // No position columns
  STATUS_CAT_NO_POS_ERROR  = -100402,             // No position error columns
  STATUS_CAT_SEL_FAILED    = -100403,             // Selection failed
  STATUS_FCT_NOT_FOUND     = -100500,             // Function not found
  STATUS_FCT_INVALID       = -100502,             // Invalid function
  STATUS_FCT_NO_CLOSING    = -100503,             // No closing parenthesis
  STATUS_FCT_BAD_NUM_ARG   = -100504              // Bad number of function args
} Status;

/* Prototypes _______________________________________________________________ */


/* Globals __________________________________________________________________ */
extern int g_u9_verbosity;


/* Namespace ends ___________________________________________________________ */
}
#endif // SOURCEIDENTIFY_H
