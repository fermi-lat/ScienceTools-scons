/**
 * @file ObsSim.cxx
 * @brief Driver for running ObsSim
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cstdlib>

#include <iostream>
#include <string>

int main() {
   std::string command;
   char * root_path = std::getenv("LIKEGUIROOT");
   if (root_path == 0) {
      std::cerr << "Environment variable LIKEGUIROOT not found." << std::endl;
      return 1;
   }
   std::string pythonDir = std::string(root_path) + "/python";
   command = std::string("python ") + pythonDir + "/ObsSim/ObsSim.py";
   std::system(command.c_str());
}
