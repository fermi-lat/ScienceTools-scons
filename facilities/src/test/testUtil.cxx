/// @file testUtil.cxx
#include <string>
#include <iostream>
#include "facilities/Util.h"
/** Miniscule program to test a couple modes of Util::expandEnvVar
 *  Caller should have an environment variable SRC with some
 *  sensible definition.
 */
main() {
  std::string name = std::string("{FACILITIESROOT}/src");
  std::string oDelim = std::string ("{");
  std::string cDelim = std::string ("}");

  int ntrans = facilities::Util::expandEnvVar(&name, oDelim, cDelim);

  std::cout << "Translated name is " << name << std::endl;

  // Process running this must have environment variable SRC
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  ntrans = facilities::Util::expandEnvVar(&multi);

  std::cout << "Translated name is " << multi << std::endl;
  std::cout << ntrans << " variables were translated." << std::endl;

  // Test the new itoa routine
  const char *str = facilities::Util::itoa(12);
  std::cout << "My String is " << str << std::endl;
}

  
