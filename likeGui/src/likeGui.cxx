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
   std::string command("python -c \"import likeGui; likeGui.likeGui()\"");
   std::system(command.c_str());
}
