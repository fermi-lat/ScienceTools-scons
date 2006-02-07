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
   std::string command("python -c \"import ObsSim; ObsSim.ObsSim()\"");
   std::system(command.c_str());
}
