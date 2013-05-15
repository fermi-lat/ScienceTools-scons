#include <crtdbg.h>
#include <iostream>

/* 
 * Define our own reporting function.
 * We'll hook it into the debug reporting
 * process later using _CrtSetReportHook.
 */
int AssertDialogOverride( int reportType, char *userMessage, int *retVal )
{
   /*
    * Tell the user our reporting function is being called.
    * In other words - verify that the hook routine worked.
    */
	std::cout << "Overriding assertion dialog box." << std::endl;
	
   /*
    * By setting retVal to zero, we are instructing _CrtDbgReport
    * to continue with normal execution after generating the report.
    * If we wanted _CrtDbgReport to start the debugger, we would set
    * retVal to one.
    */
   retVal = 0;
	
   /*
    * When the report type is for an ASSERT,
    * we'll redirect to std::cout instead of having a dialog appear
	* so we'll return false and have processing stop here.
    *
    * When the report type is a WARNing or ERROR,
    * we'll just pass it on to the system so return true.
    */
   if (reportType == _CRT_ASSERT)
   {
	  std::cout << "Assertion failure:" << std::endl;
	  std::cout << userMessage << std::endl;
      return(false);
   } else {
	  std::cout << "Warning or Error:" << std::endl;
	  std::cout << userMessage << std::endl;
      return(true);
   }
}