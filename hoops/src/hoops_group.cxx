/******************************************************************************
 *   File name: hoops_group.cxx                                               *
 *                                                                            *
 * Description: Implementation of standard parameter group.                   *
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
#include "hoops/hoops_group.h"
////////////////////////////////////////////////////////////////////////////////
namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type definitions.
  //////////////////////////////////////////////////////////////////////////////
  ParGroup::ParGroup(): mPars() {}
  ParGroup::ParGroup(const ParGroup & g): mPars() {
    std::vector<IPar *>::const_iterator it;
    for (it = g.mPars.begin(); it != g.mPars.end(); ++it) 
      mPars.push_back((*it)->Clone());
  }

  ParGroup::~ParGroup() { Clear(); }

  ParGroup & ParGroup::operator =(const IParGroup & g) {
    ConstGenParItor it;
    Clear();
    for (it = g.begin(); it != g.end(); ++it) 
      mPars.push_back((*it)->Clone());
    return *this;
  }

  ParGroup & ParGroup::operator =(const ParGroup & g) {
    std::vector<IPar *>::const_iterator it;
    Clear();
    for (it = g.mPars.begin(); it != g.mPars.end(); ++it) 
      mPars.push_back((*it)->Clone());
    return *this;
  }

  IPar & ParGroup::Find(const std::string & pname) const {
    std::vector<IPar *>::const_iterator it;

    // Look for a parameter with the given name.
    for (it = mPars.begin(); it != mPars.end(); ++it)
      if (!pname.compare((*it)->Name())) break;

    // If not found, throw an exception to indicate this fact.
    if (it == mPars.end()) throw Hexception(PAR_NOT_FOUND);

    // Otherwise, return the found parameter.
    return *(*it);
  }

  ParGroup & ParGroup::Clear() {
    std::vector<IPar *>::iterator it;
    for (it = mPars.begin(); it != mPars.end(); ++it) delete *it;
    mPars.clear();
    return *this;
  }

  ParGroup & ParGroup::Add(IPar * p)
    { if (p) mPars.push_back(p); return *this; }
  
  ParGroup & ParGroup::Remove(IPar * p) { Remove(p->Name()); return *this; }

  ParGroup & ParGroup::Remove(const std::string & pname) {
    std::vector<IPar *>::iterator it;
    for (it = mPars.begin(); it != mPars.end(); ++it) {
      if (!pname.compare((*it)->Name())) {
        delete (*it);
        mPars.erase(it);
      }
    }
    return *this;
  }
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
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

}

/******************************************************************************
 * $Log$
 * Revision 1.4  2003/06/06 20:44:05  peachey
 * Add assignment operator from IParGroup reference.
 *
 * Revision 1.3  2003/06/06 13:25:37  peachey
 * Restructure the header files. The files hoops_exception.h, hoops.h,
 * hoops_itor.h, hoops_pil_factory.h, and hoops_prim.h are now the
 * only public (installed) files.
 *
 * Revision 1.2  2003/04/23 17:43:29  peachey
 * Use new iterator classes.
 *
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
