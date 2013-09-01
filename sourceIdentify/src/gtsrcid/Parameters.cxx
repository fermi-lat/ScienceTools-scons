/*----------------------------------------------------------------------------*/
/*                               Parameters.cxx                               */
/* -------------------------------------------------------------------------- */
/* Task            : Parameter interface file.                                */
/* Author          : Jurgen Knodlseder CESR (C) (all rights reserved)         */
/* Revision        : 1.0.0                                                    */
/* Date of version : 20-May-2005                                              */
/* -------------------------------------------------------------------------- */
/* History :                                                                  */
/* 1.0.0  20-May-2005  first version                                          */
/*----------------------------------------------------------------------------*/

/* Includes _________________________________________________________________ */
#include <stdio.h>                           // for "sprintf" function
#include "sourceIdentify.h"
#include "Parameters.h"
#include "Log.h"	                     // for parameter dumping/errors

/* Namespace definition _____________________________________________________ */
namespace sourceIdentify {


/* Type defintions __________________________________________________________ */


/* Prototypes _______________________________________________________________ */


/*----------------------------------------------------------------------------*/
/*                           Parameters::init_memory                          */
/* -------------------------------------------------------------------------- */
/* Private method: init memory                                                */
/*----------------------------------------------------------------------------*/
void Parameters::init_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {

      // Intialise private members
      m_srcCatName.clear();
      m_cptCatName.clear();
      m_outCatName.clear();
      m_srcCatQty.clear();
      m_cptCatQty.clear();
      m_outCatQtyName.clear();
      m_outCatQtyFormula.clear();
      m_select.clear();
      m_probMethod.clear();
      m_maxNumCtp = 0;
      m_chatter   = 0;
      m_clobber   = 0;
      m_debug     = 0;
      m_mode.clear();

    } while (0); // End of main do-loop
    
    // Return
    return;

}


/*----------------------------------------------------------------------------*/
/*                            Parameters::free_memory                         */
/* -------------------------------------------------------------------------- */
/* Private member: free memory                                                */
/*----------------------------------------------------------------------------*/
void Parameters::free_memory(void) {

    // Declare local variables

    // Single loop for common exit point
    do {
      
      // Initialise memory
      init_memory();

    } while (0); // End of main do-loop
    
    // Return
    return;

}


/*----------------------------------------------------------------------------*/
/*                              Parameters::load                              */
/* -------------------------------------------------------------------------- */
/* Load parameters from task parameter file.                                  */
/*----------------------------------------------------------------------------*/
Status Parameters::load(st_app::AppParGroup &pars, Status status) {

    // Declare local variables
    long                   i;
    char                   parname[MAX_CHAR];
    std::string::size_type len;
    std::string::size_type pos;
    std::string::size_type len_name;
    std::string::size_type start_formula;
    std::string::size_type len_formula;
        
    // Single loop for common exit point
    do {

      // Fall through in case of an error
      if (status != STATUS_OK)
        continue;

      // Prompt for parameters
      pars.Prompt();

      // Recover task parameters
      std::string s_srcCatName = pars["srcCatName"];
      std::string s_cptCatName = pars["cptCatName"];
      std::string s_outCatName = pars["outCatName"];
      std::string s_srcCatQty  = pars["srcCatQty"];
      std::string s_cptCatQty  = pars["cptCatQty"];
      std::string s_probMethod = pars["probMethod"];
      std::string s_mode       = pars["mode"];
      m_srcCatName             = s_srcCatName;
      m_cptCatName             = s_cptCatName;
      m_outCatName             = s_outCatName;
      m_srcCatQty              = s_srcCatQty;
      m_cptCatQty              = s_cptCatQty;
      m_probMethod             = s_probMethod;
      m_probThres              = pars["probThres"];
      m_maxNumCtp              = pars["maxNumCtp"];
      m_chatter                = pars["chatter"];
      m_clobber                = pars["clobber"];
      m_debug                  = pars["debug"];
      m_mode                   = s_mode;

      // Retrieve new output quantities and decompose into quantity name and
      // evaluation string
      for (i = MIN_OUTCAT_QTY; i <= MAX_OUTCAT_QTY; i++) {
        sprintf(parname, "outCatQty%2.2ld", i);
        std::string outCatQty = pars[parname];
        len = outCatQty.length();
        if (len > 0) {
          pos           = outCatQty.find("=",0);
          len_name      = pos;
          start_formula = pos + 1;
          len_formula   = len - start_formula;
          if (pos == std::string::npos) {
            status = STATUS_PAR_BAD_PARAMETER;
            Log(Error_2, "%d : No equality symbol found in new output catalogue"
                " quantity string <%s='%s'>.", 
                (Status)status, parname, outCatQty.c_str());
            break;
          }
          if (len_name < 1) {
            status = STATUS_PAR_BAD_PARAMETER;
            Log(Error_2, "%d : No quantity name found for new output catalogue"
                " quantity <%s='%s'>.", 
                (Status)status, parname, outCatQty.c_str());
            break;
          }
          if (len_formula < 1) {
            status = STATUS_PAR_BAD_PARAMETER;
            Log(Error_2, "%d : No quantity evaluation string found for new"
                " output catalogue quantity <%s='%s'>.", 
                (Status)status, parname, outCatQty.c_str());
            break;
          }
          m_outCatQtyName.push_back(outCatQty.substr(0, len_name));
          m_outCatQtyFormula.push_back(outCatQty.substr(start_formula,
                                       len_formula));
        }
      }
      if (status != STATUS_OK)
        continue;

      // Retrieve selection strings
      for (i = MIN_OUTCAT_SEL; i <= MAX_OUTCAT_SEL; i++) {
        sprintf(parname, "select%2.2ld", i);
        std::string select = pars[parname];
        len = select.length();
        if (len > 0) {
          m_select.push_back(select);
        }
      }
      if (status != STATUS_OK)
        continue;

    } while (0); // End of main do-loop
    
    // Return status
    return status;

}


/*----------------------------------------------------------------------------*/
/*                              Parameters::dump                              */
/* -------------------------------------------------------------------------- */
/* Dump task parameters into log file.                                        */
/*----------------------------------------------------------------------------*/
Status Parameters::dump(Status status) {

    // Declare local variables
    long                   i;
    std::string::size_type n;
        
    // Single loop for common exit point
    do {

      // Dump task parameters
      Log(Log_1, "Task Parameters:");
      Log(Log_1, "================");
      Log(Log_1, " Source catalogue filename ........: %s", 
          m_srcCatName.c_str());
      Log(Log_1, " Counterpart catalogue name .......: %s", 
          m_cptCatName.c_str());
      Log(Log_1, " Output catalogue name ............: %s", 
          m_outCatName.c_str());
      Log(Log_1, " Source catalogue quantities ......: %s", 
          m_srcCatQty.c_str());
      Log(Log_1, " Counterpart catalogue quantities .: %s", 
          m_cptCatQty.c_str());
      if ((n = m_outCatQtyName.size()) > 0) {
        for (i = 0; i < n; i++) {
          Log(Log_1, " New output catalogue quantity %2d .: %s = %s", 
              i+1, m_outCatQtyName[i].c_str(), m_outCatQtyFormula[i].c_str());
        }
      }
      Log(Log_1, " Probability method ...............: %s", 
          m_probMethod.c_str());
      Log(Log_1, " Probability threshold ............: %e", m_probThres);
      Log(Log_1, " Maximum number of counterparts  ..: %d", m_maxNumCtp);
      if ((n = m_select.size()) > 0) {
        for (i = 0; i < n; i++) {
          Log(Log_1, " Output catalogue selection %2d ....: %s", 
              i+1, m_select[i].c_str());
        }
      }
      Log(Log_1, " Chatter level of output ..........: %d", m_chatter);
      Log(Log_1, " Clobber ..........................: %d", m_clobber);
      Log(Log_1, " Debugging mode activated .........: %d", m_debug);
      Log(Log_1, " Mode of automatic parameters .....: %s", m_mode.c_str());
    
    } while (0); // End of main do-loop
    
    // Return status
    return status;

}


/* Namespace ends ___________________________________________________________ */
}
