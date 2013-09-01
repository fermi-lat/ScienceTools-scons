/******************************************************************************
 *   File name: hoops_itor.h                                                  *
 *                                                                            *
 * Description: Declaration of container-independent iterator interface.      *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_ITOR_H
#define HOOPS_ITOR_H

////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include <cassert>
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class IBiDirItor {
    public:
      typedef T Value_t;
      virtual ~IBiDirItor() {}
      virtual IBiDirItor * Clone() const = 0;
      virtual T & operator *() = 0;
      virtual T * operator ->() = 0;
      virtual IBiDirItor & operator ++() = 0;
      virtual IBiDirItor & operator --() = 0;
      virtual bool operator ==(const IBiDirItor & other) const = 0;
      virtual bool operator !=(const IBiDirItor & other) const = 0;
      virtual const T & Ref() const = 0;
  };

  template <typename T, typename Iterator_t>
  class BiDirItor : public IBiDirItor<T> {
    public:
      BiDirItor(): mItor() {}
      BiDirItor(const BiDirItor & other): mItor(other.mItor) {}
      BiDirItor(const Iterator_t & other): mItor(other) {}

      virtual ~BiDirItor() {}

      virtual BiDirItor * Clone() const
        { return new BiDirItor(*this); }
      virtual T & operator *()
        { return *mItor; }
      virtual T * operator ->()
        { return &(*mItor); } // works for non-class iterators too
      virtual BiDirItor & operator ++()
        { ++mItor; return *this; }
      virtual BiDirItor & operator --() 
        { --mItor; return *this; }
      virtual bool operator ==(const IBiDirItor<T> & other) const
        { return &(*mItor) == &(other.Ref()); }
      virtual bool operator !=(const IBiDirItor<T> & other) const
        { return &(*mItor) != &(other.Ref()); }
      virtual const T & Ref() const { return *mItor; }

    private:
      Iterator_t mItor;
  };

  template <typename T>
  class GenBiDirItor : public IBiDirItor<T> {
    public:
      GenBiDirItor(): mItor() {}
      GenBiDirItor(const GenBiDirItor & other): mItor()
        { if (other.mItor) mItor = other.mItor->Clone(); }
      GenBiDirItor(const IBiDirItor<T> & other): mItor()
        { mItor = other.Clone(); }

      virtual ~GenBiDirItor() { delete mItor; }

      virtual GenBiDirItor & operator =(const GenBiDirItor & other) {
        delete mItor;
        if (other.mItor) mItor = other.mItor->Clone();
        else mItor = 0;
        return *this;
      }

      virtual GenBiDirItor & operator =(const IBiDirItor<T> & other) {
        delete mItor;
        mItor = other.Clone();
        return *this;
      }

      virtual GenBiDirItor * Clone() const
        { return new GenBiDirItor(*this); }
      virtual T & operator *()
        { return *GetItor(); }
      virtual T * operator ->()
        { return &(*GetItor()); }
      virtual GenBiDirItor & operator ++()
        { ++GetItor(); return *this; }
      virtual GenBiDirItor & operator --()
        { --GetItor(); return *this; }
      virtual bool operator ==(const IBiDirItor<T> & other) const
        { return &(Ref()) == &(other.Ref()); }
      virtual bool operator !=(const IBiDirItor<T> & other) const
        { return &(Ref()) != &(other.Ref()); }
      virtual const T & Ref() const { assert(mItor); return **mItor; }

    protected:
      virtual IBiDirItor<T> & GetItor() {
        assert(mItor);
        return *mItor;
      }

    private:
      IBiDirItor<T> * mItor;
  };

  template <typename T>
  class IConstBiDirItor {
    public:
      typedef T Value_t;
      virtual ~IConstBiDirItor() {}
      virtual IConstBiDirItor * Clone() const = 0;
      virtual const T & operator *() = 0;
      virtual const T * operator ->() = 0;
      virtual IConstBiDirItor & operator ++() = 0;
      virtual IConstBiDirItor & operator --() = 0;
      virtual bool operator ==(const IConstBiDirItor & other) const = 0;
      virtual bool operator !=(const IConstBiDirItor & other) const = 0;
      virtual const T & Ref() const = 0;
  };

  template <typename T, typename Iterator_t>
  class ConstBiDirItor : public IConstBiDirItor<T> {
    public:
      ConstBiDirItor(): mConstItor() {}
      ConstBiDirItor(const ConstBiDirItor & other):
        mConstItor(other.mConstItor) {}
      ConstBiDirItor(const Iterator_t & other): mConstItor(other) {}

      virtual ~ConstBiDirItor() {}

      virtual ConstBiDirItor * Clone() const
        { return new ConstBiDirItor(*this); }
      virtual const T & operator *()
        { return *mConstItor; }
      virtual const T * operator ->()
        { return &(*mConstItor); } // works for non-class iterators too
      virtual ConstBiDirItor & operator ++()
        { ++mConstItor; return *this; }
      virtual ConstBiDirItor & operator --() 
        { --mConstItor; return *this; }
      virtual bool operator ==(const IConstBiDirItor<T> & other) const
        { return &(*mConstItor) == &(other.Ref()); }
      virtual bool operator !=(const IConstBiDirItor<T> & other) const
        { return &(*mConstItor) != &(other.Ref()); }
      virtual const T & Ref() const { return *mConstItor; }

    private:
      Iterator_t mConstItor;
  };

  template <typename T>
  class ConstGenBiDirItor : public IConstBiDirItor<T> {
    public:
      ConstGenBiDirItor(): mItor() {}
      ConstGenBiDirItor(const ConstGenBiDirItor & other): mItor()
        { if (other.mItor) mItor = other.mItor->Clone(); }
      ConstGenBiDirItor(const IConstBiDirItor<T> & other): mItor()
        { mItor = other.Clone(); }

      virtual ~ConstGenBiDirItor() { delete mItor; }

      virtual ConstGenBiDirItor & operator =(const ConstGenBiDirItor & other) {
        delete mItor;
        if (other.mItor) mItor = other.mItor->Clone();
        else mItor = 0;
        return *this;
      }

      virtual ConstGenBiDirItor & operator =(const IConstBiDirItor<T> & other) {
        delete mItor;
        mItor = other.Clone();
        return *this;
      }

      virtual ConstGenBiDirItor * Clone() const
        { return new ConstGenBiDirItor(*this); }
      virtual const T & operator *()
        { return *GetItor(); }
      virtual const T * operator ->()
        { return &(*GetItor()); }
      virtual ConstGenBiDirItor & operator ++()
        { ++GetItor(); return *this; }
      virtual ConstGenBiDirItor & operator --()
        { --GetItor(); return *this; }
      virtual bool operator ==(const IConstBiDirItor<T> & other) const
        { return &(Ref()) == &(other.Ref()); }
      virtual bool operator !=(const IConstBiDirItor<T> & other) const
        { return &(Ref()) != &(other.Ref()); }
      virtual const T & Ref() const { assert(mItor); return **mItor; }

    protected:
      virtual IConstBiDirItor<T> & GetItor() {
        assert(mItor);
        return *mItor;
      }

    private:
      IConstBiDirItor<T> * mItor;
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
 * Revision 1.1  2003/04/23 17:40:36  peachey
 * Inline definitions of iterator classes to be used by ParGroup family.
 *
 ******************************************************************************/
