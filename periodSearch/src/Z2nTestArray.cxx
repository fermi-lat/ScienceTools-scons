/** \file Z2nTestArray.cxx
    \brief Implementation of Z2nTestArray class.
    \authors Masaharu Hirayama, GSSC
             James Peachey, HEASARC/GSSC
*/

#include <sstream>
#include <stdexcept>

#include "ChiSquaredProb.h"
#include "Z2nTestArray.h"

Z2nTestArray::Z2nTestArray(size_type array_size, data_type::size_type num_harmonics):
  m_num_harm(num_harmonics), m_sine_cont(array_size, data_type(num_harmonics, 0.)),
  m_cosine_cont(array_size, data_type(num_harmonics, 0.)), m_num_events(array_size, 0) {}

void Z2nTestArray::fill(size_type array_index, double phase) {
  // Define two pi (for convenience and clarity).
  static const double s_2pi = 2. * 4. * std::atan(1.0);

  // Get the storage for sine and consine component.
  data_type & sine_array = m_sine_cont.at(array_index);
  data_type & cosine_array = m_cosine_cont.at(array_index);

  // For each phase, the complex Fourier component is computed for each trial harmonic.
  for (size_type jj = 0; jj < m_num_harm; ++jj) {
    double phase_angle = s_2pi * (jj + 1) * phase;
    sine_array[jj] += std::sin(phase_angle);
    cosine_array[jj] += std::cos(phase_angle);
  }

  // Increment the number of events filled.
  ++(m_num_events.at(array_index));
}

void Z2nTestArray::computePower(size_type array_index, data_type & power) const {
  // Initialize the container of the Fourier powers to return.
  power.resize(m_num_harm);
  power.assign(m_num_harm, 0.);

  // Get the storage for sine and consine component.
  const data_type & sine_array = m_sine_cont.at(array_index);
  const data_type & cosine_array = m_cosine_cont.at(array_index);

  // Compute normalization.
  double fourier_norm = 2. / m_num_events.at(array_index);

  // Compute the Fourier powers.
  for (size_type jj = 0; jj < m_num_harm; ++jj) {
    power[jj] = fourier_norm * (sine_array[jj] * sine_array[jj] + cosine_array[jj] * cosine_array[jj]);
  }
}

double Z2nTestArray::testStat(size_type array_index) const {
  // Compute the Fourier powers.
  data_type power;
  computePower(array_index, power);

  // Sum up the Fourier powers over harmonics.
  double summed_power = 0.;
  for (data_type::const_iterator itor = power.begin(); itor != power.end(); ++itor) summed_power += *itor;

  // Return the summed power.
  return summed_power;
}

std::pair<double, double> Z2nTestArray::chanceProb(double stat) const {
  //      /* Leahy et al. 1983, ApJ 266, 160 */
  //      chance_prob = chi2prob(test_stat[imax], 2*N_harm) * N_Fourier;
  //      [where function chi2prob(chi2, dof) returns the chi-squared
  //       distribution for "dof" degrees of freedom, integrated from "chi2"
  //       to infinity];
  periodSearch::ChiSquaredProb prob(2 * m_num_harm);
  return prob(stat);
}

std::string Z2nTestArray::getDescription() const {
  std::ostringstream os;
  os << "Type of test: Z2n Test, " << m_num_harm << " harmonics\n" <<
    "Probability distribution: Chi-squared, " << 2 * m_num_harm << " degrees of freedom";
  return os.str();
}

Z2nTestArray::size_type Z2nTestArray::size() const {
  return m_sine_cont.size();
}

void Z2nTestArray::getPlotData(size_type array_index, std::vector<double> & harmonic, std::vector<double> & power) const {
  // TODO: Implement this method, plotting sine and cosine component against the harmonic number.
  throw std::runtime_error("Z2nTestArray::getPlotData is not implemented yet.");
}

void Z2nTestArray::getPlotLabel(std::string & x_label, std::string & y_label) const {
  x_label = "Harmonic Number";
  y_label = "Power";
}

std::string Z2nTestArray::getPlotTitle() const {
  return "Fourier Power";
}
