/******************************************************************************
 *   File name: hoops_ballistic.h                                             *
 *                                                                            *
 * Description: Conveniece wrappers for simple parameter file access.         *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_BALLISTIC_H
#define HOOPS_BALLISTIC_H

////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include "hoops/hoops.h"
#include "hoops/hoops_group.h"
#include "hoops/hoops_pil_factory.h"
#include "hoops/hoops_prompt_group.h"
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
  ParPromptGroup::ParPromptGroup(int argc, char * argv[]): IParGroup(), mFile(0),
    mPrompter(0) {
    mFile = PILParFileFactory().NewIParFile(argv[0]);
    mPrompter = PILParPromptFactory().NewIParPrompt(argc, argv);
    mGroup = &mPrompter->Group();
  }

  ParPromptGroup::ParPromptGroup(const ParPromptGroup & group): IParGroup(),
    mFile(0), mPrompter(0) {
    mFile = group.mFile->Clone();
    mPrompter = group.mPrompter->Clone();
    mGroup = &mPrompter->Group();
  }

  ParPromptGroup::~ParPromptGroup() {
    // DON'T delete mGroup! It is owned by mPrompter!
    delete mPrompter;
    delete mFile;
  }

  IParGroup & ParPromptGroup::operator =(const ParPromptGroup & g) {
    *mGroup = *g.mGroup;
    return *this;
  }

  IParGroup & ParPromptGroup::operator =(const IParGroup & g) {
    *mGroup = g;
    return *this;
  }

  IPar & ParPromptGroup::operator [](const std::string & pname) const {
    return mGroup->operator [](pname);
  }

  IPar & ParPromptGroup::Find(const std::string & pname) const {
    return mGroup->Find(pname);
  }

  IParGroup & ParPromptGroup::Clear() {
    mGroup->Clear();
    return *this;
  }

  IParGroup & ParPromptGroup::Add(IPar * p) {
    throw Hexception(PAR_UNSUPPORTED, "Editing PIL-based prompt group not supported", __FILE__, __LINE__);
    mGroup->Add(p);
    return *this;
  }

  IParGroup & ParPromptGroup::Remove(IPar * p) {
    throw Hexception(PAR_UNSUPPORTED, "Editing PIL-based prompt group not supported", __FILE__, __LINE__);
    mGroup->Remove(p);
    return *this;
  }

  IParGroup & ParPromptGroup::Remove(const std::string & pname) {
    throw Hexception(PAR_UNSUPPORTED, "Editing PIL-based prompt group not supported", __FILE__, __LINE__);
    mGroup->Remove(pname);
    return *this;
  }

  GenParItor ParPromptGroup::begin() {
    return mGroup->begin();
  }

  ConstGenParItor ParPromptGroup::begin() const {
    return (const_cast<const IParGroup *>(mGroup))->begin();
  }

  GenParItor ParPromptGroup::end() {
    return mGroup->end();
  }

  ConstGenParItor ParPromptGroup::end() const {
    return (const_cast<const IParGroup *>(mGroup))->end();
  }

  IParGroup * ParPromptGroup::Clone() const { return new ParPromptGroup(*this); }

  ParPromptGroup & ParPromptGroup::Prompt() {
    mPrompter->Prompt();
    return *this;
  }

  ParPromptGroup & ParPromptGroup::Prompt(const std::string & pname) {
    mPrompter->Prompt(pname);
    return *this;
  }

  void ParPromptGroup::Load() {
    mFile->Load();
    mPrompter->Group() = mFile->Group();
  }

  void ParPromptGroup::Save() const {
    mFile->Group() = mPrompter->Group();
    mFile->Save();
  }
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
 * Revision 1.3  2004/03/26 22:31:54  peachey
 * Prevent client from shooting self in the foot: don't allow
 * them to add/remove parameters, because PIL can't handle that. What's more,
 * don't call Save() from the destructor: Save() can throw!
 *
 * Revision 1.2  2004/03/16 20:50:57  peachey
 * Explicitly invoke constructors for base classes to shut up compiler
 * warnings in the SLAC build.
 *
 * Revision 1.1  2004/03/15 15:00:52  peachey
 * New wrapper class which handles simplest use case for getting parameters.
 *
 ******************************************************************************/
