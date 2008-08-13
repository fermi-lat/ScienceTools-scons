/**
 * @file DefaultAlignment.h
 *
 * @author J. Chiang <jchiang@slac.stanford.edu
 *
 * $Header$
 *
 */

#include <vector>

namespace gtalign {

class DefaultAlignment {

public:

   static DefaultAlignment & instance();

   bool resetInterval(double time);

   double rx() const {
      return m_rx.at(m_interval);
   }

   double ry() const {
      return m_ry.at(m_interval);
   }

   double rz() const {
      return m_rz.at(m_interval);
   }

protected:

   DefaultAlignment();

   static DefaultAlignment * s_instance;

private:

   size_t m_interval;

   std::vector<double> m_tstart;
   std::vector<double> m_rx;
   std::vector<double> m_ry;
   std::vector<double> m_rz;

};

} // namespace gtalign
