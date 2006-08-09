/** \file TimeRep.cxx
    \brief Implementation of TimeRep and related classes.
    \authors Masaharu Hirayama, GSSC
             James Peachey, HEASARC/GSSC
*/
#include "timeSystem/AbsoluteTime.h"
#include "timeSystem/GlastMetRep.h"
#include "timeSystem/TimeRep.h"
#include "timeSystem/TimeSystem.h"

#include "tip/Header.h"

#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {
  int s_digits = std::numeric_limits<double>::digits10;
}

namespace timeSystem {

  TimeRep::~TimeRep() {}

  TimeRep & TimeRep::operator =(const AbsoluteTime & abs_time) {
    abs_time.exportTimeRep(*this);
    return *this;
  }

  std::ostream & operator <<(std::ostream & os, const TimeRep & time_rep) {
    time_rep.write(os);
    return os;
  }

  st_stream::OStream & operator <<(st_stream::OStream & os, const TimeRep & time_rep) {
    time_rep.write(os);
    return os;
  }


  MetRep::MetRep(const std::string & system_name, long mjd_ref_int, double mjd_ref_frac, double met):
    m_system(&TimeSystem::getSystem(system_name)), m_mjd_ref(IntFracPair(mjd_ref_int, mjd_ref_frac), Day), m_met(met) {}

  MetRep::MetRep(const tip::Header & header, double met): m_system(0), m_mjd_ref(), m_met(met) {
    std::string system_name;
    header["TIMESYS"].get(system_name);

    bool found_mjd_ref = false;

    if (!found_mjd_ref) {
      try {
        int mjd_ref_int = 0;
        double mjd_ref_frac = 0.;
        header["MJDREFI"].get(mjd_ref_int);
        header["MJDREFF"].get(mjd_ref_frac);
        m_mjd_ref = Duration(IntFracPair(mjd_ref_int, mjd_ref_frac), Day);
        found_mjd_ref = true;
      } catch (const std::exception &) {}
    }

    if (!found_mjd_ref) {
      try {
        double mjd_ref = 0.;
        header["MJDREF"].get(mjd_ref);
        IntFracPair mjd(mjd_ref);
        int mjd_ref_int = mjd.getIntegerPart();
        double mjd_ref_frac = mjd.getFractionalPart();
        m_mjd_ref = Duration(IntFracPair(mjd_ref_int, mjd_ref_frac), Day);
        found_mjd_ref = true;
      } catch (const std::exception &) {}
    }

#if 0
    if (!found_mjd_ref) {
      try {
        std::string telescope = header["TELESCOP"].get();
        for (std::string::iterator itor = telescope.begin(); itor != telescope.end(); ++itor) *itor = std::toupper(*itor);
        // TODO To support more missions, use prototypes looked up by telescope name instead of hardwired GlastMetRep.
        if (telescope == "GLAST") {
          GlastMetRep glast_met(system_name, met);
          m_mjd_ref = glast_met.m_mjd_ref;
          found_mjd_ref = true;
        }
      } catch (const std::exception &) {}
    }
#endif

    if (!found_mjd_ref) {
      throw std::runtime_error("MetRep could not find MJDREFI/MJDREFF or MJDREF.");
    }

    m_system = &TimeSystem::getSystem(system_name);
  }

  MetRep & MetRep::operator =(const AbsoluteTime & abs_time) { TimeRep::operator =(abs_time); return *this; }

  void MetRep::get(std::string & system_name, Duration & origin, Duration & elapsed) const {
    system_name = m_system->getName(); origin = m_mjd_ref, elapsed = Duration(0, m_met);
  }

  void MetRep::set(const std::string & system_name, const Duration & origin, const Duration & elapsed) {
    // Convert from the given time into "this" system.
    Moment my_time = m_system->convertFrom(TimeSystem::getSystem(system_name), Moment(origin, elapsed));

    // Now compute met from my_time in this system.
    Duration met = my_time.second + m_system->computeTimeDifference(my_time.first, m_mjd_ref);
    IntFracPair met_pair = met.getValue(Sec);
    m_met = met_pair.getDouble();
  }

  std::string MetRep::getString() const {
    std::ostringstream os;
    os << std::setprecision(s_digits) << getValue() << " MET (" << *m_system << ") [MJDREF=" << m_mjd_ref.getValue(Day) << "]";
    return os.str();
  }

  void MetRep::assign(const std::string & value) {
    IntFracPair pair_value(value);
    setValue(pair_value.getDouble());
  }

  double MetRep::getValue() const { return m_met; }

  void MetRep::setValue(double met) { m_met = met; }


  MjdRep::MjdRep(const std::string & system_name, long mjd_int, double mjd_frac):
    m_system(&TimeSystem::getSystem(system_name)), m_mjd(IntFracPair(mjd_int, mjd_frac), Day) {}

  MjdRep & MjdRep::operator =(const AbsoluteTime & abs_time) { TimeRep::operator =(abs_time); return *this; }

  void MjdRep::get(std::string & system_name, Duration & origin, Duration & elapsed) const {
    system_name = m_system->getName(); origin = m_mjd, elapsed = Duration(0, 0.);
  }

  void MjdRep::set(const std::string & system_name, const Duration & origin, const Duration & elapsed) {
    // Convert from the given time into "this" system.
    Moment my_time = m_system->convertFrom(TimeSystem::getSystem(system_name), Moment(origin, elapsed));

    // Now compute mjd from my_time in this system.
    m_mjd = m_system->computeMjd(my_time);
  }

  std::string MjdRep::getString() const {
    std::ostringstream os;
    os << std::setprecision(s_digits) << getValue() << " MJD (" << *m_system << ")";
    return os.str();
  }

  void MjdRep::assign(const std::string & value) {
    IntFracPair pair_value(value);
    setValue(pair_value.getIntegerPart(), pair_value.getFractionalPart());
  }

  IntFracPair MjdRep::getValue() const { return m_mjd.getValue(Day); }

  void MjdRep::setValue(long mjd_int, double mjd_frac) { m_mjd = Duration(IntFracPair(mjd_int, mjd_frac), Day); }

}
