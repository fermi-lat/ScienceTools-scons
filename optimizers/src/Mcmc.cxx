/**
 * @file Mcmc.cxx
 * @brief Implementation for generating a Markov Chain Monte Carlo of
 * a Function object using the Variable-at-a-time Metropolis-Hastings
 * update method.
 * @author J. Chiang
 * $Header$
 */

#include <cmath>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <sstream>

#include "CLHEP/Random/RandFlat.h"

#include "optimizers/dArg.h"
#include "optimizers/Exception.h"
#include "optimizers/Mcmc.h"

using CLHEP::RandFlat;

namespace optimizers {

#include "fitsio.h"

Mcmc::Mcmc(Function &stat, bool verbose) : m_stat(&stat), m_verbose(verbose) {
   estimateTransWidths();
}

void Mcmc::generateSamples(std::vector< std::vector<double> > &samples, 
                           unsigned long nsamp, bool clear) {
// Dummy Arg object required by Function methods:
   dArg dummy(1.);

// Get initial values of Parameters... 
   std::vector<double> paramValues;
   m_stat->getFreeParamValues(paramValues);

// and the Parameter objects themselves (for the bounds information)
   std::vector<Parameter> params;
   m_stat->getFreeParams(params);

   if (clear) {
      samples.clear();
   }

   if (m_verbose) {
      std::cerr << "Mcmc generating samples";
   }
   unsigned long sample_size(0);
   while (sample_size < nsamp) {
// Loop over parameters, treating each update step as a trial
      for (unsigned int i = 0; i < paramValues.size(); i++) {
         if (m_verbose && (sample_size % int(nsamp/20.) == 0)) {
            std::cerr << ".";
         }
         std::vector<double> newParamValues = paramValues;
         double transProbRatio;
         newParamValues[i] = drawValue(params[i], m_transitionWidths[i],
                                       transProbRatio);
// Hastings ratio
         m_stat->setFreeParamValues(newParamValues);
         double statValueNew = m_stat->value(dummy);
         m_stat->setFreeParamValues(paramValues);
         double statValue = m_stat->value(dummy);
         double alpha = transProbRatio*exp(statValueNew - statValue);
// Metropolis rejection criterion
         double drand = RandFlat::shoot();
         if (drand < alpha) {
// Accept the new point in Parameter space
            paramValues[i] = newParamValues[i];
            params[i].setValue(paramValues[i]);
         } else {
// Retain the old one
            newParamValues[i] = paramValues[i];
         }
// Append the objective function value
         newParamValues.push_back(-m_stat->value(dummy));
// We always append the current point after the update step
         samples.push_back(newParamValues);
         sample_size++;
      }
   }
   if (m_verbose) {
      std::cerr << "!" << std::endl;
   }
}

void Mcmc::writeSamples(std::string filename, 
                        std::vector< std::vector<double> > &samples) const {

   fitsfile *fptr;
   int status = 0;

   if (m_verbose) {
      std::cout << filename << std::endl;
   }
// Create the file.
   remove(filename.c_str());
   fits_create_file(&fptr, filename.c_str(), &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw Exception("Mcmc::writeSamples: cfitsio errors.");
   }

// Create the binary table, with the labels identifying the content of
// each column

   int tfields = static_cast<int>(samples[0].size());
   char *ttype[100];
   char *tform[100];
   char *tunit[100];

   if (m_verbose) {
      std::cout << "Creating binary table" << std::endl;
   }
   for (int i = 0; i < tfields; i++) {
      std::ostringstream type;
      type << "param" << i;
      ttype[i] = strdup(std::string(type.str()).c_str());
      tform[i] = strdup("1E");
      tunit[i] = strdup("None");
      if (m_verbose) {
         std::cout << ttype[i] << "  "
                   << tform[i] << "  "
                   << tunit[i] << std::endl;
      }
   }
   char *extname = strdup("Mcmc data");
   int nrows = samples.size();
   fits_create_tbl(fptr, BINARY_TBL, nrows, tfields, ttype, tform,
		   tunit, extname, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw Exception("Mcmc::writeSamples: cfitsio errors.");
   }

   int firstrow  = 1;  /* first row in table to write   */
   int firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

// write each column
   for (int i = 0; i < tfields; i++) {
// repack the data into a vector of floats
      std::vector<float> my_data;
      for (int j = 0; j < nrows; j++) {
         my_data.push_back(samples[j][i]);
      }
// Since the data in vectors are stored sequentially, one can pass the
// pointer to the first object as a C array.
      fits_write_col(fptr, TFLOAT, i+1, firstrow, firstelem, nrows, 
                     &my_data[0], &status);
      if (status != 0) {
         fits_report_error(stderr, status);
         throw Exception("Mcmc::writeSamples: cfitsio errors.");
      }
   }
   fits_close_file(fptr, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      throw Exception("Mcmc::writeSamples: cfitsio errors.");
   }
}

void Mcmc::estimateTransWidths() {

// Dummy Arg object for Function methods.
   dArg dummy(1.);

   m_transitionWidths.clear();

// Use an approximate Hessian to get estimates of each Parameters'
// error bars.
   std::vector<double> params;
   m_stat->getFreeParamValues(params);
   std::vector<double> derivs;
   m_stat->getFreeDerivs(dummy, derivs);
   double eps = 1e-5;

// Estimate the diagonal elements of the Hessian
   for (unsigned int i = 0; i < params.size(); i++) {
      std::vector<double> new_params = params;
      double delta = eps*params[i];
      new_params[i] += delta;
      m_stat->setFreeParamValues(new_params);
      m_stat->value(dummy);
      std::vector<double> new_derivs;
      m_stat->getFreeDerivs(dummy, new_derivs);
      double hessian = fabs((new_derivs[i] - derivs[i])/delta);
      m_transitionWidths.push_back(sqrt(1./hessian));
   }
}

double Mcmc::drawValue(Parameter &param, double dx, double &transProbRatio) {
// Normalize the top-hat function (i.e., find the width) at the input
// Parameter location given the Parameter bounds.

   double xl = param.getBounds().first;
   double xu = param.getBounds().second;
   double x0 = param.getValue();
   double width = std::min(xu, x0 + dx) - std::max(xl, x0 - dx);

// Draw the trial value...
   double drand = RandFlat::shoot();
   double y = drand*width + std::max(xl, x0 - dx);

// and compute the ratio of the transition probability densities
   transProbRatio = width/(std::min(xu, y + dx) - std::max(xl, y - dx));

   return y;
}

} // namespace optimizers
