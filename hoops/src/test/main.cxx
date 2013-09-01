#include "hoops/hoops_limits.h"
#include "hoops/hoops.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_pil.h"

int main(int argc, char* argv[]) {
   hoops::IParFile * pf = hoops::PILParFileFactory().NewIParFile(argv[0]);

   pf->Load();

   hoops::IParPrompt * prompter 
      = hoops::PILParPromptFactory().NewIParPrompt(argc-1, argv+1);

   prompter->Prompt();

   hoops::IParGroup & pg = prompter->Group();

   pg["string_par"] = "This is a string parameter";
   double d = pg["double_par"];
}

      
