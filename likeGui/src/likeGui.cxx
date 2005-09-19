/**
 * @file likeGui.cxx
 * @brief Driver for running likeGui
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cstdlib>

#include <iostream>
#include <string>

int main(int iargc, char *argv[]) {
   std::string command;
   char * root_path = std::getenv("LIKEGUIROOT");
   if (root_path == 0) {
      std::cerr << "Environment variable LIKEGUIROOT not found." << std::endl;
      return 1;
   }
   std::string pythonDir = std::string(root_path) + "/python";
   if (iargc == 1) {
      command = std::string("python ") + pythonDir 
         + std::string("/likeGui.py");
   } else if (iargc == 2) {
      command = std::string("python ") + argv[1];
   }
   std::system(command.c_str());
}
