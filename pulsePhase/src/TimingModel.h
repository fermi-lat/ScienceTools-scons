/** \file TimingModel.h
    \brief Encapsulation of standard timing model.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC
*/
#include <cmath>

class TimingModel {
  public:
    TimingModel(double epoch, double phi0, double f0, double f1, double f2): m_epoch(epoch), m_phi0(phi0), m_f0(f0), m_f1(f1),
      m_f2(f2) {}

    /** \brief Compute the phase of the given time.
        \param ev_time The time.
    */
    double calcPhase(double ev_time) {
      double dt = ev_time - m_epoch;
      double int_part; // ignored, needed for modf.
      double dt_squared = dt * dt;
      double phase = modf(m_phi0 + m_f0 * dt + m_f1/2.0 * dt_squared + m_f2/6.0 * dt * dt_squared, &int_part);
      if (phase < 0.) ++phase;
      return phase;
    }

  private:
    double m_epoch;
    double m_phi0;
    double m_f0;
    double m_f1;
    double m_f2;
};
