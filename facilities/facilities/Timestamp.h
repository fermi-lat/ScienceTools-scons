// $Header$
#ifndef FACILITIES_TIMESTAMP_H
#define FACILITIES_TIMESTAMP_H

#include <string>

namespace facilities {

  /** Exception class, used to complain about bad arguments to 
      constructors, incompatible arguments for comparisons or
      arithmetic, or out of range results.
  */
  class BadTimeInput {
  public:
    BadTimeInput(std::string msg) : complaint(msg) {};
    ~BadTimeInput() {};
    std::string complaint; 

    BadTimeInput(const BadTimeInput &other) {complaint = other.complaint; };
  }; 


  /** Timestamp class, valid for dates from 1970 through 2037 and for
      non-negative delta times of up to about 68 years 

      Supports comparisons, addition and subtraction where sensible:
          compare two delta times
          compare two dates
          date + delta ==> date
          delta + delta ==> delta
          date - delta ==> date
          date - date ==> delta


      Input to constructors may be
              Julian date      
              seconds since start of 1970, Jan. 1 with optional
                  nanosecond field
              individual fields (year, month, etc.)
              string format            

           yyyy-mm-dd hh:mm:ss
                             1969 < yyyy < 2038
                          where 0 < mm < 13
                                0 < dd < 32
                               -1 < hh < 24
                               -1 < mm < 60 
                               -1 < ss < 60 

        o  only the first three fields are required.  Omitted trailing
           fields will be interpreted as equal to 0.
        o  by default : will be used to delimit fields, but user
           may specify an alternative in most circumstances
        o  leading zeros are optional

  */
  class Timestamp {
  public:
    /// Default constructor builds object representing current time
    Timestamp();

    /// Count seconds from the creation-according-to-unix, start of 1970
    Timestamp(long int seconds, int nano = 0, bool isDelta = false);

    /// Constructor for Julian date 
    Timestamp(double julian, bool isDelta = false);


    /** Create a timestamp from an ascii string of standard form
                yyyy-mm-dd hh:mm:ss
        where only the first three fields are required.   

        If the string is invalid, object will represent unix creation time.

        Assume for now -- and preferably forever -- that all times are GMT.
    */
    Timestamp(const std::string& str);

    /// Construct absolute time with specified fields
    Timestamp(int year, int month, int day, 
              int hour = 0, int minute = 0, 
              int second = 0, int nano = 0);

    /// Return string representation of time, not including nanoseconds;
    /// valid for absolute times only
    std::string getString() const;

    /// Return julian date or, for delta times, just time interval
    /// measured in days.
    double      getJulian() const {
      double julian = (m_time +  m_nano/inverseNano)/secPerDay;
      if (!m_isDelta) julian += julian1970;
      return julian;
    }

    double      getNano() const {return m_nano;}
    long int    getClibTime() const {return m_time;}

    bool        isDelta() const {return m_isDelta;}

    bool operator<(const Timestamp& other) const {
      if (m_isDelta != other.m_isDelta) 
        throw BadTimeInput("facilities::Timestamp incompatible comparison");
      return ((m_time < other.m_time) || 
              ((m_time == other.m_time) && (m_nano < other.m_nano)));
    }

    bool operator>(const Timestamp& other) const {
      return (other < (*this));
        
    }

    bool operator<=(const Timestamp& other) const {
      return ( !(other < (*this)) );
    }

    bool operator>=(const Timestamp& other) const {
      return ( !( (*this) < other ) );
    }


    Timestamp& operator= (const Timestamp& other) {
      m_time = other.m_time; m_nano = other.m_nano; 
      m_isDelta = other.m_isDelta; 
      return *this;
    }

    bool operator==(const Timestamp& other) const{
      if (m_isDelta != other.m_isDelta) 
        throw BadTimeInput("facilities::Timestamp incompatible comparison");
      return ((m_time == other.m_time) && (m_nano == other.m_nano));
    }

    bool operator!=(const Timestamp& other) const {
      return (!(*this == other));
    }

    Timestamp&  operator+=(const Timestamp delta);

    Timestamp&  operator-=(const Timestamp delta);

    // To come:  operator+   operator- 
    //             << (for debugging)
  private:
    // See the U.S. Naval Observatory site, 
    //   http://aa.usno.navy.mil/data/docs/JulianDate.html,
    // used to compute Julian date for Jan 1, 1970
    static const double julian1970;
    static const int    secPerDay;
    static const double  inverseNano;
    static const int     inverseNanoInt;
    static const long int maxInt;

    /// Representation of nominal start of time -- Jan 1, 1970 --
    /// will change between Visual Studio V6 and V7.  Time zone
    /// also has to be discovered dynamically, but only once.
    /// Do the computation in the constructor of a private class.
    class TZOffset {
    public:
      TZOffset();          
      long int m_tzseconds;     // 0 for GMT, 28800 for PDT, etc.
      int      m_isDst;   // Not yet sure if we need it
    };
    static  TZOffset s_tz;

    /// Return "standard" binary time: count in seconds since 1 jan 1970
    static time_t toBinary(const std::string& strTime); 

    /// Assemble string rep. from count of seconds
    static void toString(time_t bin, std::string& strTime);

  protected:
    /// internal binary rep of time; count seconds from Jan 1, 1970
    time_t m_time;

    /// Save fractional seconds separately (associated with m_time)
    int  m_nano;

    /// Flag to distinguish delta times from dates
    bool m_isDelta;
  };

  /// Addition
  inline Timestamp operator+ (Timestamp a, 
                              Timestamp b) {
    Timestamp t = a;
    return t += b;
  }


  /// Subtraction
  inline Timestamp operator- (Timestamp a, 
                              Timestamp b) {
    Timestamp t = a;
    return t -= b;
  }

}

#endif
