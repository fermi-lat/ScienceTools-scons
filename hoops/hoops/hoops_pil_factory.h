/******************************************************************************
 *   File name: hoops_pil_factory.h                                           *
 *                                                                            *
 * Description: Declaration of factory to create PIL-based parameter objects. *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_PIL_FACTORY_H
#define HOOPS_PIL_FACTORY_H
////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include "hoops.h"
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  class PILParFileFactory : public IParFileFactory {
    public:
      virtual ~PILParFileFactory() {}

      virtual IParFile * NewIParFile();
      virtual IParFile * NewIParFile(const IParFile & p);
      virtual IParFile * NewIParFile(const char * argv);
  };

  class PILParPromptFactory : IParPromptFactory {
    public:
      virtual ~PILParPromptFactory() {}

      virtual IParPrompt * NewIParPrompt();
      virtual IParPrompt * NewIParPrompt(const IParPrompt & p);
      virtual IParPrompt * NewIParPrompt(int argc, char * argv[]);
  };
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable forward declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

}
#endif

/******************************************************************************
 * $Log$
 * Revision 1.2  2003/06/06 20:51:20  peachey
 * Add new virtual constructor to PILParPromptFactory, to be
 * consistent with PILParPrompt constructors. Similar change
 * also to PILParFileFactory.
 *
 * Revision 1.1  2003/06/05 15:46:08  peachey
 * Move PIL-related factories into a separate header.
 *
 ******************************************************************************/
