// $Header$
/** @file test_meta.cxx
    Sample program to exercise calibration metadata database services
*/

#include <string>
#include <iostream>
#include "facilities/Timestamp.h"


int main(int argc, char* argv[]) {
  using facilities::Timestamp;
  using facilities::BadTimeInput;


  try {
    Timestamp t_ok("2001-11-10 08:00");
    Timestamp t_none("2000-09-08 10:00");
    long int       zero = 0;
    long int       negValue = -2000;
    Timestamp unixCreation(zero);
    Timestamp gmttest("1970-01-01 00:00");
    long int        aTime = 4000;

    Timestamp aTimestamp(aTime);


    std::cout << "unix creation time is " << unixCreation.getString()
              << std::endl;


    std::cout << "aTimestamp is " << aTimestamp.getString()
              << std::endl;
  }
  catch (const BadTimeInput e) {
    std::cout << "Exception message:  " << e.complaint << std::endl;
  }

  // Defining delta times
  try {
    facilities::Timestamp   days10(10.0, true);
    facilities::Timestamp   jan10((long int) 0);
    jan10 += days10;
    std::cout << "10jan is " << jan10.getString() << std::endl;

    facilities::Timestamp   cur;

    std::cout << "Now (GMT) is approx. " << cur.getString() << std::endl;

    facilities::Timestamp  ago10 = cur  - days10;

    std::cout << "10 days ago is " << ago10.getString() << std::endl;
    std::cout << "10 days ago julian is " << ago10.getJulian() << std::endl;

    facilities::Timestamp future = cur + days10;
    std::cout << "10 days from now is " << future.getString() << std::endl;
    std::cout << "10 days from now julian is " << future.getJulian() << std::endl;

    facilities::Timestamp delta = future - ago10;
    std::cout << "result of subtracting absolute times (units = days) "
              << delta.getJulian() << std::endl;
  }

  catch (const BadTimeInput f) {
    std::cout << "delta exception message: " << f.complaint << std::endl;
  }


  return 0;

}

