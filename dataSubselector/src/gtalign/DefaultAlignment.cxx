/**
 * @file DefaultAlignment.cxx
 *
 * @author J. Chiang <jchiang@slac.stanford.edu>
 * $Header
 */

#include <algorithm>
#include <iostream>

#include "DefaultAlignment.h"

namespace gtalign {

DefaultAlignment * DefaultAlignment::s_instance(0);

DefaultAlignment & DefaultAlignment::instance() {
   if (s_instance == 0) {
      s_instance = new DefaultAlignment();
   }
   return *s_instance;
}

DefaultAlignment::DefaultAlignment() : m_interval(0) {
   double tstart[] = {0, 236511638, 237154733, 237343796, 999999999};
   m_tstart = std::vector<double>(tstart, tstart+sizeof(tstart)/sizeof(double));
   double rx[] = {0, -161, -303, -170, 0};
   double ry[] = {0, -170, -218, -173, 0};
   double rz[] = {0, -475, -740, -491, 0};
   m_rx = std::vector<double>(rx, rx + sizeof(rx)/sizeof(double));
   m_ry = std::vector<double>(ry, ry + sizeof(ry)/sizeof(double));
   m_rz = std::vector<double>(rz, rz + sizeof(rz)/sizeof(double));
}

bool DefaultAlignment::resetInterval(double time) {
   if ((m_tstart.at(m_interval) <= time && 
        time < m_tstart.at(m_interval+1))) {
      return false;
   }
   m_interval = (std::upper_bound(m_tstart.begin(), m_tstart.end(), time) 
                 - m_tstart.begin()) - 1;
   return true;
}

} // namespace gtalign
