/** @file runPy.cxx
*/

#include "embed_python/Module.h"
#include <iostream>
#include <stdexcept>


int
main(int argc, char *argv[])
{
  //    return embed_python::Module::test(argc, argv, "Test");
    return embed_python::Module::test(argc, argv, "embed_python_Test");
}

/** test with the following python code:
@verbatim


def setup():
    print "seting up a dictionary..."
    dict = {'x':99}
    dict['y']=101
    dict['z']='testing'
    return dict
       
    
    @endverbatim

*/
