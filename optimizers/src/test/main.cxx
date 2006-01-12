// test program for optimizers

#ifdef TRAP_FPE
#include <fenv.h>
#endif // TRAP_FPE

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <vector>

//  include everything for the compiler to test

#include "optimizers/Exception.h"
#include "optimizers/Parameter.h"
#include "optimizers/Function.h"
#include "optimizers/dArg.h"
#include "optimizers/SumFunction.h"
#include "optimizers/ProductFunction.h"
#include "optimizers/OutOfBounds.h"
#include "optimizers/Optimizer.h"
#include "optimizers/Lbfgs.h"
#include "optimizers/Minuit.h"
#include "optimizers/Drmngb.h"
#include "optimizers/Mcmc.h"
#include "optimizers/FunctionTest.h"
#include "optimizers/FunctionFactory.h"
#include "optimizers/ChiSq.h"
#include "optimizers/newMinuit.h"
#include "MyFun.h"
#include "PowerLaw.h"
#include "Gaussian.h"
#include "Rosen.h"
#include "RosenND.h"
#include "AbsEdge.h"

#ifdef HAVE_OPT_PP
#include "optimizers/OptPP.h"
#endif

using namespace optimizers;   // for testing purposes only

void test_FunctionFactory();
void test_Parameter_class();
void test_Function_class();
void test_PowerLaw_class();
void test_CompositeFunction();
void test_Optimizers();
void test_Mcmc();
void test_ChiSq();

std::string test_path;

int main() {
#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif
   test_FunctionFactory();
   test_Parameter_class();
   test_Function_class();
   test_PowerLaw_class();
   test_CompositeFunction();
   test_Optimizers();
   test_Mcmc();
   test_ChiSq();
   return 0;
}

using namespace optimizers;   // for testing purposes only

void test_FunctionFactory() {
   std::cout << "*** testing FunctionFactory ***" << std::endl;

   std::string xmlFile;
   const char *root = ::getenv("OPTIMIZERSROOT");
   if (!root) {
      xmlFile = "../xml/FunctionModels.xml";
   } else {
      xmlFile = std::string(root) + "/xml/FunctionModels.xml";
   }

// Create the factory object and list the standard prototypes.
   FunctionFactory funcFactory;

   std::vector<std::string> funcNames;
   funcFactory.getFunctionNames(funcNames);
   std::vector<std::string>::const_iterator nameIt = funcNames.begin();
   for ( ; nameIt != funcNames.end(); nameIt++) {
      std::cout << *nameIt << std::endl;
   }

// Read in the customized prototypes.
   try {
      funcFactory.readXml(xmlFile);
   } catch (optimizers::Exception &eObs) {
      std::cerr << eObs.what() << std::endl;
   } catch (...) {
      std::cerr << "other exception caught while reading "
                << xmlFile << std::endl;
   }

   funcFactory.getFunctionNames(funcNames);
   nameIt = funcNames.begin();
   for ( ; nameIt != funcNames.end(); nameIt++) {
      std::cout << *nameIt << std::endl;
   }

// Try out one of the new ones.
   Function *gaussObj = funcFactory.create("Generic Gaussian");

   int npts = 50;
   double xmin = 6.;
   double xmax = 7.;
   double xstep = (xmax - xmin)/(npts-1);
   for (int i = 0; i < npts; i++) {
      double x = i*xstep + xmin;
      dArg xarg(x);
      std::cout << x << "  "
                << (*gaussObj)(xarg) << std::endl;
   }

   std::cout << "\n*** Test for failure of readXml() method. ***\n" 
             << std::endl;
   if (!root) {
      xmlFile = "../xml/BadModel.xml";
   } else {
      xmlFile = std::string(root) + "/xml/BadModel.xml";
   }

   try {
      funcFactory.readXml(xmlFile);
   } catch (optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
      std::cout << "*** End of readXml() failure test. ***\n" << std::endl;
   } catch (...) {
      std::cerr << "other exception caught while reading "
                << xmlFile << std::endl;
   }

// Write the Functions in funcFactory to an XML file.
   xmlFile = "outputModels.xml";
   try {
      funcFactory.writeXml(xmlFile);
   } catch (optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   } catch (...) {
      std::cerr << "other exception caught while writing "
                << xmlFile << std::endl;
   }

   std::cout << "*** test_FunctionFactory: all tests completed ***\n" 
             << std::endl;

}

void test_Mcmc() {
   std::cout << "*** test_Mcmc ***" << std::endl;

   Rosen my_rosen;

   try {
      std::vector<Parameter> params;
      my_rosen.getParams(params);
      params[0].setValue(2.);
      params[0].setBounds(-10., 10.);
      params[1].setValue(2.);
      params[1].setBounds(-10., 10.);
      my_rosen.setParams(params);

      std::vector<double> my_sig(2);
      double scale = 1.;

      int verbose = 1;
      Minuit myMinuitObj(my_rosen);
      myMinuitObj.find_min(verbose, .0001);
      std::vector<double> sig = myMinuitObj.getUncertainty();
      for (unsigned int i=0; i < sig.size(); i++) {
         std::cout << i << "  " << sig[i] << std::endl;
         my_sig[i] = sig[i]*scale;
      }

      Mcmc myMcmcObj(my_rosen);
      std::vector<double> widths;
      myMcmcObj.getTransitionWidths(widths);
      for (unsigned int i = 0; i < widths.size(); i++) {
         std::cout << widths[i] << "  "
                   << my_sig[i] << std::endl;
      }
      myMcmcObj.setTransitionWidths(my_sig);

      std::vector< std::vector<double> > mcmc_samples;
      long nsamp = 100000;
// Do a "burn in"...
      myMcmcObj.generateSamples(mcmc_samples, nsamp, true);
// then the real thing...
      nsamp = 10000;
      myMcmcObj.generateSamples(mcmc_samples, nsamp, true);
      
// generate "fake" samples to test cfitsio

      myMcmcObj.writeSamples("Mc.fits", mcmc_samples);

      std::cout << "MCMC results for " << nsamp << " trials:" << std::endl;
      for (unsigned int j = 0; j < params.size(); j++) {
         double mc_avg = 0;
         double mc2_avg = 0;
         for (unsigned int i = 0; i < mcmc_samples.size(); i++) {
            mc_avg += mcmc_samples[i][j];
            mc2_avg += pow(mcmc_samples[i][j], 2);
         }
         mc_avg /= mcmc_samples.size();
         mc2_avg /= mcmc_samples.size();
         std::cout << "Parameter " << j << ": "
                   << mc_avg << " +/- "
                   << sqrt(mc2_avg - mc_avg*mc_avg) << std::endl;
      }
   
// // Write data as an ascii file.
//       std::string filename = test_path + "Data/Mcmc.dat"; 
//       std::ofstream outfile(filename.c_str());
//       if (!outfile) {
//          std::cout << "opening Mcmc.dat failed." << std::endl;
//          assert(false);
//       }
      
//       for (unsigned int i = 0; i < mcmc_samples.size(); i++) {
//          for (unsigned int j = 0; j < params.size(); j++) {
//             outfile << mcmc_samples[i][j] << "  ";
//          }
//          outfile << "\n";
//       }

   } catch (OutOfBounds &eObj) {
      assert(eObj.code() == OutOfBounds::VALUE_ERROR);
      std::cerr << eObj.what() << "\n"
                << "Value: " << eObj.value() << "\n"
                << "minValue: " << eObj.minValue() << "\n"
                << "maxValue: " << eObj.maxValue() << "\n" 
                <<std::endl;
   } catch(optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }

   std::cout << "*** test_Mcmc: all tests completed ***\n" << std::endl;

} // test_Mcmc

/*******************/
/* Test Optimizers */
/*******************/
void test_Optimizers() {
   std::cout << "*** test_Optimizers ***" << std::endl;

// Test the OptPP code using my standard 2D Rosenbrock test with
// bounds constraints
   Rosen my_rosen;

   std::vector<Parameter> params;
   my_rosen.getParams(params);
   params[0].setValue(2.);
   params[0].setBounds(-5., 10);
   params[1].setValue(2.);
   params[1].setBounds(-4, 10);
   my_rosen.setParams(params);

   int verbose = 1;

// try the C++ version of Minuit

   newMinuit myNewMin(my_rosen);
   try {
     myNewMin.find_min(verbose, .00001);
   }
   catch (optimizers::Exception& rrr) {
     std::cout << "optimizers::Exception: " << rrr.what() << std::endl;
   }
// use Minuit's HESSE to get the covariance matrix by finite differences
   myNewMin.hesse(verbose);
   
// try lbfgs_bcm method 
   Lbfgs my_lbfgsObj(my_rosen);
   my_lbfgsObj.setMaxVarMetCorr(12);
   my_lbfgsObj.setPgtol(.0000001);
   try {
     my_lbfgsObj.find_min(verbose, .000001);
   }
   catch(optimizers::Exception& rrr) {
     std::cout << "optimizers::Exception: " << rrr.what() << std::endl;
   }
   std::cout << "LBFGS exit code: " 
             << my_lbfgsObj.getRetCode() 
             << std::endl;
   std::cout << "LBFGS end message: " 
             << my_lbfgsObj.getErrorString() 
             << std::endl;

   verbose = 3;
   params[0].setValue(2.);
   params[0].setBounds(-10., 10.);
   params[1].setValue(2.);
   params[1].setBounds(-4, 10.);
   my_rosen.setParams(params);
   Minuit myMinuitObj(my_rosen);
   myMinuitObj.find_min(verbose, .0001);
   std::vector<double> sig = myMinuitObj.getUncertainty();
   for (unsigned int i=0; i < sig.size(); i++) {
     std::cout << i << "  " << sig[i] << std::endl;
   }

   std::cout 
      << "\nTest DRMNGB method using 5 dimensional Rosenbrock function\n" 
      << std::endl;
   RosenND rosenND(5);
   rosenND.getParams(params);
   for (unsigned int i = 0; i < params.size(); i++) {
      params[i].setValue(2.);
      params[i].setBounds(-10., 10.);
   }
   rosenND.setParams(params);
   Drmngb my_Drmngb(rosenND);
   try {
      my_Drmngb.find_min(verbose, .0001);
   } catch (optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }
   std::cout << "Drmngb exit code: " << my_Drmngb.getRetCode() << std::endl;
   sig = my_Drmngb.getUncertainty();
   std::cout << "Uncertainties:" << std::endl;
   for (unsigned int i=0; i < sig.size(); i++) {
      std::cout << i << "  " << sig[i] << std::endl;
   }

   std::cout 
      << "\nTest Minuit method using 5 dimensional Rosenbrock function\n" 
      << std::endl;
//    rosenND.getParams(params);
//    for (unsigned int i = 0; i < params.size(); i++) {
//       params[i].setValue(2.);
//       params[i].setBounds(-10., 10.);
//    }
//    rosenND.setParams(params);
   Minuit my_Minuit(rosenND);
   try {
      my_Minuit.find_min(verbose, .0001);
   } catch (optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }
   sig = my_Minuit.getUncertainty();
   std::cout << "Uncertainties:" << std::endl;
   for (unsigned int i=0; i < sig.size(); i++) {
      std::cout << i << "  " << sig[i] << std::endl;
   }

#ifdef HAVE_OPT_PP
// now restart and try OptPP
   try {
      my_rosen.getParams(params);
      for (unsigned int i = 0; i < params.size(); i++) {
         params[i].setValue(2.);
      }
      my_rosen.setParams(params);
      OptPP my_OptppObj(my_rosen);
      my_OptppObj.find_min(verbose);
      std::cout << "Tested OptPP:" << std::endl;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   }
#endif  //HAVE_OPT_PP
   
   rosenND.getParams(params);
   for (unsigned int i = 0; i < params.size(); i++) 
      std::cout << params[i].getName() << ": "
                << params[i].getValue() << std::endl;

   std::cout << "*** test_Optimizers: all tests completed ***\n" << std::endl;
}
// test_Optimizers

void test_CompositeFunction() {

   std::cout << "*** test_CompositeFunction ***" << std::endl;

// add a PowerLaw and Gaussian together 

   PowerLaw pl_continuum(1., -2., 1.);
   Gaussian emission_line(10., 1., 0.1);

   SumFunction spectrum(pl_continuum, emission_line);

// multiply by an absorption edge

   AbsEdge edge(5., 1.2);
   ProductFunction absorbed_spec(edge, spectrum);

   int nmax = 100;
   double xmin = 0.1;
   double xmax = 10.;
   double xstep = log(xmax/xmin)/(nmax-1);
   for (int i = 0; i < nmax; i++) {
      double x = xmin*exp(xstep*i);
      dArg xarg(x);
      std::cout << x << "  " << absorbed_spec(xarg) << std::endl;
   }
   
// check the derivatives of absorbed_spec
   std::vector<double> params;
   absorbed_spec.getParamValues(params);
   dArg xarg(1.5);
   double spec_value = absorbed_spec(xarg);
   std::vector<double> derivs;
   absorbed_spec.getDerivs(xarg, derivs);

   double eps = 1e-7;
   for (unsigned int i = 0; i < params.size(); i++) {
      std::vector<double> new_params = params;
      double dparam = eps*new_params[i];
      new_params[i] += dparam;
      absorbed_spec.setParamValues(new_params);
      double new_spec_value = absorbed_spec(xarg);
      double num_deriv = (new_spec_value - spec_value)/dparam;
      std::cerr << derivs[i] << "  "
                << num_deriv << "  "
                << derivs[i]/num_deriv << "  "
                << std::endl;
   }

   std::cout << "*** test_CompositeFunction: all tests completed ***\n" 
             << std::endl;

}

/************************/
/* PowerLaw class tests */
/************************/
void test_PowerLaw_class() {

   std::cout << "*** test_PowerLaw_class ***" << std::endl;

   PowerLaw pl;

/* inspect the default parameters */
   std::vector<Parameter> my_params;
   pl.getParams(my_params);

   std::vector<Parameter>::iterator iter = my_params.begin();
   for (; iter != my_params.end(); iter++) {
      std::cout << (*iter).getName() << ":  " 
                << (*iter).getValue() << ", "
                << (*iter).isFree() << std::endl;
   }
   std::cout << std::endl;

/* get the free ones */
   pl.getFreeParams(my_params);

   iter = my_params.begin();
   for (; iter != my_params.end(); iter++) {
      std::cout << (*iter).getName() << ":  " 
                << (*iter).getValue() << ", "
                << (*iter).isFree() << std::endl;
   }
   std::cout << std::endl;

/* reset the parameters and compute some values */
   pl.setParam("Prefactor", 2.);
   pl.setParam("Index", -2.2);
   for (double xx = 1.05; xx < 1e3; xx *= xx) {
      dArg xarg(xx);
      std::cout << xx << "   " << pl(xarg) << std::endl;
   }

/* get the derivatives and compare to numerical estimates */
   dArg x(10.);

   std::vector<double> pl_derivs;
   pl.getDerivs(x, pl_derivs);

   std::vector<std::string> paramNames;
   pl.getParamNames(paramNames);
// save current parameter values
   std::vector<double> params_save;
   pl.getParamValues(params_save);

   std::cout << "\nDerivatives: " << std::endl;
   for (unsigned int i = 0; i < pl_derivs.size(); i++) {
      std::cout << pl_derivs[i] << ":  ";

// compute the numerical derivative wrt this parameter
      double delta_param = fabs(params_save[i]/1e5);
      double num_deriv = pl(x);
      pl.setParam(paramNames[i], params_save[i]+delta_param);
      num_deriv -= pl(x);
      num_deriv /= delta_param;
      std::cout << -num_deriv << std::endl;

// reset the parameters for next time around
      pl.setParamValues(params_save);
   }

/* free derivatives */
   pl.getFreeParamNames(paramNames);
   pl.getFreeDerivs(x, pl_derivs);

   std::cout << "\nFree Derivatives: " << std::endl;
   for (unsigned int i = 0; i < pl_derivs.size(); i++) {
      std::cout << paramNames[i] << ":  "
                << pl_derivs[i] << ":  "
                << pl.derivByParam(x, paramNames[i])
                << std::endl;
   }
   std::cout << std::endl;

/* instantiate another power-law and compare its output to that of pl */
   PowerLaw pl2(1., -2.1, 1.);

   for (double x = 1.05; x < 1e3; x *= x) {
      dArg xarg(x);
      std::cout << x << "   " 
                << pl(xarg) << "  " 
                << pl2(xarg) << std::endl;
   }
   std::cout << std::endl;

/* check the copy constructor */
   PowerLaw pl3(pl2);

   pl2.getParams(my_params);
   std::cout << "Parameters for pl2:" << std::endl;

   iter = my_params.begin();
   for (; iter != my_params.end(); iter++) {
      std::cout << (*iter).getName() << ":  " 
                << (*iter).getValue() << ", "
                << (*iter).isFree() << std::endl;
   }
   std::cout << std::endl;

   pl3.getParams(my_params);
   std::cout << "Parameters for pl3:" << std::endl;

   iter = my_params.begin();
   for (; iter != my_params.end(); iter++) {
      std::cout << (*iter).getName() << ":  " 
                << (*iter).getValue() << ", "
                << (*iter).isFree() << std::endl;
   }
   std::cout << std::endl;

   std::cout << "*** test_PowerLaw_class: all tests completed ***\n" 
             << std::endl;

} // PowerLaw class tests

/************************/
/* Function class tests */
/************************/
void test_Function_class() {

   std::cout << "*** test_Function_class ***" << std::endl;

// test constructor and addParam() method (see MyFun.cxx)
   MyFun f;

   f.setName("Hal");
   assert(f.getName() == std::string("Hal"));
   
// setting and getting parameter names and values
   double vals[] = {1., 2., 4.};
   char *names[] = {"Ruthie", "Mary", "Jane"};
   for (int i = 0; i < 3; i++) {
      f.setParam(names[i], vals[i]);
   }

   std::vector<std::string> paramNames;
   f.getParamNames(paramNames);
   for (unsigned int i = 0; i < paramNames.size(); i++) {
      assert(paramNames[i] == std::string(names[i]));
   }
      
   for (unsigned int i = 0; i < f.getNumParams(); i++) {
      assert(f.getParamValue(paramNames[i]) == vals[i]);
   }

// argument passing using dArg class
   dArg x(3);
   double f_val = 0;   // recall value(...) implementation from MyFun.cxx
   for (int i = 0; i < 3; i++) {
      f_val += vals[i]*pow(3., i);
   }
   assert(f(x) == f_val);

// try to access a parameter not named in the function
   try {
      double value = f.getParamValue("foo");
      std::cout << value << std::endl;
   } catch(optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }

// reset all of the parameters in one shot
   std::vector<double> inputVec;
   for (unsigned int i = 0; i < f.getNumParams(); i++) { 
      inputVec.push_back(i*i);
      vals[i] = i*i;
   }
   f.setParamValues(inputVec);

// retrieve them in one shot and ensure that the values are correct
   std::vector<double> returnVec;
   f.getParamValues(returnVec);
   for (unsigned int i = 0; i < returnVec.size(); i++) {
      assert(returnVec[i] == static_cast<double>(i*i));
   }

// change the value of an existing parameter
   f.setParam(std::string("Ruthie"), 10);
   assert(f.getParamValue("Ruthie") == 10);

// attempt to change the value of a non-existent parameter
   try {
      f.setParam(std::string("Oscar"), 5.);
   } catch(optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }

// check group accessors
   std::vector<double> params;
   f.getParamValues(params);
   assert(params[0] == 10);
   assert(params[1] == vals[1]);
   assert(params[2] == vals[2]);

// get derivatives wrt parameters...
//
// one-by-one:
   x = dArg(2);
   for (unsigned int i = 0; i < paramNames.size(); i++) {
      assert(f.derivByParam(x, paramNames[i]) == pow(2., static_cast<int>(i)));
   }

// all derivatives in one shot:
   std::vector<double> derivs;
   f.getDerivs(x, derivs);
   for (unsigned int i = 0; i < derivs.size(); i++) {
      assert(derivs[i] == pow(2., static_cast<int>(i)));
   }

// test getParam(...) method
   for (unsigned int i = 0; i < paramNames.size(); i++) {
      Parameter my_param = f.getParam(paramNames[i]);
      assert(my_param.getName() == paramNames[i]);
      assert(my_param.getValue() == params[i]);
   }

   try {
      Parameter my_param = f.getParam("Joan");
   } catch(optimizers::Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }

// test the default Function copy assignment operator
   MyFun f2 = f;
   for (double x = 0; x < 100.; x += 5.) {
      dArg xarg(x);
      assert(f(xarg) == f2(xarg));
   }

// default FunctionTests

   FunctionTest myTests(f, "Hal");

   std::vector<Parameter> parameters;
   parameters.push_back(Parameter("Ruthie", 1.));
   parameters.push_back(Parameter("Mary", 4.));
   parameters.push_back(Parameter("Jane", 8.));

   myTests.parameters(parameters);

   myTests.freeParameters(parameters);

   std::vector<Arg*> args;
   args.push_back(new dArg(1));
   args.push_back(new dArg(2));
   args.push_back(new dArg(4));
   args.push_back(new dArg(8));

   double ret_vals[] = {15, 28, 78, 274};
   std::vector<double> retVals(ret_vals, ret_vals+4);

   myTests.funcEvaluations(args, retVals);

   myTests.derivatives(args);

   for (unsigned int i = 0; i < args.size(); i++)
      delete args[i];

   std::cout << "*** test_Function_class: all tests passed ***\n" 
             << std::endl;
      
} // Function class (MyFun) tests

/*************************/
/* Parameter class tests */
/*************************/
void test_Parameter_class() {

   std::cout << "*** test_Parameter_class ***" << std::endl;

   std::vector<Parameter> my_params;

// test for success
   Parameter my_param("William", 42.);
   my_params.push_back(my_param);
   
   my_param.setName("Tecumseh");
   my_param.setValue(13.7);
   my_param.setBounds(0., 20.);
   my_params.push_back(my_param);
   
   my_param.setName("Sherman");
   std::pair<double, double> myBounds(-10, 30);
   my_param.setBounds(myBounds);
   my_param.setValue(25.);
   my_params.push_back(my_param);

   std::vector<Parameter>::iterator iter = my_params.begin();
   for (; iter != my_params.end(); iter++) {
      std::cout << iter->getName() << ":  " 
                << iter->getValue() << "  "
                << iter->getBounds().first << "  "
                << iter->getBounds().second << "  "
                << iter->isFree()
                << std::endl;
   }

// test for consistency...
   assert(my_params[0].getName() == std::string("William"));
   assert(my_params[0].getValue() == 42.);

   assert(my_params[1].getName() == std::string("Tecumseh"));
   assert(my_params[1].getValue() == 13.7);
   assert(my_params[1].getBounds().first == 0.);
   assert(my_params[1].getBounds().second == 20.);

   assert(my_params[2].getName() == std::string("Sherman"));
   assert(my_params[2].getValue() == 25);
   assert(my_params[2].getBounds().first == -10);
   assert(my_params[2].getBounds().second == 30);

   std::cout << std::endl;

// test for failure
   try {
      my_params[2].setValue(35);
   } catch (OutOfBounds &eObj) {
      assert(eObj.code() == OutOfBounds::VALUE_ERROR);
      std::cerr << eObj.what() << "\n"
                << "Value: " << eObj.value() << "\n"
                << "minValue: " << eObj.minValue() << "\n"
                << "maxValue: " << eObj.maxValue() << "\n" 
                <<std::endl;
   }

   try {
      my_params[2].setBounds(-20, 20);
   } catch (OutOfBounds &eObj) {
      assert(eObj.code() == OutOfBounds::BOUNDS_ERROR);
      std::cerr << eObj.what() << "\n"
                << "Value: " << eObj.value() << "\n"
                << "minValue: " << eObj.minValue() << "\n"
                << "maxValue: " << eObj.maxValue() << "\n"
                << std::endl;
   }

   std::cout << "*** test_Parameter_class: all tests passed ***\n" 
             << std::endl;

} // Parameter class tests

void test_ChiSq() {
   std::cout << "*** test_ChiSq ***" << std::endl;

   bool passed = true;
   std::vector<double> domain(2);
   std::vector<double> range(2);

// test for failure
   Gaussian gauss(10., 1., 0.1);
   try {
      ChiSq chi_sq(domain, range, &gauss);
      std::cerr << "creating ChiSq object from too small a data set "
                << "did not throw an exception"
                << std::endl;
      assert(0);
   } catch (const std::exception &) {
      // Expected.
   }

   domain.resize(10);
   range.resize(9);
   try {
      ChiSq chi_sq(domain, range, &gauss);
      std::cerr << "creating ChiSq object from domain/range with "
                << "mismatched sizes did not throw an exception"
                << std::endl;
      assert(0);
   } catch (const std::exception &) {
      // Expected.
   }

   range.resize(10);
   try {
      ChiSq chi_sq(domain, range, 0);
      std::cerr << "creating ChiSq object with a NULL function pointer "
                << "did not throw an exception"
                << std::endl;
      assert(0);
   } catch (const std::exception &) {
      // Expected.
   }

// test for success
   // Correct value of chi_sq.
   double correct_value = 0.;

   // Vector to hold correct deriv wrt Prefactor, deriv wrt Mean, deriv wrt Sigma
   ChiSq::DataCont_t correct_deriv(3, 0.);

   for (int ii = 0; ii < 10; ++ii) {
      domain[ii] = .2 * ii;
      dArg arg(domain[ii]);
      // Set range so that each term in chisq sum contributes .25 * model value
      // and each deriv contributes .75 * function deriv.
      range[ii] = .5 * gauss.value(arg);
      correct_value += .25 * gauss.value(arg);
      correct_deriv[0] += .75 * gauss.derivByParam(arg, "Prefactor");
      correct_deriv[1] += .75 * gauss.derivByParam(arg, "Mean");
      correct_deriv[2] += .75 * gauss.derivByParam(arg, "Sigma");
   }

   // Create ChiSq object from valid inputs.
   ChiSq chi_sq(domain, range, &gauss);

   // Compare ChiSq's computation of value with the known correct result.
   if (fabs((correct_value - chi_sq.value()) / correct_value) > 1.e-8) {
      passed = false;
      std::cerr << "ChiSq::value returned " << chi_sq.value() << ", not " << correct_value
                << ", as expected."
                << std::endl;
   }

   // Compare ChiSq's computation of derivatives with the known correct result.
   ChiSq::DataCont_t deriv;
   chi_sq.getFreeDerivs(deriv);
   std::cerr.precision(10);
   if (correct_deriv.size() != deriv.size()) {
      passed = false;
      std::cerr << "output vector from ChiSq::getFreeDerivs(vector &) has size "
                << deriv.size() << ", not " << correct_deriv.size() << ", as expected."
                << std::endl;
   } else {
      for (int ii = 0; ii < 3; ++ii) {
         if (fabs((correct_deriv[ii] - deriv[ii]) / correct_deriv[ii]) > 1.e-8) {
            passed = false;
            std::cerr << "output from ChiSq::getFreeDerivs(vector &) deriv[" << ii
                      << "] has value " << deriv[ii] << ", not " << correct_deriv[ii]
                      << ", as expected."
                      << std::endl;
         }
      }
   }

   if (passed)
      std::cout << "*** test_ChiSq: all tests passed ***\n" 
                << std::endl;
} // ChiSq class tests
