#include <string>
#include <iostream>
#include "facilities/Util.h"
// Miniscule test program for Util class
main() {
  std::string name = std::string("{XMLUTILROOT}/src");
  std::string oDelim = std::string ("{");
  std::string cDelim = std::string ("}");
  int ntrans = facilities::Util::expandEnvVar(&name, &oDelim, &cDelim);

  cout << "Translated name is " << name << std::endl;

  // Process running this must have environment variable SR
  std::string multi = std::string("$(FACILITIESROOT)/$(SRC)");

  ntrans = facilities::Util::expandEnvVar(&multi);

  cout << "Translated name is " << multi << std::endl;
  cout << ntrans << " variables were translated." << std::endl;
}

  
