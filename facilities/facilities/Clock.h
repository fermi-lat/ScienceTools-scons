//## begin module%3512FB51015F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3512FB51015F.cm

//## Module: Clock%3512FB51015F; Package specification
//## Subsystem: facilities%3550CC6801AC
//## Source file: D:\code\glastsim\facilities\Clock.h

#ifndef _H_CLOCK
#define _H_CLOCK 1

//## begin module%3512FB51015F.includes preserve=yes

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <cmath>

//## end module%3512FB51015F.includes

//## begin module%3512FB51015F.declarations preserve=no
//## end module%3512FB51015F.declarations


//## Class: ClockWord%3512FB52008E
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

typedef double ClockWord;

//## Class: Clock%3512FB510173
//	General purpose class to encapsulate a clock value
//## Category: facilities%3550CC5302A6
//## Subsystem: facilities%3550CC6801AC
//## Persistence: Transient
//## Cardinality/Multiplicity: n

//## Uses: <unnamed>%3523F7EF019D;ClockWord { -> }

class Clock 
{
  public:
    //## Constructors (specified)
      //## Operation: Clock%-1886651186; C++
      Clock ()
        //## begin Clock::Clock%-1886651186.initialization preserve=yes
	: m_time(0), m_freq(2.E7) 
        //## end Clock::Clock%-1886651186.initialization
      {
        //## begin Clock::Clock%-1886651186.body preserve=yes
        //## end Clock::Clock%-1886651186.body
      }


    //## Other Operations (specified)
      //## Operation: count%-1028188443; C++
      virtual ClockWord count (double t)
      {
        //## begin Clock::count%-1028188443.body preserve=yes
	m_time += t;
	return m_time; 
        //## end Clock::count%-1028188443.body
      }

      //## Operation: count%1362177038; C++
      virtual ClockWord count (unsigned int c)
      {
        //## begin Clock::count%1362177038.body preserve=yes
	m_time += c * 1./m_freq; 
	return m_time; 
        //## end Clock::count%1362177038.body
      }

      //## Operation: reset%-1049191408; C++
      virtual void reset ()
      {
        //## begin Clock::reset%-1049191408.body preserve=yes
	m_time = 0; 
        //## end Clock::reset%-1049191408.body
      }

      //## Operation: freq%-412432423; C++
      double freq () const
      {
        //## begin Clock::freq%-412432423.body preserve=yes
	return m_freq; 
        //## end Clock::freq%-412432423.body
      }

      //## Operation: cycles%104144835; C++
      virtual unsigned int cycles ()
      {
        //## begin Clock::cycles%104144835.body preserve=yes
	return static_cast<unsigned>(ceil(m_time / m_freq)); 
        //## end Clock::cycles%104144835.body
      }

      //## Operation: elapsed%-380435011; C++
      virtual double elapsed ()
      {
        //## begin Clock::elapsed%-380435011.body preserve=yes
	return m_time; 
        //## end Clock::elapsed%-380435011.body
      }

  protected:
  private:
    // Data Members for Class Attributes

      //## Attribute: m_time%3512FB51017D
      //	current elapsed time	in clock cycles
      //## begin Clock::m_time%3512FB51017D.attr preserve=no  private: ClockWord {VA} 
      ClockWord m_time;
      //## end Clock::m_time%3512FB51017D.attr

      //## Attribute: m_freq%3512FB51017E
      //	frequency in Hz
      //## begin Clock::m_freq%3512FB51017E.attr preserve=no  private: double {VA} 
      double m_freq;
      //## end Clock::m_freq%3512FB51017E.attr
};

#endif
