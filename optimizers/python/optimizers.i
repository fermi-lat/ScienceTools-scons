// -*- mode: c++ -*-
%module optimizers
%{
#include "../optimizers/Arg.h"
#include "../optimizers/CompositeFunction.h"
#include "../optimizers/Drmngb.h"
#include "../optimizers/Exception.h"
#include "../optimizers/Function.h"
#include "../optimizers/FunctionTest.h"
#include "../optimizers/Lbfgs.h"
#include "../optimizers/Mcmc.h"
#include "../optimizers/Minuit.h"
#include "../optimizers/Optimizer.h"
#include "../optimizers/OutOfBounds.h"
#include "../optimizers/Parameter.h"
#include "../optimizers/ParameterNotFound.h"
#include "../optimizers/ProductFunction.h"
#include "../optimizers/SumFunction.h"
#include "../optimizers/dArg.h"
#include "../src/AbsEdge.h"
#include "../src/Gaussian.h"
#include "../src/MyFun.h"
#include "../src/PowerLaw.h"
#include "../src/Rosen.h"
#include <vector>
#include <string>
%}
%include ../optimizers/Arg.h
%include ../optimizers/dArg.h
%include ../optimizers/Exception.h
%include ../optimizers/OutOfBounds.h
%include ../optimizers/ParameterNotFound.h
%include ../optimizers/Parameter.h
%include ../optimizers/Function.h
%include ../optimizers/CompositeFunction.h
%include ../optimizers/ProductFunction.h
%include ../optimizers/SumFunction.h
%include ../optimizers/FunctionTest.h
%include ../optimizers/Mcmc.h
%include ../optimizers/Optimizer.h
%include ../optimizers/Lbfgs.h
%include ../optimizers/Minuit.h
%include ../optimizers/Drmngb.h
%include ../src/AbsEdge.h
%include ../src/Gaussian.h
%include ../src/MyFun.h
%include ../src/PowerLaw.h
%include ../src/Rosen.h
%include stl.i
%template(DoubleVector) std::vector<double>;
%template(DoubleVectorVector) std::vector< std::vector<double> >;
%template(StringVector) std::vector<std::string>;
%extend optimizers::Function {
   void setParamBounds(char *paramName, double lower, double upper) {
      self->setParamBounds(std::string(paramName), lower, upper);
   }
   Parameter getParam(char *paramName) {
      return self->getParam(std::string(paramName));
   }
}
