/** @file main.cxx
    @ brief application to create a pointing history

*/

#include "MakePointingHistory.h"


///! The main program
int main(int argc, char* argv[])
{
    int rc=0;
    try {

        MakePointingHistory::main(argc, argv);
    }catch(const std::exception& e){
        rc=1;
        std::cerr << "Caught exception: " << typeid(e).name() << e.what() << std::endl;

    }
    if( rc!=0) MakePointingHistory::help();

    return rc;
}
