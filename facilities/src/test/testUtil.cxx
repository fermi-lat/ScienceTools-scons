/// @file testUtil.cxx
#include <string>
#include <iostream>
#include "facilities/Util.h"
/** Miniscule program to test a couple modes of Util::expandEnvVar
 *  Caller should have an environment variable SRC with some
 *  sensible definition.
 */
int main(int, char**) {
  std::string name = std::string("{FACILITIESROOT}/src");
  std::string oDelim = std::string ("{");
  std::string cDelim = std::string ("}");
  int ntrans;

  try {
    ntrans = facilities::Util::expandEnvVar(&name, oDelim, cDelim);
    std::cout << "Translated name is " << name << std::endl;  }
  catch (facilities::Untranslatable err) {
    std::cout << "Failed to translate " << err.badVar << std::endl;
  }



  // Process running this must have environment variable SRC
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  try {
    ntrans = facilities::Util::expandEnvVar(&multi);

    std::cout << "Translated name is " << multi << std::endl;
    std::cout << ntrans << " variables were translated." << std::endl;
  }
  catch (facilities::Untranslatable err) {
    std::cout << "Failed to translate " << err.badVar << std::endl;
  }

  // Test the new itoa routine
  std::string intStr;
  facilities::Util::itoa(12, intStr);
  std::cout << "My String is " << intStr << std::endl;
  return 0;
}

  
