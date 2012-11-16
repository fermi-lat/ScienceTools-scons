/**
 * @file Timer.h
 * @brief Stopwatch style timer to be used in instrumenting code when
 * gprof doesn't work properly.
 *
 * @author J. Chiang
 */

#ifndef st_facilities_Time_h
#define st_facilities_Time_h

#include <ctime>

#include "st_stream/StreamFormatter.h"

namespace st_facilities {

class Timer {

public:
   Timer(int chatter=2) 
      : m_formatter("st_facilities", "Timer", chatter), 
        m_running(false), m_counter(0) {
   }
   void start() {
      if (!m_running) {
         m_start = std::clock();
      }
      m_running = true;
   }
   void stop() {
      if (m_running) {
         m_counter += operator()();
      }
      m_running = false;
   }
   void reset() {
      m_running = false;
      m_counter = 0;
   }
   double operator()() const {
      return std::clock() - m_start;
   }
   double report(const std::string & location="") {
      m_formatter.info() << location;
      if (location != "") {
         m_formatter.info() << ": ";
      }
      double elapsed = m_counter/CLOCKS_PER_SEC;
      m_formatter.info() << elapsed << std::endl;
      return elapsed;
   }
private:
   st_stream::StreamFormatter m_formatter;
   bool m_running;
   double m_counter;
   std::clock_t m_start;
};

} // namespace st_facilities

#endif // st_facilities_Time_h
