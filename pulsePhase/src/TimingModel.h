/** \file TimingModel.h
    \brief Encapsulation of standard timing model.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC
*/
#include <cmath>
#include <vector>

class TimingModel {
  public:
    struct Coeff {
        Coeff(): m_term(3) {}

        Coeff(double c0, double c1, double c2): m_term() {
          m_term.push_back(c0);
          m_term.push_back(c1);
          m_term.push_back(c2);
        }

        Coeff invert() const {
          double term0squared = m_term[0] * m_term[0];
          Coeff new_coeff;
          // Invert the Taylor series coefficients.
          new_coeff.m_term[0] = 1.0 / m_term[0];
          new_coeff.m_term[1] = - m_term[1] / term0squared;
          new_coeff.m_term[2] = 2.0 * m_term[1] * m_term[1] / (m_term[0] * term0squared) - m_term[2] / term0squared;

          return new_coeff;
        }

        std::vector<double> m_term;
    };

    struct FrequencyCoeff : Coeff {
        FrequencyCoeff(double f0, double f1, double f2): Coeff(f0, f1, f2) {}
    };

    struct PeriodCoeff : Coeff {
        PeriodCoeff(double p0, double p1, double p2): Coeff(p0, p1, p2) {}
    };

    TimingModel(double epoch, double phi0, double f0, double f1, double f2): m_epoch(epoch), m_phi0(phi0), m_f0(f0), m_f1(f1),
      m_f2(f2) {}

    TimingModel(double epoch, double phi0, const FrequencyCoeff & freq_coeff): m_epoch(epoch), m_phi0(phi0), m_f0(0), m_f1(0),
      m_f2(0) {
      m_f0 = freq_coeff.m_term[0];
      m_f1 = freq_coeff.m_term[1];
      m_f2 = freq_coeff.m_term[2];
    }

    TimingModel(double epoch, double phi0, const PeriodCoeff & period_coeff): m_epoch(epoch), m_phi0(phi0), m_f0(0), m_f1(0),
      m_f2(0) {
      Coeff freq_coeff = period_coeff.invert();
      m_f0 = freq_coeff.m_term[0];
      m_f1 = freq_coeff.m_term[1];
      m_f2 = freq_coeff.m_term[2];
    }

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
