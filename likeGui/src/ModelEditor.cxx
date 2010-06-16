/**
 * @file ModelEditor.cxx
 * @brief Driver for running ModelEditor
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#include <cstdlib>
#include <sstream>

int main(int iargc, char * argv[]) {
   std::ostringstream command;
   command << "python -c \"import ModelEditor; ModelEditor.ModelEditor(";
   if (iargc == 2) {
      command << "\'" << argv[1] << "\'";
   }
   command << ")\"";
   std::system(command.str().c_str());
}
