/**
 * @file gtalign.cxx
 * @brief Apply alignment rotations to FT1 data.
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "CLHEP/Vector/Rotation.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "astro/GPS.h"
#include "astro/SkyDir.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "DefaultAlignment.h"

/**
 * @class Align
 * @author J. Chiang
 *
 */

class Align : public st_app::StApp {
public:
   Align() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("gtalign")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in Align constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~Align() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

   virtual void banner() const;

private:
   
   st_app::AppParGroup & m_pars;

   void check_outfile();
   void copyFile() const;
   void setAlignment(double time) const;
   void setRotAngles(double rx, double ry, double rz) const;
   void applyAlignment() const;

   static std::string s_cvs_id;
};

std::string Align::s_cvs_id("$Name$");

st_app::StAppFactory<Align> myAppFactory("gtalign");

void Align::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void Align::run() {
   m_pars.Prompt();
   m_pars.Save();
   check_outfile();
   copyFile();
   applyAlignment();
}

void Align::check_outfile() {
   st_stream::StreamFormatter formatter("Align", "run", 2);
   bool clobber = m_pars["clobber"];
   std::string outfile = m_pars["outfile"];
   if (outfile == "") {
      formatter.err() << "Please specify an output file name." 
                      << std::endl;
      std::exit(1);
   }
   if (!clobber && st_facilities::Util::fileExists(outfile)) {
      formatter.err() << "Output file " << outfile 
                      << " exists and clobber is set to 'no'.  Exiting."
                      << std::endl;
      std::exit(1);
   }
}

void Align::copyFile() const {
   std::string infile = m_pars["infile"];
   std::string outfile = m_pars["outfile"];
   std::string extname("EVENTS");

   st_facilities::FitsUtil::fcopy(infile, outfile, extname, "", true);
}

void Align::setAlignment(double time) const {
   gtalign::DefaultAlignment & alignment(gtalign::DefaultAlignment::instance());
   
   if (alignment.resetInterval(time)) {
      setRotAngles(alignment.rx(), alignment.ry(), alignment.rz());
   }
}

void Align::setRotAngles(double rx, double ry, double rz) const {
   astro::GPS * gps(astro::GPS::instance());
   static double arcsec2deg(M_PI/180./3600.);
   CLHEP::HepRotation rot(CLHEP::HepRotationX(rx*arcsec2deg)*
                          CLHEP::HepRotationY(ry*arcsec2deg)*
                          CLHEP::HepRotationZ(rz*arcsec2deg));
   
   gps->setAlignmentRotation(rot);
}

void Align::applyAlignment() const {
   astro::GPS * gps(astro::GPS::instance());
   std::string scfile = m_pars["scfile"];
   gps->setPointingHistoryFile(scfile);

   std::string outfile = m_pars["outfile"];
   tip::Table * table(tip::IFileSvc::instance().editTable(outfile, "events"));
   
   tip::Table::Iterator it(table->begin());
   tip::Table::Record & row(*it);

   bool user_alignment = m_pars["usralign"];
   if (user_alignment) {
      double rx = m_pars["rx"];
      double ry = m_pars["ry"];
      double rz = m_pars["rz"];
      setRotAngles(rx, ry, rz);
   }

   for ( ; it != table->end(); ++it) {
      double ra, dec;
      row["ra"].get(ra);
      row["dec"].get(dec);
      double met;
      row["time"].get(met);
      
      if (!user_alignment) {
         setAlignment(met);
      }

      astro::SkyDir newDir(gps->correct(astro::SkyDir(ra, dec), met));
      row["ra"].set(newDir.ra());
      row["dec"].set(newDir.dec());
      row["l"].set(newDir.l());
      row["b"].set(newDir.b());
   }
   delete table;
}
