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
    std::cout << "Failed to completely translate " << name << std::endl;
  }



  // Process running this must have environment variable SRC
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  try {
    ntrans = facilities::Util::expandEnvVar(&multi);

    std::cout << "Translated name is " << multi << std::endl;
    std::cout << ntrans << " variables were translated." << std::endl;
  }
  catch (facilities::Untranslatable err) {
    std::cout << "Failed to completely translate " << multi << std::endl;
  }

  // Test the new itoa routine
  std::string intStr;
  facilities::Util::itoa(12, intStr);
  std::cout << "My String is " << intStr << std::endl;

  // Test stringTokenize and basename
  std::string unixname("/a/path/myUnixFile.txt");
  std::string wname("C:\\a\\longer\\path\\myWindowsFile.txt");

  std::vector<std::string> tokens;
  unsigned int i;

  facilities::Util::stringTokenize(unixname, "/", tokens);

  std::cout << "Processing string " << unixname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(unixname) << std::endl;

  facilities::Util::stringTokenize(wname, "\\", tokens);

  std::cout << "Processing string " << wname << std::endl;
  for (i = 0; i < tokens.size(); i++) {
    std::cout << "Token " << i << " is: " << tokens[i] << std::endl;
  }

  std::cout << "basename is " << facilities::Util::basename(wname) << std::endl;

  return 0;
}

  
