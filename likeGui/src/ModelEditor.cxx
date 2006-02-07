/**
 * @file ModelEditor.cxx
 * @brief Driver for running ModelEditor
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cstdlib>

#include <iostream>
#include <string>

int main(int iargc, char *argv[]) {
   std::string command 
      = "python -c \"import ModelEditor; ModelEditor.ModelEditor()\"";
   std::system(command.c_str());
}
