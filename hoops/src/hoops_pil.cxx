/******************************************************************************
 *   File name: hoops_pil.cxx                                                 *
 *                                                                            *
 * Description: Implementation of PIL-based interface.                        *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Header files.
////////////////////////////////////////////////////////////////////////////////
// stdio.h is needed only because pil.h neglects to include it.
#include <stdio.h>
#include "hoops/hoops.h"
#include "hoops/hoops_group.h"
#include "hoops/hoops_par.h"
#include "hoops/hoops_pil.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_prim.h"
#include "pil.h"
////////////////////////////////////////////////////////////////////////////////
namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static function declarations.
  //////////////////////////////////////////////////////////////////////////////
  static char *CpyStr(const char *s) throw ();
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Begin PILParFile implementation.
  //////////////////////////////////////////////////////////////////////////////
  PILParFile::PILParFile(): mComponent(), mGroup(0) {}

  PILParFile::PILParFile(const PILParFile & pf):
    mComponent(pf.mComponent), mGroup(0)
    { if (pf.mGroup) mGroup = pf.mGroup->Clone(); }

  PILParFile::PILParFile(const IParFile & pf):
    mComponent(pf.Component()), mGroup(0) {
    mGroup = pf.Group().Clone();
  }

  PILParFile::PILParFile(const std::string & comp): mComponent(), mGroup(0)
    { SetComponent(comp); }

  PILParFile::~PILParFile() { delete mGroup; }

  PILParFile & PILParFile::operator =(const PILParFile & pf) {
    mComponent = pf.mComponent;
    if (mGroup) {
      if (pf.mGroup) *mGroup = *pf.mGroup;
      else mGroup->Clear();
    } else {
      if (pf.mGroup) mGroup = pf.mGroup->Clone();
    }
    return *this;
  }

  PILParFile & PILParFile::operator =(const IParFile & pf) {
    mComponent = pf.Component();
    if (mGroup) {
      *mGroup = pf.Group();
    } else {
      mGroup = pf.Group().Clone();
    }
    return *this;
  }

  // Synchronize memory image with parameter file and vice versa.
  void PILParFile::Load() throw (Hexception) {
    PIL_PARAM * pp = 0;

    try {
      int status = PIL_OK;

      // Use PIL to open the par file specified by the component and
      // path fields.
      OpenParFile();

      // Get number of parameters.
      int nPar = 0;
      status = PILGetNumParameters(&nPar);
      if (PIL_OK != status) throw Hexception(status);

      // Make space to store parameter array.
      pp = new PIL_PARAM[nPar];

      // Loop over parameters in file, reading them all into local memory.
      PIL_VALUE vmin;
      PIL_VALUE vmax;
      int minmaxok;
      for (int ii = 0; ii < nPar; ++ii) {
        memset(pp + ii, 0, sizeof(PIL_PARAM));
        status = PILGetParameter(ii, pp + ii, &minmaxok, &vmin, &vmax);
        if (PIL_OK != status) break;
      }
      if (PIL_OK != status) throw Hexception(status);

      // Loop over local copy of parameters, making sure they are all valid.
      for (int ii = 0; ii < nPar; ++ii) {
        if (!IPrim::IsBlank(pp[ii].strline) &&
            PIL_FORMAT_BLANK != pp[ii].format &&
            PIL_FORMAT_COMMENT != pp[ii].format &&
            PIL_FORMAT_OK != pp[ii].format) {
          status = PAR_FILE_CORRUPT;
          break;
        }
      }
      if (PIL_OK != status) throw Hexception(status);

      // At this point, no further problems _should_ happen, so go
      // ahead and clear the current parameter group.
      if (mGroup) mGroup->Clear(); else mGroup = new ParGroup;

      // Loop over local copy of parameters, and use them to create
      // the parameter group.
      bool inquote;
      const char * comment;
      for (int ii = 0; ii < nPar; ++ii) {
        // Handle comments. They start with an unquoted #.
        inquote = false;
        if (pp[ii].strline) {
          comment = pp[ii].strline;
          while (*comment) {
            if (inquote) {
              if ('"' == *comment) inquote = false;
            } else {
              if ('"' == *comment) inquote = true;
              else if ('#' == *comment) break;
            }
            ++comment;
          }
          // Back up to include whitespace before the comment.
          while (comment > pp[ii].strline && isspace(*(comment - 1))) --comment;
        } else comment = "";

        // There is a check above to make sure the line is valid.
        // At this point, we can therefore assume that the 7 standard
        // fields are either all present, or all absent (blank line or
        // comment line.)
        if (pp[ii].strname)
          mGroup->Add(new Par(pp[ii].strname, pp[ii].strtype, pp[ii].strmode,
              pp[ii].strvalue, pp[ii].strmin, pp[ii].strmax, pp[ii].strprompt,
              comment));
        else
          mGroup->Add(new Par("", "", "", "", "", "", "", comment));
      }
    } catch (...) {
      delete [] pp;
      CloseParFile();
      throw;
    }

    delete [] pp;
    CloseParFile();
  }

  void PILParFile::Save() const throw (Hexception) {
    try {
      if (mGroup) {
        int status = PIL_OK;
        const IParGroup * constGroup = mGroup;

        OpenParFile();

        // Loop over parameter group in memory, and save each parameter.
        const IPar * par;
        ConstGenParItor it;
        for (it = constGroup->begin(); it != constGroup->end(); ++it) {
          par = *it;
          if (par->Name().empty()) continue;
          const std::string & type = par->Type();
          if (std::string::npos != type.find("b")) {
            bool p = *par;
            status = PILPutBool(par->Name().c_str(), p);
          } else if (std::string::npos != type.find("f")) {
            status = PILPutFname(par->Name().c_str(), *par);
          } else if (std::string::npos != type.find("i")) {
            long p = *par;
            status = PILPutInt(par->Name().c_str(), p);
          } else if (std::string::npos != type.find("r")) {
            status = PILPutReal(par->Name().c_str(), *par);
          } else if (std::string::npos != type.find("s")) {
            status = PILPutString(par->Name().c_str(), *par);
          } else {
            status = PAR_INVALID_TYPE;
          }
          if (PIL_OK != status) throw Hexception(status);
        }
      } else {
        throw Hexception(PAR_NULL_PTR);
      }
    } catch (...) {
      CloseParFile();
      throw;
    }
    CloseParFile();
  }

  PILParFile & PILParFile::SetComponent(const std::string & comp) {
    CleanComponent(comp, mComponent);
    return *this;
  }

  IParGroup * PILParFile::SetGroup(IParGroup * group)
    { IParGroup * retval = mGroup; mGroup = group; return retval; }

  GenParItor PILParFile::begin() throw (Hexception)
    { if (!mGroup) throw Hexception(PAR_NULL_PTR); return mGroup->begin(); }

  ConstGenParItor PILParFile::begin() const throw (Hexception) {
    if (!mGroup) throw Hexception(PAR_NULL_PTR);
    return static_cast<const IParGroup *>(mGroup)->begin();
  }

  GenParItor PILParFile::end() throw (Hexception)
    { if (!mGroup) throw Hexception(PAR_NULL_PTR); return mGroup->end(); }

  ConstGenParItor PILParFile::end() const throw (Hexception) {
    if (!mGroup) throw Hexception(PAR_NULL_PTR);
    return static_cast<const IParGroup *>(mGroup)->end();
  }

  void PILParFile::CleanComponent(const std::string & comp, std::string & clean)
    const {
    clean = comp;
    // Trim path name.
    std::string::size_type pos = clean.rfind("/");
    if (std::string::npos != pos) clean.erase(0, pos + 1);

    // Trim extension.
    pos = clean.rfind(".");
    if (std::string::npos != pos) clean.erase(pos);
  }

  void PILParFile::OpenParFile(int argc, char * argv[]) const {
    int status = PIL_OK;
    char * pil_comp = 0;
    if (!argc || !argv) {
      if (mComponent.empty()) throw Hexception(PAR_COMP_UNDEF);
      pil_comp = CpyStr(mComponent.c_str());
      argc = 1;
      argv = &pil_comp;
    }

    // Open the par file using PIL.
    status = PILInit(argc, argv);
    delete [] pil_comp;

    if (PIL_OK != status) throw Hexception(status);
  }

  void PILParFile::CloseParFile(int status) const {
    PILClose(status);
  }
  //////////////////////////////////////////////////////////////////////////////
  // End PILParFile implementation.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Begin PILParPrompt implementation.
  //////////////////////////////////////////////////////////////////////////////
  PILParPrompt::PILParPrompt(): mGroup(0), mArgc(), mArgv(0) {}

  PILParPrompt::PILParPrompt(const PILParPrompt & prompt):
    mGroup(0), mArgc(prompt.mArgc), mArgv(0) { SetArgv(prompt.mArgv); }

  PILParPrompt::PILParPrompt(const IParPrompt & prompt):
    mGroup(0), mArgc(prompt.Argc()), mArgv(0) { SetArgv(prompt.Argv()); }

  PILParPrompt::PILParPrompt(int argc, char ** argv):
    mGroup(0), mArgc(argc), mArgv(0) { SetArgv(argv); }

  PILParPrompt::~PILParPrompt() throw() { delete mGroup; DeleteArgv(); }

  PILParPrompt & PILParPrompt::operator =(const PILParPrompt & p) {
    DeleteArgv();
    mArgc = p.mArgc;
    SetArgv(p.mArgv);
    return *this;
  }

  PILParPrompt & PILParPrompt::operator =(const IParPrompt & p) {
    DeleteArgv();
    mArgc = p.Argc();
    SetArgv(p.Argv());
    return *this;
  }

  PILParPrompt & PILParPrompt::Prompt() {
    // Must have defined at least one argument.
    if (!mArgv) throw Hexception(PAR_NULL_PTR);

    // Get group of parameters from a temporary file object.
    PILParFile f(mArgv[0]);
    f.Load();
    const IParGroup * g = &f.Group();

    // Make a list of all the parameters which may need prompting.
    std::vector<std::string> plist;
    ConstGenParItor it;
    for (it = g->begin(); it != g->end(); ++it) {
      const std::string & name = (*it)->Name();
      if (!name.empty()) plist.push_back(name);
    }

    // Pass list to overloaded Prompt().
    return Prompt(plist);
  }

  PILParPrompt & PILParPrompt::Prompt(const std::string & pname) {
    // Make a list of one parameter name: pname.
    std::vector<std::string> plist;
    plist.push_back(pname);

    // Pass list to overloaded Prompt().
    return Prompt(plist);
  }

  PILParPrompt & PILParPrompt::Prompt(const std::vector<std::string> & pnames) {

    int status = PIL_OK;

    // Must have defined at least one argument.
    if (!mArgv) throw Hexception(PAR_NULL_PTR);

    // Use a temporary file object to get the parameter group.
    PILParFile f(mArgv[0]);
    try {
      f.Load();

      std::string comp = f.Component();
      if (comp.empty()) throw Hexception(PAR_COMP_UNDEF);
      PILSetModuleName(comp.c_str());

      status = PILInit(mArgc, mArgv);
      if (PIL_OK != status) throw Hexception(status);

      status = PILVerifyCmdLine();
      if (PIL_OK != status) throw Hexception(status);

      std::string type;
      std::vector<std::string>::const_iterator it;

      // Loop over parameter names in the list.
      for (it = pnames.begin(); it != pnames.end(); ++it) {
        if(it->empty()) continue; // Skip blank/comment lines.

        // Find the corresponding parameter in the group.
        IPar & par = f.Group().Find(*it);

        // Prompt using the appropriate PILGet function.
        type = par.Type();

        if (std::string::npos != type.find("b")) {
          int r = 0;
          status = PILGetBool(it->c_str(), &r);
          if (PIL_OK != status) break;
          par = (bool) r;
        } else if (std::string::npos != type.find("f")) {
          char r[PIL_LINESIZE] = "";
          status = PILGetFname(it->c_str(), r);
          if (PIL_OK != status) break;
          par = r;
        } else if (std::string::npos != type.find("i")) {
          int r = 0;
          status = PILGetInt(it->c_str(), &r);
          if (PIL_OK != status) break;
          par = r;
        } else if (std::string::npos != type.find("r")) {
          double r = 0.;
          status = PILGetReal(it->c_str(), &r);
          if (PIL_OK != status) break;
          par = r;
        } else if (std::string::npos != type.find("s")) {
          char r[PIL_LINESIZE] = "";
          status = PILGetString(it->c_str(), r);
          if (PIL_OK != status) break;
          par = r;
        } else {
          status = PAR_INVALID_TYPE; break;
        }

      }
    } catch (...) {
      // Clean up: make sure PIL is closed, and file object cleaned.
      PILClose(-1); // Call with non-0 argument so pars wont be saved.
      throw;
    }

    if (PIL_OK == status) {
      // Save parameters which were just prompted for to this object's
      // group object.
      if (mGroup) *mGroup = f.Group(); else mGroup = f.Group().Clone();
    }

    // Clean up.
    PILClose(-1); // Don't save parameters.

    if (PIL_OK != status) throw Hexception(status);
    return *this;
  }

  IParGroup & PILParPrompt::Group() throw (Hexception) {
    if (!mGroup) mGroup = new ParGroup;
    return *mGroup;
  }

  const IParGroup & PILParPrompt::Group() const throw (Hexception) {
    if (!mGroup) throw Hexception(PAR_NULL_PTR);
    return *mGroup;
  }

  PILParPrompt & PILParPrompt::SetArgv(char ** argv) {
    if (argv) {
      mArgv = new char *[mArgc];
      for (int ii = 0; ii < mArgc; ++ii) {
        mArgv[ii] = CpyStr(argv[ii]);
        if (!argv[ii]) {
          mArgc = ii;
          break;
        }
      }
    } else {
      mArgc = 0;
      mArgv = 0;
    }
    return *this;
  }

  IParGroup * PILParPrompt::SetGroup(IParGroup * group)
    { IParGroup * retval = mGroup; mGroup = group; return retval; }

  void PILParPrompt::DeleteArgv() {
    for (int ii = 0; ii < mArgc; ++ii) delete [] mArgv[ii];
    delete mArgv;
  }
  //////////////////////////////////////////////////////////////////////////////
  // End PILParPrompt implementation.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Begin PILParFileFactory implementation.
  //////////////////////////////////////////////////////////////////////////////
  IParFile * PILParFileFactory::NewIParFile()
    { return new PILParFile(); }

  IParFile * PILParFileFactory::NewIParFile(const IParFile & p)
    { return new PILParFile(p); }

  IParFile * PILParFileFactory::NewIParFile(const char * argv)
    { return new PILParFile(argv); }
  //////////////////////////////////////////////////////////////////////////////
  // End PILParFileFactory implementation.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Begin PILParPromptFactory implementation.
  //////////////////////////////////////////////////////////////////////////////

  IParPrompt * PILParPromptFactory::NewIParPrompt()
    { return new PILParPrompt(); }

  IParPrompt * PILParPromptFactory::NewIParPrompt(const IParPrompt & p)
    { return new PILParPrompt(p); }

  IParPrompt * PILParPromptFactory::NewIParPrompt(int argc, char * argv[])
    { return new PILParPrompt(argc, argv); }
  //////////////////////////////////////////////////////////////////////////////
  // End PILParPromptFactory implementation.
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static variable definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Static function definitions.
  //////////////////////////////////////////////////////////////////////////////
  // Utility CpyStr: copy a string, allocating a new string
  // just large enough to hold the string being copied.
  static char *CpyStr(const char *s) throw () {
    char *r = 0;
    if (s) {
      char *ptr;
      const char *c_ptr = s;
      while (*c_ptr) ++c_ptr;
      r = new char[c_ptr - s + 1];
      ptr = r;
      while (0 != (*ptr = *s)) { ++s; ++ptr; }
    }
    return r;
  }
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

}

/******************************************************************************
 * Revision 1.9  2003/11/26 18:50:02  peachey
 * Merging changes made to SLAC repository into Goddard repository.
 *
 * Revision 1.8  2003/11/26 17:54:09  peachey
 * Explicitly zero-initialize all pointers, as VS does not adhere to
 * the ISO standard for default-initialized pointers.
 *
 * Revision 1.7  2003/11/10 18:16:12  peachey
 * Moved header files into hoops subdirectory.
 *
 * Revision 1.6  2003/06/18 18:19:59  peachey
 * CpyStr facility moved to IPrim.
 *
 * Revision 1.1.1.1  2003/11/04 01:48:29  jchiang
 * First import
 *
 * Revision 1.5  2003/06/06 20:50:21  peachey
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
 * Revision 1.4  2003/06/06 13:51:21  peachey
 * Default for internal closes should be to forget the parameters.
 *
 * Revision 1.3  2003/06/05 15:46:08  peachey
 * Move PIL-related factories into a separate header.
 *
 * Revision 1.2  2003/04/23 17:45:06  peachey
 * Use new abstract base class IParGroup instead of ParGroup, and new
 * iterator classes.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
