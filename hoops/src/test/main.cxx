#include <cstring>

#include "hoops/hoops_limits.h"
#include "hoops/hoops.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_pil.h"

int main(int argc, char* argv[]) {
   std::string filename("file.par");
   hoops::IParFile * pf 
      = hoops::PILParFileFactory().NewIParFile(filename.c_str());

   pf->Load();  // should be in constructor?

   char *lame_char_array[1];
   lame_char_array[0] = strdup(filename.c_str());
   hoops::IParPrompt * prompter 
      = hoops::PILParPromptFactory().NewIParPrompt(1, lame_char_array);

   prompter->Prompt();

   hoops::IParGroup & pg = prompter->Group();

   string my_string = pg["string_par"];
   double my_double = pg["double_par"];

   std::cout << my_string << "\n"
             << my_double << std::endl;

   pf->Save();   // should be in destructor?

   delete pf;
   delete prompter;
}

      
