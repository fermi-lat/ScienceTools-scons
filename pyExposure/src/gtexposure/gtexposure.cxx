/**
 * @file gtexposure.cxx
 * @brief Calculate exposure as a function of time at a specific location
 * on the sky as given by an LC file created by gtbin and add the exposure
 * column to the LC file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>
#include <string>
#include <vector>

#include <xercesc/util/XercesDefs.hpp>

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

#include "optimizers/dArg.h"
#include "optimizers/Function.h"
#include "optimizers/FunctionFactory.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/RangeCut.h"
#include "dataSubselector/SkyConeCut.h"

#include "Likelihood/BrokenPowerLawExpCutoff.h"
#include "Likelihood/BrokenPowerLaw2.h"
#include "Likelihood/ExpCutoff.h"
#include "Likelihood/FileFunction.h"
#include "Likelihood/LogParabola.h"
#include "Likelihood/MapCubeFunction.h"
#include "Likelihood/PowerLaw2.h"

#include "pyExposure/Exposure.h"

XERCES_CPP_NAMESPACE_USE

class GtExposure : public st_app::StApp {

public:

   GtExposure();

   virtual ~GtExposure() throw() {
      try {
         delete m_formatter;
      } catch (std::exception & eObj) {
         std::cout << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

   virtual void banner() const;

private:

   st_app::AppParGroup & m_pars;
   st_stream::StreamFormatter * m_formatter;
   optimizers::FunctionFactory * m_funcFactory;
   pyExposure::Exposure * m_exposure;
   optimizers::Function * m_function;

   double m_emin;
   double m_emax;
   double m_ra;
   double m_dec;
   double m_radius;

   std::vector<double> m_weightedExps;

   static std::string s_cvs_id;

   void prepareFunctionFactory();
   void promptForParameters();
   void parseDssKeywords();
   void setExposure();
   void getLcTimes(std::vector<double> & tlims) const;
   void prepareModel();
   void performSpectralWeighting();
   void writeExposure();
};

st_app::StAppFactory<GtExposure> myAppFactory("gtexposure");

std::string GtExposure::s_cvs_id("$Name$");

GtExposure::GtExposure() 
   : st_app::StApp(), 
     m_pars(st_app::StApp::getParGroup("gtexposure")),
     m_formatter(new st_stream::StreamFormatter("gtexposure", "", 2)),
     m_funcFactory(new optimizers::FunctionFactory()),
     m_exposure(0),
     m_function(0),
     m_radius(180) {
   setVersion(s_cvs_id);
   prepareFunctionFactory();
}

void GtExposure::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void GtExposure::run() {
   promptForParameters();
   parseDssKeywords();
   setExposure();
   prepareModel();
   performSpectralWeighting();
   writeExposure();
}

void GtExposure::promptForParameters() {
   m_pars.Prompt("infile");
   m_pars.Prompt("scfile");
   m_pars.Prompt("irfs");
   m_pars.Prompt("srcmdl");
   std::string xmlFile = m_pars["srcmdl"];
   if (xmlFile == "none") {
      m_pars.Prompt("specin");
   } else {
      m_pars.Prompt("target");
   }
   m_pars.Save();
}

void GtExposure::parseDssKeywords() {
   std::string lc_file = m_pars["infile"];
   bool aperture_correct = m_pars["apcorr"];
   dataSubselector::Cuts cuts(lc_file, "RATE", false);
   m_ra = m_pars["ra"];
   m_dec = m_pars["dec"];
   if (aperture_correct){
      m_radius = m_pars["rad"];
   }
   m_emin = m_pars["emin"];
   m_emax = m_pars["emax"];
   for (size_t i(0); i < cuts.size(); i++) {
      if (cuts[i].type() == "SkyCone") {
         const dataSubselector::SkyConeCut & my_cut =
            dynamic_cast<dataSubselector::SkyConeCut &>(
               const_cast<dataSubselector::CutBase &>(cuts[i]));
         m_ra = my_cut.ra();
         m_dec = my_cut.dec();
         if (aperture_correct) {
            m_radius = my_cut.radius();
         }
      }
      if (cuts[i].type() == "range") {
         const dataSubselector::RangeCut & my_cut =
            dynamic_cast<dataSubselector::RangeCut &>(
               const_cast<dataSubselector::CutBase &>(cuts[i]));
         if (my_cut.colname() == "ENERGY") {
            m_emin = my_cut.minVal();
            m_emax = my_cut.maxVal();
         }
      }
   }
}

void GtExposure::setExposure() {
   std::vector<double> energies;
   int nee = m_pars["enumbins"];
   double estep(std::log(m_emax/m_emin)/(nee-1));
   for (int k(0); k < nee; k++) {
      energies.push_back(m_emin*std::exp(estep*k));
   }
   std::vector<double> tlims;
   getLcTimes(tlims);
   std::string ft2file = m_pars["scfile"];
   std::string irfs = m_pars["irfs"];
   if (irfs == "DSS") {
      irfs = "DC2";
   }
   m_exposure = new pyExposure::Exposure(ft2file, tlims, energies, 
                                         m_ra, m_dec, m_radius, irfs);
}

void GtExposure::getLcTimes(std::vector<double> & tlims) const {
   std::string lc_file = m_pars["infile"];
   const tip::Table * table = 
      tip::IFileSvc::instance().readTable(lc_file, "RATE");

   tip::Table::ConstIterator it(table->begin());
   tip::ConstTableRecord & row(*it);

   double time;
   double dt;
   tlims.clear();
   for ( ; it != table->end(); ++it) {
      row["TIME"].get(time);
      row["TIMEDEL"].get(dt);
      tlims.push_back(time - dt/2.);
   }
   tlims.push_back(time + dt/2.);

   delete table;
}

void GtExposure::prepareModel() {
   std::string xmlFile = m_pars["srcmdl"];
   std::string srcName = m_pars["target"];
   double gamma = m_pars["specin"];
   
   if (xmlFile == "none") {
      m_function = m_funcFactory->create("PowerLaw2");
      m_function->setParam("Index", gamma);
      return;
   }

   xmlBase::XmlParser * parser(new xmlBase::XmlParser());
   
   DOMDocument * doc(parser->parse(xmlFile.c_str()));
   DOMElement * source_library(doc->getDocumentElement());
   if (!xmlBase::Dom::checkTagName(source_library, "source_library")) {
      throw std::runtime_error("source_library not found in " + xmlFile);
   }
   typedef std::vector<DOMElement *> ElementVec_t;
   ElementVec_t srcs;
   xmlBase::Dom::getChildrenByTagName(source_library, "source", srcs);
   for (ElementVec_t::const_iterator it(srcs.begin()); 
        it != srcs.end(); ++it) {
      std::string name(xmlBase::Dom::getAttribute(*it, "name"));
      if (name == srcName) {
         ElementVec_t children;
         xmlBase::Dom::getChildrenByTagName(*it, "spectrum", children);
         DOMElement * spectrum(children.front());
         std::string type(xmlBase::Dom::getAttribute(spectrum, "type"));
         m_function = m_funcFactory->create(type);
         m_function->setParams(spectrum);
         return;
      }
   }
   throw std::runtime_error("Source named " + srcName + " not found in "
                            + xmlFile);
}

void GtExposure::performSpectralWeighting() {
   const std::vector<double> & energies(m_exposure->energies());
   std::vector<double> dndes;
   for (size_t k(0); k < energies.size(); k++) {
      optimizers::dArg arg(energies.at(k));
      dndes.push_back(m_function->value(arg));
   }
   double dnde_int(0);
   for (size_t k(0); k < energies.size()-1; k++) {
      dnde_int += ((dndes.at(k+1) + dndes.at(k))/2.
                   *(energies.at(k+1) - energies.at(k)));
   }
   const std::vector< std::vector<double> > & exposures(m_exposure->values());
   std::vector< std::vector<double> >::const_iterator row(exposures.begin());
   m_weightedExps.clear();
   for ( ; row != exposures.end(); ++row) {
      double my_exposure(0);
      for (size_t k(0); k < energies.size()-1; k++) {
         my_exposure += ((row->at(k+1)*dndes.at(k+1) 
                           + row->at(k)*dndes.at(k))/2.
                          *(energies.at(k+1) - energies.at(k)))/dnde_int;
      }
      m_weightedExps.push_back(my_exposure);
   }
}

void GtExposure::writeExposure() {
   std::string lc_file = m_pars["infile"];
   tip::Table * table =
      tip::IFileSvc::instance().editTable(lc_file, "RATE");

   try {
      table->appendField("EXPOSURE", "E");
   } catch (tip::TipException & eObj) {
      if (!st_facilities::Util::expectedException(eObj, "already exists")) {
         throw;
      }
   }
   
   tip::Table::Iterator it(table->begin());
   tip::TableRecord & row(*it);

   if (m_weightedExps.size() != static_cast<size_t>(table->getNumRecords())) {
      throw std::runtime_error("Size of exposures does not equal size of "
                               "lc file RATE table");
   }

   for (size_t i(0); it != table->end(); ++it, i++) {
      row["exposure"].set(m_weightedExps.at(i));
   }
   delete table;
}

void GtExposure::prepareFunctionFactory() {
   m_funcFactory->addFunc("BPLExpCutoff",
                          new Likelihood::BrokenPowerLawExpCutoff(), 
                          false);
   m_funcFactory->addFunc("BrokenPowerLaw2", new Likelihood::BrokenPowerLaw2(),
                          false);
   m_funcFactory->addFunc("ExpCutoff", new Likelihood::ExpCutoff(), false);
   m_funcFactory->addFunc("LogParabola", new Likelihood::LogParabola(), false);
   m_funcFactory->addFunc("FileFunction", new Likelihood::FileFunction(), 
                          false);
   m_funcFactory->addFunc("MapCubeFunction", new Likelihood::MapCubeFunction(),
                          false);
   m_funcFactory->addFunc("PowerLaw2", new Likelihood::PowerLaw2(), false);
}
