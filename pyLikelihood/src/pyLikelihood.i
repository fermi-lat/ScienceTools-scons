// -*- mode: c++ -*-
%module pyLikelihood
%{
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_stream/st_stream.h"
#include "irfLoader/Loader.h"
#include "irfInterface/IrfsFactory.h"
#include "healpix/CosineBinner.h"
#include "map_tools/Exposure.h"
#include "map_tools/SkyImage.h"
#include "optimizers/Optimizer.h"
#include "optimizers/OptimizerFactory.h"
#include "optimizers/Parameter.h"
#include "optimizers/ParameterNotFound.h"
#include "optimizers/Function.h"
#include "optimizers/FunctionFactory.h"
#include "optimizers/FunctionTest.h"
#include "optimizers/Mcmc.h"
#include "optimizers/Statistic.h"
#include "st_facilities/FitsImage.h"
#include "st_facilities/Util.h"
#include "Likelihood/AppHelpers.h"
#include "Likelihood/BinnedExposure.h"
#include "Likelihood/BrokenPowerLaw2.h"
#include "Likelihood/Convolve.h"
#include "Likelihood/CountsSpectra.h"
#include "Likelihood/Source.h"
#include "Likelihood/DiffuseSource.h"
#include "Likelihood/EquinoxRotation.h"
#include "Likelihood/Event.h"
#include "Likelihood/EventContainer.h"
#include "Likelihood/ExpCutoff.h"
#include "Likelihood/BrokenPowerLawExpCutoff.h"
#include "Likelihood/ExposureCube.h"
#include "Likelihood/ExposureMap.h"
#include "Likelihood/FileFunction.h"
#include "Likelihood/LogParabola.h"
#include "Likelihood/MapCubeFunction.h"
#include "Likelihood/MeanPsf.h"
#include "Likelihood/Npred.h"
#include "Likelihood/OneSourceFunc.h"
#include "Likelihood/OptEM.h"
#include "Likelihood/PointSource.h"
#include "Likelihood/PowerLawSuperExpCutoff.h"
#include "Likelihood/PowerLaw2.h"
#include "Likelihood/ResponseFunctions.h"
#include "Likelihood/RoiCuts.h"
#include "Likelihood/ScData.h"
#include "Likelihood/SkyDirArg.h"
#include "Likelihood/SkyDirFunction.h"
#include "Likelihood/SourceFactory.h"
#include "Likelihood/SourceModel.h"
#include "Likelihood/SourceMap.h"
#include "Likelihood/SpatialMap.h"
#include "Likelihood/SrcArg.h"
#include "Likelihood/TrapQuad.h"
#include "Likelihood/Exception.h"
#include "Likelihood/LogLike.h"
#include "Likelihood/BinnedLikelihood.h"
#include "Likelihood/Pixel.h"
#include "Likelihood/CountsMap.h"
#include "Likelihood/Observation.h"
#include "Likelihood/WcsMap.h"
#include <vector>
#include <string>
#include <exception>

class StAppInterface : public st_app::StApp {
public:
   StAppInterface(const std::string & appName)
      : st_app::StApp(), m_appName(appName) {}

   virtual ~StAppInterface() throw() {}

   virtual void run() {}

   st_app::AppParGroup & getParGroup() {
      return st_app::StApp::getParGroup(m_appName);
   }
private:
   const std::string m_appName;
};

using optimizers::Parameter;
using optimizers::ParameterNotFound;
using optimizers::Function;
using optimizers::Exception;
%}
%include stl.i
%exception {
   try {
      $action
   } catch (std::exception & eObj) {
      PyErr_SetString(PyExc_RuntimeError, const_cast<char*>(eObj.what()));
      return NULL;
   }
}
%template(DoublePair) std::pair<double, double>;
%template(EventVector) std::vector<Likelihood::Event>;
%template(DoubleVector) std::vector<double>;
%template(FloatVector) std::vector<float>;
%template(DoubleVectorVector) std::vector< std::vector<double> >;
%template(DoubleVectorPair) std::vector< std::pair<double, double> >;
%template(StringVector) std::vector<std::string>;
%include st_app/AppParGroup.h
%include st_app/StApp.h
%include astro/SkyProj.h
%include astro/SkyDir.h
%template(SkyDirVector) std::vector<astro::SkyDir>;
%feature("autodoc", "1");
%include healpix/CosineBinner.h
%include map_tools/Exposure.h
%include map_tools/SkyImage.h
%include optimizers/Arg.h
%include optimizers/dArg.h
%include optimizers/Parameter.h
%template(ParameterVector) std::vector<optimizers::Parameter>;
%include optimizers/Function.h
%include optimizers/Statistic.h
%include optimizers/FunctionTest.h
%include optimizers/FunctionFactory.h
%include optimizers/Optimizer.h
%include optimizers/OptimizerFactory.h
%include optimizers/Mcmc.h
%include st_facilities/FitsImage.h
%include st_facilities/Util.h
%include Likelihood/EquinoxRotation.h
%template (FuncPair) std::pair<std::string, optimizers::Function *>;
%template (FuncMap) std::map<std::string, optimizers::Function *>;
%include Likelihood/Convolve.h
%include Likelihood/Exception.h
%include Likelihood/ExpCutoff.h
%include Likelihood/BrokenPowerLawExpCutoff.h
%include Likelihood/ResponseFunctions.h
%include Likelihood/Event.h
%include Likelihood/Source.h
%include Likelihood/ExposureCube.h
%include Likelihood/ExposureMap.h
%include Likelihood/FileFunction.h
%include Likelihood/RoiCuts.h
%include Likelihood/ScData.h
%include Likelihood/EventContainer.h
%include Likelihood/Observation.h
%include Likelihood/BinnedExposure.h
%include Likelihood/AppHelpers.h
//%include Likelihood/SourceModel.h
%include Likelihood/DiffuseSource.h
%include Likelihood/Pixel.h
%template (PixelVector) std::vector<Likelihood::Pixel>;
%include Likelihood/CountsMap.h
%include Likelihood/SourceModel.h
%include Likelihood/LogLike.h
%include Likelihood/CountsSpectra.h
%include Likelihood/MeanPsf.h
%include Likelihood/BinnedLikelihood.h
%include Likelihood/Npred.h
%include Likelihood/OneSourceFunc.h
%include Likelihood/OptEM.h
%include Likelihood/PointSource.h
%include Likelihood/SourceMap.h
%include Likelihood/SpatialMap.h
%include Likelihood/SkyDirArg.h
%include Likelihood/SkyDirFunction.h
%include Likelihood/SourceFactory.h
%include Likelihood/SrcArg.h
%include Likelihood/TrapQuad.h
%include Likelihood/MapCubeFunction.h
%include Likelihood/WcsMap.h
%extend st_facilities::Util {
   static std::vector<std::string> 
      resolveFitsFiles(const std::string & infile) {
      std::vector<std::string> outfiles;
      st_facilities::Util::resolve_fits_files(infile, outfiles);
      return outfiles;
   }
}
%extend Likelihood::EquinoxRotation {
   astro::SkyDir fromMapCoords(const astro::SkyDir & inDir) {
      astro::SkyDir outDir(0, 0);
      self->do_rotation(inDir, outDir, false);
      return outDir;
   }
   astro::SkyDir toMapCoords(const astro::SkyDir & inDir) {
      astro::SkyDir outDir(0, 0);
      self->do_rotation(inDir, outDir, true);
      return outDir;
   }
}
%extend Likelihood::SourceFactory {
   static optimizers::FunctionFactory * funcFactory() {
      optimizers::FunctionFactory * myFuncFactory 
         = new optimizers::FunctionFactory;
      myFuncFactory->addFunc("SkyDirFunction", 
                             new Likelihood::SkyDirFunction(), false);
      myFuncFactory->addFunc("SpatialMap", new Likelihood::SpatialMap(), 
                             false);
      myFuncFactory->addFunc("MapCubeFunction",
                             new Likelihood::MapCubeFunction(), 
                             false);
      myFuncFactory->addFunc("PowerLaw2",
                             new Likelihood::PowerLaw2(), 
                             false);
      myFuncFactory->addFunc("BrokenPowerLaw2",
                             new Likelihood::BrokenPowerLaw2(), 
                             false);
      myFuncFactory->addFunc("LogParabola",
                             new Likelihood::LogParabola(), 
                             false);
      myFuncFactory->addFunc("FileFunction",
                             new Likelihood::FileFunction(), 
                             false);
      myFuncFactory->addFunc("ExpCutoff",
                             new Likelihood::ExpCutoff(), 
                             false);
      myFuncFactory->addFunc("BPLExpCutoff",
                             new Likelihood::BrokenPowerLawExpCutoff(),
                             false);
      myFuncFactory->addFunc("PLSuperExpCutoff",
                             new Likelihood::PowerLawSuperExpCutoff(),
                             false);
      return myFuncFactory;
   }
}
%extend Likelihood::FileFunction {
   static Likelihood::FileFunction * cast(optimizers::Function * func) {
      Likelihood::FileFunction * file_func = 
         dynamic_cast<Likelihood::FileFunction *>(func);
      if (file_func == 0) {
         throw std::runtime_error("Cannot cast to a FileFunction.");
      }
      return file_func;
   }
}
%extend Likelihood::PointSource {
   static Likelihood::PointSource * cast(Likelihood::Source * src) {
      Likelihood::PointSource * ptsrc = 
         dynamic_cast<Likelihood::PointSource *>(src);
      if (ptsrc == 0) {
         throw std::runtime_error("Cannot cast to a PointSource.");
      }
      return ptsrc;
   }
}
%extend Likelihood::Source {
   double flux() {
      Likelihood::PointSource * ptsrc = 
         dynamic_cast<Likelihood::PointSource *>(self);
      if (ptsrc == 0) {
         std::ostringstream message;
         message << "Integrated flux is not available for this source.";
         throw std::runtime_error(message.str());
      }
      return ptsrc->flux();
   }
   double flux(double emin, double emax, size_t npts=100) {
      Likelihood::PointSource * ptsrc = 
         dynamic_cast<Likelihood::PointSource *>(self);
      if (ptsrc == 0) {
         std::ostringstream message;
         message << "Integrated flux is not available for this source.";
         throw std::runtime_error(message.str());
      }
      return ptsrc->flux(emin, emax, npts);
   }
}
%extend Likelihood::LogLike {
   void initOutputStreams(unsigned int maxChatter=2) {
      st_stream::OStream::initStdStreams();
      st_stream::SetMaximumChatter(maxChatter);
   }
   void print_source_params() {
      std::vector<std::string> srcNames;
      self->getSrcNames(srcNames);
      std::vector<Parameter> parameters;
      for (unsigned int i = 0; i < srcNames.size(); i++) {
         Likelihood::Source *src = self->getSource(srcNames[i]);
         Likelihood::Source::FuncMap srcFuncs = src->getSrcFuncs();
         srcFuncs["Spectrum"]->getParams(parameters);
         std::cout << "\n" << srcNames[i] << ":\n";
         for (unsigned int j = 0; j < parameters.size(); j++) {
            std::cout << parameters[j].getName() << ": "
                      << parameters[j].getValue() << std::endl;
         }
         if (!dynamic_cast<Likelihood::BinnedLikelihood *>(self)) {
            std::cout << "Npred: "
                      << src->Npred() << std::endl;
         }
      }
   }
   void src_param_table() {
      std::vector<std::string> srcNames;
      self->getSrcNames(srcNames);
      std::vector<Parameter> parameters;
      for (unsigned int i = 0; i < srcNames.size(); i++) {
         Likelihood::Source *src = self->getSource(srcNames[i]);
         Likelihood::Source::FuncMap srcFuncs = src->getSrcFuncs();
         srcFuncs["Spectrum"]->getParams(parameters);
         for (unsigned int j = 0; j < parameters.size(); j++) {
            std::cout << parameters[j].getValue() << "  ";
         }
         std::cout << src->Npred() << "  ";
         std::cout << srcNames[i] << std::endl;
      }
   }
   int getNumFreeParams() {
      return self->getNumFreeParams();
   }
   void getFreeParamValues(std::vector<double> & params) {
      self->getFreeParamValues(params);
   }
   std::vector<std::string> srcNames() {
      std::vector<std::string> my_names;
      self->getSrcNames(my_names);
      return my_names;
   }
   optimizers::Mcmc * Mcmc() {
      return new optimizers::Mcmc(*self);
   }
}
%extend Likelihood::BinnedLikelihood {
   std::vector<double> modelCounts(const std::string & srcName) {
      const Likelihood::Source * src(self->getSource(srcName));
      const Likelihood::SourceMap & srcMap(self->sourceMap(srcName));
      const std::vector<float> & model(srcMap.model());
      const std::vector<float> & counts(self->countsMap().data());
      const std::vector<double> & energies(self->energies());

      size_t numpix(self->countsMap().pixels().size());
      std::vector<double> model_counts(numpix*(energies.size() - 1), 0);
      for (size_t k(0); k < energies.size() - 1; k++) {
         for (size_t j(0); j < numpix; j++) {
            size_t imin(k*numpix + j);
            if (counts.at(imin) > 0) {
               size_t imax(imin + numpix);
               model_counts.at(imin) += src->pixelCounts(energies.at(k),
                                                         energies.at(k+1),
                                                         model.at(imin),
                                                         model.at(imax));
            }
         }
      }
      return model_counts;
   }
}
// %extend Likelihood::BinnedLikelihood {
//    static Likelihood::
//       BinnedLikelihood * create(const std::string & countsMapFile) {
//       Likelihood::CountsMap * dataMap = 
//          new Likelihood::CountsMap(countsMapFile);
//       Likelihood::BinnedLikelihood * logLike = 
//          new Likelihood::BinnedLikelihood(*dataMap, countsMapFile);
//       return logLike;
//    }
// }
%extend Likelihood::Event {
   std::pair<double, double> ra_dec() const {
      return std::make_pair(self->getDir().ra(), self->getDir().dec());
   }
   std::pair<double, double> sc_dir() const {
      return std::make_pair(self->getScDir().ra(), self->getScDir().dec());
   }
}

%extend st_app::StApp {
   static st_app::AppParGroup parGroup(const std::string & appName) {
      char * argv[] = {"dummy_app"};
      int argc(1);
      st_app::StApp::processCommandLine(argc, argv);
      StAppInterface foo(appName);
      return foo.getParGroup();
   }
}
%extend st_app::AppParGroup {
   std::string __getitem__(const std::string & parname) {
      std::string my_string = self->operator[](parname);
      return my_string;
   }
   double getDouble(const std::string & parname) {
      double my_value = self->operator[](parname);
      return my_value;
   }
   int getInt(const std::string & parname) {
      int my_value = self->operator[](parname);
      return my_value;
   }
   bool getBool(const std::string & parname) {
      bool  my_value = self->operator[](parname);
      return my_value;
   }
   void Save() {
      self->Save();
   }
}
%extend map_tools::Exposure {
   healpix::CosineBinner data(const astro::SkyDir & srcDir) {
      healpix::HealpixArray<healpix::CosineBinner> skyBinner(self->data());
      return skyBinner[srcDir];
   }
}
%extend healpix::CosineBinner {
   double costheta(size_t i) {
      if (i < 0 || i >= self->size()) {
         throw std::runtime_error("IndexError");
      }
      std::vector<float>::const_iterator it(self->begin() + i);
      return self->costheta(it);
   }
   float __getitem__(size_t i) {
      if (i < 0 || i >= self->size()) {
         throw std::runtime_error("IndexError");
      }
//      return self->operator[](i);
      return *(self->begin() + i);
   }
}
