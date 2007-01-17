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

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

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
   static std::string s_cvs_id;

   double m_emin;
   double m_emax;
   double m_ra;
   double m_dec;

   optimizers::FunctionFactory * m_funcFactory;
   pyExposure::Exposure * m_exposure;
   optimizers::Function * m_function;

   void parseDssKeywords();
   void setExposure();
   void getLcTimes(std::vector<double> & tlims) const;
   void prepareFunctionFactory();
};

st_app::StAppFactory<GtExposure> myAppFactory("gtexposure");

std::string GtExposure::s_cvs_id("$Name$");

void GtExposure::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

GtExposure::GtExposure() 
   : st_app::StApp(), 
     m_pars(st_app::StApp::getParGroup("gtexposure")),
     m_formatter(new st_stream::StreamFormatter("gtexposure", "", 2)),
     m_funcFactory(new optimizers::FunctionFactory()),
     m_exposure(0),
     m_function(0) {
   setVersion(s_cvs_id);
   prepareFunctionFactory();
}

void GtExposure::run() {
   m_pars.Prompt();
   m_pars.Save();
   parseDssKeywords();
   setExposure();
   prepareModel();
}

void GtExposure::parseDssKeywords() {
   std::string lc_file = m_pars["lcfile"];
   dataSubselector::Cuts cuts(lc_file, "RATE", false);
   m_ra = m_pars["ra"];
   m_dec = m_pars["dec"];
   m_emin = m_pars["emin"];
   m_emax = m_pars["emax"];
   for (size_t i(0); i < cuts.size(); i++) {
      if (cuts[i].type() == "SkyCone") {
         const dataSubselector::SkyConeCut & my_cut =
            dynamic_cast<dataSubselector::SkyConeCut &>(cuts[i]);
         ra = my_cut.ra();
         dec = my_cut.dec();
      }
      if (cuts[i].type == "range") {
         const dataSubselector::RangeCut & my_cut =
            dynamic_cast<dataSubselector::RangeCut &>(cuts[i]);
         if (my_cut.colname() == "ENERGY") {
            m_emin = my_cut.minVal();
            m_emax = my_cut.maxVal();
         }
      }
   }
}

void GtExposure::setExposure() {
   std::vector<double> energies;
   size_t nee(21);
   double estep(std::log(m_emax/m_emin)/(nee-1));
   for (size_t k(0); k < nee; k++) {
      energies.push_back(m_emin*std::exp(estep*k));
   }
   std::vector<double> tlims;
   getLcTimes(tlims);
   std::string ft2file = m_pars["scfile"];
   std::string irfs = m_pars["rspfunc"];
   if (irfs == "DSS") {
      irfs = "DC2";
   }
   m_exposure = pyExposure::Exposure(ft2file, tlims, energies, ra, dec, irfs);
}

void GtExposure::getLcTimes(std::vector<double> & tlims) const {
   std::string lc_file = m_pars["lcfile"];
   const tip::Table * table = 
      tip::IFileSvc::instance().readTable(lc_file, "RATE");

   tip::Table::ConstIterator it(table->begin());
   tip::ConstTableRecord row(*it);

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
   std::string xmlFile = m_pars["source_model_file"];
   std::string srcName = m_pars["target_source"];
   
   xmlBase::XmlParser * parser(Likelihood::XmlParser_instance());
   
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
         m_function = m_functionFactory->create(type);
         m_function->setParams(spectrum);
         return;
      }
   }
   throw std::runtime_error("Source named " + srcName + " not found in "
                            + xmlFile);
}

void prepareFunctionFactory() {
   m_funcFactory->addFunc("SkyDirFunction", new Likelihood::SkyDirFunction(),
                          false);
   m_funcFactory->addFunc("SpatialMap", new Likelihood::SpatialMap(), false);
   m_funcFactory->addFunc("MapCubeFunction", new Likelihood::MapCubeFunction(),
                          false);
   m_funcFactory->addFunc("PowerLaw2", new Likelihood::PowerLaw2(), false);
   m_funcFactory->addFunc("BrokenPowerLaw2", new Likelihood::BrokenPowerLaw2(),
                          false);
   m_funcFactory->addFunc("LogParabola", new Likelihood::LogParabola(), false);
   m_funcFactory->addFunc("FileFunction", new Likelihood::FileFunction(), 
                          false);
   m_funcFactory->addFunc("ExpCutoff", new Likelihood::ExpCutoff(), false);
   m_funcFactory->addFunc("BPLExpCutoff",
                          new Likelihood::BrokenPowerLawExpCutoff(), 
                          false);
}
