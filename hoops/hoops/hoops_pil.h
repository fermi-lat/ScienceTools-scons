/******************************************************************************
 *   File name: hoops_pil.h                                                   *
 *                                                                            *
 * Description: Definition of PIL-based interface.                            *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_PIL_H
#define HOOPS_PIL_H
////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include "hoops/hoops.h"
////////////////////////////////////////////////////////////////////////////////

#ifndef EXPSYM
#ifdef WIN32
#define EXPSYM __declspec(dllexport)
#else
#define EXPSYM
#endif
#endif

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  class EXPSYM PILParFile : public IParFile {
    public:
      PILParFile();
      PILParFile(const PILParFile & pf);
      PILParFile(const IParFile & pf);
      PILParFile(const std::string & comp);

      virtual ~PILParFile();

      virtual PILParFile & operator =(const PILParFile & pf);
      virtual PILParFile & operator =(const IParFile & pf);

      // Synchronize memory image with parameter file and vice versa.
      virtual void Load() throw (Hexception);
      virtual void Save() const throw (Hexception);

      // Read member access.
      virtual const std::string & Component() const { return mComponent; }
      virtual IParGroup & Group() throw (Hexception)
        { if (!mGroup) throw Hexception(PAR_NULL_PTR); return *mGroup; }
      virtual const IParGroup & Group() const throw (Hexception)
        { if (!mGroup) throw Hexception(PAR_NULL_PTR); return *mGroup; }

      // Write member access.
      virtual PILParFile & SetComponent(const std::string & comp);
      virtual IParGroup * SetGroup(IParGroup * group = 0);

      virtual GenParItor begin() throw (Hexception);
      virtual ConstGenParItor begin() const throw (Hexception);
      virtual GenParItor end() throw (Hexception);
      virtual ConstGenParItor end() const throw (Hexception);

    protected:
      std::string mComponent;
      IParGroup * mGroup;
      void OpenParFile(int argc = 0, char * argv[] = 0) const;
      void CloseParFile(int status = 0) const;
      void CleanComponent(const std::string & comp, std::string & clean) const;
  };

  class EXPSYM PILParPrompt : public IParPrompt {
    public:
      PILParPrompt();
      PILParPrompt(const PILParPrompt & prompt);
      PILParPrompt(const IParPrompt & prompt);
      PILParPrompt(int argc, char ** argv);

      virtual ~PILParPrompt() throw();

      virtual PILParPrompt & operator =(const PILParPrompt & p);
      virtual PILParPrompt & operator =(const IParPrompt & p);

      virtual PILParPrompt & Prompt();
      virtual PILParPrompt & Prompt(const std::string & pname);
      virtual PILParPrompt & Prompt(const std::vector<std::string> & pnames);

      virtual int Argc() const { return mArgc; }
      virtual char ** const Argv() const { return mArgv; }
      virtual IParGroup & Group() throw (Hexception);
      virtual const IParGroup & Group() const throw (Hexception);

      virtual PILParPrompt & SetArgc(int argc) { mArgc = argc; return *this; }
      virtual PILParPrompt & SetArgv(char ** argv);
      virtual IParGroup * SetGroup(IParGroup * group = 0);

    protected:
      IParGroup * mGroup;
      int mArgc;
      char ** mArgv;
      void DeleteArgv();
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
 * Revision 1.7  2003/11/26 18:50:03  peachey
 * Merging changes made to SLAC repository into Goddard repository.
 *
 * Revision 1.6  2003/11/13 19:29:29  peachey
 * Add preprocessor macro needed on Windows to export symbols.
 *
 * Revision 1.5  2003/11/10 18:19:45  peachey
 * Moved header files into hoops subdirectory.
 *
 * Revision 1.1.1.1  2003/11/04 01:48:30  jchiang
 * First import
 *
 * Revision 1.4  2003/06/06 20:50:21  peachey
 * From PILParFile, remove Open() and Close(), which are actually
 * redundant with Load() and Save(). Change PILParPrompt::SetGroup
 * so that it returns the current group, thus overriding the base
 * class method. Add similar SetGroup method to PILParFile.
 *
 * These changes prompted some changes in the memory management.
 * The member parameter group object of PILParFile is now
 * deleted only by the destructor. This allows calling code to
 * cause of group object to be "adopted" by the IParFile object.
 * Also, by default, the parameters prompted for are *not* saved,
 * allowing the developer to choose when saving occurs.
 *
 * Add new virtual constructor to PILParPromptFactory, to be
 * consistent with PILParPrompt constructors.
 *
 * Revision 1.3  2003/06/05 15:46:08  peachey
 * Move PIL-related factories into a separate header.
 *
 * Revision 1.2  2003/04/23 17:44:38  peachey
 * Use new abstract base class IParGroup instead of ParGroup.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
