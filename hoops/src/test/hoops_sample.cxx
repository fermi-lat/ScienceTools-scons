/******************************************************************************
 *   File name: hoops_sample.cxx                                              *
 *                                                                            *
 * Description: Sample tool main showing how to use hoops in the most         *
 *              natural, simple fashion.                                      *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#include <iostream>

#include "hoops/hoops.h"
#include "hoops/hoops_prompt_group.h"

int main(int argc, char * argv[]) {
  using namespace hoops;
  int status = 0;

  try {
    ParPromptGroup pars(argc, argv);

    // Prompt for all parameters in the order in the par file:
//    pars.Prompt();

    // Or prompt for just selected parameters:
    pars.Prompt("test_int");
    pars.Prompt("test_real");

    // Refer to any parameters you want:
    double d = pars["test_real"];
    std::cout << "test_real is " << d << std::endl;

    int ii = pars["test_int"];
    std::cout << "test_int is " << ii << std::endl;

    // You can also assign to them:
    pars["test_real"] = 2 * d;
    std::cout << "2 * test_real is " << double(pars["test_real"]) << std::endl;

    // Reset test_real's value so that when it's saved it will be what the
    // user entered:
    pars["test_real"] = d;

    // If you try something ill-advised:
    try {
      pars["test_real"] = "A real value -- NOT!";
      std::cerr << "You should not see this." << std::endl;
    } catch (Hexception & x) {
      // An exception is thrown indicating an invalid string conversion:
      assert(P_STR_INVALID == x.Code());

      // The desired value of test_real was destroyed by the
      // bad conversion above, so reset it again to what it was before:
      pars["test_real"] = d;
      // (This is a feature: hoops converts the value no matter what
      // in case you want to ignore an exception, e.g. changing
      // signedness.)
    }

    // If you want to lose the changes you just made, just load
    // the parameters from the original file again:
    // pars.Load();

    // You can save them explicitly if you want, but ~ParPromptGroup()
    // will also save them.
    // pars.Save(); // Not wrong, but not necessary.

  } catch (Hexception & x) {
    std::cerr << "Error " << x.Code() << ": " << x.Msg() << std::endl;
    status = 1;
  }

  return status;
}

/******************************************************************************
 * Revision 1.4  2004/03/16 15:03:29  peachey
 * Do not throw exceptions any more when converting from one type to a
 * potentially smaller type.
 *
 * Revision 1.3  2004/03/16 14:40:02  peachey
 * Change sample to use new, more convenient ParPromptGroup class.
 *
 * Revision 1.2  2004/03/15 14:13:14  peachey
 * A couple corrections to make the sample actually work properly.
 *
 ******************************************************************************/
