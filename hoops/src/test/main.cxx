#include <cstring>
#include <iostream>
#include <string>

#include "hoops/hoops_limits.h"
#include "hoops/hoops.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_pil.h"

int main(int iargc, char* argv[]) {
   std::string filename("file.par");

// Replace argv[0] with this filename
   delete argv[0];
   argv[0] = strdup(filename.c_str());

   hoops::IParFile * pf 
      = hoops::PILParFileFactory().NewIParFile(argv[0]);

   pf->Load();  // should be in constructor?

   hoops::IParPrompt * prompter 
      = hoops::PILParPromptFactory().NewIParPrompt(iargc, argv);

   prompter->Prompt();

   hoops::IParGroup & pg = prompter->Group();

   std::string my_string = pg["string_par"];
   double my_double = pg["double_par"];

   std::cout << my_string << "\n"
             << my_double << std::endl;

   pf->Group() = pg;
   pf->Save();   // should be in destructor?

   delete pf;
   delete prompter;
}

      
